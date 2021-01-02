#include <mutex>
#include <vector>
#include <functional>
#include <Geometry/Vector3.h>
#include <Tools/Parallel.h>

static std::vector<std::thread> threads;
extern thread_local uint64_t ProfilerState = 0;
static bool shutdownThreads = false;

inline uint64_t CurrentProfilerState() { return ProfilerState; }
static std::mutex workListMutex;

static ParallelForLoop* workList = nullptr;
static std::condition_variable workListCondition;
thread_local int ThreadIndex;

void ParallelFor2D(std::function<void(Point2i)> func, const Point2i& count) {
	if (threads.empty() || count.x() * count.y() <= 1) {
		for (int y = 0; y < count.y(); ++y)
			for (int x = 0; x < count.x(); ++x) func(Point2i(x, y));
		return;
	}

	ParallelForLoop loop(std::move(func), count, CurrentProfilerState());
	{
		std::lock_guard<std::mutex> lock(workListMutex);
		loop.next = workList;
		workList = &loop;
	}

	std::unique_lock<std::mutex> lock(workListMutex);
	workListCondition.notify_all();

	// Help out with parallel loop iterations in the current thread
	while (!loop.Finished()) {
		// Run a chunk of loop iterations for _loop_

		// Find the set of loop iterations to run next
		int64_t indexStart = loop.nextIndex;
		int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

		// Update _loop_ to reflect iterations this thread will run
		loop.nextIndex = indexEnd;
		if (loop.nextIndex == loop.maxIndex) workList = loop.next;
		loop.activeWorkers++;

		// Run loop indices in _[indexStart, indexEnd)_
		lock.unlock();
		for (int64_t index = indexStart; index < indexEnd; ++index) {
			uint64_t oldState = ProfilerState;
			ProfilerState = loop.profilerState;
			if (loop.func1D) {
				loop.func1D(index);
			}
			// Handle other types of loops
			else {
				loop.func2D(Point2i(index % loop.nX, index / loop.nX));
			}
			ProfilerState = oldState;
		}
		lock.lock();

		// Update _loop_ to reflect completion of iterations
		loop.activeWorkers--;
	}
}

void Barrier::Wait()
{
	std::unique_lock<std::mutex> lock(mutex);
	assert(count > 0);
	if (--count == 0)
		// This is the last thread to reach the barrier; wake up all of the
		// other ones before exiting.
		cv.notify_all();
	else
		// Otherwise there are still threads that haven't reached it. Give
		// up the lock and wait to be notified.
		cv.wait(lock, [this] { return count == 0; });
}

static void workerThreadFunc(int tIndex, std::shared_ptr<Barrier> barrier) {
	ThreadIndex = tIndex;

	// The main thread sets up a barrier so that it can be sure that all
	// workers have called ProfilerWorkerThreadInit() before it continues
	// (and actually starts the profiling system).
	barrier->Wait();

	// Release our reference to the Barrier so that it's freed once all of
	// the threads have cleared it.
	barrier.reset();

	std::unique_lock<std::mutex> lock(workListMutex);
	while (!shutdownThreads) {
		if (!workList) {
			// Sleep until there are more tasks to run
			workListCondition.wait(lock);
		}
		else {
			// Get work from _workList_ and run loop iterations
			ParallelForLoop& loop = *workList;

			// Run a chunk of loop iterations for _loop_

			// Find the set of loop iterations to run next
			int64_t indexStart = loop.nextIndex;
			int64_t indexEnd =
				std::min(indexStart + loop.chunkSize, loop.maxIndex);

			// Update _loop_ to reflect iterations this thread will run
			loop.nextIndex = indexEnd;
			if (loop.nextIndex == loop.maxIndex) workList = loop.next;
			loop.activeWorkers++;

			// Run loop indices in _[indexStart, indexEnd)_
			lock.unlock();
			for (int64_t index = indexStart; index < indexEnd; ++index) {
				uint64_t oldState = ProfilerState;
				ProfilerState = loop.profilerState;
				if (loop.func1D) {
					loop.func1D(index);
				}
				// Handle other types of loops
				else {
					loop.func2D(Point2i(index % loop.nX, index / loop.nX));
				}
				ProfilerState = oldState;
			}
			lock.lock();

			// Update _loop_ to reflect completion of iterations
			loop.activeWorkers--;
			if (loop.Finished()) workListCondition.notify_all();
		}
	}
}

void ParallelInit() {
	int nThreads = std::thread::hardware_concurrency();
	ThreadIndex = 0;

	// Create a barrier so that we can be sure all worker threads get past
	// their call to ProfilerWorkerThreadInit() before we return from this
	// function.  In turn, we can be sure that the profiling system isn't
	// started until after all worker threads have done that.
	std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(nThreads);

	// Launch one fewer worker thread than the total number we want doing
	// work, since the main thread helps out, too.
	for (int i = 0; i < nThreads - 1; ++i)
		threads.push_back(std::thread(workerThreadFunc, i + 1, barrier));

	barrier->Wait();
}

void ParallelCleanup() {
	if (threads.empty()) return;

	{
		std::lock_guard<std::mutex> lock(workListMutex);
		shutdownThreads = true;
		workListCondition.notify_all();
	}

	for (std::thread& thread : threads) thread.join();
	threads.erase(threads.begin(), threads.end());
	shutdownThreads = false;
}
#pragma once
#include <cassert>
#include <functional>
#include <mutex>
#include <Geometry/Vector3.h>
inline thread_local int ThreadIndex;
void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize, bool benchmark = false);
void ParallelFor2D(std::function<void(Point2i)> func, const Point2i& count, bool benchmark = false);
class ParallelForLoop {
public:
	// ParallelForLoop Public Methods
	ParallelForLoop(std::function<void(int64_t)> func1D, int64_t maxIndex, int chunkSize, uint64_t profilerState)
		: func1D(std::move(func1D)),
		maxIndex(maxIndex),
		chunkSize(chunkSize),
		profilerState(profilerState) {}
	ParallelForLoop(const std::function<void(Point2i)>& f, const Point2i& count, uint64_t profilerState)
		: func2D(f),
		maxIndex(count.x()* count.y()),
		chunkSize(1),
		profilerState(profilerState) {
		nX = count.x();
	}

public:
	// ParallelForLoop Private Data
	std::function<void(int64_t)> func1D;
	std::function<void(Point2i)> func2D;
	const int64_t maxIndex;
	const int chunkSize;
	uint64_t profilerState;
	int64_t nextIndex = 0;
	int activeWorkers = 0;
	ParallelForLoop* next = nullptr;
	int nX = -1;

	// ParallelForLoop Private Methods
	bool Finished() const {
		return nextIndex >= maxIndex && activeWorkers == 0;
	}
};

class Barrier {
public:
	Barrier(int count) : count(count)
	{
		assert(count > 0);
	}
	~Barrier() { assert(count == 0); }
	void Wait();

private:
	std::mutex mutex;
	std::condition_variable cv;
	int count;
};

void ParallelInit();
void ParallelCleanup();
int NumSystemCores();
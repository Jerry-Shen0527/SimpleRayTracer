#include <thread>
#include <Tools/Parallel.h>

#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

int arr[6][6];
int main()
{
	Point2i b(6, 6);

	ParallelInit();

	ParallelFor2D(
		[&](Point2i p) {
		for (int i = 1; i < 10000000; ++i)
		{
			arr[p.x()][p.y()] += i;
		}}, b
	);

	ParallelCleanup();
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			std::cout << arr[i][j] << ' ';
		}
		std::cout << std::endl;
	}

#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}
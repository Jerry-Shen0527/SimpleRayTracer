#include <thread>
#include <Tools/Parallel.h>
int arr[6][6];
int main()
{
	Point2i b(6, 6);

	ParallelInit();

	ParallelFor2D([&](Point2i p) {
		for (int i = 1; i < 10; ++i)
		{
			arr[p.x()][p.y()] += i;
		}}, b);

	ParallelCleanup();
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			std::cout << arr[i][j] << ' ';
		}
		std::cout << std::endl;
	}
}
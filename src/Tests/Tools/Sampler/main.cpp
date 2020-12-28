#include <Tools/Sampler.h>
int main()
{
	std::cout << PowerHeuristic(3, 1, 2, 1);
	StratifiedSampler sampler(10, 10, true, 1);

	sampler.Request1DArray(10);

	sampler.StartPixel(Point2i(0, 0));

	for (int i = 0; i < 10; ++i)
	{
		auto ptr = sampler.Get1DArray(0);

		sampler.StartNextSample();
		std::cout << ptr[i] << ' ';
	}
}
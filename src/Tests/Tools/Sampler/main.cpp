#include <Tools/Sampler.h>
int main()
{
	std::cout << PowerHeuristic(3, 1, 2, 1);
	StratifiedSampler sampler(10, 10, true, 10);
}
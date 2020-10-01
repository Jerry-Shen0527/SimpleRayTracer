#include <ctime>
#include <thread>

#include <Integrators/SamplerIntegrator.h>
#include <Scene/WorldFactory.h>
#include <Tools/Files/FileWrite.h>

#include "Integrators/SpectrumIntegrator.h"

int main(int argc, char** argv) {
	// Image

	const int max_depth = 50;
	int samples_per_pixel = 1;

	clock_t start, finish;

	//World
	hittable_list world;

	camera cam;
	WorldFactory world_factory;
	color background;

	world_factory.get_world(6, world, cam, background);

	if (argc < 2)
	{
		std::cerr << "Usage: main.exe filename <spp>" << std::endl;
	}
	if (argc == 3)
	{
		samples_per_pixel = stoi(std::string(argv[2]));
	}
	Spectrum::Init();

	// Render

	SpectrumIntegrator spectrum_integrator(samples_per_pixel, max_depth);
	SamplerIntegrator simple_integrator(world, samples_per_pixel, max_depth);

	start = clock();

	//spectrum_integrator.integrate(cam, world, background);
	simple_integrator.integrate(cam, world, background);

	finish = clock();

	//film_to_file("out", film);
	film_to_file(std::string(argv[1]), *(cam.film));

	std::cerr << "time = " << float(finish - start) / CLOCKS_PER_SEC << "s" << std::endl;  //输出时间（单位：ｓ）

	std::cerr << "\nDone.\n";
}

//#include <Tools/Sampler/PixelSampler.h>
//
//#include "Tools/Sampler/StratifiedSampler.h"
//
//int main()
//{
//	StratifiedSampler sampler(3, 2, false, 100);
//
//	sampler.StartPixel({ 1,1 });
//
//	sampler.Request1DArray(10);
//	sampler.Request2DArray(10);
//	std::cout << sampler.Get2D() << std::endl;
//	std::cout << sampler.Get2D() << std::endl;
//	std::cout << sampler.Get2D() << std::endl;
//	std::cout << sampler.Get2D() << std::endl;
//	std::cout << sampler.Get2D() << std::endl;
//
//	auto p = sampler.Get2DArray(10);
//
//	for (int i = 0; i < 10; ++i)
//	{
//		std::cout << p[i] << std::endl;
//	}
//}
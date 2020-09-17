#include <ctime>
#include <thread>

#include <Integrators/SimpleIntegrator.h>
#include <Scene/WorldFactory.h>
#include <Tools/Files/FileWrite.h>

#include "Integrators/SpectrumIntegrator.h"

constexpr auto aspect_ratio = 1.6/0.9;
constexpr int image_width = 2560;
constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

int main(int argc, char** argv) {
	// Image

	const int max_depth = 50;
	int samples_per_pixel = 16;

	clock_t start, finish;

	//World
	hittable_list world;
	camera cam;
	WorldFactory world_factory;
	color background;

	world_factory.get_world(8, aspect_ratio, world, cam, background);

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
	start = clock();


	SpectrumIntegrator spectrum_integrator(samples_per_pixel, max_depth);
	SimpleIntegrator simple_integrator(samples_per_pixel, max_depth);

	Film film(image_width, image_height);
	spectrum_integrator.integrate(cam, film, world, background);
	//simple_integrator.integrate(cam, film, world, background);

	finish = clock();

	//film_to_file("out", film);
	film_to_file(std::string(argv[1]), film);

	std::cerr << "time = " << double(finish - start) / CLOCKS_PER_SEC << "s" << std::endl;  //输出时间（单位：ｓ）

	std::cerr << "\nDone.\n";
}
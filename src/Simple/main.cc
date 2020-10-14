#include <ctime>
#include <memory>
#include <memory>
#include <memory>
#include <memory>
#include <memory>
#include <thread>

#include <Integrators/SamplerIntegrator.h>
#include <Scene/WorldFactory.h>
#include <Tools/Files/FileWrite.h>
#include <Tools/camera.h>

#include "BRDF/dielectric.h"
#include "BRDF/diffuse_light.h"
#include "BRDF/lambertian.h"
#include "Geometry/sphere.h"
#include "Integrators/SpectrumIntegrator.h"

int main(int argc, char** argv) {
	// Image
	using std::make_shared;
	const int max_depth = 50;
	int samples_per_pixel = 1;

	clock_t start, finish;

	std::shared_ptr<Primitive> aggregate;
	std::vector<std::shared_ptr<Light>> lights;

	Transform transform = Translate(Point3f(190, 90, 190));

	auto red = std::make_shared<lambertian>(Color(.65, .05, .05));
	auto white = std::make_shared<lambertian>(Color(.73, .73, .73));
	auto green = std::make_shared<lambertian>(Color(.12, .45, .15));
	auto light = std::make_shared<diffuse_light>(Color(15, 15, 15));

	Sphere sphere(&transform, &transform, false, 90.0, -90.0, 90.0, 360);
	shared_ptr<material> glass = std::make_shared<dielectric>(1.5);
	GeometricPrimitive glass_ball(make_shared<Sphere>(sphere), glass, nullptr);

	//World
	Scene world(aggregate, lights);

	camera cam;
	//WorldFactory world_factory;
	Color background;

	//world_factory.get_world(6, world, cam, background);

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

	//SpectrumIntegrator spectrum_integrator(samples_per_pixel, max_depth);
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
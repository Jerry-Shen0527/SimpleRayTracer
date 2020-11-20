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

	std::vector<shared_ptr<Primitive>> geometries;

	std::shared_ptr<Primitive> aggregate;
	std::vector<std::shared_ptr<Light>> lights;

	auto red = make_shared<lambertian>(Color(.65, .05, .05));
	auto white = make_shared<lambertian>(Color(.73, .73, .73));
	auto green = make_shared<lambertian>(Color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(Color(15, 15, 15));

	geometries.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	geometries.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	geometries.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light, true)));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	//shared_ptr<material> aluminum = make_shared<metal>(Color(0.8, 0.85, 0.88), 0.0);
	shared_ptr<hittable> box1 = make_shared<box>(Point3f(0, 0, 0), Point3f(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, Vector3f(265, 0, 295));
	world.add(box1);

	//shared_ptr<hittable> box2 = make_shared<box>(Point3f(0, 0, 0), Point3f(165, 165, 165), white);
	//box2 = make_shared<rotate_y>(box2, -18);
	//box2 = make_shared<translate>(box2, Vector3f(130, 0, 65));
	//world.add(box2);
	shared_ptr<material> glass = make_shared<dielectric>(1.5);
	shared_ptr<hittable> glass_sphere = make_shared<sphere>(Point3f(190, 90, 190), 90.0, glass);

	world.add(glass_sphere);

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
#include <ctime>
#include <thread>
//
//#include <Integrators/SimpleIntegrator.h>
//#include <Scene/WorldFactory.h>
//#include <Tools/Files/FileWrite.h>
//
//constexpr auto aspect_ratio = 1.0;
//constexpr int image_width = 1000;
//constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
//
//int main(int argc, char** argv) {
//	// Image
//
//	const int max_depth = 50;
//	int samples_per_pixel = 16;
//
//	clock_t start, finish;
//
//	//World
//	hittable_list world;
//	camera cam;
//	WorldFactory world_factory;
//	color background;
//
//	world_factory.get_world(6, aspect_ratio, world, cam, background);
//
//	if (argc < 2)
//	{
//		std::cerr << "Usage: main.exe filename <spp>" << std::endl;
//	}
//	if (argc == 3)
//	{
//		samples_per_pixel = stoi(std::string(argv[2]));
//	}
//
//	// Render
//	start = clock();
//
//	SimpleIntegrator simple_integrator(samples_per_pixel, max_depth);
//	Film film(image_width, image_height);
//	simple_integrator.integrate(cam, film, world, background);
//
//	finish = clock();
//
//	//film_to_file("out", film);
//	film_to_file(std::string(argv[1]), film);
//
//	std::cerr << "time = " << double(finish - start) / CLOCKS_PER_SEC << "s" << std::endl;  //���ʱ�䣨��λ����
//
//	std::cerr << "\nDone.\n";
//}


#include <Tools/Spectrum/SampledSpectrum.h>

int main()
{
	CoefficientSpectrum<3> a(1);
	CoefficientSpectrum<3> b(2);


	Lerp(0.5, a, b).print();
}
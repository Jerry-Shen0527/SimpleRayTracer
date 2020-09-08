#include "rtweekend.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ctime>
#include <thread>
#include <Windows.h>
#include <Tools/stb_image_write.h>

#include "pdf/hittable_pdf.h"
#include "Scene/WorldFactory.h"

constexpr auto aspect_ratio = 9.0 / 9.0;
constexpr int image_width = 1024;
constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
constexpr  int pixelcount = image_width * image_height;
unsigned char image[pixelcount * 3];

color ray_color(
	const ray& r,
	const color& background,
	const hittable& world,
	shared_ptr<hittable> lights,
	int depth
) {
	hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	scatter_record srec;
	color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;
	if (srec.is_specular) {
		return srec.attenuation
			* ray_color(srec.specular_ray, background, world, lights, depth - 1);
	}

	shared_ptr<pdf> light_ptr = make_shared<hittable_pdf>(lights, rec.p);
	//mixture_pdf p(light_ptr, srec.pdf_ptr);

	mixture_pdf p(light_ptr, srec.pdf_ptr, 0.3);

	ray scattered = ray(rec.p, p.generate(), r.time());
	auto pdf_val = p.value(scattered.direction());

	return emitted + srec.attenuation
		* rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_color(scattered, background, world, lights, depth - 1)
		/ pdf_val;
}

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

	world_factory.get_world(6, aspect_ratio, world, cam, background);

	if (argc < 2)
	{
		std::cerr << "Usage: main.exe filename <spp>" << std::endl;
	}
	if (argc == 3)
	{
		samples_per_pixel = stoi(std::string(argv[2]));
	}

	auto lights = make_shared<hittable_list>(world);

	// Render
#ifndef _DEBUG
	int MAX_THREAD = std::thread::hardware_concurrency() - 2;
	start = clock();

	//std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	auto worker = [MAX_THREAD, samples_per_pixel, &world, max_depth, &cam, background, &lights](int arg)
	{
		int i, j, old_j;

		for (int idx = 0; idx < pixelcount; idx += MAX_THREAD)
		{
			idx_to_ij(idx + arg, i, j, image_width);
			if (arg == 0 && j % 10 == 0 && old_j != j)
			{
				std::cerr << "\r[ ";

				for (int i = 0; i < 50; ++i)
				{
					if (i<double(j) / image_height * 50)
					{
						std::cerr << "-";
					}
					else
					{
						std::cerr << " ";
					}
				}
				std::cerr << int(double(j) / image_height * 100) << '%' << "]" << std::flush;
			}
			old_j = j;
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, 1 - v);
				pixel_color += ray_color(r, background, world, lights, max_depth);
			}
			write_color(image, i, j, image_width, pixel_color, samples_per_pixel);
		}
	};

	std::vector<std::thread> pool;

	for (int n_thread = 0; n_thread < MAX_THREAD; n_thread++)
	{
		pool.emplace_back(worker, n_thread);
	}

	for (auto& thread : pool)
	{
		thread.join();
	}
#else

	int i, j, old_j = 0;

	for (int idx = 0; idx < pixelcount; idx++)
	{
		idx_to_ij(idx, i, j, image_width);
		if (j != old_j)
		{
			std::cerr << "\rAlready finishd: " << double(j) / image_height * 100 << '%' << std::flush;
		}
		color pixel_color(0, 0, 0);
		for (int s = 0; s < samples_per_pixel; ++s) {
			auto u = (i + random_double()) / (image_width - 1);
			auto v = (j + random_double()) / (image_height - 1);
			ray r = cam.get_ray(u, 1 - v);
			pixel_color += ray_color(r, background, world, lights, max_depth);
		}
		write_color(image, i, j, image_width, pixel_color, samples_per_pixel);
		old_j = j;
	}

#endif
	finish = clock();

	std::cerr << "time = " << double(finish - start) / CLOCKS_PER_SEC << "s" << std::endl;  //输出时间（单位：ｓ）
	//for (int i = 0; i < pixelcount; ++i)
	//{
	//	std::cout << static_cast<int>(image[i].x()) << ' ';
	//	std::cout << static_cast<int>(image[i].y()) << ' ';
	//	std::cout << static_cast<int>(image[i].z()) << ' ';
	//	std::cout << "\n";
	//}

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	stbi_write_png((std::string(argv[1]) + " " + std::to_string(sys.wMonth) + "_" + std::to_string(sys.wDay) + " " + std::to_string(sys.wHour) + "_" + std::to_string(sys.wMinute) + "_" + std::to_string(sys.wSecond) + ".png").c_str(), image_width, image_height, 3, image, 0);

	std::cerr << "\nDone.\n";
	}
#include <camera.h>
#include <dielectric.h>

#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include <iostream>
#include <lambertian.h>
#include <metal.h>
#include <thread>

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

color ray_color(const ray& r, const hittable& world, int depth) {
	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);
	hit_record rec;
	if (world.hit(r, 0.0001, infinity, rec)) {
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void idx_to_ij(int idx, int& i, int& j, int width)
{
	i = idx % width;
	j = idx / width;
}

constexpr auto aspect_ratio = 16.0 / 10.0;
constexpr int image_width = 2560;
constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
constexpr  int pixelcount = image_width * image_height;

vec3 image[pixelcount];

int main() {
	// Image

	const int samples_per_pixel = 600;
	const int max_depth = 50;

	//world
	//hittable_list world;
	//auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	//auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	//auto material_left = make_shared<dielectric>(1.5);
	//auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	//world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	//world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	//world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	//world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
	//world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));

	auto world = random_scene();

	// Camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

	// Render

#ifndef _DEBUG
	int MAX_THREAD = std::thread::hardware_concurrency();

	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	auto worker = [MAX_THREAD, samples_per_pixel, &world, max_depth, &cam](int arg)
	{
		int i, j, old_j;

		for (int idx = 0; idx < pixelcount; idx += MAX_THREAD)
		{
			idx_to_ij(idx + arg, i, j, image_width);
			if (j != old_j)
			{
				std::cerr << "\nAlready finishd: " << double(j) / image_height * 100 << '%' << std::flush;
			}
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, 1 - v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(image, i, j, image_width, pixel_color, samples_per_pixel);
			old_j = j;
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

	int i, j, old_j;

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
			pixel_color += ray_color(r, world, max_depth);
		}
		write_color(image, i, j, image_width, pixel_color, samples_per_pixel);
		old_j = j;
	}

#endif

	for (int i = 0; i < pixelcount; ++i)
	{
		std::cout << static_cast<int>(image[i].x()) << ' ';
		std::cout << static_cast<int>(image[i].y()) << ' ';
		std::cout << static_cast<int>(image[i].z()) << ' ';
		std::cout << "\n";
	}

	std::cerr << "\nDone.\n";
}
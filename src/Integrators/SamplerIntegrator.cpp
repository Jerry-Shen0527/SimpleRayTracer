#include <thread>
#include <Integrators/SimpleIntegrator.h>
#include <pdf/hittable_pdf.h>
#include <pdf/scatter_record.h>
#include <BRDF/Material.h>
#include <Geometry/hit_record.h>

#include "Tools/camera.h"
#include <Tools/Film.h>

void SimpleIntegrator::integrate(camera& cam, hittable_list& world, color background)
{
#ifndef _DEBUG
	//int MAX_THREAD = 3;
	int MAX_THREAD = std::thread::hardware_concurrency() - 2;

	//std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	auto worker = [MAX_THREAD, &world, &cam, background, this](int arg)
	{
		int i, j, old_j;

		auto _thread_sampler = sampler->Clone(0);

		//_thread_sampler->StartPixel(Point2i{ 0,0 });

		for (int idx = 0; idx < cam.film->pixelcount; idx += MAX_THREAD)
		{
			idx_to_ij(idx + arg, i, j, cam.film->width);
			_thread_sampler->StartPixel(Point2i{ i,j });

			if (arg == 0 && j % 10 == 0 && old_j != j)
			{
				std::cerr << "\r[ ";

				for (int i = 0; i < 50; ++i)
				{
					if (i<float(j) / cam.film->height * 50)
					{
						std::cerr << "-";
					}
					else
					{
						std::cerr << " ";
					}
				}
				std::cerr << int(float(j) / cam.film->height * 100) << '%' << "]" << std::flush;
			}
			old_j = j;
			color pixel_color(0, 0, 0);

			for (int s = 0; s < sample_per_pixel; ++s) {
				auto u = (i + random_float()) / (cam.film->width - 1);
				auto v = (j + random_float()) / (cam.film->height - 1);
				//auto a = _thread_sampler->Get2D();
				//auto u = (i + a.x()) / (cam.film->width - 1);
				//auto v = (j + a.y()) / (cam.film->height - 1);
				ray r = cam.get_ray(u, 1.0f - v);
				pixel_color += ray_color(r, background, world, lights, max_depth);
				_thread_sampler->StartNextSample();
			}
			cam.film->write_color(i, j, pixel_color, sample_per_pixel);
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

	for (int idx = 0; idx < cam.film->pixelcount; idx++)
	{
		idx_to_ij(idx, i, j, cam.film->width);
		if (j != old_j)
		{
			std::cerr << "\rAlready finishd: " << float(j) / cam.film->height * 100 << '%' << std::flush;
		}
		color pixel_color(0, 0, 0);
		sampler->StartPixel(Point2i{ i,j });
		for (int s = 0; s < sample_per_pixel; ++s) {
			//auto u = (i + random_float()) / (cam.film->width - 1);
			//auto v = (j + random_float()) / (cam.film->height - 1);
			auto a = sampler->Get2D();
			auto u = (i + a.x()) / (cam.film->width - 1);
			auto v = (j + a.y()) / (cam.film->height - 1);
			ray r = cam.get_ray(u, 1 - v);
			pixel_color += ray_color(r, background, world, lights, max_depth);
			sampler->StartNextSample();
		}
		cam.film->write_color(i, j, pixel_color, sample_per_pixel);
		old_j = j;
	}

#endif
}

color SimpleIntegrator::ray_color(const ray& r, const color& background, const hittable& world,
	std::shared_ptr<hittable> lights, int depth)
{
	surface_hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (!world.hit(r, rec))
		return background;

	scatter_record srec;
	color emitted = rec.mat_ptr->emitted(r, rec, rec.uv, rec.p);
	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;
	if (srec.is_specular) {
		return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, depth - 1);
	}

	shared_ptr<pdf> light_ptr = make_shared<hittable_pdf>(lights, rec.p);
	//mixture_pdf p(light_ptr, srec.pdf_ptr);

	mixture_pdf p(light_ptr, srec.pdf_ptr, 0.3);

	ray scattered = ray(rec.p, p.generate(), infinity, r.time());
	auto pdf_val = p.value(scattered.direction());

	return emitted + srec.attenuation
		* rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_color(scattered, background, world, lights, depth - 1)
		/ pdf_val;
}
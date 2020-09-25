#include <thread>
#include <Integrators/SpectrumIntegrator.h>

#include "pdf/hittable_pdf.h"
#include "pdf/scatter_record.h"
#include "Tools/camera.h"

void SpectrumIntegrator::integrate(camera& cam, hittable_list& world, color background)
{
	auto lights = make_shared<hittable_list>(world);

#ifndef _DEBUG
	int MAX_THREAD = std::thread::hardware_concurrency() - 2;

	//std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	auto worker = [MAX_THREAD, &world, &cam, background, &lights, this](int arg)
	{
		int i, j, old_j;

		for (int idx = 0; idx < cam.film->pixelcount; idx += MAX_THREAD)
		{
			idx_to_ij(idx + arg, i, j, cam.film->width);
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
				std::cerr << int(double(j) / cam.film->height * 100) << '%' << "]" << std::flush;
			}
			old_j = j;
			color pixel_color(0, 0, 0);
			for (int s = 0; s < sample_per_pixel; ++s) {
				auto u = (i + random_float()) / (cam.film->width - 1);
				auto v = (j + random_float()) / (cam.film->height - 1);
				ray r = cam.get_ray(u, 1 - v);
				color c;
				ray_color(r, background, world, lights, max_depth).ToRGB(c);
				pixel_color += c;
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
		for (int s = 0; s < sample_per_pixel; ++s) {
			auto u = (i + random_float()) / (cam.film->width - 1);
			auto v = (j + random_float()) / (cam.film->height - 1);
			ray r = cam.get_ray(u, 1 - v);
			color c;
			ray_color(r, background, world, lights, max_depth).ToRGB(c);
			pixel_color += c;
		}
		cam.film->write_color(i, j, pixel_color, sample_per_pixel);
		old_j = j;
	}

#endif
}

Spectrum SpectrumIntegrator::ray_color(const ray& r, const color& background, const hittable& world,
	shared_ptr<hittable> lights, int depth)
{
	surface_hit_record rec;

	if (depth <= 0)
		return Spectrum(0);

	if (!world.hit(r, 0.001, infinity, rec))
		return Spectrum::FromRGB(background);

	spectrum_scatter_record srec;
	Spectrum emitted = Spectrum::FromRGB(rec.mat_ptr->emitted(r, rec, rec.uv, rec.p));
	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;
	if (srec.is_specular) {
		return srec.sp_attenuation
			* ray_color(srec.specular_ray, background, world, lights, depth - 1);
	}

	shared_ptr<pdf> light_ptr = make_shared<hittable_pdf>(lights, rec.p);
	//mixture_pdf p(light_ptr, srec.pdf_ptr);

	mixture_pdf p(light_ptr, srec.pdf_ptr, 0.3);

	ray scattered = ray(rec.p, p.generate(), r.time());
	auto pdf_val = p.value(scattered.direction());

	return emitted + ray_color(scattered, background, world, lights, depth - 1) * srec.sp_attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) / pdf_val;
}
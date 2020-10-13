#include <memory>
#include <thread>
#include <Integrators/SamplerIntegrator.h>
#include <pdf/hittable_pdf.h>
#include <pdf/scatter_record.h>
#include <BRDF/Material.h>
#include <Geometry/hit_record.h>

#include "Tools/camera.h"
#include <Tools/Film.h>

void SamplerIntegrator::integrate(camera& cam, Scene& scene, Color background)
{
#ifndef _DEBUG
	//int MAX_THREAD = 3;
	int MAX_THREAD = std::thread::hardware_concurrency() - 2;
#else
	int MAX_THREAD = 1;
#endif

	auto worker = [MAX_THREAD, &scene, &cam, background, this](int arg)
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
			Color pixel_Color(0, 0, 0);

			for (int s = 0; s < sample_per_pixel; ++s) {
				auto a = _thread_sampler->Get2D();
				auto u = (i + a.x()) / (cam.film->width - 1);
				auto v = (j + a.y()) / (cam.film->height - 1);
				Ray r = cam.get_ray(u, 1.0f - v);
				pixel_Color += ray_color(r, background, scene, max_depth);
				_thread_sampler->StartNextSample();
			}
			cam.film->write_Color(i, j, pixel_Color, sample_per_pixel);
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
}

Color SamplerIntegrator::ray_color(const Ray& r, const Color& background, const Scene& world, int depth)
{
	SurfaceInteraction rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	if (!world.Intersect(r, &rec))
		return background;

	scatter_record srec;
	Color emitted = rec.mat_ptr->emitted(r, rec, rec.uv, rec.p);
	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;
	if (srec.is_specular) {
		return srec.attenuation * ray_color(srec.specular_ray, background, world, depth - 1);
	}

	//mixture_pdf p(light_ptr, srec.pdf_ptr);

	//mixture_pdf p(light_ptr, srec.pdf_ptr, 0.3);

	Ray scattered = Ray(rec.p, srec.pdf_ptr->generate(), Infinity, r.time);
	auto pdf_val = srec.pdf_ptr->value(scattered.d);

	return emitted + srec.attenuation
		* rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_color(scattered, background, world, depth - 1)
		/ pdf_val;
}
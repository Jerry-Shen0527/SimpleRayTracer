#pragma once
#include <Integrators/Integrator.h>

#include "Tools/Sampler/Sampler.h"

class SimpleIntegrator :public Integrator
{
public:
	SimpleIntegrator(Scene scene, hittable_list, int spp, int max_depth) :Integrator(spp, max_depth)
	{
		lights = make_shared<hittable_list>(scene);
		for (int i = 0; i < max_depth; ++i) {
			for (size_t j = 0; j < lights->pdf_objects.size(); ++j) {
				sampler->Request2DArray(nLightSamples[j]);
				sampler->Request2DArray(nLightSamples[j]);
			}
		}
	}
	void integrate(camera& cam, hittable_list& world, color background = color(0, 0, 0)) override;
	color ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable> lights,
		int depth);

	std::shared_ptr<Sampler> sampler;
	std::vector<int> nLightSamples;

	shared_ptr<hittable_list> lights;
};

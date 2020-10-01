#pragma once
#include <Integrators/Integrator.h>

#include "Tools/Sampler/Sampler.h"
#include "Tools/Sampler/StratifiedSampler.h"

class SamplerIntegrator :public Integrator
{
public:
	SamplerIntegrator(Scene scene, int spp, int max_depth) :Integrator(spp, max_depth)
	{
		auto a = static_cast<int>(sqrtf(spp));
		if (a * a < spp) a++;

		sampler = make_shared<StratifiedSampler>(a, a, true, 1);
		lights = make_shared<hittable_list>(scene);
	}
	void integrate(camera& cam, hittable_list& world, color background = color(0, 0, 0)) override;
	color ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable> lights,
		int depth);

	std::shared_ptr<Sampler> sampler;

	shared_ptr<hittable_list> lights;
};

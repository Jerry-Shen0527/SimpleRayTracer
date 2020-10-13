#pragma once
#include <memory>
#include <memory>
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

		sampler = std::make_shared<StratifiedSampler>(a, a, true, 1);
	}
	void integrate(camera& cam, Scene& scene, Color background = Color(0, 0, 0)) override;
	Color ray_color(const Ray& r, const Color& background, const Scene& world, int depth);

	std::shared_ptr<Sampler> sampler;
};

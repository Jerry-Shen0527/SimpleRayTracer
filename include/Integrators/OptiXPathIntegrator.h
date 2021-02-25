#pragma once
#include <Tools/camera.h>

#include <optix.h>

#include "Integrator.h"
#include "SamplerIntegrator.h"

class OptiXPathIntegrator :public Integrator
{
public:

	OptiXPathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
		std::shared_ptr<Sampler> sampler)
		: camera(camera), sampler(sampler), maxDepth(maxDepth)
	{
	}

	void Render(const Scene& scene, bool benchmark = false) override;
	shared_ptr<const Camera> camera;
	shared_ptr<Sampler> sampler;
	int maxDepth;
};

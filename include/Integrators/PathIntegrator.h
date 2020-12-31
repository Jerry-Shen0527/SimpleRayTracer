#pragma once
#include "SamplerIntegrator.h"

class PathIntegrator:public SamplerIntegrator
{
public:
	PathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
		std::shared_ptr<Sampler> sampler)
		: SamplerIntegrator(camera, sampler), maxDepth(maxDepth) { }

	Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena,
		int depth) const override;
private:
	int maxDepth;
};

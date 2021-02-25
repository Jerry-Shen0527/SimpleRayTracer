#pragma once
#include "SamplerIntegrator.h"

class StokesIntegrator:public SamplerIntegrator
{
public:
	Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena,
		int depth) const override;
};

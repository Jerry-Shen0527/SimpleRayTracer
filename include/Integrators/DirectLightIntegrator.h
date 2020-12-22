#pragma once
#include "SamplerIntegrator.h"
#include "BRDF/BxDF.h"

enum class LightStrategy { UniformSampleAll, UniformSampleOne };

class DirectLightingIntegrator : public SamplerIntegrator {
public:
	Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena,
		int depth) const override;

	Spectrum UniformSampleAllLights(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler,
		const std::vector<int>& nLightSamples, bool handleMedia);
	Spectrum UniformSampleOneLight(const Interaction& it,
		const Scene& scene, MemoryArena& arena, Sampler& sampler,
		bool handleMedia);

	Spectrum EstimateDirect(const Interaction& it, const Point2f& uScattering, const Light& light,
	                        const Point2f& uLight, const Scene& scene, Sampler& sampler, MemoryArena& arena,
	                        bool handleMedia, bool specular);
	void Preprocess(const Scene& scene, Sampler& sampler) override;

private:
	std::vector<int> nLightSamples; //Stores number of light samples for each light
	const LightStrategy strategy;
	const int maxDepth;
};

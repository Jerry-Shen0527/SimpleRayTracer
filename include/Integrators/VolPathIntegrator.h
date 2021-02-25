//#pragma once
//
//#include "SamplerIntegrator.h"
//
//class VolPathIntegrator :public SamplerIntegrator
//{
//public:
//	VolPathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
//		std::shared_ptr<Sampler> sampler,
//		const Bounds2i& pixelBounds, Float rrThreshold = 1,
//		const std::string& lightSampleStrategy = "spatial")
//		: SamplerIntegrator(camera, sampler, pixelBounds),
//		maxDepth(maxDepth),
//		rrThreshold(rrThreshold),
//		lightSampleStrategy(lightSampleStrategy) { }
//
//	Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena,
//		int depth) const override;
//
//private:
//	// VolPathIntegrator Private Data
//	const int maxDepth;
//	const Float rrThreshold;
//	const std::string lightSampleStrategy;
//	std::unique_ptr<LightDistribution> lightDistribution;
//};

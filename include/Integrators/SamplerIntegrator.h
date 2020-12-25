#pragma once
#include <Tools/camera.h>
#include <Tools/Sampler.h>

#include "Integrator.h"

class SamplerIntegrator : public Integrator {
public:
	SamplerIntegrator(std::shared_ptr<const Camera> camera,
		std::shared_ptr<Sampler> sampler,
		const Bounds2i& pixelBounds)
		: camera(camera), sampler(sampler), pixelBounds(pixelBounds) {}
	virtual void Preprocess(const Scene& scene, Sampler& sampler) { }
	void Render(const Scene& scene) override;

	virtual Spectrum Li(const RayDifferential& ray, const Scene& scene,
		Sampler& sampler, MemoryArena& arena,
		int depth = 0) const = 0;
protected:
private:
	std::shared_ptr<Sampler> sampler;
	std::shared_ptr<const Camera> camera;
	const Bounds2i pixelBounds;
};

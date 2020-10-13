#pragma once
#include "Integrator.h"

//TODO: replace the sampling strategy
class SpectrumIntegrator : public Integrator
{
public:
	SpectrumIntegrator(int spp, int max_depth) :Integrator(spp, max_depth) {}
	void integrate(camera& cam, Scene& scene, Color background) override;
	Spectrum ray_Color(const Ray& r, const Color& background, const Scene& world, int depth);
};

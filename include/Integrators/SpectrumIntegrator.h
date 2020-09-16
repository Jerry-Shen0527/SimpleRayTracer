#pragma once
#include <Integrators/Integrator.h>
#include <Tools/Spectrum/RGBSpectrum.h>
#include <Tools/Spectrum/SampledSpectrum.h>

#include "common.h"

class SpectrumIntegrator : public Integrator
{
public:
	SpectrumIntegrator(int spp, int max_depth) :Integrator(spp, max_depth) {}
	void integrate(camera& cam, Film& film, hittable_list& world, color background) override;
	Spectrum ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable> lights,
		int depth);
	
};

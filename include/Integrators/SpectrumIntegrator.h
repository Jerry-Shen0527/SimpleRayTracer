#pragma once
#include <Integrators/Integrator.h>
#include <Tools/Spectrum/RGBSpectrum.h>
#include <Tools/Spectrum/SampledSpectrum.h>
class SpectrumIntegrator : public Integrator
{
public:
	void integrate(camera& cam, Film& film, hittable_list& world, color background) override;

	
};

#pragma once
#include <Integrators/Integrator.h>
class SpectrumIntegrator : public Integrator
{
public:
	void integrate(camera& cam, Film& film, hittable_list& world, color background) override;
};

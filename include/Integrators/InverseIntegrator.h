#pragma once
#include "Integrator.h"
class InverseIntegrator : public Integrator
{
public:
	void integrate(camera& cam, Film& film, hittable_list& world, color background) override;
};

#pragma once
#include <Integrators/Integrator.h>

class SimpleIntegrator :public Integrator
{
public:
	SimpleIntegrator(int spp, int max_depth) :Integrator(spp, max_depth) {}
	void integrate(camera& cam, Film& film, hittable_list& world, color background = color(0, 0, 0)) override;
	color ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable> lights,
		int depth);
};

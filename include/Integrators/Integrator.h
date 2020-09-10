#pragma once

#include <Integrators/Film.h>

#include <Geometry/hittable_list.h>

class camera;

class Integrator
{
public:
	Integrator(int spp, int max_depth) :sample_per_pixel(spp), max_depth(max_depth) {}

	virtual void integrate(camera& cam, Film& film, hittable_list& world, color background) = 0;

	int sample_per_pixel;
	int max_depth;
};

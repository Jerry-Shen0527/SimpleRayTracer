#pragma once
#include <Tools/Film.h>
#include <Geometry/hittable_list.h>

#include "Tools/camera.h"

class Integrator
{
public:
	Integrator(int spp, int max_depth) :sample_per_pixel(spp), max_depth(max_depth) {}

	virtual void integrate(camera& cam, Film& film, hittable_list& world, color background) = 0;

	virtual  color ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable> lights, int depth) = 0;

	int sample_per_pixel;
	int max_depth;
};

#pragma once

#include <Integrators/Film.h>
#include <Geometry/Geometry.h>

class camera;

class Integrator
{
public:
	Integrator(int spp, int max_depth) :sample_per_pixel(spp), max_depth(max_depth) {}

	virtual void integrate(camera& cam, Film& film, hittable_list& world, color background) = 0;

	virtual  color ray_color(const ray& r, const color& background, const hittable& world, std::shared_ptr<hittable> lights, int depth) = 0;

	int sample_per_pixel;
	int max_depth;
};

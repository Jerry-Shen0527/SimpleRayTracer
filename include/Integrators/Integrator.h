#pragma once

#include <Geometry/hittable_list.h>
#include <Scene/Scene.h>

class camera;

class Integrator
{
public:
	Integrator(int spp, int max_depth) :sample_per_pixel(spp), max_depth(max_depth) {}

	virtual void integrate(camera& cam, Scene& scene, Color background) = 0;

	int sample_per_pixel;
	int max_depth;
};

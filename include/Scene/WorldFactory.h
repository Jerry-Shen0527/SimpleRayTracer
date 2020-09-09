#pragma once

#pragma once
#include <common.h>
#include <Tools/texture.h>
#include <BRDF/dielectric.h>
#include <BRDF/diffuse_light.h>
#include <BRDF/lambertian.h>
#include <BRDF/metal.h>
#include <Geometry/aarect.h>
#include <Geometry/box.h>
#include <Geometry/constant_medium.h>
#include <Geometry/MovingSphere.h>
#include <Geometry/sphere.h>
#include <Geometry/translation.h>
#include <Tools/camera.h>

class WorldFactory
{
public:
	WorldFactory() = default;

	hittable_list random_scene();
	hittable_list two_spheres();
	hittable_list two_perlin_spheres();
	hittable_list earth();
	hittable_list simple_light();
	hittable_list cornell_box();
	hittable_list cornell_smoke();
	hittable_list final_scene();
	void get_world(int idx, double aspect_ratio, hittable_list& world, camera& cam, color& background);

private:
};

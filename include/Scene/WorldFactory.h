#pragma once

#include <Worlds.h>
#include <Tools/camera.h>

class WorldFactory
{
public:
	WorldFactory() = default;

	void get_world(int idx, double aspect_ratio, hittable_list& world, camera& cam, color& background);
};

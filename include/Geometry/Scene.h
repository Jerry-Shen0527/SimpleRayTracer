#pragma once
#include <memory>
#include <vector>

#include "Tools/Light/Light.h"

class Scene
{
public:

	Scene(std::shared_ptr<Primitive> aggregate, const std::vector<std::shared_ptr<Light>>& lights) : lights(lights), aggregate(aggregate)
	{
		Scene Constructor Implementation 24
	}

	std::vector<std::shared_ptr<Light>> lights;
	std::shared_ptr<Primitive> aggregate;
};

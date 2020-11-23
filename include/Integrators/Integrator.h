#pragma once

#include <Scene/Scene.h>

class Integrator
{
public:
	virtual void Render(const Scene& scene) = 0;
};

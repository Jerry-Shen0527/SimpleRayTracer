#pragma once
#include "SimpleIntegrator.h"

enum class LightStrategy { UniformSampleAll, UniformSampleOne };


class DirectLightingIntegrator : public SamplerIntegrator {
public:
	
private:

	const LightStrategy strategy;
	const int maxDepth;
};

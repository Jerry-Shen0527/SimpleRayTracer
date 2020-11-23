#pragma once
#include "Integrator.h"

class SamplerIntegrator : public Integrator {
public:
protected:
private:
	std::shared_ptr<Sampler> sampler;
	std::shared_ptr<const Camera> camera;
};

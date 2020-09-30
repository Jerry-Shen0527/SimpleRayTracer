#pragma once

#include <common.h>
#include <ray.h>
#include <Tools/Sampler/Sampler.h>

class ray;

// Medium Declarations
class Medium {
public:
	// Medium Interface
	virtual ~Medium() {}
	virtual Spectrum Tr(const ray& ray) const = 0;
	virtual Spectrum Tr(const ray& ray, Sampler& sampler) const = 0;

	virtual Spectrum Sample(const ray& ray) const = 0;
};

// MediumInterface Declarations
struct MediumInterface {
	MediumInterface() : inside(nullptr), outside(nullptr) {}
	// MediumInterface Public Methods
	MediumInterface(const Medium* medium) : inside(medium), outside(medium) {}
	MediumInterface(const Medium* inside, const Medium* outside)
		: inside(inside), outside(outside) {}
	bool IsMediumTransition() const { return inside != outside; }

	const Medium* inside, * outside;
};
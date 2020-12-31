#pragma once

#include <config.h>
#include <Geometry/Vector3.h>
class Sampler;

class PhaseFunction
{
public:
	virtual Float p(const Vector3f& wo, const Vector3f& wi) const = 0;
};

inline Float Inv4Pi = 1 / Pi / 4.0;

inline Float PhaseHG(Float cosTheta, Float g) {
	Float denom = 1 + g * g + 2 * g * cosTheta;
	return Inv4Pi * (1 - g * g) / (denom * std::sqrt(denom));
}

class HenyeyGreenstein : public PhaseFunction {
public:
	HenyeyGreenstein(Float g) : g(g) { }
	Float p(const Vector3f& wo, const Vector3f& wi) const {
		return PhaseHG(Dot(wo, wi), g);
	}
private:
	const Float g;
};

// Medium Declarations
class Medium {
public:
	// Medium Interface
	virtual ~Medium() {}
	virtual Spectrum Tr(const Ray& ray) const = 0;
	virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const = 0;

	virtual Spectrum Sample(const Ray& ray) const = 0;
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
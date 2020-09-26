#pragma once
#include <BRDF/BxDF_Utility.h>

class PhaseFunction
{
public:
	virtual Float p(const Vector3f& wo, const Vector3f& wi) const = 0;
};

Float Inv4Pi = 1 / pi / 4.0;

inline Float PhaseHG(Float cosTheta, Float g) {
	Float denom = 1 + g * g + 2 * g * cosTheta;
	return Inv4Pi * (1 - g * g) / (denom * std::sqrt(denom));
}

class HenyeyGreenstein : public PhaseFunction {
public:
	HenyeyGreenstein(Float g) : g(g) { }
	Float p(const Vector3f& wo, const Vector3f& wi) const {
		return PhaseHG(dot(wo, wi), g);
	}
private:
	const Float g;
};

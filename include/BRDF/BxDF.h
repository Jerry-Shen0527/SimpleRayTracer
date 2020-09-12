#pragma once
#include "common.h"
#include <Tools/Math/vec3.h>

enum BxDFType {
	BSDF_REFLECTION = 1 << 0,
	BSDF_TRANSMISSION = 1 << 1,
	BSDF_DIFFUSE = 1 << 2,
	BSDF_GLOSSY = 1 << 3,
	BSDF_SPECULAR = 1 << 4,
	BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
};

class BxDF
{
public:
	BxDF(BxDFType type) : type(type) { }
	virtual Spectrum f(const vec3& wo, const vec3& wi) const = 0;

	virtual Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType = nullptr) const = 0;
	virtual Spectrum rho(const vec3& wo, int nSamples, const point2* samples) const = 0;
	virtual Spectrum rho(int nSamples, const point2* samples1, const point2* samples2) const = 0;

	const BxDFType type;

	bool MatchesFlags(BxDFType t) const;
};

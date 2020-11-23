#pragma once
#include "BxDF.h"


class LambertianReflection : public BxDF {
public:
	//LambertianReflection Public Methods 532
	LambertianReflection(const Spectrum& R)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) { }

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
	Spectrum Sample_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, float& pdf, BxDFType* sampledType) const override;
	Spectrum rho(const Vector3f& wo, int nSamples, const Point2f* samples) const override;
	Spectrum rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const override;
private:
	//LambertianReflection Private Data 532
	const Spectrum R;
};

inline Spectrum LambertianReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	return R * InvPi;
}

inline Spectrum LambertianReflection::rho(const Vector3f& wo, int nSamples, const Point2f* samples) const
{
	return R;
}

inline Spectrum LambertianReflection::rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const
{
	return  R;
}

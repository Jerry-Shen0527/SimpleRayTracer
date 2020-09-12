#pragma once
#include "BxDF.h"
#include "Fresnel/Fresnel.h"
class SpecularReflection : public BxDF {
public:
	SpecularReflection(const Spectrum& R, Fresnel* fresnel)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), R(R),
		fresnel(fresnel) { }

	Spectrum f(const vec3& wo, const vec3& wi) const override;
	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType) const override;
	Spectrum rho(const vec3& wo, int nSamples, const point2* samples) const override;
	Spectrum rho(int nSamples, const point2* samples1, const point2* samples2) const override;
private:
	const Spectrum R;
	const Fresnel* fresnel;
};

inline Spectrum SpecularReflection::Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf,
	BxDFType* sampledType) const
{
	wi = Vector3f(-wo.x(), -wo.y(), wo.z());
	pdf = 1;
	return fresnel->Evaluate(CosTheta(wi)) * R / AbsCosTheta(wi);
}



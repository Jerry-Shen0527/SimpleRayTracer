#pragma once
#include "BxDF.h"
#include "Fresnel/Fresnel.h"
class SpecularReflection : public BxDF {
public:
	SpecularReflection(const Spectrum& R, Fresnel* fresnel)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), R(R),
		fresnel(fresnel) { }
	Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
		return Spectrum(0.f);
	}

	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType) const override;
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

class SpecularTransmission : public BxDF {
public:
	SpecularTransmission(const Spectrum& T, Float etaA, Float etaB,
		TransportMode mode)
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), T(T), etaA(etaA),
		etaB(etaB), fresnel(etaA, etaB), mode(mode) {
	}

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
		return Spectrum(0.f);
	}

	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType) const override;
private:
	const Spectrum T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};

inline Spectrum SpecularTransmission::Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf,
	BxDFType* sampledType) const
{
	//Figure out which ¦Ç is incidentand which is transmitted 529
	bool entering = CosTheta(wo) > 0;
	Float etaI = entering ? etaA : etaB;
	Float etaT = entering ? etaB : etaA;
	//Compute ray direction for specular transmission 529
	pdf = 1;
	Spectrum ft = T * (Spectrum(1.) - fresnel.Evaluate(CosTheta(wi)));
	//Account for non - symmetry with transmission to different medium 961
	return ft / AbsCosTheta(wi);
}

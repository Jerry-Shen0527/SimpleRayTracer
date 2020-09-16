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

	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, double& pdf, BxDFType* sampledType) const override;
private:
	const Spectrum R;
	const Fresnel* fresnel;
};

inline Spectrum SpecularReflection::Sample_f(const vec3& wo, vec3& wi, const point2& sample, double& pdf,
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

	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, double& pdf, BxDFType* sampledType) const override;
private:
	const Spectrum T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};

inline bool Refract(const Vector3f& wi, const normal3& n, Float eta,
	Vector3f& wt) {
	//Compute cos ¦Èt using Snell¡¯s law 531

	Float cosThetaI = dot(n, wi);
	Float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
	Float sin2ThetaT = eta * eta * sin2ThetaI;

	//Handle total internal reflection for transmission 531
	if (sin2ThetaT >= 1) return false;

	Float cosThetaT = std::sqrt(1 - sin2ThetaT);
	wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
	return true;
}

inline Spectrum SpecularTransmission::Sample_f(const vec3& wo, vec3& wi, const point2& sample, double& pdf,
	BxDFType* sampledType) const
{
	//Figure out which ¦Ç is incidentand which is transmitted 529
	bool entering = CosTheta(wo) > 0;
	Float etaI = entering ? etaA : etaB;
	Float etaT = entering ? etaB : etaA;
	//Compute ray direction for specular transmission 529
	if (!Refract(wo, Faceforward(normal3(0, 0, 1), wo), etaI / etaT, wi))
		return 0;
	pdf = 1;
	Spectrum ft = T * (Spectrum(1.) - fresnel.Evaluate(CosTheta(wi)));
	//Account for non - symmetry with transmission to different medium 961
	return ft / AbsCosTheta(wi);
}

class FresnelSpecular : public BxDF {
public:
	//FresnelSpecular Public Methods 532
	FresnelSpecular(const Spectrum& R, const Spectrum& T, Float etaA,
		Float etaB, TransportMode mode)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
		R(R), T(T), etaA(etaA), etaB(etaB), fresnel(etaA, etaB),
		mode(mode) { }

	Spectrum f(const vec3& wo, const vec3& wi) const override;
	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, double& pdf, BxDFType* sampledType) const override;
private:
	const Spectrum R, T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
	//FresnelSpecular Private Data 532
};

inline Spectrum FresnelSpecular::f(const vec3& wo, const vec3& wi) const
{
	return Spectrum(0.f);
}

inline Spectrum FresnelSpecular::Sample_f(const vec3& wo, vec3& wi, const point2& sample, double& pdf,
	BxDFType* sampledType) const
{
	Float F = FrDielectric(CosTheta(wo), etaA, etaB);
	if (sample[0] < F) {
		//Compute specular reflection for FresnelSpecular 817
		wi = Vector3f(-wo.x(), -wo.y(), wo.z());
		if (sampledType)
			*sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
		pdf = F;
		return F * R / AbsCosTheta(wi);
	}
	else {
		//Compute specular transmission for FresnelSpecular 817
		//Figure out which ¦Ç is incidentand which is transmitted 529
		bool entering = CosTheta(wo) > 0;
		Float etaI = entering ? etaA : etaB;
		Float etaT = entering ? etaB : etaA;
		//Compute ray direction for specular transmission 529

		Spectrum ft = T * (1 - F);
		//Account for non - symmetry with transmission to different medium 961
		if (sampledType)
			*sampledType = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
		pdf = 1 - F;
		return ft / AbsCosTheta(wi);
	}
}

#pragma once
#include "BxDF.h"
#include "Geometry/AnimatedTransform.h"

class SpecularReflection : public BxDF<Spectrum> {
public:
	SpecularReflection(const Spectrum& R, Fresnel* fresnel)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), R(R),
		fresnel(fresnel) { }
	Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
		return Spectrum(0.f);
	}

	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType = nullptr) const override;
private:
	const Spectrum R;
	const Fresnel* fresnel;
};

class SpecularTransmission : public BxDF<Spectrum> {
public:
	SpecularTransmission(const Spectrum& T, Float etaA, Float etaB,
		TransportMode mode)
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), T(T), etaA(etaA),
		etaB(etaB), fresnel(etaA, etaB), mode(mode) {
	}

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
		return Spectrum(0.f);
	}

	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType = nullptr) const override;
private:
	const Spectrum T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};

class FresnelSpecular : public BxDF<Spectrum> {
public:
	//FresnelSpecular Public Methods 532
	FresnelSpecular(const Spectrum& R, const Spectrum& T, Float etaA,
		Float etaB, TransportMode mode)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
		R(R), T(T), etaA(etaA), etaB(etaB), fresnel(etaA, etaB),
		mode(mode) { }

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
	Float Pdf(const Vector3f& wo, const Vector3f& wi) const;

	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType = nullptr) const override;
private:
	const Spectrum R, T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
	//FresnelSpecular Private Data 532
};

bool Refract(const Vector3f& wi, const Normal3f& n, Float eta, Vector3f* wt);
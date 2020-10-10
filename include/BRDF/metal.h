#pragma once
#include "BxDF.h"
#include "Material.h"
#include "MicrofacetDistribution.h"
#include "Fresnel/Fresnel.h"

Vector3f reflect(const Vector3f& v, const Vector3f& n);

class metal : public Material {
public:
	metal(const Color& a) : albedo(a) {}

	metal(const Color& a, float f);

	virtual bool scatter(
		const Ray& r_in, const surface_hit_record& rec, scatter_record& srec
	) const override;

public:
	Color albedo;
	float fuzz;
};

class MicrofacetReflection : public BxDF {
public:
	MicrofacetReflection(const Spectrum& R,
		shared_ptr<MicrofacetDistribution> distribution, shared_ptr<Fresnel> fresnel)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), R(R),
		distribution(distribution), fresnel(fresnel) { }

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
private:
	const Spectrum R;
	shared_ptr<MicrofacetDistribution> distribution;
	shared_ptr<Fresnel> fresnel;
};

class MicrofacetTransmission : public BxDF {
public:
	MicrofacetTransmission(const Spectrum& T,
		shared_ptr<MicrofacetDistribution> distribution, Float etaA, Float etaB,
		TransportMode mode)
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
		T(T), distribution(distribution), etaA(etaA), etaB(etaB),
		fresnel(etaA, etaB), mode(mode) { }

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
private:
	const Spectrum T;
	shared_ptr<MicrofacetDistribution> distribution;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};

class FresnelBlend : public BxDF {
public:
	FresnelBlend::FresnelBlend(const Spectrum& Rd, const Spectrum& Rs,
		shared_ptr<MicrofacetDistribution> distribution)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
		Rd(Rd), Rs(Rs), distribution(distribution) { }

	Spectrum SchlickFresnel(Float cosTheta) const {
		auto pow5 = [](Float v) { return (v * v) * (v * v) * v; };
		return Rs + pow5(1 - cosTheta) * (Spectrum(1.) - Rs);
	}

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
		auto pow5 = [](Float v) { return (v * v) * (v * v) * v; };
		Spectrum diffuse = (28.f / (23.f * pi)) * Rd *
			(Spectrum(1.f) - Rs) *
			(1 - pow5(1 - .5f * AbsCosTheta(wi))) *
			(1 - pow5(1 - .5f * AbsCosTheta(wo)));
		Vector3f wh = wi + wo;
		if (wh.x() == 0 && wh.y() == 0 && wh.z() == 0) return Spectrum(0);
		wh = wh.normalize();
		Spectrum specular = distribution->D(wh) /
			(4 * abs(Dot(wi, wh)) *
				std::max(AbsCosTheta(wi), AbsCosTheta(wo))) *
			SchlickFresnel(Dot(wi, wh));
		return diffuse + specular;
	}
private:
	const Spectrum Rd, Rs;
	shared_ptr<MicrofacetDistribution> distribution;
};
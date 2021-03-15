#pragma once
#include "BxDF.h"
#include "BxDF_Utility.h"
#include "Fresnel.h"

class MicrofacetDistribution
{
public:
	MicrofacetDistribution(bool sampleVisibleArea)
		: sampleVisibleArea(sampleVisibleArea) {}
	virtual Float D(const Vector3f& wh) const = 0;
	virtual Float Lambda(const Vector3f& w) const = 0;

	Float G(const Vector3f& wo, const Vector3f& wi) const {
		return 1 / (1 + Lambda(wo) + Lambda(wi));
	}

	Float G1(const Vector3f& w) const {
		return 1 / (1 + Lambda(w));
	}
	Float Pdf(const Vector3f& wo, const Vector3f& wh) const;

	virtual Vector3f Sample_wh(const Vector3f& wo, const Point2f& u) const = 0;

protected:
	const bool sampleVisibleArea;
};

class BeckmannDistribution : public MicrofacetDistribution {
public:

	Float D(const Vector3f& wh) const override;

	Float Lambda(const Vector3f& w) const;

	Vector3f Sample_wh(const Vector3f& wo, const Point2f& u) const override;

	Float Pdf(const Vector3f& wo,
		const Vector3f& wh) const {
		if (sampleVisibleArea)
			return D(wh) * G1(wo) * AbsDot(wo, wh) / AbsCosTheta(wo);
		else
			return D(wh) * AbsCosTheta(wh);
	}
private:
	const Float alphax, alphay;
};

inline Float BeckmannDistribution::D(const Vector3f& wh) const
{
	Float tan2Theta = Tan2Theta(wh);
	if (std::isinf(tan2Theta)) return 0.;
	Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
	return std::exp(-tan2Theta * (Cos2Phi(wh) / (alphax * alphax) +
		Sin2Phi(wh) / (alphay * alphay))) /
		(Pi * alphax * alphay * cos4Theta);
}

inline Float BeckmannDistribution::Lambda(const Vector3f& w) const
{
	Float absTanTheta = std::abs(TanTheta(w));
	if (std::isinf(absTanTheta)) return 0.;
	Float alpha = std::sqrt(Cos2Phi(w) * alphax * alphax +
		Sin2Phi(w) * alphay * alphay);
	Float a = 1 / (alpha * absTanTheta);
	if (a >= 1.6f)
		return 0;
	return (1 - 1.259f * a + 0.396f * a * a) /
		(3.535f * a + 2.181f * a * a);
}

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
public:

	static Float RoughnessToAlpha(Float roughness);

	TrowbridgeReitzDistribution(Float alphax, Float alphay,
		bool samplevis = true)
		: MicrofacetDistribution(samplevis),
		alphax(std::max(Float(0.001), alphax)),
		alphay(std::max(Float(0.001), alphay)) {}

	Float D(const Vector3f& wh) const;

	Float Lambda(const Vector3f& w) const {
		Float absTanTheta = std::abs(TanTheta(w));
		if (std::isinf(absTanTheta)) return 0.;
		Float alpha = std::sqrt(Cos2Phi(w) * alphax * alphax +
			Sin2Phi(w) * alphay * alphay);
		Float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
		return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
	}

	Vector3f Sample_wh(const Vector3f& wo, const Point2f& u) const override;
private:
	const Float alphax, alphay;
};

inline Float TrowbridgeReitzDistribution::D(const Vector3f& wh) const
{
	Float tan2Theta = Tan2Theta(wh);
	if (std::isinf(tan2Theta)) return 0.;
	const Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
	Float e = (Cos2Phi(wh) / (alphax * alphax) +
		Sin2Phi(wh) / (alphay * alphay)) * tan2Theta;
	return 1 / (Pi * alphax * alphay * cos4Theta * (1 + e) * (1 + e));
}

class MicrofacetReflection : public BxDF<Spectrum> {
public:
	// MicrofacetReflection Public Methods
	MicrofacetReflection(const Spectrum& R,
		MicrofacetDistribution* distribution, Fresnel* fresnel, TransportMode mode = TransportMode::Radiance)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
		R(R),
		distribution(distribution),
		fresnel(fresnel), mode(mode) {}
	Spectrum f(const Vector3f& wo, const Vector3f& wi) const;
	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
		Float* pdf, BxDFType* sampledType) const;
	Float Pdf(const Vector3f& wo, const Vector3f& wi) const override;

private:
	// MicrofacetReflection Private Data
	const Spectrum R;
	const MicrofacetDistribution* distribution;
	const Fresnel* fresnel;
	const TransportMode mode;
};

class MicrofacetTransmission : public BxDF<Spectrum> {
public:
	// MicrofacetTransmission Public Methods
	MicrofacetTransmission(const Spectrum& T,
		MicrofacetDistribution* distribution, Float etaA,
		Float etaB, TransportMode mode)
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
		T(T),
		distribution(distribution),
		etaA(etaA),
		etaB(etaB),
		fresnel(etaA, etaB),
		mode(mode) {}
	Spectrum f(const Vector3f& wo, const Vector3f& wi) const;
	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
		Float* pdf, BxDFType* sampledType) const;
	Float Pdf(const Vector3f& wo, const Vector3f& wi) const;

private:
	// MicrofacetTransmission Private Data
	const Spectrum T;
	const MicrofacetDistribution* distribution;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};
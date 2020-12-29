#pragma once
#include "BxDF.h"
#include "BxDF_Utility.h"

class MicrofacetDistribution;

inline Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {
	cosThetaI = Clamp(cosThetaI, -1, 1);

	bool entering = cosThetaI > 0.f;
	if (!entering) {
		std::swap(etaI, etaT);
		cosThetaI = std::abs(cosThetaI);
	}

	Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
	Float sinThetaT = etaI / etaT * sinThetaI;

	Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));

	if (sinThetaT >= 1)
		return 1;

	Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
		((etaT * cosThetaI) + (etaI * cosThetaT));
	Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
		((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl * Rparl + Rperp * Rperp) / 2;
}

inline Vector3f Reflect(const Vector3f& wo, const Vector3f& n) {
	return -wo + 2 * Dot(wo, n) * n;
}

inline Spectrum FrConductor(Float cosThetaI, const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k)
{
	cosThetaI = Clamp(cosThetaI, -1, 1);
	Spectrum eta = etaT / etaI;
	Spectrum etak = k / etaI;

	Float cosThetaI2 = cosThetaI * cosThetaI;
	Float sinThetaI2 = 1. - cosThetaI2;
	Spectrum eta2 = eta * eta;
	Spectrum etak2 = etak * etak;

	Spectrum t0 = eta2 - etak2 - sinThetaI2;
	Spectrum a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
	Spectrum t1 = a2plusb2 + cosThetaI2;
	Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
	Spectrum t2 = (Float)2 * cosThetaI * a;
	Spectrum Rs = (t1 - t2) / (t1 + t2);

	Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	Spectrum t4 = t2 * sinThetaI2;
	Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5 * (Rp + Rs);
}

class Fresnel
{
public:
	virtual Spectrum Evaluate(Float cosI) const = 0;
};

class FresnelConductor : public Fresnel {
public:
	FresnelConductor(const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k) : etaI(etaI), etaT(etaT), k(k) { }

	Spectrum FresnelConductor::Evaluate(Float cosThetaI) const {
		return FrConductor(std::abs(cosThetaI), etaI, etaT, k);
	}
private:
	Spectrum etaI, etaT, k;
};

class FresnelDielectric : public Fresnel {
public:

	FresnelDielectric(Float etaI, Float etaT) : etaI(etaI), etaT(etaT) { }

	Spectrum FresnelDielectric::Evaluate(Float cosThetaI) const {
		return FrDielectric(cosThetaI, etaI, etaT);
	}

private:
	Float etaI, etaT;
};

class FresnelNoOp : public Fresnel {
public:
	Spectrum Evaluate(Float) const { return Spectrum(1.); }
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

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const;
private:
	const Spectrum Rd, Rs;
	shared_ptr<MicrofacetDistribution> distribution;
};

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

class MicrofacetReflection : public BxDF {
public:
	// MicrofacetReflection Public Methods
	MicrofacetReflection(const Spectrum& R,
		MicrofacetDistribution* distribution, Fresnel* fresnel)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
		R(R),
		distribution(distribution),
		fresnel(fresnel) {}
	Spectrum f(const Vector3f& wo, const Vector3f& wi) const;
	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
		Float* pdf, BxDFType* sampledType) const;
	Float Pdf(const Vector3f& wo, const Vector3f& wi) const;

private:
	// MicrofacetReflection Private Data
	const Spectrum R;
	const MicrofacetDistribution* distribution;
	const Fresnel* fresnel;
};

class MicrofacetTransmission : public BxDF {
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
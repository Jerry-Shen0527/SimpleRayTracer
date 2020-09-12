#pragma once
#include "common.h"
#include "BRDF/BxDF_Utility.h"
#include "Tools/Spectrum/SampledSpectrum.h"

inline Spectrum FrConductor(Float cosThetaI, const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k)
{
	cosThetaI = clamp(cosThetaI, -1, 1);
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
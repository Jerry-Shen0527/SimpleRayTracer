#pragma once

#include "BxDF.h"
#include "BxDF_Utility.h"
#include <complex>
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

std::tuple<std::complex<Float>, std::complex<Float>, Float, Float, Float>
inline fresnel_polarized(Float cos_theta_i, Float eta) {
	auto outside_mask = cos_theta_i >= 0.f;

	Float rcp_eta = 1 / eta;
	Float eta_it = outside_mask ? eta : rcp_eta;
	Float eta_ti = outside_mask ? rcp_eta : eta;

	/* Using Snell's law, calculate the squared sine of the
	   angle between the surface normal and the transmitted ray */
	Float cos_theta_t_sqr = fma(-fma(-cos_theta_i, cos_theta_i, 1.f), eta_ti * eta_ti, 1.f);

	/* Find the cosines of the incident/transmitted rays */
	Float cos_theta_i_abs = abs(cos_theta_i);
	std::complex<Float> cos_theta_t = sqrt(cos_theta_t_sqr);

	/* Choose the appropriate sign of the root (important when computing the
	   phase difference under total internal reflection, see appendix A.2 of
	   "Stellar Polarimetry" by David Clarke) */
	cos_theta_t *= cos_theta_t_sqr > 0 ? 1. : -1.;

	/* Amplitudes of reflected waves. The sign convention of 'a_p' used here
	   matches Fresnel's original paper from 1823 and is different from some
	   contemporary references. See appendix A.1 of "Stellar Polarimetry" by
	   David Clarke for a historical perspective. */
	std::complex<Float> a_s = (-eta_it * cos_theta_t + cos_theta_i_abs) /
		(eta_it * cos_theta_t + cos_theta_i_abs);
	std::complex<Float> a_p = (-eta_it * cos_theta_i_abs + cos_theta_t) /
		(eta_it * cos_theta_i_abs + cos_theta_t);

	auto index_matched = eta == 1.f;
	auto invalid = eta == 0.f;
	if (index_matched || invalid)a_s = 0.f;
	if (index_matched || invalid)a_p = 0.f;

	/* Adjust the sign of the transmitted direction */
	Float cos_theta_t_signed = cos_theta_t_sqr >= 0.f ? (real(cos_theta_t) * cos_theta_i < 0 ? 1 : -1) : 0.f;

	return { a_s, a_p, cos_theta_t_signed, eta_it, eta_ti };
}

std::tuple<std::complex<Float>, std::complex<Float>, Float, std::complex<Float>, std::complex<Float>>
inline fresnel_polarized(Float cos_theta_i, std::complex<Float> eta) {
	auto outside_mask = cos_theta_i >= 0.f;

	auto z2 = abs(eta) * abs(eta);

	std::complex<Float> rcp_eta(eta.real() / z2, -eta.imag() / z2);
	std::complex<Float>	eta_it = outside_mask ? eta : rcp_eta;
	std::complex<Float>	eta_ti = outside_mask ? rcp_eta : eta;

	/* Using Snell's law, calculate the squared sine of the
	   angle between the surface normal and the transmitted ray */
	std::complex<Float> cos_theta_t_sqr =
		1.f - (eta_ti * eta_ti) * (-cos_theta_i * cos_theta_i + 1.f);

	/* Find the cosines of the incident/transmitted rays */
	Float cos_theta_i_abs = abs(cos_theta_i);
	std::complex<Float> cos_theta_t = sqrt(cos_theta_t_sqr);

	/* Choose the appropriate sign of the root (important when computing the
	   phase difference under total internal reflection, see appendix A.2 of
	   "Stellar Polarimetry" by David Clarke) */
	cos_theta_t *= real(cos_theta_t_sqr) >= 0 ? 1 : -1;

	/* Amplitudes of reflected waves. The sign convention of 'a_p' used here
	   matches Fresnel's original paper from 1823 and is different from some
	   contemporary references. See appendix A.1 of "Stellar Polarimetry" by
	   David Clarke for a historical perspective. */
	std::complex<Float> a_s = (-eta_it * cos_theta_t + cos_theta_i_abs) / (eta_it * cos_theta_t + cos_theta_i_abs);
	std::complex<Float> a_p = (-eta_it * cos_theta_i_abs + cos_theta_t) / (eta_it * cos_theta_i_abs + cos_theta_t);

	auto index_matched = (abs(eta) * abs(eta) == 1.f) && (imag(eta) == 0.f);
	auto invalid = abs(eta) * abs(eta) == 0.f;
	if (index_matched || invalid)a_s = 0.f;
	if (index_matched || invalid)a_p = 0.f;

	/* Adjust the sign of the transmitted direction */
	Float cos_theta_t_signed = real(cos_theta_t_sqr) >= 0.f ? (real(cos_theta_t) * cos_theta_i < 0 ? 1 : -1) : 0.f;

	return { a_s, a_p, cos_theta_t_signed, eta_it, eta_ti };
}
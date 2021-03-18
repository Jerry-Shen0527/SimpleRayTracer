#pragma once
#include <Tools/Math/matrix.h>
#include <Tools/Spectrum/SampledSpectrum.h>

#include "BxDF_Utility.h"

enum BxDFType {
	BSDF_REFLECTION = 1 << 0,
	BSDF_TRANSMISSION = 1 << 1,
	BSDF_DIFFUSE = 1 << 2,
	BSDF_GLOSSY = 1 << 3,
	BSDF_SPECULAR = 1 << 4,
	BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
};

template<typename Spectrum>
class BxDF
{
public:
	BxDF(BxDFType type) : type(type) { }
	virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const = 0;

	virtual Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType = nullptr) const;

	virtual float Pdf(const Vector3f& wo, const Vector3f& wi) const;
	virtual Spectrum rho(const Vector3f& wo, int nSamples, const Point2f* samples) const;
	virtual Spectrum rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const;

	const BxDFType type;

	bool MatchesFlags(BxDFType t) const;
	bool HasFlags(BxDFType t) const;
};

template<typename Spectrum>
class LambertianReflection : public BxDF<Spectrum> {
public:
	//LambertianReflection Public Methods 532
	LambertianReflection(const Spectrum& R)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) { }

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
	Spectrum rho(const Vector3f& wo, int nSamples, const Point2f* samples) const override;
	Spectrum rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const override;
private:
	//LambertianReflection Private Data 532
	const Spectrum R;
};

template<typename Spectrum>
inline Spectrum LambertianReflection<Spectrum>::f(const Vector3f& wo, const Vector3f& wi) const
{
	return R;
}

template<typename Spectrum>
inline Spectrum LambertianReflection<Spectrum>::rho(const Vector3f& wo, int nSamples, const Point2f* samples) const
{
	return R;
}

template<typename Spectrum>
inline Spectrum LambertianReflection<Spectrum>::rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const
{
	return  R;
}

template<typename Spectrum>
class OrenNayar :public BxDF<Spectrum>
{
public:
	OrenNayar(const Spectrum& R, Float sigma)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {
		sigma = Radians(sigma);
		Float sigma2 = sigma * sigma;
		A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
		B = 0.45f * sigma2 / (sigma2 + 0.09f);
	}

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;

private:
	const Spectrum R;
	Float A, B;
};

template<typename Spectrum>
inline Spectrum OrenNayar<Spectrum>::f(const Vector3f& wo, const Vector3f& wi) const
{
	if constexpr (is_polarized_t<Spectrum>())
	{
	}
	else {
		Float sinThetaI = SinTheta(wi);
		Float sinThetaO = SinTheta(wo);
		//Compute cosine term of Oren¨CNayar model	536	
		Float maxCos = 0;
		if (sinThetaI > 1e-4 && sinThetaO > 1e-4) {
			Float sinPhiI = SinPhi(wi), cosPhiI = CosPhi(wi);
			Float sinPhiO = SinPhi(wo), cosPhiO = CosPhi(wo);
			Float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
			maxCos = std::max((Float)0, dCos);
		}
		//Compute sine and tangent terms of Oren¨CNayar model537	
		Float sinAlpha, tanBeta;
		if (AbsCosTheta(wi) > AbsCosTheta(wo)) {
			sinAlpha = sinThetaO;
			tanBeta = sinThetaI / AbsCosTheta(wi);
		}
		else {
			sinAlpha = sinThetaI;
			tanBeta = sinThetaO / AbsCosTheta(wo);
		}
		return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
	}
}


template<typename Spectrum> inline
Spectrum BxDF<Spectrum>::Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType) const
{
	*wi = CosineSampleHemisphere(sample);
	if (wo.z() < 0) wi->z() *= -1;
	*pdf = Pdf(wo, *wi);
	return f(wo, *wi);
}
template<typename Spectrum> inline
float BxDF<Spectrum>::Pdf(const Vector3f& wo, const Vector3f& wi) const
{
	return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}
template<typename Spectrum> inline
Spectrum BxDF<Spectrum>::rho(const Vector3f& w, int nSamples, const Point2f* u) const {
	Spectrum r(0.);
	for (int i = 0; i < nSamples; ++i) {
		// Estimate one term of $\rho_\roman{hd}$
		Vector3f wi;
		Float pdf = 0;
		Spectrum f = Sample_f(w, &wi, u[i], &pdf);
		if (pdf > 0) r += f * AbsCosTheta(wi) / pdf;
	}
	return r / nSamples;
}
template<typename Spectrum> inline
Spectrum BxDF<Spectrum>::rho(int nSamples, const Point2f* u1, const Point2f* u2) const {
	Spectrum r(0.f);
	for (int i = 0; i < nSamples; ++i) {
		// Estimate one term of $\rho_\roman{hh}$
		Vector3f wo, wi;
		wo = UniformSampleHemisphere(u1[i]);
		Float pdfo = UniformHemispherePdf(), pdfi = 0;
		Spectrum f = Sample_f(wo, &wi, u2[i], &pdfi);
		if (pdfi > 0)
			r += f * AbsCosTheta(wi) * AbsCosTheta(wo) / (pdfo * pdfi);
	}
	return r / (Pi * nSamples);
}
template<typename Spectrum> inline
bool BxDF<Spectrum>::MatchesFlags(BxDFType t) const
{
	return (type & t) == type;
}
template<typename Spectrum> inline
bool BxDF<Spectrum>::HasFlags(BxDFType t) const
{
	return type & t;
}
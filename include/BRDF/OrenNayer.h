#pragma once
#include "BxDF.h"

class OrenNayer :public BxDF
{
public:
	OrenNayer(const Spectrum& R, Float sigma)
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

inline Spectrum OrenNayer::f(const Vector3f& wo, const Vector3f& wi) const
{
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
	return R * inv_pi * (A + B * maxCos * sinAlpha * tanBeta);
}

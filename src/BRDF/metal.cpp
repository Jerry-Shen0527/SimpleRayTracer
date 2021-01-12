#include <cassert>
#include <BRDF/MicrofacetDistribution.h>

#include "BRDF/Specular.h"
#include "Tools/Math/Sampling.h"

static void BeckmannSample11(Float cosThetaI, Float U1, Float U2,
	Float* slope_x, Float* slope_y) {
	/* Special case (normal incidence) */
	if (cosThetaI > .9999) {
		Float r = std::sqrt(-std::log(1.0f - U1));
		Float sinPhi = std::sin(2 * Pi * U2);
		Float cosPhi = std::cos(2 * Pi * U2);
		*slope_x = r * cosPhi;
		*slope_y = r * sinPhi;
		return;
	}

	/* The original inversion routine from the paper contained
	   discontinuities, which causes issues for QMC integration
	   and techniques like Kelemen-style MLT. The following code
	   performs a numerical inversion with better behavior */
	Float sinThetaI =
		std::sqrt(std::max((Float)0, (Float)1 - cosThetaI * cosThetaI));
	Float tanThetaI = sinThetaI / cosThetaI;
	Float cotThetaI = 1 / tanThetaI;

	/* Search interval -- everything is parameterized
	   in the Erf() domain */
	Float a = -1, c = Erf(cotThetaI);
	Float sample_x = std::max(U1, (Float)1e-6f);

	/* Start with a good initial guess */
	// Float b = (1-sample_x) * a + sample_x * c;

	/* We can do better (inverse of an approximation computed in
	 * Mathematica) */
	Float thetaI = std::acos(cosThetaI);
	Float fit = 1 + thetaI * (-0.876f + thetaI * (0.4265f - 0.0594f * thetaI));
	Float b = c - (1 + c) * std::pow(1 - sample_x, fit);

	/* Normalization factor for the CDF */
	static const Float SQRT_PI_INV = 1.f / std::sqrt(Pi);
	Float normalization =
		1 /
		(1 + c + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

	int it = 0;
	while (++it < 10) {
		/* Bisection criterion -- the oddly-looking
		   Boolean expression are intentional to check
		   for NaNs at little additional cost */
		if (!(b >= a && b <= c)) b = 0.5f * (a + c);

		/* Evaluate the CDF and its derivative
		   (i.e. the density function) */
		Float invErf = ErfInv(b);
		Float value =
			normalization *
			(1 + b + SQRT_PI_INV * tanThetaI * std::exp(-invErf * invErf)) -
			sample_x;
		Float derivative = normalization * (1 - invErf * tanThetaI);

		if (std::abs(value) < 1e-5f) break;

		/* Update bisection intervals */
		if (value > 0)
			c = b;
		else
			a = b;

		b -= value / derivative;
	}

	/* Now convert back into a slope value */
	*slope_x = ErfInv(b);

	/* Simulate Y component */
	*slope_y = ErfInv(2.0f * std::max(U2, (Float)1e-6f) - 1.0f);

	assert(!std::isinf(*slope_x));
	assert(!std::isnan(*slope_x));
	assert(!std::isinf(*slope_y));
	assert(!std::isnan(*slope_y));
}

static Vector3f BeckmannSample(const Vector3f& wi, Float alpha_x, Float alpha_y,
	Float U1, Float U2) {
	// 1. stretch wi
	Vector3f wiStretched =
		Normalize(Vector3f(alpha_x * wi.x(), alpha_y * wi.y(), wi.z()));

	// 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
	Float slope_x, slope_y;
	BeckmannSample11(CosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

	// 3. rotate
	Float tmp = CosPhi(wiStretched) * slope_x - SinPhi(wiStretched) * slope_y;
	slope_y = SinPhi(wiStretched) * slope_x + CosPhi(wiStretched) * slope_y;
	slope_x = tmp;

	// 4. unstretch
	slope_x = alpha_x * slope_x;
	slope_y = alpha_y * slope_y;

	// 5. compute normal
	return Normalize(Vector3f(-slope_x, -slope_y, 1.f));
}

Spectrum FresnelBlend::f(const Vector3f& wo, const Vector3f& wi) const
{
	auto pow5 = [](Float v) { return (v * v) * (v * v) * v; };
	Spectrum diffuse = (28.f / (23.f * Pi)) * Rd *
		(Spectrum(1.f) - Rs) *
		(1 - pow5(1 - .5f * AbsCosTheta(wi))) *
		(1 - pow5(1 - .5f * AbsCosTheta(wo)));
	Vector3f wh = wi + wo;
	if (wh.x() == 0 && wh.y() == 0 && wh.z() == 0) return Spectrum(0);
	wh = wh.Normalize();
	Spectrum specular = distribution->D(wh) /
		(4 * abs(Dot(wi, wh)) *
			std::max(AbsCosTheta(wi), AbsCosTheta(wo))) *
		SchlickFresnel(Dot(wi, wh));
	return diffuse + specular;
}

Float MicrofacetDistribution::Pdf(const Vector3f& wo, const Vector3f& wh) const
{
	if (sampleVisibleArea)
		return D(wh) * G1(wo) * AbsDot(wo, wh) / AbsCosTheta(wo);
	else
		return D(wh) * AbsCosTheta(wh);
}

Vector3f BeckmannDistribution::Sample_wh(const Vector3f& wo, const Point2f& u) const
{
	if (!sampleVisibleArea) {
		//Sample full distribution of normals for Beckmann distribution 808

		//Compute tan2 ¦Èand ¦Õ for Beckmann distribution sample 809
		Float tan2Theta, phi;
		if (alphax == alphay) {
			Float logSample = std::log(u[0]);
			if (std::isinf(logSample)) logSample = 0;
			tan2Theta = -alphax * alphax * logSample;
			phi = u[1] * 2 * Pi;
		}
		else {
			//Compute tan2Thetaand phi for anisotropic Beckmann distribution
			Float logSample = std::log(1 - u[0]);
			if (std::isinf(logSample)) logSample = 0;
			phi = std::atan(alphay / alphax *
				std::tan(2 * Pi * u[1] + 0.5f * Pi));
			if (u[1] > 0.5f) phi += Pi;
			Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
			Float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
			tan2Theta = -logSample / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
		}
		//Map sampled Beckmann angles to normal direction wh 809
		Float cosTheta = 1 / std::sqrt(1 + tan2Theta);
		Float sinTheta = std::sqrt(std::max((Float)0, 1 - cosTheta * cosTheta));
		Vector3f wh = SphericalDirection(sinTheta, cosTheta, phi);
		if (!SameHemisphere(wo, wh)) wh = -wh;
		return wh;
	}
	else {
		//Sample visible area of normals for Beckmann distribution
				// Sample visible area of normals for Beckmann distribution
		Vector3f wh;
		bool flip = wo.z() < 0;
		wh = BeckmannSample(flip ? -wo : wo, alphax, alphay, u[0], u[1]);
		if (flip) wh = -wh;
		return wh;
	}
}

Float TrowbridgeReitzDistribution::RoughnessToAlpha(Float roughness)
{
	roughness = std::max(roughness, (Float)1e-3);
	Float x = std::log(roughness);
	return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
		0.000640711f * x * x * x * x;
}

static void TrowbridgeReitzSample11(Float cosTheta, Float U1, Float U2,
	Float* slope_x, Float* slope_y) {
	// special case (normal incidence)
	if (cosTheta > .9999) {
		Float r = sqrt(U1 / (1 - U1));
		Float phi = 6.28318530718 * U2;
		*slope_x = r * cos(phi);
		*slope_y = r * sin(phi);
		return;
	}

	Float sinTheta =
		std::sqrt(std::max((Float)0, (Float)1 - cosTheta * cosTheta));
	Float tanTheta = sinTheta / cosTheta;
	Float a = 1 / tanTheta;
	Float G1 = 2 / (1 + std::sqrt(1.f + 1.f / (a * a)));

	// sample slope_x
	Float A = 2 * U1 / G1 - 1;
	Float tmp = 1.f / (A * A - 1.f);
	if (tmp > 1e10) tmp = 1e10;
	Float B = tanTheta;
	Float D = std::sqrt(
		std::max(Float(B * B * tmp * tmp - (A * A - B * B) * tmp), Float(0)));
	Float slope_x_1 = B * tmp - D;
	Float slope_x_2 = B * tmp + D;
	*slope_x = (A < 0 || slope_x_2 > 1.f / tanTheta) ? slope_x_1 : slope_x_2;

	// sample slope_y
	Float S;
	if (U2 > 0.5f) {
		S = 1.f;
		U2 = 2.f * (U2 - .5f);
	}
	else {
		S = -1.f;
		U2 = 2.f * (.5f - U2);
	}
	Float z =
		(U2 * (U2 * (U2 * 0.27385f - 0.73369f) + 0.46341f)) /
		(U2 * (U2 * (U2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
	*slope_y = S * z * std::sqrt(1.f + *slope_x * *slope_x);
}

static Vector3f TrowbridgeReitzSample(const Vector3f& wi, Float alpha_x,
	Float alpha_y, Float U1, Float U2) {
	// 1. stretch wi
	Vector3f wiStretched =
		Normalize(Vector3f(alpha_x * wi.x(), alpha_y * wi.y(), wi.z()));

	// 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
	Float slope_x, slope_y;
	TrowbridgeReitzSample11(CosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

	// 3. rotate
	Float tmp = CosPhi(wiStretched) * slope_x - SinPhi(wiStretched) * slope_y;
	slope_y = SinPhi(wiStretched) * slope_x + CosPhi(wiStretched) * slope_y;
	slope_x = tmp;

	// 4. unstretch
	slope_x = alpha_x * slope_x;
	slope_y = alpha_y * slope_y;

	// 5. compute normal
	return Normalize(Vector3f(-slope_x, -slope_y, 1.));
}

Vector3f TrowbridgeReitzDistribution::Sample_wh(const Vector3f& wo, const Point2f& u) const
{
	Vector3f wh;
	if (!sampleVisibleArea) {
		Float cosTheta = 0, phi = (2 * Pi) * u[1];
		if (alphax == alphay) {
			Float tanTheta2 = alphax * alphax * u[0] / (1.0f - u[0]);
			cosTheta = 1 / std::sqrt(1 + tanTheta2);
		}
		else {
			phi =
				std::atan(alphay / alphax * std::tan(2 * Pi * u[1] + .5f * Pi));
			if (u[1] > .5f) phi += Pi;
			Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
			const Float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
			const Float alpha2 =
				1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
			Float tanTheta2 = alpha2 * u[0] / (1 - u[0]);
			cosTheta = 1 / std::sqrt(1 + tanTheta2);
		}
		Float sinTheta =
			std::sqrt(std::max((Float)0., (Float)1. - cosTheta * cosTheta));
		wh = SphericalDirection(sinTheta, cosTheta, phi);
		if (!SameHemisphere(wo, wh)) wh = -wh;
	}
	else {
		bool flip = wo.z() < 0;
		wh = TrowbridgeReitzSample(flip ? -wo : wo, alphax, alphay, u[0], u[1]);
		if (flip) wh = -wh;
	}
	return wh;
}

Spectrum MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	Float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
	Vector3f wh = wi + wo;
	//Handle degenerate cases for microfacet reflection 547
	if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0.);
	if (wh.x() == 0 && wh.y() == 0 && wh.z() == 0) return Spectrum(0.);
	wh = wh.Normalize();
	Spectrum F = fresnel->Evaluate(Dot(wi, wh));
	return R * distribution->D(wh) * distribution->G(wo, wi) * F /
		(4 * cosThetaI * cosThetaO);
}

Spectrum MicrofacetReflection::Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u, Float* pdf,
	BxDFType* sampledType) const
{
	// Sample microfacet orientation $\wh$ and reflected direction $\wi$
	if (wo.z() == 0) return 0.;
	Vector3f wh = distribution->Sample_wh(wo, u);
	if (Dot(wo, wh) < 0) return 0.;   // Should be rare
	*wi = Reflect(wo, wh);
	if (!SameHemisphere(wo, *wi)) return Spectrum(0.f);

	// Compute PDF of _wi_ for microfacet reflection
	*pdf = distribution->Pdf(wo, wh) / (4 * Dot(wo, wh));
	return f(wo, *wi);
}

Float MicrofacetReflection::Pdf(const Vector3f& wo, const Vector3f& wi) const
{
	if (!SameHemisphere(wo, wi)) return 0;
	Vector3f wh = Normalize(wo + wi);
	return distribution->Pdf(wo, wh) / (4 * Dot(wo, wh));
}

Spectrum MicrofacetTransmission::f(const Vector3f& wo, const Vector3f& wi) const
{
	if (SameHemisphere(wo, wi)) return 0;  // transmission only

	Float cosThetaO = CosTheta(wo);
	Float cosThetaI = CosTheta(wi);
	if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0);

	// Compute $\wh$ from $\wo$ and $\wi$ for microfacet transmission
	Float eta = CosTheta(wo) > 0 ? (etaB / etaA) : (etaA / etaB);
	Vector3f wh = (wo + wi * eta).Normalize();
	if (wh.z() < 0) wh = -wh;

	// Same side?
	if (Dot(wo, wh) * Dot(wi, wh) > 0) return Spectrum(0);

	Spectrum F = fresnel.Evaluate(Dot(wo, wh));

	Float sqrtDenom = Dot(wo, wh) + eta * Dot(wi, wh);
	Float factor = (mode == TransportMode::Radiance) ? (1 / eta) : 1;

	return (Spectrum(1.f) - F) * T *
		std::abs(distribution->D(wh) * distribution->G(wo, wi) * eta * eta *
			abs(Dot(wi, wh) * Dot(wo, wh)) * factor * factor /
			(cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));
}

Spectrum MicrofacetTransmission::Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u, Float* pdf,
	BxDFType* sampledType) const
{
	if (wo.z() == 0) return 0.;
	Vector3f wh = distribution->Sample_wh(wo, u);
	if (Dot(wo, wh) < 0) return 0.;  // Should be rare

	Float eta = CosTheta(wo) > 0 ? (etaA / etaB) : (etaB / etaA);
	if (!Refract(wo, (Normal3f)wh, eta, wi)) return 0;
	*pdf = Pdf(wo, *wi);
	return f(wo, *wi);
}

Float MicrofacetTransmission::Pdf(const Vector3f& wo, const Vector3f& wi) const
{
	if (SameHemisphere(wo, wi)) return 0;
	// Compute $\wh$ from $\wo$ and $\wi$ for microfacet transmission
	Float eta = CosTheta(wo) > 0 ? (etaB / etaA) : (etaA / etaB);
	Vector3f wh = Normalize(wo + wi * eta);

	if (Dot(wo, wh) * Dot(wi, wh) > 0) return 0;

	// Compute change of variables _dwh\_dwi_ for microfacet transmission
	Float sqrtDenom = Dot(wo, wh) + eta * Dot(wi, wh);
	Float dwh_dwi =
		std::abs((eta * eta * Dot(wi, wh)) / (sqrtDenom * sqrtDenom));
	return distribution->Pdf(wo, wh) * dwh_dwi;
}
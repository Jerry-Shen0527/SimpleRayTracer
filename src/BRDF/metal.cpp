#include <cassert>
#include <BRDF/MicrofacetDistribution.h>

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
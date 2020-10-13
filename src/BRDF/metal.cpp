#include <BRDF/metal.h>

#include "Tools/Math/Sampling.h"

Vector3f reflect(const Vector3f& v, const Vector3f& n) {
	return v - 2 * Dot(v, n) * n;
}

metal::metal(const Color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1)
{
}

bool metal::scatter(const Ray& r_in, const SurfaceInteraction& rec, scatter_record& srec) const
{
	Vector3f reflected = reflect(unit_vector(r_in.d), rec.n);
	srec.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
	srec.attenuation = albedo;
	srec.update();
	srec.is_specular = true;
	srec.pdf_ptr = nullptr;
	return true;
}

Spectrum MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	Float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
	Vector3f wh = wi + wo;
	//Handle degenerate cases for microfacet reflection 547
	if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0.);
	if (wh.x() == 0 && wh.y() == 0 && wh.z() == 0) return Spectrum(0.);
	wh = wh.normalize();
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
	Vector3f wh = (wo + wi * eta).normalize();
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
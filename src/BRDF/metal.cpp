#include <BRDF/metal.h>

#include "Tools/Math/Sampling.h"
#include <Geometry/surface_hit_record.h>

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

metal::metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1)
{
}

bool metal::scatter(const ray& r_in, const surface_hit_record& rec, scatter_record& srec) const
{
	vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
	srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
	srec.attenuation = albedo;
	srec.update();
	srec.is_specular = true;
	srec.pdf_ptr = nullptr;
	return true;
}

Spectrum MicrofacetReflection::f(const vec3& wo, const vec3& wi) const
{
	Float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
	Vector3f wh = wi + wo;
	//Handle degenerate cases for microfacet reflection 547
	if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0.);
	if (wh.x() == 0 && wh.y() == 0 && wh.z() == 0) return Spectrum(0.);
	wh = wh.normalize();
	Spectrum F = fresnel->Evaluate(dot(wi, wh));
	return R * distribution->D(wh) * distribution->G(wo, wi) * F /
		(4 * cosThetaI * cosThetaO);
}

Spectrum MicrofacetTransmission::f(const vec3& wo, const vec3& wi) const
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
	if (dot(wo, wh) * dot(wi, wh) > 0) return Spectrum(0);

	Spectrum F = fresnel.Evaluate(dot(wo, wh));

	Float sqrtDenom = dot(wo, wh) + eta * dot(wi, wh);
	Float factor = (mode == TransportMode::Radiance) ? (1 / eta) : 1;

	return (Spectrum(1.f) - F) * T *
		std::abs(distribution->D(wh) * distribution->G(wo, wi) * eta * eta *
			abs(dot(wi, wh) * dot(wo, wh)) * factor * factor /
			(cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));
}
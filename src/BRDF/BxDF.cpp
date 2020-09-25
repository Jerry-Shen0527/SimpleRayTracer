#include <BRDF/BxDF.h>
#include <BRDF/BxDF_Utility.h>

Spectrum BxDF::Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType) const
{
	wi = CosineSampleHemisphere(sample);
	if (wo.z() < 0) wi.z() *= -1;
	return Spectrum(0);
}

float BxDF::pdf(const vec3& wo, const vec3& wi)
{
	return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * inv_pi : 0;
}

bool BxDF::MatchesFlags(BxDFType t) const
{
	return (type & t) == type;
}

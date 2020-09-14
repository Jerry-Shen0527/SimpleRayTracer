#include <BRDF/BxDF.h>

Spectrum BxDF::Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType) const
{
	wi = CosineSampleHemisphere(u);
	if (wo.z < 0) wi->z *= -1;
}

bool BxDF::MatchesFlags(BxDFType t) const
{
	return (type & t) == type;
}

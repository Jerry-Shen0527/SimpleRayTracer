#include <BRDF/BxDF.h>
#include <BRDF/BxDF_Utility.h>

Spectrum BxDF::Sample_f(const Vector3f& wo, Vector3f& wi, const point2& sample, float& pdf, BxDFType* sampledType) const
{
	wi = CosineSampleHemisphere(sample);
	if (wo.z() < 0) wi.z() *= -1;
	return Spectrum(0);
}

float BxDF::pdf(const Vector3f& wo, const Vector3f& wi)
{
	return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * inv_pi : 0;
}



bool BxDF::MatchesFlags(BxDFType t) const
{
	return (type & t) == type;
}

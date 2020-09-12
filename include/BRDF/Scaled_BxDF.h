#pragma once
#include "BxDF.h"
#include "BxDF_Utility.h"
#include "Tools/Spectrum/SampledSpectrum.h"

class ScaledBxDF : public BxDF {
public:
	ScaledBxDF(BxDF* bxdf, const Spectrum& scale)
		: BxDF(BxDFType(bxdf->type)), bxdf(bxdf), scale(scale) {
	}
	Spectrum ScaledBxDF::f(const Vector3f& wo, const Vector3f& wi) const {
		return scale * bxdf->f(wo, wi);
	}
private:
	BxDF* bxdf;
	Spectrum scale;
};

#pragma once
#include <Tools/Spectrum/SampledSpectrum.h>

enum BxDFType {
	BSDF_REFLECTION = 1 << 0,
	BSDF_TRANSMISSION = 1 << 1,
	BSDF_DIFFUSE = 1 << 2,
	BSDF_GLOSSY = 1 << 3,
	BSDF_SPECULAR = 1 << 4,
	BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
};

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
};


class LambertianReflection : public BxDF {
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

inline Spectrum LambertianReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	return R * InvPi;
}


inline Spectrum LambertianReflection::rho(const Vector3f& wo, int nSamples, const Point2f* samples) const
{
	return R;
}

inline Spectrum LambertianReflection::rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const
{
	return  R;
}

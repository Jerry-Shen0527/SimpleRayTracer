#pragma once
#include "BxDF.h"
#include <Tools/Spectrum/SampledSpectrum.h>

#include "pdf/pdf.h"

class lambertian : public material {
public:
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const override {
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared< cosine_pdf>(rec.normal);
        return true;
    }

    double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const {
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / pi;
    }

public:
    shared_ptr<texture> albedo;
};

class LambertianReflection : public BxDF {
public:
	//LambertianReflection Public Methods 532
	LambertianReflection(const Spectrum& R)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) { }

	Spectrum f(const vec3& wo, const vec3& wi) const override;
	Spectrum Sample_f(const vec3& wo, vec3& wi, const point2& sample, float& pdf, BxDFType* sampledType) const override;
	Spectrum rho(const vec3& wo, int nSamples, const point2* samples) const override;
	Spectrum rho(int nSamples, const point2* samples1, const point2* samples2) const override;
private:
	//LambertianReflection Private Data 532
	const Spectrum R;
};

inline Spectrum LambertianReflection::f(const vec3& wo, const vec3& wi) const
{
	return R * inv_pi;
}

inline Spectrum LambertianReflection::rho(const vec3& wo, int nSamples, const point2* samples) const
{
	return R;
}

inline Spectrum LambertianReflection::rho(int nSamples, const point2* samples1, const point2* samples2) const
{
	return  R;
}

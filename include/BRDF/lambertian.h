#pragma once
#include <memory>

#include "BxDF.h"
#include <Tools/Spectrum/SampledSpectrum.h>


#include "Material.h"
#include "texture.h"
#include "Geometry/Interaction.h"
#include "pdf/pdf.h"

class lambertian : public material {
public:
	lambertian(const Color& a) : albedo(std::make_shared<solid_Color>(a)) {}
	lambertian(shared_ptr<texture> a) : albedo(a) {}
	virtual bool scatter(
		const Ray& r_in, const SurfaceInteraction& rec, scatter_record& srec
	) const override {
		srec.is_specular = false;
		srec.attenuation = albedo->value(rec.uv, rec.p);
		srec.update();
		srec.pdf_ptr = std::make_shared< cosine_pdf>(rec.n);
		return true;
	}

	float scattering_pdf(
		const Ray& r_in, const SurfaceInteraction& rec, const Ray& scattered
	) const {
		auto cosine = Dot(rec.n, unit_vector(scattered.d));
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

	Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
	Spectrum Sample_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, float& pdf, BxDFType* sampledType) const override;
	Spectrum rho(const Vector3f& wo, int nSamples, const Point2f* samples) const override;
	Spectrum rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const override;
private:
	//LambertianReflection Private Data 532
	const Spectrum R;
};

inline Spectrum LambertianReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	return R * inv_pi;
}

inline Spectrum LambertianReflection::rho(const Vector3f& wo, int nSamples, const Point2f* samples) const
{
	return R;
}

inline Spectrum LambertianReflection::rho(int nSamples, const Point2f* samples1, const Point2f* samples2) const
{
	return  R;
}

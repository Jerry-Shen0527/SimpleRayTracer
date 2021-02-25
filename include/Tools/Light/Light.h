#pragma once
#include <algorithm>
#include <Geometry/hittable_list.h>

#include "BRDF/Medium.h"
#include "Geometry/Interaction.h"
#include "Geometry/Transform.h"
#include "Tools/Spectrum/SampledSpectrum.h"

enum class LightFlags : int { DeltaPosition = 1, DeltaDirection = 2, Area = 4, Infinite = 8 };

inline bool IsDeltaLight(int flags)
{
	return flags & (int)LightFlags::DeltaPosition || flags & (int)LightFlags::DeltaDirection;
}

class Light
{
public:
	Light(int flags, const Transform& LightToWorld, const MediumInterface& mediumInterface, int nSamples = 1)
		: flags(flags), nSamples(std::max(1, nSamples)), mediumInterface(mediumInterface), LightToWorld(LightToWorld), WorldToLight(Inverse(LightToWorld)) {	}

	virtual Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f* wi, Float* pdf, VisibilityTester* vis) const = 0;
	virtual Float Pdf_Li(const Interaction& ref, const Vector3f& wi) const = 0;
	virtual Spectrum Le(const RayDifferential& r) const { return  Spectrum(0); }

	virtual Spectrum Sample_Le(const Point2f& u1, const Point2f& u2, Float time, Ray* ray, Normal3f* nLight, Float* pdfPos, Float* pdfDir) const = 0;
	virtual void Pdf_Le(const Ray& ray, const Normal3f& nLight, Float* pdfPos, Float* pdfDir) const = 0;

	virtual Spectrum Power() const = 0;
	virtual void Preprocess(const Scene& scene) { }

public:
	const int flags;
	const int nSamples;
	const MediumInterface mediumInterface;

protected:
	const Transform LightToWorld, WorldToLight;
};

class VisibilityTester {
public:
	VisibilityTester() {}
	VisibilityTester(const Interaction& p0, const Interaction& p1) : p0(p0), p1(p1) { }

	const Interaction& P0() const { return p0; }
	const Interaction& P1() const { return p1; }
	bool Unoccluded(const Scene& scene) const;

	virtual Spectrum Le(const RayDifferential& ray) const { return Spectrum(0.f); }

	Spectrum Tr(const Scene& scene, Sampler& sampler) const;
private:
	Interaction p0, p1;
};

class AreaLight :public Light
{
public:
	AreaLight(const Transform& LightToWorld, const MediumInterface& medium, int nSamples);
	virtual Spectrum L(const Interaction& intr, const Vector3f& w) const = 0;
};

inline AreaLight::AreaLight(const Transform& LightToWorld, const MediumInterface& medium, int nSamples) :Light(int(LightFlags::Area), LightToWorld, medium, nSamples)
{
}

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight(const Transform& LightToWorld,
		const MediumInterface& mediumInterface, const Spectrum& Lemit,
		int nSamples, const std::shared_ptr<Shape>& shape, bool twoSided = false);

	Spectrum L(const Interaction& intr, const Vector3f& w) const override;

	Spectrum Power() const;

	Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f* wi, Float* pdf,
		VisibilityTester* vis) const override;
	Float Pdf_Li(const Interaction& ref, const Vector3f& wi) const override;
	Spectrum Sample_Le(const Point2f& u1, const Point2f& u2, Float time, Ray* ray, Normal3f* nLight, Float* pdfPos, Float* pdfDir) const override;
	void Pdf_Le(const Ray& ray, const Normal3f& nLight, Float* pdfPos, Float* pdfDir) const override;
protected:
	const Spectrum Lemit;
	std::shared_ptr<Shape> shape;
	const bool twoSided;

	const Float area;
};

class DistantLight : public Light {
public:
	// DistantLight Public Methods
	DistantLight(const Transform& LightToWorld, const Spectrum& L,
		const Vector3f& w);
	void Preprocess(const Scene& scene);
	Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f* wi,
	                   Float* pdf, VisibilityTester* vis) const override;
	Spectrum Power() const override;
	Float Pdf_Li(const Interaction&, const Vector3f&) const override;
	Spectrum Sample_Le(const Point2f& u1, const Point2f& u2, Float time,
		Ray* ray, Normal3f* nLight, Float* pdfPos,
		Float* pdfDir) const override;
	void Pdf_Le(const Ray&, const Normal3f&, Float* pdfPos,
		Float* pdfDir) const override;

private:
	// DistantLight Private Data
	const Spectrum L;
	const Vector3f wLight;
	Point3f worldCenter;
	Float worldRadius;
};
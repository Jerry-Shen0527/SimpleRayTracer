#include <Tools/Light/Light.h>

#include "Geometry/Scene.h"
#include "Geometry/Shape.h"

bool VisibilityTester::Unoccluded(const Scene& scene) const {
	return !scene.IntersectP(p0.SpawnRayTo(p1.p));
}

Spectrum VisibilityTester::Tr(const Scene& scene, Sampler& sampler) const {
	Ray ray(p0.SpawnRayTo(p1));
	Spectrum Tr(1.f);
	while (true) {
		SurfaceInteraction* isect;
		bool hitSurface = scene.Intersect(ray, isect);
		//Handle opaque surface along ray¡¯s path 718
		if (hitSurface && isect->mat_ptr != nullptr)
			return Spectrum(0.0f);
		//	Update transmittance for current ray segment 719
		if (ray.medium)
			Tr *= ray.medium->Tr(ray, sampler);
		//	Generate next ray segment or return final transmittance 719
		if (!hitSurface)
			break;
		ray = isect->SpawnRayTo(p1);
	}
	return Tr;
}

DiffuseAreaLight::DiffuseAreaLight(const Transform& LightToWorld, const MediumInterface& mediumInterface,
                                   const Spectrum& Lemit, int nSamples, const std::shared_ptr<Shape>& shape):
	AreaLight(LightToWorld, mediumInterface, nSamples), Lemit(Lemit),
	shape(shape), area(shape->Area())
{
}

Spectrum DiffuseAreaLight::L(const Interaction& intr, const Vector3f& w) const
{
	return Dot(intr.n, w) > 0.f ? Lemit : Spectrum(0.f);
}

Spectrum DiffuseAreaLight::Power() const
{
	return Lemit * area * Pi;
}

Spectrum DiffuseAreaLight::Sample_Li(const Interaction& ref, const Point2f& u, Vector3f* wi, Float* pdf,
	VisibilityTester* vis) const
{
	Interaction pShape = shape->Sample(ref, u, pdf);
	pShape.mediumInterface = mediumInterface;
	if (*pdf == 0 || (pShape.p - ref.p).LengthSquared() == 0) {
		*pdf = 0;
		return 0.f;
	}
	*wi = Normalize(pShape.p - ref.p);
	*vis = VisibilityTester(ref, pShape);
	return L(pShape, -*wi);
}

Float DiffuseAreaLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return shape->Pdf(ref, wi);
}

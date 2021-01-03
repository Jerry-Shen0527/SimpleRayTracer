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
	const Spectrum& Lemit, int nSamples, const std::shared_ptr<Shape>& shape, bool twoSided) :
	AreaLight(LightToWorld, mediumInterface, nSamples), Lemit(Lemit),
	shape(shape), area(shape->Area()), twoSided(twoSided)
{
}

Spectrum DiffuseAreaLight::L(const Interaction& intr, const Vector3f& w) const
{
	return (twoSided || Dot(intr.n, w) > 0) ? Lemit : Spectrum(0.f);
}

Spectrum DiffuseAreaLight::Power() const
{
	return (twoSided ? 2 : 1) * Lemit * area * Pi;
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

Spectrum DiffuseAreaLight::Sample_Le(const Point2f& u1, const Point2f& u2, Float time, Ray* ray, Normal3f* nLight,
	Float* pdfPos, Float* pdfDir) const
{
	// Sample a point on the area light's _Shape_, _pShape_
	Interaction pShape = shape->Sample(u1, pdfPos);
	pShape.mediumInterface = mediumInterface;
	*nLight = pShape.n;

	// Sample a cosine-weighted outgoing direction _w_ for area light
	Vector3f w;
	if (twoSided) {
		Point2f u = u2;
		// Choose a side to sample and then remap u[0] to [0,1] before
		// applying cosine-weighted hemisphere sampling for the chosen side.
		if (u[0] < .5) {
			u[0] = std::min(u[0] * 2, OneMinusEpsilon);
			w = CosineSampleHemisphere(u);
		}
		else {
			u[0] = std::min((u[0] - .5f) * 2, OneMinusEpsilon);
			w = CosineSampleHemisphere(u);
			w.z() *= -1;
		}
		*pdfDir = 0.5f * CosineHemispherePdf(std::abs(w.z()));
	}
	else {
		w = CosineSampleHemisphere(u2);
		*pdfDir = CosineHemispherePdf(w.z());
	}

	Vector3f v1, v2, n(pShape.n);
	CoordinateSystem(n, &v1, &v2);
	w = w.x() * v1 + w.y() * v2 + w.z() * n;
	*ray = pShape.SpawnRay(w);
	return L(pShape, w);
}

void DiffuseAreaLight::Pdf_Le(const Ray& ray, const Normal3f& nLight, Float* pdfPos, Float* pdfDir) const
{
	Interaction it(ray.o, nLight, Vector3f(), Vector3f(nLight), ray.time);
	*pdfPos = shape->Pdf(it);
	*pdfDir = twoSided ? (.5 * CosineHemispherePdf(AbsDot(nLight, ray.d)))
		: CosineHemispherePdf(Dot(nLight, ray.d));
}
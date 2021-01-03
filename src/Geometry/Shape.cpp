#include <Geometry/Shape.h>

#include "Geometry/Interaction.h"

Interaction Shape::Sample(const Interaction& ref, const Point2f& u, Float* pdf) const
{
	Interaction intr = Sample(u, pdf);
	Vector3f wi = intr.p - ref.p;
	if (wi.LengthSquared() == 0)
		*pdf = 0;
	else {
		wi = Normalize(wi);
		// Convert from area measure, as returned by the Sample() call
		// above, to solid angle measure.
		*pdf *= (ref.p - intr.p).LengthSquared() / AbsDot(intr.n, -wi);
		if (std::isinf(*pdf)) *pdf = 0.f;
	}
	return intr;
}

Float Shape::Pdf(const Interaction& ref, const Vector3f& wi) const {
	// Intersect sample ray with area light geometry
	Ray ray = ref.SpawnRay(wi);
	Float tHit;
	SurfaceInteraction isectLight;
	// Ignore any alpha textures used for trimming the shape when performing
	// this intersection. Hack for the "San Miguel" scene, where this is used
	// to make an invisible area light.
	if (!Intersect(ray, &tHit, &isectLight, false)) return 0;

	// Convert light sample weight to solid angle measure
	Float pdf = (ref.p - isectLight.p).LengthSquared() /
		(AbsDot(isectLight.n, -wi) * Area());
	if (std::isinf(pdf)) pdf = 0.f;
	return pdf;
}

bool Shape::IntersectP(const Ray& ray, bool testAlphaTexture) const
{
	Float tHit = ray.tMax;
	SurfaceInteraction isect;
	return Intersect(ray, &tHit, &isect, testAlphaTexture);
}

Bounds3f Sphere::ObjectBound() const
{
	return Bounds3f(Point3f(-radius, -radius, zMin), Point3f(radius, radius, zMax));
}

bool Sphere::Intersect(const Ray& r, Float* tHit, SurfaceInteraction* isect, bool testAlphaTexture) const
{
	Float phi;
	Point3f pHit;
	//Transform Ray to object space 134
	Vector3f oErr, dErr;
	Ray r_new = (*WorldToObject)(r);
	//	Compute quadratic sphere coefficients 135
	EFloat ox(r.o.x(), oErr.x()), oy(r.o.y(), oErr.y()), oz(r.o.z(), oErr.z());
	EFloat dx(r.d.x(), dErr.x()), dy(r.d.y(), dErr.y()), dz(r.d.z(), dErr.z());

	EFloat a = dx * dx + dy * dy + dz * dz;
	EFloat b = 2 * (dx * ox + dy * oy + dz * oz);
	EFloat c = ox * ox + oy * oy + oz * oz - EFloat(radius) * EFloat(radius);
	//	Solve quadratic equation for t values 136
	EFloat t0, t1;
	if (!Quadratic(a, b, c, &t0, &t1))
		return false;
	if (t0.UpperBound() > r.tMax || t1.LowerBound() <= 0)
		return false;
	EFloat tShapeHit = t0;
	if (tShapeHit.LowerBound() <= 0)
	{
		tShapeHit = t1;
		if (tShapeHit.UpperBound() > r.tMax)
			return false;
	}
	//	Compute sphere hit position and ¦Õ 137
	pHit = r.at((Float)tShapeHit);
	//Refine sphere intersection point 225
	pHit *= radius / (pHit - Point3f(0, 0, 0)).Length();
	if (pHit.x() == 0 && pHit.y() == 0) pHit.x() = 1e-5f * radius;
	phi = std::atan2(pHit.y(), pHit.x());
	if (phi < 0) phi += 2 * Pi;
	//	Test sphere intersection against clipping parameters 137

	if (zMin > -radius && pHit.z() < zMin ||
		zMax < radius && pHit.z() > zMax || phi > phiMax)
	{
		if (tShapeHit == t1) return false;
		if (t1.UpperBound() > r.tMax) return false;
		tShapeHit = t1;
		//Compute sphere hit positionand ¦Õ 137
		pHit = r.at((Float)tShapeHit);
		//Refine sphere intersection point 225
		pHit *= radius / (pHit - Point3f(0, 0, 0)).Length();
		if (pHit.x() == 0 && pHit.y() == 0) pHit.x() = 1e-5f * radius;
		phi = std::atan2(pHit.y(), pHit.x());
		if (phi < 0) phi += 2 * Pi;
		if ((zMin > -radius && pHit.z() < zMin) || (zMax < radius && pHit.z() > zMax) || phi > phiMax)
			return false;
	}
	//	Find parametric representation of sphere hit 137
	Float u = phi / phiMax;
	Float theta = std::acos(Clamp(pHit.z() / radius, -1, 1));
	Float v = (theta - thetaMin) / (thetaMax - thetaMin);
	Float zRadius = std::sqrt(pHit.x() * pHit.x() + pHit.y() * pHit.y());
	Float invZRadius = 1 / zRadius;
	Float cosPhi = pHit.x() * invZRadius;
	Float sinPhi = pHit.y() * invZRadius;
	Vector3f dpdu(-phiMax * pHit.y(), phiMax * pHit.x(), 0);
	Vector3f dpdv = (thetaMax - thetaMin) * Vector3f(pHit.z() * cosPhi, pHit.z() * sinPhi, -radius * std::sin(theta));

	//	Compute error bounds for sphere intersection 225
	Vector3f pError = gamma(5) * pHit.Abs();

	Vector3f d2Pduu = -phiMax * phiMax * Vector3f(pHit.x(), pHit.y(), 0);
	Vector3f d2Pduv = (thetaMax - thetaMin) * pHit.z() * phiMax *
		Vector3f(-sinPhi, cosPhi, 0.);
	Vector3f d2Pdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) *
		Vector3f(pHit.x(), pHit.y(), pHit.z());

	Float E = Dot(dpdu, dpdu);
	Float F = Dot(dpdu, dpdv);
	Float G = Dot(dpdv, dpdv);
	Vector3f N = Cross(dpdu, dpdv).Normalize();
	Float e = Dot(N, d2Pduu);
	Float f = Dot(N, d2Pduv);
	Float g = Dot(N, d2Pdvv);

	Float invEGF2 = 1 / (E * G - F * F);
	Normal3f dndu = Normal3f((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
	Normal3f dndv = Normal3f((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

	//	Initialize SurfaceInteraction from parametric information 140
	*isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError, Point2f(u, v), -r.d, dpdu, dpdv, dndu, dndv, r.time, this));
	//	Update tHit for quadric intersection 140

	*tHit = (Float)tShapeHit;
	return true;
}

Float Sphere::Area() const
{
	return phiMax * radius * (zMax - zMin);
}
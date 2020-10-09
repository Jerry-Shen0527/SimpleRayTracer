#ifndef SPHERE_H
#define SPHERE_H

#include <Geometry/hittable.h>
#include <Tools/Math/EFloat.h>
#include <Tools/Math/onb.h>

#include "Shape.h"
#include "translation.h"

class sphere : public hittable {
public:
	sphere() {}
	sphere(Point3f cen, float r, std::shared_ptr<Material> m, bool pdf = false)
		: center(cen), radius(r), mat_ptr(m) {
		pdf_enabled = pdf;
	}

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

	void get_sphere_uv(const Vector3f& p, Vector2f& uv) const;
	float pdf_value(const Point3f& o, const Vector3f& v) const override;
	Vector3f random(const Point3f& o) const override;

public:

	Point3f center;
	float radius;
	std::shared_ptr<Material> mat_ptr;
};

class Sphere :public Shape
{
public:
	Sphere(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation, Float radius, Float zMin, Float zMax, Float phiMax)
		: Shape(ObjectToWorld, WorldToObject, reverseOrientation), radius(radius), zMin(Clamp(std::min(zMin, zMax), -radius, radius)), zMax(Clamp(std::max(zMin, zMax), -radius, radius)), thetaMin(std::acos(Clamp(zMin / radius, -1, 1))), thetaMax(std::acos(Clamp(zMax / radius, -1, 1))), phiMax(Radians(Clamp(phiMax, 0, 360))) { }

	Bounds3f Sphere::ObjectBound() const {
		return Bounds3f(Point3f(-radius, -radius, zMin),
			Point3f(radius, radius, zMax));
	}

	bool Sphere::Intersect(const ray& r, Float& tHit, SurfaceInteraction& isect, bool testAlphaTexture) const {
		Float phi;
		Point3f pHit;
		//Transform Ray to object space 134
		Vector3f oErr, dErr;
		ray r_new = (*WorldToObject)(r);
		//	Compute quadratic sphere coefficients 135
		EFloat ox(r.orig.x(), oErr.x()), oy(r.orig.y(), oErr.y()), oz(r.orig.z(), oErr.z());
		EFloat dx(r.dir.x(), dErr.x()), dy(r.dir.y(), dErr.y()), dz(r.dir.z(), dErr.z());

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
		if (tShapeHit.LowerBound() <= 0) {
			tShapeHit = t1;
			if (tShapeHit.UpperBound() > r.tMax)
				return false;
		}
		//	Compute sphere hit position and ¦Õ 137
		pHit = r.at((Float)tShapeHit);
		//Refine sphere intersection point 225
		pHit *= radius / (pHit - Point3f(0, 0, 0)).length();
		if (pHit.x() == 0 && pHit.y() == 0) pHit.x() = 1e-5f * radius;
		phi = std::atan2(pHit.y(), pHit.x());
		if (phi < 0) phi += 2 * pi;
		//	Test sphere intersection against clipping parameters 137

		if (zMin > -radius && pHit.z() < zMin ||
			zMax < radius && pHit.z() > zMax || phi > phiMax) {
			if (tShapeHit == t1) return false;
			if (t1.UpperBound() > r.tMax) return false;
			tShapeHit = t1;
			//Compute sphere hit positionand ¦Õ 137
			pHit = r.at((Float)tShapeHit);
			//Refine sphere intersection point 225
			pHit *= radius / (pHit - Point3f(0, 0, 0)).length();
			if (pHit.x() == 0 && pHit.y() == 0) pHit.x() = 1e-5f * radius;
			phi = std::atan2(pHit.y(), pHit.x());
			if (phi < 0) phi += 2 * pi;
			if ((zMin > -radius && pHit.z() < zMin) ||
				(zMax < radius && pHit.z() > zMax) || phi > phiMax)
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
		Vector3f dpdv = (thetaMax - thetaMin) *
			Vector3f(pHit.z() * cosPhi, pHit.z() * sinPhi,
				-radius * std::sin(theta));

		//	Compute error bounds for sphere intersection 225
		Vector3f pError = gamma(5) * pHit.abs();
		
		Vector3f d2Pduu = -phiMax * phiMax * Vector3f(pHit.x(), pHit.y(), 0);
		Vector3f d2Pduv = (thetaMax - thetaMin) * pHit.z() * phiMax *
			Vector3f(-sinPhi, cosPhi, 0.);
		Vector3f d2Pdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) *
			Vector3f(pHit.x(), pHit.y(), pHit.z());

		Float E = dot(dpdu, dpdu);
		Float F = dot(dpdu, dpdv);
		Float G = dot(dpdv, dpdv);
		Vector3f N = cross(dpdu, dpdv).normalize();
		Float e = dot(N, d2Pduu);
		Float f = dot(N, d2Pduv);
		Float g = dot(N, d2Pdvv);

		Float invEGF2 = 1 / (E * G - F * F);
		Normal3f dndu = Normal3f((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
		Normal3f dndv = Normal3f((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

		//	Initialize SurfaceInteraction from parametric information 140
		isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError, Point2f(u, v), -r.dir, dpdu, dpdv, dndu, dndv, r.time(), this));
		//	Update tHit for quadric intersection 140
		tHit = (Float)tShapeHit;
		return true;
	}

private:
	const Float radius;
	const Float zMin, zMax;
	const Float thetaMin, thetaMax, phiMax;
};

#endif

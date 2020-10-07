#ifndef SPHERE_H
#define SPHERE_H

#include <Geometry/hittable.h>
#include <Tools/Math/onb.h>

#include "Shape.h"
#include "translation.h"

class sphere : public hittable {
public:
	sphere() {}
	sphere(point3 cen, float r, std::shared_ptr<Material> m, bool pdf = false)
		: center(cen), radius(r), mat_ptr(m) {
		pdf_enabled = pdf;
	}

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

	void get_sphere_uv(const vec3& p, vec2& uv) const;
	float pdf_value(const point3& o, const vec3& v) const override;
	vec3 random(const point3& o) const override;

public:

	point3 center;
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
		//	Compute quadratic sphere coefficients 135
		//	Solve quadratic equation for t values 136
		//	Compute sphere hit position and ¦Õ 137
		//	Test sphere intersection against clipping parameters 137
		//	Find parametric representation of sphere hit 137
		//	Compute error bounds for sphere intersection 225
		//	Initialize SurfaceInteraction from parametric information 140
		//	Update tHit for quadric intersection 140
		return true;
	}

private:
	const Float radius;
	const Float zMin, zMax;
	const Float thetaMin, thetaMax, phiMax;
};

#endif

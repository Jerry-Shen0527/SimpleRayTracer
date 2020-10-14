#ifndef SPHERE_H
#define SPHERE_H

#include <Geometry/hittable.h>

#include "Shape.h"
//
//class sphere : public hittable {
//public:
//	sphere() {}
//	sphere(Point3f cen, float r, std::shared_ptr<material> m, bool pdf = false)
//		: center(cen), radius(r), mat_ptr(m) {
//		pdf_enabled = pdf;
//	}
//
//	virtual bool hit(const Ray& r, SurfaceInteraction& rec) const override;
//
//	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;
//
//	void get_sphere_uv(const Vector3f& p, Vector2f& uv) const;
//	float pdf_value(const Point3f& o, const Vector3f& v) const override;
//	Vector3f random(const Point3f& o) const override;
//
//public:
//
//	Point3f center;
//	float radius;
//	std::shared_ptr<material> mat_ptr;
//};

class Sphere :public Shape
{
public:
	Sphere(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation, Float radius, Float zMin, Float zMax, Float phiMax)
		: Shape(ObjectToWorld, WorldToObject, reverseOrientation), radius(radius), zMin(Clamp(std::min(zMin, zMax), -radius, radius)), zMax(Clamp(std::max(zMin, zMax), -radius, radius)), thetaMin(std::acos(Clamp(zMin / radius, -1, 1))), thetaMax(std::acos(Clamp(zMax / radius, -1, 1))), phiMax(Radians(Clamp(phiMax, 0, 360))) { }

	Bounds3f ObjectBound() const override;

	bool Intersect(const Ray& r, Float* tHit, SurfaceInteraction* isect, bool testAlphaTexture) const;

	Float Area() const override;
private:
	const Float radius;
	const Float zMin, zMax;
	const Float thetaMin, thetaMax, phiMax;

	std::shared_ptr<material> mat_ptr;
};

#endif

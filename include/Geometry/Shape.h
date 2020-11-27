#pragma once
#include <Geometry/Transform.h>

class Shape {
public:
	Shape(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation = false)
		: ObjectToWorld(ObjectToWorld), WorldToObject(WorldToObject), reverseOrientation(reverseOrientation), transformSwapsHandedness(ObjectToWorld->SwapsHandedness()) {	}

	virtual Float Area() const = 0;
	virtual Bounds3f ObjectBound() const = 0;
	Bounds3f WorldBound() const { return (*ObjectToWorld)(ObjectBound()); }

	virtual bool Intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect, bool testAlphaTexture = true) const = 0;

	virtual bool IntersectP(const Ray& ray, bool testAlphaTexture = true) const;

	const Transform* ObjectToWorld;
	const Transform* WorldToObject;
	const bool reverseOrientation;
	const bool transformSwapsHandedness;
};


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

	std::shared_ptr<Material> mat_ptr;
};
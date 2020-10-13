#pragma once
#include <Geometry/Transform.h>

class Shape {
public:
	Shape(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation = false) : ObjectToWorld(ObjectToWorld), WorldToObject(WorldToObject), reverseOrientation(reverseOrientation),
		transformSwapsHandedness(ObjectToWorld->SwapsHandedness()) {
	}

	virtual Float Area() const = 0;
	virtual Bounds3f ObjectBound() const = 0;
	virtual bool Intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect, bool testAlphaTexture = true) const = 0;

	virtual bool IntersectP(const Ray& ray, bool testAlphaTexture = true) const
	{
		Float tHit = ray.tMax;
		SurfaceInteraction isect;
		return Intersect(ray, &tHit, &isect, testAlphaTexture);
	}

	const Transform* ObjectToWorld;
	const Transform* WorldToObject;
	const bool reverseOrientation;
	const bool transformSwapsHandedness;
};

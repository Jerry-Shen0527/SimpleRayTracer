#ifndef RAY_H
#define RAY_H

#include <Tools/Math/Vector3.h>

#include "BRDF/BxDF_Utility.h"
#include "Tools/Math/EFloat.h"

class Medium;

class ray {
public:
	ray() {}

	ray(const Point3f& o, const Vector3f& d, Float tMax = infinity, Float time = 0.f, const Medium* medium = nullptr)
		: orig(o), dir(d), tMax(tMax), tm(time), medium(medium) { }

	Point3f origin() const { return orig; }
	Vector3f direction() const { return dir; }
	float time() const { return tm; }

	Point3f at(float t) const
	{
		return  t * dir + orig;
	}

public:
	Point3f orig;
	float tm;
	float tMax;
	Vector3f dir;
	const Medium* medium;
};

inline Point3f OffsetRayOrigin(const Point3f& p, const Vector3f& pError, const Normal3f& n, const Vector3f& w) {
	Float d = dot(n.abs(), pError);
	Vector3f offset = d * Vector3f(n);
	if (dot(w, n) < 0)
		offset = -offset;
	Point3f po = p + offset;

	for (int i = 0; i < 3; ++i) {
		if (offset[i] > 0) po[i] = NextFloatUp(po[i]);
		else if (offset[i] < 0) po[i] = NextFloatDown(po[i]);
	}

	return po;
}

#endif
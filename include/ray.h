#ifndef RAY_H
#define RAY_H

#include "Tools/Math/EFloat.h"

class Medium;

class Ray {
public:
	Ray() {}

	Ray(const Point3f& o, const Vector3f& d, Float tMax = Infinity, Float time = 0.f, const Medium* medium = nullptr)
		: o(o), d(d), tMax(tMax), time(time), medium(medium) { }

	Point3f operator()(Float t) const { return o + d * t; }

	Point3f origin() const { return o; }

	Point3f at(Float t) const
	{
		return   o + t * d;
	}

public:
	Point3f o;
	Float time;
	mutable Float tMax;
	Vector3f d;
	const Medium* medium;
};

inline Point3f OffsetRayOrigin(const Point3f& p, const Vector3f& pError, const Normal3f& n, const Vector3f& w) {
	Float d = Dot(n.Abs(), pError);
	Vector3f offset = d * Vector3f(n);
	if (Dot(w, n) < 0)
		offset = -offset;
	Point3f po = p + offset;

	for (int i = 0; i < 3; ++i) {
		if (offset[i] > 0) po[i] = NextFloatUp(po[i]);
		else if (offset[i] < 0) po[i] = NextFloatDown(po[i]);
	}

	return po;
}

class RayDifferential : public Ray {
public:
	RayDifferential() { hasDifferentials = false; }
	RayDifferential(const Point3f& o, const Vector3f& d, Float tMax = Infinity, Float time = 0.f, const Medium* medium = nullptr) : Ray(o, d, tMax, time, medium)
	{
		hasDifferentials = false;
	}

	RayDifferential(const Ray& ray) : Ray(ray) {
		hasDifferentials = false;
	}
	void ScaleDifferentials(Float s) {
		rxOrigin = o + (rxOrigin - o) * s;
		ryOrigin = o + (ryOrigin - o) * s;
		rxDirection = d + (rxDirection - d) * s;
		ryDirection = d + (ryDirection - d) * s;
	}

	bool hasDifferentials;
	Point3f rxOrigin, ryOrigin;
	Vector3f rxDirection, ryDirection;
};

#endif
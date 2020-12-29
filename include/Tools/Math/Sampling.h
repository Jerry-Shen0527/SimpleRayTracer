#pragma once
#include "math_tools.h"
#include <Tools/Math/rng.h>

#include "Geometry/Vector3.h"

//Uniformly sample in a disk
inline Point2f ConcentricSampleDisk(const Point2f& u)
{
	// Map uniform random numbers to $[-1,1]^2$
	Point2f uOffset = 2.f * u - Vector2f(1, 1);

	// Handle degeneracy at the origin
	if (uOffset.x() == 0 && uOffset.y() == 0) return Point2f(0, 0);

	// Apply concentric mapping to point
	Float theta, r;
	if (std::abs(uOffset.x()) > std::abs(uOffset.y())) {
		r = uOffset.x();
		theta = PiOver4 * (uOffset.y() / uOffset.x());
	}
	else {
		r = uOffset.y();
		theta = PiOver2 - PiOver4 * (uOffset.x() / uOffset.y());
	}
	return r * Point2f(std::cos(theta), std::sin(theta));
}

inline Vector3f UniformSampleHemisphere(const Point2f& u) {
	Float z = u[0];
	Float r = std::sqrt(std::max((Float)0, (Float)1. - z * z));
	Float phi = 2 * Pi * u[1];
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

inline Float UniformHemispherePdf() { return Inv2Pi; }

inline Vector3f UniformSampleSphere(const Point2f& u) {
	Float z = 1 - 2 * u[0];
	Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
	Float phi = 2 * Pi * u[1];
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}
#pragma once
#include "math_tools.h"
#include <Tools/Math/rng.h>
#include <Tools/Math/Vector3.h>

inline Vector3f random_in_unit_sphere() {
	while (true) {
		auto p = Vector3f::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

inline Vector3f random_unit_vector() {
	auto a = random_float(0, 2 * pi);
	auto z = random_float(-1, 1);
	auto r = sqrt(1 - z * z);
	return Vector3f(r * std::cosf(a), r * std::sinf(a), z);
}

inline Vector3f random_cosine_direction() {
	auto r1 = random_float();
	auto r2 = random_float();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return Vector3f(x, y, z);
}

inline Vector3f random_to_sphere(float radius, float distance_squared) {
	auto r1 = random_float();
	auto r2 = random_float();
	auto z = 1 + r2 * (sqrtf(1 - radius * radius / distance_squared) - 1);

	auto phi = 2 * pi * r1;
	auto x = cosf(phi) * sqrtf(1 - z * z);
	auto y = sinf(phi) * sqrtf(1 - z * z);

	return Vector3f(x, y, z);
}

inline Point2f ConcentricSampleDisk(const Point2f& u) {
	//Map uniform random numbers to[−1, 1]2 779
	Point2f uOffset = 2.f * u - Vector2f{ 1, 1 };

	//	Handle degeneracy at the origin 779
	//	Apply concentric mapping to point 779
	return u;
}
inline Vector3f random_in_unit_disk() {
	while (true) {
		auto p = Vector3f(random_float(-1, 1), random_float(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}
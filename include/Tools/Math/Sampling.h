#pragma once
#include "rng.h"
#include "vec3.h"
#include "math_tools.h"

inline vec3 random_in_unit_sphere() {
	while (true) {
		auto p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

inline vec3 random_unit_vector() {
	auto a = random_float(0, 2 * pi);
	auto z = random_float(-1, 1);
	auto r = sqrt(1 - z * z);
	return vec3(r * std::cosf(a), r * std::sinf(a), z);
}

inline vec3 random_cosine_direction() {
	auto r1 = random_float();
	auto r2 = random_float();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

inline vec3 random_to_sphere(float radius, float distance_squared) {
	auto r1 = random_float();
	auto r2 = random_float();
	auto z = 1 + r2 * (sqrtf(1 - radius * radius / distance_squared) - 1);

	auto phi = 2 * pi * r1;
	auto x = cosf(phi) * sqrtf(1 - z * z);
	auto y = sinf(phi) * sqrtf(1 - z * z);

	return vec3(x, y, z);
}

typedef point2 Point2f;
typedef vec2 Vector2f;

Point2f ConcentricSampleDisk(const Point2f& u) {
	//Map uniform random numbers to[−1, 1]2 779
	Point2f uOffset = 2.f * u - Vector2f{ 1, 1 };
	
	//	Handle degeneracy at the origin 779
	//	Apply concentric mapping to point 779
	return u;
}

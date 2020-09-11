#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
using std::make_shared;

#include "Tools/Math/math_tools.h"
#include "Tools/Math/rng.h"

#include "vec.h"

using std::sqrt;

class vec3 :public vec<3>
{
public:
	vec3() : vec<3>() {}
	vec3(vec<3>& in_vec) :vec<3>(in_vec) {}
	vec3(float e0, float e1, float e2)
	{
		data[0] = e0; data[1] = e1; data[2] = e2;
	}

	float x() const { return data[0]; }
	float y() const { return data[1]; }
	float z() const { return data[2]; }
};

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.data[1] * v.data[2] - u.data[2] * v.data[1],
		u.data[2] * v.data[0] - u.data[0] * v.data[2],
		u.data[0] * v.data[1] - u.data[1] * v.data[0]);
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

using color = vec3;
using point3 = vec3;   // 3D point

void vec3_test();

#endif

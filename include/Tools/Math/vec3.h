#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
using std::make_shared;

#include "Tools/Math/rng.h"

#include "vec.h"

using std::sqrt;

class vec3 :public vec<float, 3>
{
public:
	vec3() : vec<float, 3>() {}
	vec3(vec<float, 3>& in_vec) :vec<float, 3>(in_vec) {}
	vec3(float e0, float e1, float e2)
	{
		data[0] = e0; data[1] = e1; data[2] = e2;
	}
};

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.data[1] * v.data[2] - u.data[2] * v.data[1],
		u.data[2] * v.data[0] - u.data[0] * v.data[2],
		u.data[0] * v.data[1] - u.data[1] * v.data[0]);
}

using color = vec3;
using point3 = vec3;   // 3D point
using vec2 = vec<float, 2>;
using point2 = vec2;
using normal3 = vec3;
using Point2i = vec<int, 2>;

void vec3_test();

#endif

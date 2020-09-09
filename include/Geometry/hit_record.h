#pragma once
#include <ray.h>
#include <vec3.h>

class material;

struct hit_record {
	point3 p;
	vec3 normal;
	double u;
	double v;
	double t;
	bool front_face;
	std::shared_ptr<material> mat_ptr;
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

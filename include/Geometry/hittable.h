#ifndef HITTABLE_H
#define HITTABLE_H

#include <AABB/AABB.h>
#include <BRDF/material.h>

struct hit_record {
	point3 p;
	vec3 normal;
	double u;
	double v;
	double t;
	bool front_face;
	shared_ptr<material> mat_ptr;
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
	virtual double pdf_value(const point3& o, const vec3& v) const {
		return 0.0;
	}

	virtual vec3 random(const vec3& o) const {
		return vec3(1, 0, 0);
	}
};

inline bool hittable::bounding_box(double t0, double t1, aabb& output_box) const
{
	output_box = aabb(vec3(-DBL_MAX, -DBL_MAX, -DBL_MAX), vec3(DBL_MAX, DBL_MAX, DBL_MAX));
	return false;
}

#endif

#pragma once
#include <ray.h>
#include "hit_record.h"

class aabb;
class material;

class hittable {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, surface_hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& output_box) const;

	virtual float pdf_value(const point3& o, const vec3& v) const {
		return 0.0;
	}

	virtual vec3 random(const vec3& o) const {
		return vec3(1, 0, 0);
	}

	virtual bool get_pdf_enabled();

	bool pdf_enabled = false;
};

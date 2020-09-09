#pragma once
#include <ray.h>
#include <vec3.h>
#include <BRDF/material.h>

class aabb;



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

	virtual bool get_pdf_enabled();

	bool pdf_enabled = false;
};

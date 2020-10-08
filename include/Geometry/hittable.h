#pragma once
#include <ray.h>
#include "hit_record.h"

class aabb;
class Material;

class hittable {
public:
	virtual bool hit(const ray& r, surface_hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& output_box) const;

	virtual float pdf_value(const Point3f& o, const Vector3f& v) const {
		return 0.0;
	}

	virtual Vector3f random(const Vector3f& o) const {
		return Vector3f(1, 0, 0);
	}

	virtual bool get_pdf_enabled();

	bool pdf_enabled = false;
};

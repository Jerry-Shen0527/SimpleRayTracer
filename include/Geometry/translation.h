#pragma once
#include <Geometry/hittable.h>
#include <Geometry/bvh.h>

#undef min
#undef max

class translate : public hittable {
public:
	translate(std::shared_ptr<hittable> p, const vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(
		const ray& r, double t_min, double t_max, surface_hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> ptr;
	vec3 offset;
};


class rotate_y : public hittable {
public:
	rotate_y(std::shared_ptr<hittable> p, double angle);

	virtual bool hit(
		const ray& r, double t_min, double t_max, surface_hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override {
		output_box = bbox;
		return hasbox;
	}

public:
	std::shared_ptr<hittable> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	aabb bbox;
};

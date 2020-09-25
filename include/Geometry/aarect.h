#ifndef AARECT_H
#define AARECT_H
//do not change the consequence of this include!
#include "hittable.h"
#include <Geometry/bvh.h>

class xy_rect : public hittable {
public:
	xy_rect() {}

	xy_rect(float _x0, float _x1, float _y0, float _y1, float _k,
		std::shared_ptr<material> mat)
		: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

	virtual bool hit(const ray& r, float t0, float t1, surface_hit_record& rec) const override;
	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<material> mp;
	float x0, x1, y0, y1, k;
};

class xz_rect : public hittable {
public:
	xz_rect() {}

	xz_rect(float _x0, float _x1, float _z0, float _z1, float _k,
		std::shared_ptr<material> mat, bool pdf_ = false)
		: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {
		pdf_enabled = pdf_;
	}

	virtual bool hit(const ray& r, float t0, float t1, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override {
		// The bounding box must have non-zero width in each dimension, so pad the Y
		// dimension a small amount.
		output_box = aabb(point3(x0, k - 0.0001, z0), point3(x1, k + 0.0001, z1));
		return true;
	}

	virtual float pdf_value(const point3& origin, const vec3& v) const override;

	virtual vec3 random(const point3& origin) const override;

public:
	std::shared_ptr<material> mp;
	float x0, x1, z0, z1, k;
};

class yz_rect : public hittable {
public:
	yz_rect() {}

	yz_rect(float _y0, float _y1, float _z0, float _z1, float _k,
		std::shared_ptr<material> mat)
		: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool hit(const ray& r, float t0, float t1, surface_hit_record& rec) const override;
	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<material> mp;
	float y0, y1, z0, z1, k;
};

class flip_face : public hittable {
public:
	flip_face(std::shared_ptr<hittable> p) : ptr(p) {}

	virtual bool hit(
		const ray& r, float t_min, float t_max, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

	bool get_pdf_enabled() override;

	float pdf_value(const point3& o, const vec3& v) const override;
	vec3 random(const vec3& o) const override;

	std::shared_ptr<hittable> ptr;
};

#endif

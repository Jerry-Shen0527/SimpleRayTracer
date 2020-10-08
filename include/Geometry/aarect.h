#ifndef AARECT_H
#define AARECT_H
//do not change the consequence of this include!
#include "hittable.h"
#include <Geometry/bvh.h>

class xy_rect : public hittable {
public:
	xy_rect() {}

	xy_rect(float _x0, float _x1, float _y0, float _y1, float _k,
		std::shared_ptr<Material> mat)
		: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;
	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<Material> mp;
	float x0, x1, y0, y1, k;
};

class xz_rect : public hittable {
public:
	xz_rect() {}

	xz_rect(float _x0, float _x1, float _z0, float _z1, float _k,
		std::shared_ptr<Material> mat, bool pdf_ = false)
		: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {
		pdf_enabled = pdf_;
	}

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override {
		// The bounding box must have non-zero width in each dimension, so pad the Y
		// dimension a small amount.
		output_box = aabb(Point3f(x0, k - 0.0001, z0), Point3f(x1, k + 0.0001, z1));
		return true;
	}

	virtual float pdf_value(const Point3f& origin, const Vector3f& v) const override;

	virtual Vector3f random(const Point3f& origin) const override;

public:
	std::shared_ptr<Material> mp;
	float x0, x1, z0, z1, k;
};

class yz_rect : public hittable {
public:
	yz_rect() {}

	yz_rect(float _y0, float _y1, float _z0, float _z1, float _k,
		std::shared_ptr<Material> mat)
		: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;
	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<Material> mp;
	float y0, y1, z0, z1, k;
};

class flip_face : public hittable {
public:
	flip_face(std::shared_ptr<hittable> p) : ptr(p) {}

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

	bool get_pdf_enabled() override;

	float pdf_value(const Point3f& o, const Vector3f& v) const override;
	Vector3f random(const Vector3f& o) const override;

	std::shared_ptr<hittable> ptr;
};

#endif

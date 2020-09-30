#pragma once
#include <Geometry/hittable.h>
#include <Geometry/bvh.h>

#include "Tools/Math/matrix.h"

#undef min
#undef max

class Transform
{
public:
	Transform(const Float mat[4][4]);

	Transform(const Matrix4x4& m, const Matrix4x4& mInv);

	friend Transform Inverse(const Transform& t);

	friend Transform Transpose(const Transform& t);

private:
	Matrix4x4 m, mInv;
};

inline Transform Inverse(const Transform& t) {
	return Transform(t.mInv, t.m);
}

inline Transform Transpose(const Transform& t) {
	return Transform(Transpose(t.m), Transpose(t.mInv));
}

class translate : public hittable {
public:
	translate(std::shared_ptr<hittable> p, const vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(
		const ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> ptr;
	vec3 offset;
};

class rotate_y : public hittable {
public:
	rotate_y(std::shared_ptr<hittable> p, float angle);

	virtual bool hit(const ray& r,  surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override {
		output_box = bbox;
		return hasbox;
	}

public:
	std::shared_ptr<hittable> ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	aabb bbox;
};

#pragma once
#include <Geometry/hittable.h>
#include <Geometry/bvh.h>

#include "Tools/Math/matrix.h"

#undef min
#undef max

class Transform
{
public:
	Transform(const Float mat[4][4]) {
		m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
		mInv = Inverse(m);
	}

	Transform(const Matrix4x4& m, const Matrix4x4& mInv)
		: m(m), mInv(mInv) {
	}

	friend Transform Inverse(const Transform& t) {
		return Transform(t.mInv, t.m);
	}

	friend Transform Transpose(const Transform& t) {
		return Transform(Transpose(t.m), Transpose(t.mInv));
	}

private:
	Matrix4x4 m, mInv;
};

class translate : public hittable {
public:
	translate(std::shared_ptr<hittable> p, const vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(
		const ray& r, float t_min, float t_max, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> ptr;
	vec3 offset;
};

class rotate_y : public hittable {
public:
	rotate_y(std::shared_ptr<hittable> p, float angle);

	virtual bool hit(
		const ray& r, float t_min, float t_max, surface_hit_record& rec) const override;

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

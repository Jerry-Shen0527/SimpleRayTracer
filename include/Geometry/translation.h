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
	template <typename T>
	inline vec<T, 3> operator()(const vec<T, 3>& v) const;

	bool SwapsHandedness() const;
private:
	Matrix4x4 m, mInv;
};

template <typename T>
vec<T, 3> Transform::operator()(const vec<T, 3>& v) const
{
	T x = v.x, y = v.y, z = v.z;
	return Vector3<T>(m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
		m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
		m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
}

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

	virtual bool hit(const ray& r, surface_hit_record& rec) const override;

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

inline Transform Translate(const Vector3f& delta) {
	Matrix4x4 m(1, 0, 0, delta.x(),
		0, 1, 0, delta.y(),
		0, 0, 1, delta.z(),
		0, 0, 0, 1);
	Matrix4x4 minv(1, 0, 0, -delta.x(),
		0, 1, 0, -delta.y(),
		0, 0, 1, -delta.z(),
		0, 0, 0, 1);
	return Transform(m, minv);
}
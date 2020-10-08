#pragma once
#include <Geometry/hittable.h>
#include <Geometry/bvh.h>

#include "Tools/Bound.h"
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
	Vector<T, 3> operator()(const Vector<T, 3>& v) const;
	template <typename T>
	inline Point<T, 3> operator()(const Point<T, 3>& p) const;

	Bounds3f operator()(const Bounds3f& b) const;
	ray operator()(const ray& r) const;

	SurfaceInteraction operator()(const SurfaceInteraction& si) const;

	bool HasScale() const;

	bool SwapsHandedness() const;
private:
	Matrix4x4 m, mInv;
};

template <typename T>
Vector<T, 3> Transform::operator()(const Vector<T, 3>& v) const
{
	T x = v.x(), y = v.y(), z = v.z();
	return Vector<T, 3>(m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
		m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
		m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
}

template <typename T>
Point<T, 3> Transform::operator()(const Point<T, 3>& p) const
{
	T x = p.x(), y = p.y(), z = p.z();
	T xp = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
	T yp = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
	T zp = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
	T wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
	if (wp == 1) return Point<T,3>(xp, yp, zp);
	else return Point<T,3>(xp, yp, zp) / wp;
}

inline Transform Inverse(const Transform& t) {
	return Transform(t.mInv, t.m);
}

inline Transform Transpose(const Transform& t) {
	return Transform(Transpose(t.m), Transpose(t.mInv));
}

class translate : public hittable {
public:
	translate(std::shared_ptr<hittable> p, const Vector3f& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(
		const ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> ptr;
	Vector3f offset;
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

inline Transform Scale(Float x, Float y, Float z) {
	Matrix4x4 m(x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1);
	Matrix4x4 minv(1 / x, 0, 0, 0,
		0, 1 / y, 0, 0,
		0, 0, 1 / z, 0,
		0, 0, 0, 1);
	return Transform(m, minv);
}

inline Transform RotateX(Float theta) {
	Float sinTheta = std::sin(Radians(theta));
	Float cosTheta = std::cos(Radians(theta));
	Matrix4x4 m(1, 0, 0, 0,
		0, cosTheta, -sinTheta, 0,
		0, sinTheta, cosTheta, 0,
		0, 0, 0, 1);
	return Transform(m, Transpose(m));
}

//Here theta is measured by degrees.
inline Transform Rotate(Float theta, const Vector3f& axis) {
	Vector3f a = axis.normalize();
	Float sinTheta = std::sin(Radians(theta));
	Float cosTheta = std::cos(Radians(theta));
	Matrix4x4 m;
	//Compute rotation of first basis vector 91
	m.m[0][0] = a.x() * a.x() + (1 - a.x() * a.x()) * cosTheta;
	m.m[0][1] = a.x() * a.y() * (1 - cosTheta) - a.z() * sinTheta;
	m.m[0][2] = a.x() * a.z() * (1 - cosTheta) + a.y() * sinTheta;
	m.m[0][3] = 0;
	//Compute rotations of second and third basis vectors
	return Transform(m, Transpose(m));
}
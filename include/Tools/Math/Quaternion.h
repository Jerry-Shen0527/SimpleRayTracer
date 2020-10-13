#pragma once
#include "config.h"
#include "Vector3.h"

class Quaternion
{
public:
	Quaternion() : v(0, 0, 0), w(1) { }

	Quaternion& operator+=(const Quaternion& q) {
		v += q.v;
		w += q.w;
		return *this;
	}

	Quaternion operator/(Float f) const
	{
		Quaternion rst(*this);
		rst.v /= f;
		rst.w /= f;
		return rst;
	}

	//Interface to transform
	Quaternion(const Transform& t);
	Transform ToTransform() const;

	Vector3f v;
	Float w;
};

inline Float Dot(const Quaternion& q1, const Quaternion& q2) {
	return Dot(q1.v, q2.v) + q1.w * q2.w;
}

inline Quaternion Normalize(const Quaternion& q) {
	return q / std::sqrt(Dot(q, q));
}
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
	friend Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
		Quaternion ret = q1;
		return ret += q2;
	}
	Quaternion operator-() const {
		Quaternion ret;
		ret.v = -v;
		ret.w = -w;
		return ret;
	}
	Quaternion& operator-=(const Quaternion& q) {
		v -= q.v;
		w -= q.w;
		return *this;
	}

	Quaternion operator*(Float f) const {
		Quaternion ret = *this;
		ret.v *= f;
		ret.w *= f;
		return ret;
	}
	friend Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
		Quaternion ret = q1;
		return ret -= q2;
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

inline Quaternion operator*(Float f, const Quaternion& q) { return q * f; }

Quaternion Slerp(Float t, const Quaternion& q1, const Quaternion& q2) {
	Float cosTheta = Dot(q1, q2);
	if (cosTheta > .9995f)
		return Normalize((1 - t) * q1 + t * q2);
	else {
		Float theta = std::acos(Clamp(cosTheta, -1, 1));
		Float thetap = theta * t;
		Quaternion qperp = Normalize(q2 - q1 * cosTheta);
		return q1 * std::cos(thetap) + qperp * std::sin(thetap);
	}
}
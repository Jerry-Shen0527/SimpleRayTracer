#pragma once
#include "Tools/Math/matrix.h"
#include <Tools/Bound.h>

#include "Interaction.h"

class Transform
{
public:
	Transform() {}
	Transform(const Float mat[4][4]);
	Transform(const Matrix4x4& m) : m(m), mInv(Inverse(m)) {}
	Transform(const Matrix4x4& m, const Matrix4x4& mInv);

	friend Transform Inverse(const Transform& t);
	friend Transform Transpose(const Transform& t);

	bool operator==(const Transform& t) const {
		return t.m == m && t.mInv == mInv;
	}
	bool operator!=(const Transform& t) const {
		return t.m != m || t.mInv != mInv;
	}

	template <typename T>
	Vector<T, 3> operator()(const Vector<T, 3>& v) const;
	template <typename T>
	inline Point<T, 3> operator()(const Point<T, 3>& p) const;

	Bounds3f operator()(const Bounds3f& b) const;
	Ray operator()(const Ray& r) const;

	Transform operator*(const Transform& t2) const;

	SurfaceInteraction operator()(const SurfaceInteraction& si) const;

	bool HasScale() const;

	bool SwapsHandedness() const;

	bool IsIdentity() const;
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
	if (wp == 1) return Point<T, 3>(xp, yp, zp);
	else return Point<T, 3>(xp, yp, zp) / wp;
}

inline Transform Inverse(const Transform& t) {
	return Transform(t.mInv, t.m);
}

inline Transform Transpose(const Transform& t) {
	return Transform(Transpose(t.m), Transpose(t.mInv));
}

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

// Interval Definitions
class Interval {
public:
	// Interval Public Methods
	Interval(Float v) : low(v), high(v) {}
	Interval(Float v0, Float v1)
		: low(std::min(v0, v1)), high(std::max(v0, v1)) {}
	Interval operator+(const Interval& i) const {
		return Interval(low + i.low, high + i.high);
	}
	Interval operator-(const Interval& i) const {
		return Interval(low - i.high, high - i.low);
	}
	Interval operator*(const Interval& i) const {
		return Interval(std::min(std::min(low * i.low, high * i.low),
			std::min(low * i.high, high * i.high)),
			std::max(std::max(low * i.low, high * i.low),
				std::max(low * i.high, high * i.high)));
	}
	Float low, high;
};

inline Interval Sin(const Interval& i) {
	Float sinLow = std::sin(i.low), sinHigh = std::sin(i.high);
	if (sinLow > sinHigh) std::swap(sinLow, sinHigh);
	if (i.low < Pi / 2 && i.high > Pi / 2) sinHigh = 1.;
	if (i.low < (3.f / 2.f) * Pi && i.high >(3.f / 2.f) * Pi) sinLow = -1.;
	return Interval(sinLow, sinHigh);
}

inline Interval Cos(const Interval& i) {
	Float cosLow = std::cos(i.low), cosHigh = std::cos(i.high);
	if (cosLow > cosHigh) std::swap(cosLow, cosHigh);
	if (i.low < Pi && i.high > Pi) cosLow = -1.;
	return Interval(cosLow, cosHigh);
}

inline void IntervalFindZeros(Float c1, Float c2, Float c3, Float c4, Float c5,
	Float theta, Interval tInterval, Float* zeros,
	int* zeroCount, int depth = 8) {
	// Evaluate motion derivative in interval form, return if no zeros
	Interval range = Interval(c1) +
		(Interval(c2) + Interval(c3) * tInterval) *
		Cos(Interval(2 * theta) * tInterval) +
		(Interval(c4) + Interval(c5) * tInterval) *
		Sin(Interval(2 * theta) * tInterval);
	if (range.low > 0. || range.high < 0. || range.low == range.high) return;
	if (depth > 0) {
		// Split _tInterval_ and check both resulting intervals
		Float mid = (tInterval.low + tInterval.high) * 0.5f;
		IntervalFindZeros(c1, c2, c3, c4, c5, theta,
			Interval(tInterval.low, mid), zeros, zeroCount,
			depth - 1);
		IntervalFindZeros(c1, c2, c3, c4, c5, theta,
			Interval(mid, tInterval.high), zeros, zeroCount,
			depth - 1);
	}
	else {
		// Use Newton's method to refine zero
		Float tNewton = (tInterval.low + tInterval.high) * 0.5f;
		for (int i = 0; i < 4; ++i) {
			Float fNewton =
				c1 + (c2 + c3 * tNewton) * std::cos(2.f * theta * tNewton) +
				(c4 + c5 * tNewton) * std::sin(2.f * theta * tNewton);
			Float fPrimeNewton = (c3 + 2 * (c4 + c5 * tNewton) * theta) *
				std::cos(2.f * tNewton * theta) +
				(c5 - 2 * (c2 + c3 * tNewton) * theta) *
				std::sin(2.f * tNewton * theta);
			if (fNewton == 0 || fPrimeNewton == 0) break;
			tNewton = tNewton - fNewton / fPrimeNewton;
		}
		if (tNewton >= tInterval.low - 1e-3f &&
			tNewton < tInterval.high + 1e-3f) {
			zeros[*zeroCount] = tNewton;
			(*zeroCount)++;
		}
	}
}
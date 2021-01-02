#pragma once

// Constants

#include "Geometry/Vector3.h"



inline Float degrees_to_radians(Float degrees) {
	return degrees * Pi / 180.0;
}

inline Float Clamp(Float x, Float min, Float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline void idx_to_ij(int idx, int& i, int& j, int width)
{
	i = idx % width;
	j = idx / width;
}

inline Float Lerp(Float t, Float v1, Float v2) { return (1 - t) * v1 + t * v2; }

inline bool Quadratic(Float a, Float b, Float c, Float* t0, Float* t1) {
	//Find quadratic discriminant 1079
	//	Compute quadratic t values 1080
	double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
	if (discrim < 0) return false;
	double rootDiscrim = std::sqrt(discrim);

	double q;
	if (b < 0) q = -.5 * (b - rootDiscrim);
	else q = -.5 * (b + rootDiscrim);
	*t0 = q / a;
	*t1 = c / q;
	if (*t0 > *t1) std::swap(*t0, *t1);
	return true;
}

inline bool SolveLinearSystem2x2(const Float A[2][2],
	const Float B[2], Float* x0, Float* x1) {
	Float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
	if (std::abs(det) < 1e-10f)
		return false;
	*x0 = (A[1][1] * B[0] - A[0][1] * B[1]) / det;
	*x1 = (A[0][0] * B[1] - A[1][0] * B[0]) / det;
	if (std::isnan(*x0) || std::isnan(*x1))
		return false;
	return true;
}

inline Float SphericalTheta(const Vector3f& v) {
	return std::acos(Clamp(v.z(), -1, 1));
}

inline Float SphericalPhi(const Vector3f& v) {
	Float p = std::atan2(v.y(), v.x());
	return (p < 0) ? (p + 2 * Pi) : p;
}

template <typename T>
inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2) {
	return std::abs(Dot(v1, v2));
}

inline Float Erf(Float x) {
	// constants
	Float a1 = 0.254829592f;
	Float a2 = -0.284496736f;
	Float a3 = 1.421413741f;
	Float a4 = -1.453152027f;
	Float a5 = 1.061405429f;
	Float p = 0.3275911f;

	// Save the sign of x
	int sign = 1;
	if (x < 0) sign = -1;
	x = std::abs(x);

	// A&S formula 7.1.26
	Float t = 1 / (1 + p * x);
	Float y =
		1 -
		(((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

	return sign * y;
}

inline Float ErfInv(Float x) {
	Float w, p;
	x = Clamp(x, -.99999f, .99999f);
	w = -std::log((1 - x) * (1 + x));
	if (w < 5) {
		w = w - 2.5f;
		p = 2.81022636e-08f;
		p = 3.43273939e-07f + p * w;
		p = -3.5233877e-06f + p * w;
		p = -4.39150654e-06f + p * w;
		p = 0.00021858087f + p * w;
		p = -0.00125372503f + p * w;
		p = -0.00417768164f + p * w;
		p = 0.246640727f + p * w;
		p = 1.50140941f + p * w;
	}
	else {
		w = std::sqrt(w) - 3;
		p = -0.000200214257f;
		p = 0.000100950558f + p * w;
		p = 0.00134934322f + p * w;
		p = -0.00367342844f + p * w;
		p = 0.00573950773f + p * w;
		p = -0.0076224613f + p * w;
		p = 0.00943887047f + p * w;
		p = 1.00167406f + p * w;
		p = 2.83297682f + p * w;
	}
	return p * x;
}

template <typename Predicate>
int FindInterval(int size, const Predicate& pred) {
	int first = 0, len = size;
	while (len > 0) {
		int half = len >> 1, middle = first + half;
		// Bisect range based on value of _pred_ at _middle_
		if (pred(middle)) {
			first = middle + 1;
			len -= half + 1;
		}
		else
			len = half;
	}
	return Clamp(first - 1, 0, size - 2);
}
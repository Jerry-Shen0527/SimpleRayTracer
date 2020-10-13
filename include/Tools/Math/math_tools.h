#pragma once

// Constants
#include <config.h>

const Float Infinity = std::numeric_limits<Float>::infinity();
const Float pi = 3.1415926535897932385f;
const Float inv_pi = 1 / pi;

inline Float degrees_to_radians(Float degrees) {
	return degrees * pi / 180.0;
}

namespace tool
{
	inline Float Clamp(Float x, Float min, Float max) {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}
}
using namespace tool;

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
	if (*t0 > * t1) std::swap(*t0, *t1);
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
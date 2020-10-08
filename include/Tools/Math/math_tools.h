#pragma once

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;
const float inv_pi = 1 / pi;

inline float degrees_to_radians(float degrees) {
	return degrees * pi / 180.0;
}

inline float Clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline void idx_to_ij(int idx, int& i, int& j, int width)
{
	i = idx % width;
	j = idx / width;
}

inline float Lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }

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
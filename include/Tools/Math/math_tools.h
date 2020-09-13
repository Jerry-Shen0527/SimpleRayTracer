#pragma once
#include <limits>

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;
const float inv_pi = 1/pi;

inline float degrees_to_radians(float degrees) {
	return degrees * pi / 180.0;
}

inline float clamp(float x, float min, float max) {
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

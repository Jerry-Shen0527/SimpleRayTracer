#pragma once
#include <limits>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385f;
const double inv_pi = 1/pi;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline void idx_to_ij(int idx, int& i, int& j, int width)
{
	i = idx % width;
	j = idx / width;
}

inline double Lerp(double t, double v1, double v2) { return (1 - t) * v1 + t * v2; }

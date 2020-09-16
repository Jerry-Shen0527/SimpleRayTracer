#pragma once
#include <cstdlib>

//inline double random_double() {
//	// Returns a random real in [0,1).
//	return rand() / (RAND_MAX + 1.0);
//}
//
//inline double random_double(double min, double max) {
//	// Returns a random real in [min,max).
//	return min + (max - min) * random_double();
//}

inline double random_double() {
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	// Returns a random real in [min,max).
	return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
	// Returns a random integer in [min,max].
	return static_cast<int>(random_double(min, max + 1));
}


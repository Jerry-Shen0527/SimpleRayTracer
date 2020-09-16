#pragma once
#include <Tools/Math/Sampling.h>

#include "Tools/Math/vec3.h"

inline double CosTheta(const vec3& w) { return w.z(); }
inline double Cos2Theta(const vec3& w) { return w.z() * w.z(); }
inline double AbsCosTheta(const vec3& w) { return std::abs(w.z()); }

inline double Sin2Theta(const vec3& w) {
	return std::max((double)0, (double)1 - Cos2Theta(w));
}
inline double SinTheta(const vec3& w) {
	return std::sqrt(Sin2Theta(w));
}

inline double TanTheta(const vec3& w) {
	return SinTheta(w) / CosTheta(w);
}
inline double Tan2Theta(const vec3& w) {
	return Sin2Theta(w) / Cos2Theta(w);
}

inline double CosPhi(const vec3& w) {
	double sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : clamp(w.x() / sinTheta, -1, 1);
}
inline double SinPhi(const vec3& w) {
	double sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : clamp(w.y() / sinTheta, -1, 1);
}

inline double Cos2Phi(const vec3& w) {
	return CosPhi(w) * CosPhi(w);
}
inline double Sin2Phi(const vec3& w) {
	return SinPhi(w) * SinPhi(w);
}

//Calculate different phi between two vectors
inline double CosDPhi(const vec3& wa, const vec3& wb) {
	return clamp((wa.x() * wb.x() + wa.y() * wb.y()) / std::sqrt((wa.x() * wa.x() + wa.y() * wa.y()) * (wb.x() * wb.x() + wb.y() * wb.y())), -1, 1);
}

typedef double Float;
typedef vec3 Vector3f;

inline normal3
Faceforward(const normal3& n, const vec3& v) {
	return (dot(n, v) < 0.f) ? -n : n;
}

inline Vector3f CosineSampleHemisphere(const point2& u) {
	point2 d = ConcentricSampleDisk(u);
	Float z = std::sqrt(std::max((Float)0, 1 - d.x() * d.x() - d.y() * d.y()));
	return Vector3f(d.x(), d.y(), z);
}

inline bool SameHemisphere(const Vector3f& w, const Vector3f& wp) {
return w.z() * wp.z() > 0;
}
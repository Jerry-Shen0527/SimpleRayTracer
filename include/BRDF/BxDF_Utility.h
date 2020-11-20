#pragma once
#include <Tools/Math/Sampling.h>
#include <Tools/Math/Vector3.h>

inline float CosTheta(const Vector3f& w) { return w.z(); }
inline float Cos2Theta(const Vector3f& w) { return w.z() * w.z(); }
inline float AbsCosTheta(const Vector3f& w) { return std::abs(w.z()); }

inline float Sin2Theta(const Vector3f& w) {
	return std::max((float)0, (float)1 - Cos2Theta(w));
}
inline float SinTheta(const Vector3f& w) {
	return std::sqrt(Sin2Theta(w));
}

inline float TanTheta(const Vector3f& w) {
	return SinTheta(w) / CosTheta(w);
}
inline float Tan2Theta(const Vector3f& w) {
	return Sin2Theta(w) / Cos2Theta(w);
}

inline float CosPhi(const Vector3f& w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : Clamp(w.x() / sinTheta, -1, 1);
}
inline float SinPhi(const Vector3f& w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : Clamp(w.y() / sinTheta, -1, 1);
}

inline float Cos2Phi(const Vector3f& w) {
	return CosPhi(w) * CosPhi(w);
}
inline float Sin2Phi(const Vector3f& w) {
	return SinPhi(w) * SinPhi(w);
}

//Calculate different phi between two vectors
inline float CosDPhi(const Vector3f& wa, const Vector3f& wb) {
	return Clamp((wa.x() * wb.x() + wa.y() * wb.y()) / std::sqrt((wa.x() * wa.x() + wa.y() * wa.y()) * (wb.x() * wb.x() + wb.y() * wb.y())), -1, 1);
}

inline Normal3f Faceforward(const Normal3f& n, const Vector3f& v) {
	return Dot(n, v) < 0.f ? -n : n;
}

inline Vector3f CosineSampleHemisphere(const Point2f& u) {
	Point2f d = ConcentricSampleDisk(u);
	Float z = std::sqrt(std::max((Float)0, 1 - d.x() * d.x() - d.y() * d.y()));
	return Vector3f(d.x(), d.y(), z);
}

inline bool SameHemisphere(const Vector3f& w, const Vector3f& wp) {
	return w.z() * wp.z() > 0;
}
inline Float Radians(Float deg) { return (Pi / 180) * deg; }
#pragma once
#include "Tools/Math/vec3.h"

inline float CosTheta(const vec3& w) { return w.z(); }
inline float Cos2Theta(const vec3& w) { return w.z() * w.z(); }
inline float AbsCosTheta(const vec3& w) { return std::abs(w.z()); }

inline float Sin2Theta(const vec3& w) {
	return std::max((float)0, (float)1 - Cos2Theta(w));
}
inline float SinTheta(const vec3& w) {
	return std::sqrt(Sin2Theta(w));
}

inline float TanTheta(const vec3& w) {
	return SinTheta(w) / CosTheta(w);
}
inline float Tan2Theta(const vec3& w) {
	return Sin2Theta(w) / Cos2Theta(w);
}

inline float CosPhi(const vec3& w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : clamp(w.x() / sinTheta, -1, 1);
}
inline float SinPhi(const vec3& w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : clamp(w.y() / sinTheta, -1, 1);
}

inline float Cos2Phi(const vec3& w) {
	return CosPhi(w) * CosPhi(w);
}
inline float Sin2Phi(const vec3& w) {
	return SinPhi(w) * SinPhi(w);
}

//Calculate different phi between two vectors
inline float CosDPhi(const vec3& wa, const vec3& wb) {
	return clamp((wa.x() * wb.x() + wa.y() * wb.y()) / std::sqrt((wa.x() * wa.x() + wa.y() * wa.y()) * (wb.x() * wb.x() + wb.y() * wb.y())), -1, 1);
}

typedef float Float;
typedef vec3 Vector3f;

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {
	cosThetaI = clamp(cosThetaI, -1, 1);

	bool entering = cosThetaI > 0.f;
	if (!entering) {
		std::swap(etaI, etaT);
		cosThetaI = std::abs(cosThetaI);
	}

	Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
	Float sinThetaT = etaI / etaT * sinThetaI;

	Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));

	if (sinThetaT >= 1)
		return 1;

	Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
		((etaT * cosThetaI) + (etaI * cosThetaT));
	Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
		((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl * Rparl + Rperp * Rperp) / 2;
}

inline Vector3f Reflect(const Vector3f& wo, const Vector3f& n) {
	return -wo + 2 * dot(wo, n) * n;
}
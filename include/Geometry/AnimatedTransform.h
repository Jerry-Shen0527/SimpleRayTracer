#pragma once
#include "Transform.h"
#include <Tools/Math/Quaternion.h>

class AnimatedTransform {
public:
	// AnimatedTransform Public Methods
	AnimatedTransform(const Transform* startTransform, Float startTime,
		const Transform* endTransform, Float endTime);
	static void Decompose(const Matrix4x4& m, Vector3f* T, Quaternion* R,
		Matrix4x4* S);
	void Interpolate(Float time, Transform* t) const;
	Ray operator()(const Ray& r) const;
	RayDifferential operator()(const RayDifferential& r) const;
	Point3f operator()(Float time, const Point3f& p) const;
	Vector3f operator()(Float time, const Vector3f& v) const;
	bool HasScale() const {
		return startTransform->HasScale() || endTransform->HasScale();
	}
	Bounds3f MotionBounds(const Bounds3f& b) const;
	Bounds3f BoundPointMotion(const Point3f& p) const;

private:
	// AnimatedTransform Private Data
	const Transform* startTransform, * endTransform;
	const Float startTime, endTime;
	const bool actuallyAnimated;
	Vector3f T[2];
	Quaternion R[2];
	Matrix4x4 S[2];
	bool hasRotation;
	struct DerivativeTerm {
		DerivativeTerm() {}
		DerivativeTerm(Float c, Float x, Float y, Float z)
			: kc(c), kx(x), ky(y), kz(z) {}
		Float kc, kx, ky, kz;
		Float Eval(const Point3f& p) const {
			return kc + kx * p.x() + ky * p.y() + kz * p.z();
		}
	};
	DerivativeTerm c1[3], c2[3], c3[3], c4[3], c5[3];
};
#pragma once
#include <cassert>

#include "ray.h"
#include "BRDF/Medium.h"

const Float ShadowEpsilon = 0.0001f;

class Interaction
{
public:
	Interaction() {}
	Interaction(const Point3f& p, const Normal3f& n, const Vector3f& pError, const Vector3f& r_in, Float time, const MediumInterface& mediumInterface)
		: p(p), t(time), pError(pError), wo(r_in), n(n), time(time), mediumInterface(mediumInterface)
	{ }

	Interaction(const Point3f& p, Float time, const MediumInterface& mediumInterface)
		: p(p), time(time), mediumInterface(mediumInterface) {}

	bool IsSurfaceInteraction() const {
		return n != Normal3f();
	}

	Point3f p;
	float t;
	Normal3f n;
	Vector3f wo;
	float time;

	Vector3f pError;

	Ray SpawnRay(const Vector3f& d) const;

	Ray SpawnRayTo(const Point3f& p2) const;
	Ray SpawnRayTo(const Interaction& h2) const;

	const Medium* GetMedium(const Vector3f& w) const {
		return Dot(w, n) > 0 ? mediumInterface.outside : mediumInterface.inside;
	}

	const Medium* GetMedium() const {
		assert(mediumInterface.inside == mediumInterface.outside);
		return mediumInterface.inside;
	}

public:
	MediumInterface mediumInterface;

	BSDF* bsdf = nullptr;
	BSSRDF* bssrdf = nullptr;
};

class Material;
class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}

	std::shared_ptr<Material> mat_ptr;

	SurfaceInteraction(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& r_in, Float time) :Interaction(p, Normal3f((Cross(dpdu, dpdv)).Normalize()), pError, r_in, time, nullptr) {}

	SurfaceInteraction(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, Float time, const Shape* shape);

	void ComputeScatteringFunctions(const RayDifferential& ray, MemoryArena& arena, bool allowMultipleLobes = false, TransportMode mode = TransportMode::Radiance);

	void ComputeDifferentials(const RayDifferential& ray) const;

	void set_face_normal(const Vector3f& r_in, const Normal3f& outward_normal);

	void SetShadingGeometry(const Vector3f& dpdus,
		const Vector3f& dpdvs, const Normal3f& dndus,
		const Normal3f& dndvs, bool orientationIsAuthoritative);

	Point2f uv;
	Vector3f dpdu, dpdv;
	Normal3f dndu, dndv;

	mutable Vector3f dpdx, dpdy;
	mutable Float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

	bool front_face;

	struct {
		Normal3f n;
		Vector3f dpdu, dpdv;
		Normal3f dndu, dndv;
	} shading;

	Spectrum Le(const Vector3f& w) const;
	const Primitive* primitive = nullptr;
	const Shape* shape = nullptr;
	int faceIndex = 0;
};

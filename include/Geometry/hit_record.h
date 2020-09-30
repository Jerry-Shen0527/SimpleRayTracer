#pragma once
#include <BRDF/material.h>
#include <BRDF/Volume/Medium.h>
const Float ShadowEpsilon = 0.0001f;


struct hit_record
{
	hit_record() {}
	hit_record(const Point3f& p, const Normal3f& n, const Vector3f& pError, const Vector3f& r_in, Float time)
		: p(p), t(time), pError(pError), ray_in(r_in), normal(n), time(time)
	{ }

	hit_record(const Point3f& p, Float time,
		const MediumInterface& mediumInterface)
		: p(p), time(time), mediumInterface(mediumInterface) {}

	bool IsSurfaceInteraction() const {
		return normal != Normal3f();
	}
	Point3f p;
	float t;
	Normal3f normal;
	Vector3f ray_in;
	float time;

	Vector3f pError;

	ray SpawnRay(const Vector3f& d) const;

	ray SpawnRayTo(const Point3f& p2) const;
	ray SpawnRayTo(const hit_record& h2) const;

	const Medium* GetMedium(const Vector3f& w) const {
		return dot(w, normal) > 0 ? mediumInterface.outside : mediumInterface.inside;
	}

	const Medium* GetMedium() const {
		assert(mediumInterface.inside == mediumInterface.outside);
		return mediumInterface.inside;
	}

public:
	MediumInterface mediumInterface;
};


struct surface_hit_record :public hit_record {
	surface_hit_record() {}

	std::shared_ptr<material> mat_ptr;
	inline void set_face_normal(const vec3& r_in, const vec3& outward_normal);

	surface_hit_record(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& r_in, Float time) :hit_record(p, Normal3f((cross(dpdu, dpdv)).normalize()), pError, r_in, time) {}

	surface_hit_record(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, Float time);

	Point2f uv;
	Vector3f dpdu, dpdv;
	Normal3f dndu, dndv;

	bool front_face;

	struct {
		Normal3f n;
		Vector3f dpdu, dpdv;
		Normal3f dndu, dndv;
	} shading;

	Spectrum Le(const Vector3f& w) const;
};


using Interaction = hit_record;
using  SurfaceInteraction = surface_hit_record;
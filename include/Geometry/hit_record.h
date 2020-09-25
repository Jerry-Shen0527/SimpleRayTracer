#pragma once
#include <ray.h>

#include "BRDF/material.h"

struct hit_record
{
	hit_record() {}
	hit_record(const Point3f& p, const Normal3f& n, const Vector3f& pError,
		const Vector3f& r_in, Float time)
		: p(p), t(time), pError(pError), ray_in(r_in), normal(n)
	{ }

	bool IsSurfaceInteraction() const {
		return normal != Normal3f();
	}
	Point3f p;
	float t;
	Normal3f normal;
	Vector3f ray_in;

	Vector3f pError;

	ray SpawnRay(const Vector3f& d) const {
		Point3f o = OffsetRayOrigin(p, pError, normal, d);
		return ray(o, d, infinity, time, GetMedium(d));
	}
	ray SpawnRayTo(const Point3f& p2) const {
		Point3f origin = OffsetRayOrigin(p, pError, n, p2 - p);
		Vector3f d = p2 - p;
		return ray(origin, d, 1 - ShadowEpsilon, time, GetMedium(d));
	}
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
};

inline void surface_hit_record::set_face_normal(const vec3& r_in, const vec3& outward_normal)
{
	front_face = dot(r_in, outward_normal) < 0;
	normal = front_face ? outward_normal : -outward_normal;
}

inline surface_hit_record::surface_hit_record(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, Float time) : hit_record(p, Normal3f((cross(dpdu, dpdv)).normalize()), pError, wo, time), uv(uv), dpdu(dpdu), dpdv(dpdv), dndu(dndu), dndv(dndv)
{
	set_face_normal(wo, normal);

	shading.n = normal;
	shading.dpdu = dpdu;
	shading.dpdv = dpdv;
	shading.dndu = dndu;
	shading.dndv = dndv;
}

using Interaction = hit_record;
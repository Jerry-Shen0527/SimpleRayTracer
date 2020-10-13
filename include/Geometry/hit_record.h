#pragma once
#include <BRDF/Material.h>
#include <BRDF/Volume/Medium.h>
#include <Geometry/Shape.h>

class Primitive;
const Float ShadowEpsilon = 0.0001f;
class BSDF;
class BSSRDF;

struct hit_record
{
	hit_record() {}
	hit_record(const Point3f& p, const Normal3f& n, const Vector3f& pError, const Vector3f& r_in, Float time)
		: p(p), t(time), pError(pError), ray_in(r_in), n(n), time(time)
	{ }

	hit_record(const Point3f& p, Float time, const MediumInterface& mediumInterface)
		: p(p), time(time), mediumInterface(mediumInterface) {}

	bool IsSurfaceInteraction() const {
		return n != Normal3f();
	}
	Point3f p;
	float t;
	Normal3f n;
	Vector3f ray_in;
	float time;

	Vector3f pError;

	Ray SpawnRay(const Vector3f& d) const;

	Ray SpawnRayTo(const Point3f& p2) const;
	Ray SpawnRayTo(const hit_record& h2) const;

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

struct SurfaceInteraction :public hit_record {
	SurfaceInteraction() {}

	std::shared_ptr<material> mat_ptr;

	SurfaceInteraction(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& r_in, Float time) :hit_record(p, Normal3f((Cross(dpdu, dpdv)).normalize()), pError, r_in, time) {}

	SurfaceInteraction(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, Float time, const Shape* shape);

	void ComputeScatteringFunctions(const RayDifferential& ray, MemoryArena& arena, bool allowMultipleLobes, TransportMode mode)
	{
		ComputeDifferentials(ray);
		primitive->ComputeScatteringFunctions(this, arena, mode, allowMultipleLobes);
	}

	void ComputeDifferentials(const RayDifferential& ray) const
	{
		if (ray.hasDifferentials) {
			//Estimate screen space change in p and (u, v) 601
			Float d = Dot(n, Vector3f(p.x(), p.y(), p.z()));
			Float tx = -(Dot(n, Vector3f(ray.rxOrigin)) - d) / Dot(n, ray.rxDirection);
			Point3f px = ray.rxOrigin + tx * ray.rxDirection;
			Float ty = -(Dot(n, Vector3f(ray.ryOrigin)) - d) / Dot(n, ray.ryDirection);
			Point3f py = ray.ryOrigin + ty * ray.ryDirection;

			dpdx = px - p;
			dpdy = py - p;

			int dim[2];
			if (std::abs(n.x()) > std::abs(n.y()) && std::abs(n.x()) > std::abs(n.z())) {
				dim[0] = 1; dim[1] = 2;
			}
			else if (std::abs(n.y()) > std::abs(n.z())) {
				dim[0] = 0; dim[1] = 2;
			}
			else {
				dim[0] = 0; dim[1] = 1;
			}
			Float A[2][2] = { { dpdu[dim[0]], dpdv[dim[0]] },{ dpdu[dim[1]], dpdv[dim[1]] } };
			Float Bx[2] = { px[dim[0]] - p[dim[0]], px[dim[1]] - p[dim[1]] };
			Float By[2] = { py[dim[0]] - p[dim[0]], py[dim[1]] - p[dim[1]] };

			if (!SolveLinearSystem2x2(A, Bx, &dudx, &dvdx))
				dudx = dvdx = 0;
			if (!SolveLinearSystem2x2(A, By, &dudy, &dvdy))
				dudy = dvdy = 0;
		}
		else {
			dudx = dvdx = 0;
			dudy = dvdy = 0;
			dpdx = dpdy = Vector3f(0, 0, 0);
		}
	}

	void set_face_normal(const Vector3f& r_in, const Normal3f& outward_normal);

	void SetShadingGeometry(const Vector3f& dpdus,
		const Vector3f& dpdvs, const Normal3f& dndus,
		const Normal3f& dndvs, bool orientationIsAuthoritative) {
		//Compute shading.n for SurfaceInteraction 119
		shading.n = Normalize((Normal3f)Cross(dpdus, dpdvs));
		if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness))
			shading.n = -shading.n;
		if (orientationIsAuthoritative)
			n = Faceforward(n, shading.n);
		else
			shading.n = Faceforward(shading.n, n);

		//Initialize shading partial derivative values 119
		shading.dpdu = dpdus;
		shading.dpdv = dpdvs;
		shading.dndu = dndus;
		shading.dndv = dndvs;
	}

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

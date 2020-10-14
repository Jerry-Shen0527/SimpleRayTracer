#include <Geometry/Interaction.h>
#include <Geometry/Shape.h>

#include "Geometry/Primitive.h"

void SurfaceInteraction::ComputeScatteringFunctions(const RayDifferential& ray, MemoryArena& arena,
                                                    bool allowMultipleLobes, TransportMode mode)
{
	ComputeDifferentials(ray);
	//primitive->ComputeScatteringFunctions(this, arena, mode, allowMultipleLobes);
}

void SurfaceInteraction::ComputeDifferentials(const RayDifferential& ray) const
{
	if (ray.hasDifferentials)
	{
		//Estimate screen space change in p and (u, v) 601
		Float d = Dot(n, Vector3f(p.x(), p.y(), p.z()));
		Float tx = -(Dot(n, Vector3f(ray.rxOrigin)) - d) / Dot(n, ray.rxDirection);
		Point3f px = ray.rxOrigin + tx * ray.rxDirection;
		Float ty = -(Dot(n, Vector3f(ray.ryOrigin)) - d) / Dot(n, ray.ryDirection);
		Point3f py = ray.ryOrigin + ty * ray.ryDirection;

		dpdx = px - p;
		dpdy = py - p;

		int dim[2];
		if (std::abs(n.x()) > std::abs(n.y()) && std::abs(n.x()) > std::abs(n.z()))
		{
			dim[0] = 1;
			dim[1] = 2;
		}
		else if (std::abs(n.y()) > std::abs(n.z()))
		{
			dim[0] = 0;
			dim[1] = 2;
		}
		else
		{
			dim[0] = 0;
			dim[1] = 1;
		}
		Float A[2][2] = {{dpdu[dim[0]], dpdv[dim[0]]}, {dpdu[dim[1]], dpdv[dim[1]]}};
		Float Bx[2] = {px[dim[0]] - p[dim[0]], px[dim[1]] - p[dim[1]]};
		Float By[2] = {py[dim[0]] - p[dim[0]], py[dim[1]] - p[dim[1]]};

		if (!SolveLinearSystem2x2(A, Bx, &dudx, &dvdx))
			dudx = dvdx = 0;
		if (!SolveLinearSystem2x2(A, By, &dudy, &dvdy))
			dudy = dvdy = 0;
	}
	else
	{
		dudx = dvdx = 0;
		dudy = dvdy = 0;
		dpdx = dpdy = Vector3f(0, 0, 0);
	}
}

void SurfaceInteraction::SetShadingGeometry(const Vector3f& dpdus, const Vector3f& dpdvs, const Normal3f& dndus,
                                            const Normal3f& dndvs, bool orientationIsAuthoritative)
{
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

#include <Geometry/Primitive.h>

bool GeometricPrimitive::Intersect(const Ray& r, SurfaceInteraction* isect) const
{
	Float tHit;
	if (!shape->Intersect(r, &tHit, isect))
		return false;
	r.tMax = tHit;
	isect->primitive = this;
	//Initialize SurfaceInteraction::mediumInterface after Shape intersection 685
	return true;
}

void GeometricPrimitive::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
	if (material)
		material->ComputeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
}

bool TransformedPrimitive::Intersect(const Ray& r, SurfaceInteraction* isect) const
{
	// Compute _ray_ after transformation by _PrimitiveToWorld_
	Transform InterpolatedPrimToWorld;
	PrimitiveToWorld.Interpolate(r.time, &InterpolatedPrimToWorld);
	Ray ray = Inverse(InterpolatedPrimToWorld)(r);
	if (!primitive->Intersect(ray, isect)) return false;
	r.tMax = ray.tMax;
	// Transform instance's intersection data to world space
	if (!InterpolatedPrimToWorld.IsIdentity())
		*isect = InterpolatedPrimToWorld(*isect);
	return true;
}
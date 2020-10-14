#include <Geometry/Primitive.h>

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<material>& material,
	const std::shared_ptr<AreaLight>& areaLight) : shape(shape), m(material), areaLight(areaLight)
{
}

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

//void GeometricPrimitive::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
//{
//	if (m)
//		m->ComputeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
//}

Bounds3f GeometricPrimitive::WorldBound() const
{
	return shape->WorldBound();
}

bool GeometricPrimitive::IntersectP(const Ray& r) const
{
	return shape->IntersectP(r);
}

const AreaLight* GeometricPrimitive::GetAreaLight() const
{
	return areaLight.get();
}

const material* GeometricPrimitive::GetMaterial() const
{
	return m.get();
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
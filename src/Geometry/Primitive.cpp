#include <Geometry/Primitive.h>

#include "BRDF/Material.h"
#include "Geometry/Shape.h"

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& material,
                                       const std::shared_ptr<AreaLight>& areaLight) : shape(shape), material(material), areaLight(areaLight)
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

const Material* GeometricPrimitive::GetMaterial() const
{
	return material.get();
}

void GeometricPrimitive::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode,
	bool allowMultipleLobes) const
{
	if (material)
		material->ComputeScatteringFunctions(isect, arena, mode,
			allowMultipleLobes);
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

const AreaLight* Aggregate::GetAreaLight() const
{
	return nullptr;
}

const Material* Aggregate::GetMaterial() const
{
	return nullptr;
}

void Aggregate::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode,
	bool allowMultipleLobes) const
{
}

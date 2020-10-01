#pragma once
#include "hit_record.h"
#include "Tools/Bound.h"

class Primitive
{
public:
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const ray& r, SurfaceInteraction&) const = 0;
	virtual bool IntersectP(const ray& r) const = 0;
	virtual const AreaLight* GetAreaLight() const = 0;
	virtual const Material* GetMaterial() const = 0;

	//virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
};
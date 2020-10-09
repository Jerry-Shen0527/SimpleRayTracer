#pragma once
#include "Shape.h"

class MemoryArena;

class Primitive
{
public:
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const ray& r, SurfaceInteraction&) const = 0;
	virtual bool IntersectP(const ray& r) const = 0;
	virtual const AreaLight* GetAreaLight() const = 0;
	virtual const Material* GetMaterial() const = 0;

	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
};


class GeometricPrimitive :public Primitive
{
public:
	bool GeometricPrimitive::Intersect(const ray& r, SurfaceInteraction* isect) const {
		Float tHit;
		if (!shape->Intersect(r, &tHit, isect))
			return false;
		r.tMax = tHit;
		isect->primitive = this;
		//Initialize SurfaceInteraction::mediumInterface after Shape intersection 685
		return true;
	}

	void ComputeScatteringFunctions(
		SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode,
		bool allowMultipleLobes) const {
		if (material)
			material->ComputeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
	}
private:
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> areaLight;
	MediumInterface mediumInterface;
};

class TransformedPrimitive : public Primitive {
public:
	TransformedPrimitive(std::shared_ptr<Primitive>& primitive,
		const AnimatedTransform& PrimitiveToWorld)
		: primitive(primitive), PrimitiveToWorld(PrimitiveToWorld) { }
private:
	std::shared_ptr<Primitive> primitive;
	const AnimatedTransform PrimitiveToWorld;
};
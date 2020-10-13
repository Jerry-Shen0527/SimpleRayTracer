#pragma once
#include <Tools/MemoryArena.h>

#include "Shape.h"
#include "Tools/Bound.h"

class Primitive
{
public:
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const Ray& r, SurfaceInteraction&) const = 0;
	virtual bool IntersectP(const Ray& r) const = 0;
	virtual const AreaLight* GetAreaLight() const = 0;
	virtual const Material* GetMaterial() const = 0;

	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
};

class GeometricPrimitive :public Primitive
{
public:
	bool GeometricPrimitive::Intersect(const Ray& r, SurfaceInteraction* isect) const {
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
	TransformedPrimitive(std::shared_ptr<Primitive>& primitive, const AnimatedTransform& PrimitiveToWorld)
		: primitive(primitive), PrimitiveToWorld(PrimitiveToWorld) { }

	Bounds3f WorldBound() const {
		return PrimitiveToWorld.MotionBounds(primitive->WorldBound());
	}

	bool Intersect(const Ray& r, SurfaceInteraction& isect) const {
		//Compute ray after transformation by PrimitiveToWorld 253
		Transform InterpolatedPrimToWorld;
		PrimitiveToWorld.Interpolate(r.time(), &InterpolatedPrimToWorld);
		Ray ray = Inverse(InterpolatedPrimToWorld)(r);
		if (!primitive->Intersect(ray, isect))
			return false;
		r.tMax = ray.tMax;
		//Transform instance¡¯s intersection data to world space 253
		if (!InterpolatedPrimToWorld.IsIdentity())
			isect = InterpolatedPrimToWorld(isect);
		return true;
	}

private:
	std::shared_ptr<Primitive> primitive;
	const AnimatedTransform PrimitiveToWorld;
};

class Aggregate : public Primitive {
public:
};
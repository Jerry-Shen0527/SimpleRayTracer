#pragma once
#include "Tools/Light/AreaLight.h"
#include <Geometry/AnimatedTransform.h>

#include "BRDF/Medium.h"

class Primitive
{
public:
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const Ray& r, SurfaceInteraction*) const = 0;
	virtual bool IntersectP(const Ray& r) const = 0;
	virtual const AreaLight* GetAreaLight() const = 0;
	virtual const Material* GetMaterial() const = 0;

	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
};

class GeometricPrimitive :public Primitive
{
public:
	GeometricPrimitive(const std::shared_ptr<Shape>& shape,
		const std::shared_ptr<Material>& material,
		const std::shared_ptr<AreaLight>& areaLight);
	bool Intersect(const Ray& r, SurfaceInteraction* isect) const override;

	//void ComputeScatteringFunctions(
	//	SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode,
	//	bool allowMultipleLobes) const override;

	Bounds3f WorldBound() const override;
	bool IntersectP(const Ray& r) const override;
	const AreaLight* GetAreaLight() const override;
	const Material* GetMaterial() const override;
	void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode,
		bool allowMultipleLobes) const override;
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

	Bounds3f WorldBound() const { return PrimitiveToWorld.MotionBounds(primitive->WorldBound()); }

	bool Intersect(const Ray& r, SurfaceInteraction* isect) const override;

private:
	std::shared_ptr<Primitive> primitive;
	const AnimatedTransform PrimitiveToWorld;
};

class Aggregate : public Primitive {
public:
	// Aggregate Public Methods
	const AreaLight* GetAreaLight() const;
	const Material* GetMaterial() const;
	void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const;
};
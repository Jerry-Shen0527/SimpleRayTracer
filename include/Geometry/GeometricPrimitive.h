#pragma once
#include "Primitive.h"
#include "Shape.h"

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
private:
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> areaLight;
	MediumInterface mediumInterface;
};

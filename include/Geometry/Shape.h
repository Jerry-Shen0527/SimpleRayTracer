#pragma once
#include "translation.h"
#include "Tools/Bound.h"

class Shape {
public:
	Shape::Shape(const Transform* ObjectToWorld,
		const Transform* WorldToObject, bool reverseOrientation)
		: ObjectToWorld(ObjectToWorld), WorldToObject(WorldToObject),
		reverseOrientation(reverseOrientation),
		transformSwapsHandedness(ObjectToWorld->SwapsHandedness()) {
	}

	virtual Float Area() const = 0;
	virtual Bounds3f ObjectBound() const = 0;
	Bounds3f Shape::WorldBound() const {
		return (*ObjectToWorld)(ObjectBound());
	}

	const Transform* ObjectToWorld, * WorldToObject;
	const bool reverseOrientation;
	const bool transformSwapsHandedness;
};

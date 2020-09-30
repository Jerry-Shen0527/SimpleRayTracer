#pragma once
#include "translation.h"

class Shape {
public:
	Shape::Shape(const Transform* ObjectToWorld,
		const Transform* WorldToObject, bool reverseOrientation)
		: ObjectToWorld(ObjectToWorld), WorldToObject(WorldToObject),
		reverseOrientation(reverseOrientation),
		transformSwapsHandedness(ObjectToWorld->SwapsHandedness()) {
	}

	virtual Float Area() const = 0;

	const Transform* ObjectToWorld, * WorldToObject;
	const bool reverseOrientation;
	const bool transformSwapsHandedness;
};

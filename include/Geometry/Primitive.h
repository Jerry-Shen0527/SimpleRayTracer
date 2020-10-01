#pragma once
class Primitive
{
public:
	virtual Bounds3f WorldBound() const = 0;
};

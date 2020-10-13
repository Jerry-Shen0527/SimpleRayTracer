#pragma once
#include <Geometry/Shape.h>

#include "Light.h"
class AreaLight :public Light
{
public:
	AreaLight(const Transform& LightToWorld, const MediumInterface& medium, int nSamples);
	virtual Spectrum L(const Interaction& intr, const Vector3f& w) const = 0;
};

inline AreaLight::AreaLight(const Transform& LightToWorld, const MediumInterface& medium, int nSamples) :Light((int)LightFlags::Area, LightToWorld, medium, nSamples)
{
}

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight::DiffuseAreaLight(const Transform& LightToWorld,
		const MediumInterface& mediumInterface, const Spectrum& Lemit,
		int nSamples, const std::shared_ptr<Shape>& shape)
		: AreaLight(LightToWorld, mediumInterface, nSamples), Lemit(Lemit),
		shape(shape), area(shape->Area()) { }

	Spectrum L(const Interaction& intr, const Vector3f& w) const {
		return Dot(intr.n, w) > 0.f ? Lemit : Spectrum(0.f);
	}

	Spectrum DiffuseAreaLight::Power() const {
		return Lemit * area * pi;
	}

protected:
	const Spectrum Lemit;
	std::shared_ptr<Shape> shape;
	const Float area;
};

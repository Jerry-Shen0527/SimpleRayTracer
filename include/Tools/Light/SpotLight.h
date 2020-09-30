#pragma once
#include "Light.h"

class SpotLight :public Light
{
public:
	SpotLight::SpotLight(const Transform& LightToWorld,
		const MediumInterface& mediumInterface, const Spectrum& I,
		Float totalWidth, Float falloffStart)
		: Light((int)LightFlags::DeltaPosition, LightToWorld,
			mediumInterface),
		pLight(LightToWorld(Point3f(0, 0, 0))), I(I),
		cosTotalWidth(std::cos(Radians(totalWidth))),
		cosFalloffStart(std::cos(Radians(falloffStart))) { }

	Spectrum SpotLight::Sample_Li(const Interaction& ref, const Point2f& u, Vector3f& wi, Float& pdf, VisibilityTester& vis) const {
		wi = (pLight - ref.p).normalize();
		pdf = 1.f;
		vis = VisibilityTester(ref, Interaction(pLight, ref.time,
			mediumInterface));
		return I * Falloff(-wi) / (pLight - ref.p).length_squared();
	}

	Float SpotLight::Falloff(const Vector3f& w) const {
		Vector3f wl = WorldToLight(w).normalize();
		Float cosTheta = wl.z;
		if (cosTheta < cosTotalWidth) return 0;
		if (cosTheta > cosFalloffStart) return 1;
		//Compute falloff inside spotlight cone 724
		Float delta = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
		return (delta * delta) * (delta * delta);
	}

	Spectrum SpotLight::Power() const {
		return I * 2 * pi * (1 - .5f * (cosFalloffStart + cosTotalWidth));
	}

private:
	const Point3f pLight;
	const Spectrum I;
	const Float cosTotalWidth, cosFalloffStart;
};

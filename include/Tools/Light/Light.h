#pragma once
#include <algorithm>
#include <Geometry/hittable_list.h>


#include "common.h"
#include "BRDF/Medium.h"
#include "Geometry/Interaction.h"
#include "Geometry/Transform.h"
#include "Tools/Sampler/Sampler.h"



enum class LightFlags : int { DeltaPosition = 1, DeltaDirection = 2, Area = 4, Infinite = 8 };

inline bool IsDeltaLight(int flags)
{
	return flags & (int)LightFlags::DeltaPosition || flags & (int)LightFlags::DeltaDirection;
}

class Light
{
public:
	Light(int flags, const Transform& LightToWorld, const MediumInterface& mediumInterface, int nSamples = 1)
		: flags(flags), nSamples(std::max(1, nSamples)), mediumInterface(mediumInterface), LightToWorld(LightToWorld), WorldToLight(Inverse(LightToWorld)) {	}

	virtual Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f& wi, Float& pdf, VisibilityTester* vis) const = 0;

	virtual Spectrum Power() const = 0;
	virtual void Preprocess(const Scene& scene) { }

public:
	const int flags;
	const int nSamples;
	const MediumInterface mediumInterface;

protected:
	const Transform LightToWorld, WorldToLight;
};

class VisibilityTester {
public:
	VisibilityTester(const Interaction& p0, const Interaction& p1) : p0(p0), p1(p1) { }

	const Interaction& P0() const { return p0; }
	const Interaction& P1() const { return p1; }
	bool Unoccluded(const Scene& scene) const;
	Spectrum Tr(const Scene& scene, Sampler& sampler) const;
private:
	Interaction p0, p1;
};

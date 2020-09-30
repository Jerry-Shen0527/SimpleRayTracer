#pragma once
#include <algorithm>
#include <Geometry/hittable_list.h>
#include <Geometry/translation.h>

#include "BRDF/Volume/Medium.h"
#include "Tools/Sampler/Sampler.h"

class VisibilityTester;

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

	virtual Spectrum Sample_Li(const hit_record& ref, const Point2f& u, Vector3f& wi, Float& pdf, VisibilityTester* vis) const = 0;

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

bool VisibilityTester::Unoccluded(const Scene& scene) const {
	return !scene.hit(p0.SpawnRayTo(p1.p));
}

Spectrum VisibilityTester::Tr(const Scene& scene, Sampler& sampler) const {
	ray ray(p0.SpawnRayTo(p1));
	Spectrum Tr(1.f);
	while (true) {
		SurfaceInteraction isect;
		bool hitSurface = scene.hit(
			
			, isect);
		//Handle opaque surface along ray¡¯s path 718
		if (hitSurface && isect.mat_ptr != nullptr)
			return Spectrum(0.0f);
		//	Update transmittance for current ray segment 719
		if (ray.medium)
			Tr *= ray.medium->Tr(ray, sampler);
		//	Generate next ray segment or return final transmittance 719
		if (!hitSurface)
			break;
		ray = isect.SpawnRayTo(p1);
	}
	return Tr;
}
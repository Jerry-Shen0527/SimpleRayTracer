#include <Tools/Light/Light.h>

#include "Geometry/Scene.h"

bool VisibilityTester::Unoccluded(const Scene& scene) const {
	return !scene.IntersectP(p0.SpawnRayTo(p1.p));
}

Spectrum VisibilityTester::Tr(const Scene& scene, Sampler& sampler) const {
	Ray ray(p0.SpawnRayTo(p1));
	Spectrum Tr(1.f);
	while (true) {
		SurfaceInteraction* isect;
		bool hitSurface = scene.Intersect(ray, isect);
		//Handle opaque surface along ray¡¯s path 718
		if (hitSurface && isect->mat_ptr != nullptr)
			return Spectrum(0.0f);
		//	Update transmittance for current ray segment 719
		if (ray.medium)
			Tr *= ray.medium->Tr(ray, sampler);
		//	Generate next ray segment or return final transmittance 719
		if (!hitSurface)
			break;
		ray = isect->SpawnRayTo(p1);
	}
	return Tr;
}
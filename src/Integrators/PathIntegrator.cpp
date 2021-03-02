#include <Integrators/PathIntegrator.h>

#include "BRDF/BSDF.h"
#include "Integrators/DirectLightIntegrator.h"

#define sqr(a) (a*a)
Spectrum PathIntegrator::Li(const RayDifferential& r, const Scene& scene, Sampler& sampler, MemoryArena& arena,
	int depth) const
{
	Spectrum L(0.f), beta(1.f);
	beta.mueller_spectrum = linear_polarizer(1);
	RayDifferential ray(r);
	bool specularBounce = false;

	Float etaScale = 1;

	for (int bounces = 0; ; ++bounces) {
		//Find next path vertexand accumulate contribution กิ 876
		//	Intersect ray with scene and store intersection in isect 877
		SurfaceInteraction isect;
		bool foundIntersection = scene.Intersect(ray, &isect);
		//	Possibly add emitted light at intersection 877
		if (polarized)
		{
			if (bounces == 0 || specularBounce) {
				//Add emitted light at path vertex or from the environment 877
				if (foundIntersection)
					L += sqr(beta.mueller_spectrum.m[0][0]) * beta * isect.Le(-ray.d);
				else
					for (const auto& light : scene.lights)
						L += sqr(beta.mueller_spectrum.m[0][0]) * beta * light->Le(ray);
			}
		}
		else
		{
			if (bounces == 0 || specularBounce) {
				//Add emitted light at path vertex or from the environment 877
				if (foundIntersection)
					L += beta * isect.Le(-ray.d);
				else
					for (const auto& light : scene.lights)
						L += beta * light->Le(ray);
			}
		}
		//	Terminate path if ray escaped or maxDepth was reached 877
		if (!foundIntersection || bounces >= maxDepth)
			break;
		//	Compute scattering functions and skip over medium boundaries 878
		//No bsdf means it could be in the medium
		isect.ComputeScatteringFunctions(beta, ray, arena, true);
		if (!isect.bsdf) {
			ray = isect.SpawnRay(ray.d);
			bounces--;
			continue;
		}
		//	Sample illumination from lights to find path contribution 878

				//	Sample BSDF to get new path direction 878
		Vector3f wo = -ray.d, wi;
		Float pdf;
		BxDFType flags;
		Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, BSDF_ALL, &flags);

		beta.mueller_spectrum = beta.mueller_spectrum * f.mueller_spectrum;
		if (polarized)
		{
			L += beta * sqr(beta.mueller_spectrum.m[0][0]) * UniformSampleOneLight(isect, scene, arena, sampler);
		}
		else
		{
			L += beta * UniformSampleOneLight(isect, scene, arena, sampler);
		}
		if (f.IsBlack() || pdf == 0.f)
			break;
		beta *= f * AbsDot(wi, isect.shading.n) / pdf;

		specularBounce = (flags & BSDF_SPECULAR) != 0;
		if (flags & BSDF_SPECULAR && flags & BSDF_TRANSMISSION) {
			Float eta = isect.bsdf->eta;
			// Update the term that tracks radiance scaling for refraction
			// depending on whether the ray is entering or leaving the
			// medium.
			etaScale *= (Dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
		}
		ray = isect.SpawnRay(wi);
		//	Account for subsurface scattering, if applicable 915
		//	Possibly terminate the path with Russian roulette 879
		Spectrum rrBeta = beta * etaScale;

		if (bounces > 3) {
			Float q = std::max((Float).05, 1 - rrBeta.y());
			if (sampler.Get1D() < q)
				break;
			beta /= 1 - q;
		}
	}
	return L;
}
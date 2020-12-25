#include <Integrators/DirectLightIntegrator.h>

#include "BRDF/BSDF.h"
#include "Tools/Light/Light.h"

Spectrum DirectLightingIntegrator::Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const
{
	Spectrum L(0.f);
	SurfaceInteraction isect;
	// Find closest ray intersection or return background radiance
	if (!scene.Intersect(ray, &isect)) {
		for (const auto& light : scene.lights) L += light->Le(ray);
		return L;
	}

	isect.ComputeScatteringFunctions(ray, arena);

	//What happens when the bsdf is nullptr?
	if (!isect.bsdf)
		return Li(isect.SpawnRay(ray.d), scene, sampler, arena, depth);

	if (strategy == LightStrategy::UniformSampleAll)
		L += UniformSampleAllLights(isect, scene, arena, sampler, nLightSamples);
	else
		L += UniformSampleOneLight(isect, scene, arena, sampler);
}

Spectrum UniformSampleAllLights(const Interaction& it, const Scene& scene, MemoryArena& arena,
	Sampler& sampler, const std::vector<int>& nLightSamples,
	bool handleMedia)
{
	Spectrum L(0.f);
	for (size_t j = 0; j < scene.lights.size(); ++j)
	{
		const std::shared_ptr<Light>& light = scene.lights[j];
		int nSamples = nLightSamples[j];
		const Point2f* uLightArray = sampler.Get2DArray(nSamples);
		const Point2f* uScatteringArray = sampler.Get2DArray(nSamples);
		if (!uLightArray || !uScatteringArray)
		{
			//Use a single sample for illumination from light 855
			Point2f uLight = sampler.Get2D();
			Point2f uScattering = sampler.Get2D();
			L += EstimateDirect(it, uScattering, *light, uLight, scene, sampler, arena, handleMedia);
		}
		else
		{
			//Estimate direct lighting using sample arrays 855
			Spectrum Ld(0.f);
			for (int k = 0; k < nSamples; ++k)
				Ld += EstimateDirect(it, uScatteringArray[k], *light, uLightArray[k], scene, sampler, arena, handleMedia);
			L += Ld / nSamples;
		}
	}
	return L;
}

Spectrum UniformSampleOneLight(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, bool handleMedia)
{
	//Randomly choose a single light to sample, light 856
	int nLights = int(scene.lights.size());
	if (nLights == 0) return Spectrum(0.f);
	int lightNum = std::min((int)(sampler.Get1D() * nLights), nLights - 1);
	const std::shared_ptr<Light>& light = scene.lights[lightNum];

	Point2f uLight = sampler.Get2D();
	Point2f uScattering = sampler.Get2D();
	return (Float)nLights * EstimateDirect(it, uScattering, *light, uLight, scene, sampler, arena, handleMedia);
}

Spectrum EstimateDirect(const Interaction& it, const Point2f& uScattering, const Light& light, const Point2f& uLight, const Scene& scene, Sampler& sampler,
	MemoryArena& arena, bool handleMedia, bool specular)
{
	BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
	Spectrum Ld(0.f);
	//Sample light source with multiple importance sampling 858

	Vector3f wi;
	Float lightPdf = 0, scatteringPdf = 0;
	VisibilityTester visibility;
	Spectrum Li = light.Sample_Li(it, uLight, &wi, &lightPdf, &visibility);

	if (lightPdf > 0 && !Li.IsBlack())
	{
		//Compute BSDF or phase function¡¯s value for light sample 859
		Spectrum f;
		if (it.IsSurfaceInteraction())
		{
			//Evaluate BSDF for light sampling strategy 859
			const SurfaceInteraction& isect = (const SurfaceInteraction&)it;
			f = isect.bsdf->f(isect.wo, wi, bsdfFlags) * AbsDot(wi, isect.shading.n);
			scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, bsdfFlags);
		}
		else
		{
			//TODO:Evaluate phase function for light sampling strategy 900
		}
		if (!f.IsBlack())
		{
			//Compute effect of visibility for light source sample 859
			if (handleMedia)
				Li *= visibility.Tr(scene, sampler);
			else if (!visibility.Unoccluded(scene))
				Li = Spectrum(0.f);
			//Add light¡¯s contribution to reflected radiance 860
			if (!Li.IsBlack()) {
				if (IsDeltaLight(light.flags))
					Ld += f * Li / lightPdf;
				else {
					Float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
					Ld += f * Li * weight / lightPdf;
				}
			}
		}
	}
	//Sample BSDF with multiple importance sampling 860
	if (!IsDeltaLight(light.flags)) {
		Spectrum f;
		bool sampledSpecular = false;
		if (it.IsSurfaceInteraction()) {
			//Sample scattered direction for surface interactions 860
			BxDFType sampledType;
			const SurfaceInteraction& isect = (const SurfaceInteraction&)it;
			f = isect.bsdf->Sample_f(isect.wo, &wi, uScattering, &scatteringPdf, bsdfFlags, &sampledType);
			f *= AbsDot(wi, isect.shading.n);
			sampledSpecular = sampledType & BSDF_SPECULAR;
		}
		else {
			//Sample scattered direction for medium interactions 900
		}
		if (!f.IsBlack() && scatteringPdf > 0) {
			//Account for light contributions along sampled direction wi 861
			Float weight = 1;
			if (!sampledSpecular) {
				lightPdf = light.Pdf_Li(it, wi);
				if (lightPdf == 0)
					return Ld;
				weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
			}
			//Find intersectionand compute transmittance 861
			SurfaceInteraction lightIsect;
			Ray ray = it.SpawnRay(wi);
			Spectrum Tr(1.f);
			bool foundSurfaceInteraction = handleMedia ? scene.IntersectTr(ray, sampler, &lightIsect, &Tr) : scene.Intersect(ray, &lightIsect);
			//Add light contribution from material sampling 861
			Spectrum Li(0.f);
			if (foundSurfaceInteraction) {
				if (lightIsect.primitive->GetAreaLight() == &light)
					Li = lightIsect.Le(-wi);
			}
			else
				Li = light.Le(ray);
			if (!Li.IsBlack())
				Ld += f * Li * Tr * weight / scatteringPdf;
		}
	}
	return Ld;
}

void DirectLightingIntegrator::Preprocess(const Scene& scene, Sampler& sampler)
{
	if (strategy == LightStrategy::UniformSampleAll)
	{
		//Compute number of samples to use for each light 853
		for (const auto& light : scene.lights)
			nLightSamples.push_back(sampler.RoundCount(light->nSamples));
		//Request samples for sampling all lights 853
		for (int i = 0; i < maxDepth; ++i)
		{
			for (size_t j = 0; j < scene.lights.size(); ++j)
			{
				sampler.Request2DArray(nLightSamples[j]);
				sampler.Request2DArray(nLightSamples[j]);
			}
		}
	}
}
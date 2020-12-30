#include <Integrators/PathIntegrator.h>

Spectrum PathIntegrator::Li(const RayDifferential& r, const Scene& scene, Sampler& sampler, MemoryArena& arena,
	int depth) const
{
	Spectrum L(0.f), beta(1.f);
	RayDifferential ray(r);
	bool specularBounce = false;
	for (int bounces = 0; ; ++bounces) {
		//Find next path vertexand accumulate contribution กิ 876
		//	Intersect ray with scene and store intersection in isect 877
		
		//	Possibly add emitted light at intersection 877
		//	Terminate path if ray escaped or maxDepth was reached 877
		//	Compute scattering functions and skip over medium boundaries 878
		//	Sample illumination from lights to find path contribution 878
		//	Sample BSDF to get new path direction 878
		//	Account for subsurface scattering, if applicable 915
		//	Possibly terminate the path with Russian roulette 879
	}
	return L;
}
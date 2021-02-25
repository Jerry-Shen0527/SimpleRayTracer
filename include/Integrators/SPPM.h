#pragma once
#include "Integrator.h"
#include "Tools/camera.h"

class SPPMIntegrator : public Integrator {
public:

	SPPMIntegrator(std::shared_ptr<const Camera>& camera, int nIterations,
		int photonsPerIteration, int maxDepth,
		Float initialSearchRadius, int writeFrequency)
		: camera(camera),
		initialSearchRadius(initialSearchRadius),
		nIterations(nIterations),
		maxDepth(maxDepth),
		photonsPerIteration(photonsPerIteration),
		writeFrequency(writeFrequency) {}
	void Render(const Scene& scene, bool benchmark = false) override;

	static bool ToGrid(const Point3f& p, const Bounds3f& bounds,
		const int gridRes[3], Point3i* pi);
	//SPPMIntegrator Public Methods

private:

	std::shared_ptr<const Camera> camera;
	const Float initialSearchRadius;
	const int nIterations;
	const int maxDepth;
	const int photonsPerIteration;
	const int writeFrequency;
	//SPPMIntegrator Private Data 973
};

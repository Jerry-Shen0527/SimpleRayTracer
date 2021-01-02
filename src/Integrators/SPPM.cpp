#include <Integrators/SPPM.h>

struct SPPMPixel {
	//SPPMPixel Public Methods
		//SPPMPixel Public Data 974
	Float radius = 0;
};

inline std::unique_ptr<Distribution1D> ComputeLightPowerDistribution(
	const Scene& scene) {
	std::vector<Float> lightPower;
	for (const auto& light : scene.lights)
		lightPower.push_back(light->Power().y());
	return std::unique_ptr<Distribution1D>(
		new Distribution1D(&lightPower[0], lightPower.size()));
}

void SPPMIntegrator::Render(const Scene& scene)
{
	//Initialize pixelBoundsand pixels array for SPPM 973
	Bounds2i pixelBounds = camera->film->croppedPixelBounds;
	int nPixels = pixelBounds.Volume();
	std::unique_ptr<SPPMPixel[]> pixels(new SPPMPixel[nPixels]);
	for (int i = 0; i < nPixels; ++i)
		pixels[i].radius = initialSearchRadius;

	//Compute lightDistr for sampling lights proportional to power 974
	std::unique_ptr<Distribution1D> lightDistr = ComputeLightPowerDistribution(scene);

	
}
#pragma once
#include "PixelSampler.h"

template <typename T>
void Shuffle(T* samp, int count, int nDimensions); 

class StratifiedSampler :public PixelSampler
{
public:
	StratifiedSampler(int xPixelSamples, int yPixelSamples, bool jitterSamples, int nSampledDimensions)
		: PixelSampler(xPixelSamples* yPixelSamples, nSampledDimensions), xPixelSamples(xPixelSamples), yPixelSamples(yPixelSamples), jitterSamples(jitterSamples) { }

	void StartPixel(const Point2i& p) override;

	std::unique_ptr<Sampler> Clone(int seed) override;
private:

	void StratifiedSample1D(Float* samp, int nSamples, bool jitter);
	void StratifiedSample2D(Point2f* samp, int nx, int ny, bool jitter);

	void LatinHypercube(Float* samples, int nSamples, int nDim);

	const int xPixelSamples, yPixelSamples;
	const bool jitterSamples;
};

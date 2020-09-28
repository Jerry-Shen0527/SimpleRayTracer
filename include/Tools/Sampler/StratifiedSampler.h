#pragma once
#include "PixelSampler.h"

template <typename T>
void Shuffle(T* samp, int count, int nDimensions) {
	for (int i = 0; i < count; ++i) {
		int other = i + random_int(0, count - i);
		for (int j = 0; j < nDimensions; ++j)
			std::swap(samp[nDimensions * i + j],
				samp[nDimensions * other + j]);
	}
}

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

inline void StratifiedSampler::StartPixel(const Point2i& p)
{
	for (size_t i = 0; i < samples1D.size(); ++i)
	{
		StratifiedSample1D(&samples1D[i][0], xPixelSamples * yPixelSamples, jitterSamples);
		Shuffle(&samples1D[i][0], xPixelSamples * yPixelSamples, 1);
	}
	for (size_t i = 0; i < samples2D.size(); ++i)
	{
		StratifiedSample2D(&samples2D[i][0], xPixelSamples, yPixelSamples, jitterSamples);
		Shuffle(&samples2D[i][0], xPixelSamples * yPixelSamples, 1);
	}

	for (size_t i = 0; i < samples1DArraySizes.size(); ++i)
		for (int64_t j = 0; j < samplesPerPixel; ++j) {
			int count = samples1DArraySizes[i];
			StratifiedSample1D(&sampleArray1D[i][j * count], count, jitterSamples);
			Shuffle(&sampleArray1D[i][j * count], count, 1);
		}
	for (size_t i = 0; i < samples2DArraySizes.size(); ++i)
		for (int64_t j = 0; j < samplesPerPixel; ++j) {
			int count = samples2DArraySizes[i];
			LatinHypercube(&sampleArray2D[i][j * count].x(), count, 2);
		}

	PixelSampler::StartPixel(p);
}

inline std::unique_ptr<Sampler> StratifiedSampler::Clone(int seed)
{
	StratifiedSampler* ss = new StratifiedSampler(*this);
	return std::unique_ptr<Sampler>(ss);
}

inline void StratifiedSampler::StratifiedSample1D(Float* samp, int nSamples, bool jitter)
{
	Float invNSamples = (Float)1 / nSamples;
	for (int i = 0; i < nSamples; ++i)
	{
		Float delta = jitter ? random_float() : 0.5f;
		samp[i] = std::min((i + delta) * invNSamples, OneMinusEpsilon);
	}
}

inline void StratifiedSampler::StratifiedSample2D(Point2f* samp, int nx, int ny, bool jitter)
{
	Float dx = (Float)1 / nx, dy = (Float)1 / ny;
	for (int y = 0; y < ny; ++y)
		for (int x = 0; x < nx; ++x)
		{
			Float jx = jitter ? random_float() : 0.5f;
			Float jy = jitter ? random_float() : 0.5f;
			samp->x() = std::min((x + jx) * dx, OneMinusEpsilon);
			samp->y() = std::min((y + jy) * dy, OneMinusEpsilon);
			++samp;
		}
}

inline void StratifiedSampler::LatinHypercube(Float* samples, int nSamples, int nDim)
{
	Float invNSamples = (Float)1 / nSamples;
	for (int i = 0; i < nSamples; ++i)
		for (int j = 0; j < nDim; ++j)
		{
			Float sj = (i + random_float()) * invNSamples;
			samples[nDim * i + j] = std::min(sj, OneMinusEpsilon);
		}
	for (int i = 0; i < nDim; ++i)
	{
		for (int j = 0; j < nSamples; ++j)
		{
			int other = j + random_int(0, nSamples - j);
			std::swap(samples[nDim * j + i], samples[nDim * other + i]);
		}
	}
}

#pragma once
#include <cstdint>
#include <vector>
#include <BRDF/BxDF_Utility.h>

#include "Bound.h"
#include "LowDiscrepency.h"

struct CameraSample {
	Point2f pFilm;
	Point2f pLens;
	Float time;
};

class Sampler
{
public:
	Sampler::Sampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel) { }

	virtual ~Sampler() {}

	virtual void StartPixel(const Point2i& p);

	virtual Float Get1D() = 0;
	virtual Point2f Get2D() = 0;

	void Request1DArray(int n);
	void Request2DArray(int n);

	const Float* Get1DArray(int n);
	const Point2f* Get2DArray(int n);

	virtual bool StartNextSample();

	virtual int RoundCount(int n) const {
		return n;
	}

	virtual std::unique_ptr<Sampler> Clone(int seed) = 0;

	virtual bool SetSampleNumber(int64_t sampleNum);

	CameraSample GetCameraSample(const Point2i& pRaster) {
		CameraSample cs;
		auto p = Get2D();
		cs.pFilm = Point2f(p.x() + pRaster.x(), p.y() + pRaster.y());
		cs.time = Get1D();
		cs.pLens = Get2D();
		return cs;
	}
	const int64_t samplesPerPixel;

protected:
	Point2i currentPixel;
	int64_t currentPixelSampleIndex;

	// Don't know where this will be used.
	std::vector<int> samples1DArraySizes, samples2DArraySizes;
	std::vector<std::vector<Float>> sampleArray1D;
	std::vector<std::vector<Point2f>> sampleArray2D;

	size_t array1DOffset, array2DOffset;
};
class PixelSampler : public Sampler {
public:
	PixelSampler::PixelSampler(int64_t samplesPerPixel, int nSampledDimensions)
		: Sampler(samplesPerPixel) {
		for (int i = 0; i < nSampledDimensions; ++i) {
			samples1D.push_back(std::vector<Float>(samplesPerPixel));
			samples2D.push_back(std::vector<Point2f>(samplesPerPixel));
		}
	}

	virtual ~PixelSampler();

	Float Get1D() override;
	Point2f Get2D() override;
	inline void StartPixel(const Point2i& p) override;
	bool StartNextSample() override;
	//int RoundCount(int n) const override;
	inline bool SetSampleNumber(int64_t sampleNum) override;
	std::unique_ptr<Sampler> Clone(int seed) override;
protected:
	std::vector<std::vector<Float>> samples1D;
	std::vector<std::vector<Point2f>> samples2D;
	int current1DDimension = 0, current2DDimension = 0;
};

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
	RNG rng;

};

class GlobalSampler : public Sampler {
public:
	// GlobalSampler Public Methods
	bool StartNextSample();
	void StartPixel(const Point2i&);
	bool SetSampleNumber(int64_t sampleNum);
	Float Get1D();
	Point2f Get2D();
	GlobalSampler(int64_t samplesPerPixel) : Sampler(samplesPerPixel) {}
	virtual int64_t GetIndexForSample(int64_t sampleNum) const = 0;
	virtual Float SampleDimension(int64_t index, int dimension) const = 0;

private:
	// GlobalSampler Private Data
	int dimension;
	int64_t intervalSampleIndex;
	static const int arrayStartDim = 5;
	int arrayEndDim;

};

inline Float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) {
	Float f = nf * fPdf, g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}

class HaltonSampler : public GlobalSampler {
public:
	HaltonSampler(int samplesPerPixel, const Bounds2i& sampleBounds);

	int64_t GetIndexForSample(int64_t sampleNum) const override
	{
		if (currentPixel != pixelForOffset) {
			//Compute Halton sample offset for currentPixel
			pixelForOffset = currentPixel;
		}
		return offsetForCurrentPixel + sampleNum * sampleStride;
	}

	Float SampleDimension(int64_t index, int dim) const override;

	const uint16_t* PermutationForDimension(int dim) const {
		if (dim >= PrimeTableSize)
			std::cout << "HaltonSampler can only sample 5 dimensions.";
		return &radicalInversePermutations[PrimeSums[dim]];
	}

	std::unique_ptr<Sampler> Clone(int seed) override;
	//HaltonSampler Public Methods
	static std::vector<uint16_t> radicalInversePermutations;

	const int PrimeSums[PrimeTableSize] = { 0, 2, 5, 10, 17, };
	Point2i baseScales, baseExponents;
	static constexpr int kMaxResolution = 128;
	int sampleStride;

private:
	mutable Point2i pixelForOffset = Point2i(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	mutable int64_t offsetForCurrentPixel;
};
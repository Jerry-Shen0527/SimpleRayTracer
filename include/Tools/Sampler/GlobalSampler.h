#pragma once
#include "Sampler.h"

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

inline bool GlobalSampler::StartNextSample() {
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(currentPixelSampleIndex + 1);
	return Sampler::StartNextSample();
}

inline bool GlobalSampler::SetSampleNumber(int64_t sampleNum) {
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(sampleNum);
	return Sampler::SetSampleNumber(sampleNum);
}

inline Float GlobalSampler::Get1D() {
	if (dimension >= arrayStartDim && dimension < arrayEndDim)
		dimension = arrayEndDim;
	return SampleDimension(intervalSampleIndex, dimension++);
}

inline Point2f GlobalSampler::Get2D() {
	if (dimension + 1 >= arrayStartDim && dimension < arrayEndDim)
		dimension = arrayEndDim;
	Point2f p{ SampleDimension(intervalSampleIndex, dimension), SampleDimension(intervalSampleIndex, dimension + 1) };
	dimension += 2;
	return p;
}

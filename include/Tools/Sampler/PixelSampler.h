#pragma once
#include "Sampler.h"

class PixelSampler : public Sampler {
public:
	PixelSampler::PixelSampler(int64_t samplesPerPixel, int nSampledDimensions)
		: Sampler(samplesPerPixel) {
		for (int i = 0; i < nSampledDimensions; ++i) {
			samples1D.push_back(std::vector<Float>(samplesPerPixel));
			samples2D.push_back(std::vector<Point2f>(samplesPerPixel));
		}
	}

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

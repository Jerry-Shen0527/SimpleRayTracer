#pragma once
#include "Sampler.h"

class PixelSampler : public Sampler {
public:
	PixelSampler::PixelSampler(int64_t samplesPerPixel,
		int nSampledDimensions)
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
	int RoundCount(int n) const override;
	inline bool SetSampleNumber(int64_t sampleNum) override;
protected:
	std::vector<std::vector<Float>> samples1D;
	std::vector<std::vector<Point2f>> samples2D;
	int current1DDimension = 0, current2DDimension = 0;
};

inline Float PixelSampler::Get1D()
{
	if (current1DDimension < samples1D.size())
		return samples1D[current1DDimension++][currentPixelSampleIndex];
	else
		return random_float();
}

inline Point2f PixelSampler::Get2D()
{
	if (current2DDimension < samples2D.size())
		return samples2D[current1DDimension++][currentPixelSampleIndex];
	else
		return vec2{ random_float(), random_float() };
}

inline bool PixelSampler::StartNextSample()
{
	current1DDimension = current2DDimension = 0;
	return Sampler::StartNextSample();
}

inline bool PixelSampler::SetSampleNumber(int64_t sampleNum)
{
	current1DDimension = current2DDimension = 0;
	return Sampler::SetSampleNumber(sampleNum);
}

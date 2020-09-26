#pragma once
#include <cstdint>
#include <vector>
#include <BRDF/BxDF_Utility.h>

static const Float OneMinusEpsilon = 0x1.fffffep-1;

class Sampler
{
public:
	Sampler::Sampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel) { }

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

protected:
	const int64_t samplesPerPixel;
	Point2i currentPixel;
	int64_t currentPixelSampleIndex;

	std::vector<int> samples1DArraySizes, samples2DArraySizes;
	std::vector<std::vector<Float>> sampleArray1D;
	std::vector<std::vector<Point2f>> sampleArray2D;

private:
	size_t array1DOffset, array2DOffset;
};

inline void Sampler::StartPixel(const Point2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
	array1DOffset = array2DOffset = 0;
}

inline void Sampler::Request1DArray(int n)
{
	samples1DArraySizes.push_back(n);
	sampleArray1D.push_back(std::vector<Float>(n * samplesPerPixel));
}

inline void Sampler::Request2DArray(int n)
{
	samples2DArraySizes.push_back(n);
	sampleArray2D.push_back(std::vector<Point2f>(n * samplesPerPixel));
}

inline const Float* Sampler::Get1DArray(int n)
{
	if (array1DOffset == sampleArray1D.size())
		return nullptr;
	return &sampleArray1D[array1DOffset++][currentPixelSampleIndex * n];
}

inline const Point2f* Sampler::Get2DArray(int n)
{
	if (array2DOffset == sampleArray2D.size())
		return nullptr;
	return &sampleArray2D[array2DOffset++][currentPixelSampleIndex * n];
}

inline bool Sampler::SetSampleNumber(int64_t sampleNum)
{
	array1DOffset = array2DOffset = 0;
	currentPixelSampleIndex = sampleNum;
	return currentPixelSampleIndex < samplesPerPixel;
}

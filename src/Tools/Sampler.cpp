#include <Tools/Sampler.h>

#include "Tools/LowDiscrepency.h"

template <typename T>
void Shuffle(T* samp, int count, int nDimensions, RNG& rng) {
	for (int i = 0; i < count; ++i) {
		int other = i + rng.UniformUInt32(count - i);
		for (int j = 0; j < nDimensions; ++j)
			std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
	}
}

void StratifiedSampler::StartPixel(const Point2i& p)
{
	for (size_t i = 0; i < samples1D.size(); ++i)
	{
		StratifiedSample1D(&samples1D[i][0], xPixelSamples * yPixelSamples, jitterSamples);
		Shuffle(&samples1D[i][0], xPixelSamples * yPixelSamples, 1, rng);
	}
	for (size_t i = 0; i < samples2D.size(); ++i)
	{
		StratifiedSample2D(&samples2D[i][0], xPixelSamples, yPixelSamples, jitterSamples);
		Shuffle(&samples2D[i][0], xPixelSamples * yPixelSamples, 1, rng);
	}

	for (size_t i = 0; i < samples1DArraySizes.size(); ++i)
		for (int64_t j = 0; j < samplesPerPixel; ++j) {
			int count = samples1DArraySizes[i];
			StratifiedSample1D(&sampleArray1D[i][j * count], count, jitterSamples);
			Shuffle(&sampleArray1D[i][j * count], count, 1, rng);
		}
	for (size_t i = 0; i < samples2DArraySizes.size(); ++i)
		for (int64_t j = 0; j < samplesPerPixel; ++j) {
			int count = samples2DArraySizes[i];
			LatinHypercube(&sampleArray2D[i][j * count].x(), count, 2);
		}

	PixelSampler::StartPixel(p);
}

std::unique_ptr<Sampler> StratifiedSampler::Clone(int seed)
{
	return std::unique_ptr<Sampler>(new StratifiedSampler(*this));
}

void StratifiedSampler::StratifiedSample1D(Float* samp, int nSamples, bool jitter)
{
	Float invNSamples = (Float)1 / nSamples;
	for (int i = 0; i < nSamples; ++i)
	{
		Float delta = jitter ? random_float() : 0.5f;
		samp[i] = std::min((i + delta) * invNSamples, OneMinusEpsilon);
	}
}

void StratifiedSampler::StratifiedSample2D(Point2f* samp, int nx, int ny, bool jitter)
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

void StratifiedSampler::LatinHypercube(Float* samples, int nSamples, int nDim)
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

PixelSampler::~PixelSampler()
{
}

Float PixelSampler::Get1D()
{
	if (current1DDimension < samples1D.size())
		return samples1D[current1DDimension++][currentPixelSampleIndex];
	else
		return random_float();
}

Point2f PixelSampler::Get2D()
{
	if (current2DDimension < samples2D.size())
		return samples2D[current2DDimension++][currentPixelSampleIndex];
	else
		return Vector2f{ random_float(), random_float() };
}

void PixelSampler::StartPixel(const Point2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
	// Reset array offsets for next pixel sample
	array1DOffset = array2DOffset = 0;
}

bool PixelSampler::StartNextSample()
{
	current1DDimension = current2DDimension = 0;
	return Sampler::StartNextSample();
}

bool PixelSampler::SetSampleNumber(int64_t sampleNum)
{
	current1DDimension = current2DDimension = 0;
	return Sampler::SetSampleNumber(sampleNum);
}

std::unique_ptr<Sampler> PixelSampler::Clone(int seed)
{
	PixelSampler* ss = new PixelSampler(*this);
	return std::unique_ptr<Sampler>(ss);
}

void Sampler::StartPixel(const Point2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
	array1DOffset = array2DOffset = 0;
}

void Sampler::Request1DArray(int n)
{
	samples1DArraySizes.push_back(n);
	sampleArray1D.push_back(std::vector<Float>(n * samplesPerPixel));
}

void Sampler::Request2DArray(int n)
{
	samples2DArraySizes.push_back(n);
	sampleArray2D.push_back(std::vector<Point2f>(n * samplesPerPixel));
}

const Float* Sampler::Get1DArray(int n)
{
	if (array1DOffset == sampleArray1D.size())
		return nullptr;
	return &sampleArray1D[array1DOffset++][currentPixelSampleIndex * n];
}

const Point2f* Sampler::Get2DArray(int n)
{
	if (array2DOffset == sampleArray2D.size())
		return nullptr;
	return &sampleArray2D[array2DOffset++][currentPixelSampleIndex * n];
}

bool Sampler::StartNextSample()
{
	array1DOffset = array2DOffset = 0;
	return ++currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::SetSampleNumber(int64_t sampleNum)
{
	array1DOffset = array2DOffset = 0;
	currentPixelSampleIndex = sampleNum;
	return currentPixelSampleIndex < samplesPerPixel;
}

bool GlobalSampler::StartNextSample() {
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(currentPixelSampleIndex + 1);
	return Sampler::StartNextSample();
}

void GlobalSampler::StartPixel(const Point2i& p)
{
	Sampler::StartPixel(p);
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(0);
	// Compute _arrayEndDim_ for dimensions used for array samples
	arrayEndDim =
		arrayStartDim + sampleArray1D.size() + 2 * sampleArray2D.size();

	// Compute 1D array samples for _GlobalSampler_
	for (size_t i = 0; i < samples1DArraySizes.size(); ++i) {
		int nSamples = samples1DArraySizes[i] * samplesPerPixel;
		for (int j = 0; j < nSamples; ++j) {
			int64_t index = GetIndexForSample(j);
			sampleArray1D[i][j] = SampleDimension(index, arrayStartDim + i);
		}
	}

	// Compute 2D array samples for _GlobalSampler_
	int dim = arrayStartDim + samples1DArraySizes.size();
	for (size_t i = 0; i < samples2DArraySizes.size(); ++i) {
		int nSamples = samples2DArraySizes[i] * samplesPerPixel;
		for (int j = 0; j < nSamples; ++j) {
			int64_t idx = GetIndexForSample(j);
			sampleArray2D[i][j].x() = SampleDimension(idx, dim);
			sampleArray2D[i][j].y() = SampleDimension(idx, dim + 1);
		}
		dim += 2;
	}
}

bool GlobalSampler::SetSampleNumber(int64_t sampleNum) {
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(sampleNum);
	return Sampler::SetSampleNumber(sampleNum);
}

Float GlobalSampler::Get1D() {
	if (dimension >= arrayStartDim && dimension < arrayEndDim)
		dimension = arrayEndDim;
	return SampleDimension(intervalSampleIndex, dimension++);
}
Point2f GlobalSampler::Get2D() {
	if (dimension + 1 >= arrayStartDim && dimension < arrayEndDim)
		dimension = arrayEndDim;
	Point2f p{ SampleDimension(intervalSampleIndex, dimension), SampleDimension(intervalSampleIndex, dimension + 1) };
	dimension += 2;
	return p;
}

std::vector<uint16_t> ComputeRadicalInversePermutations(RNG& rng) {
	std::vector<uint16_t> perms;
	// Allocate space in _perms_ for radical inverse permutations
	int permArraySize = 0;
	for (int i = 0; i < PrimeTableSize; ++i) permArraySize += Primes[i];
	perms.resize(permArraySize);
	uint16_t* p = &perms[0];
	for (int i = 0; i < PrimeTableSize; ++i) {
		// Generate random permutation for $i$th prime base
		for (int j = 0; j < Primes[i]; ++j) p[j] = j;
		Shuffle(p, Primes[i], 1, rng);
		p += Primes[i];
	}
	return perms;
}

HaltonSampler::HaltonSampler(int samplesPerPixel, const Bounds2i& sampleBounds) : GlobalSampler(samplesPerPixel)
{
	//Generate random digit permutations for Halton sampler 452
	if (radicalInversePermutations.size() == 0)
	{
		RNG rng;
		radicalInversePermutations = ComputeRadicalInversePermutations(rng);
	}
	//	Find radical inverse base scales and exponents that cover sampling area 452
	Vector2i res = sampleBounds.pMax - sampleBounds.pMin;
	for (int i = 0; i < 2; ++i)
	{
		int base = (i == 0) ? 2 : 3;
		int scale = 1, exp = 0;
		while (scale < std::min(res[i], kMaxResolution))
		{
			scale *= base;
			++exp;
		}
		baseScales[i] = scale;
		baseExponents[i] = exp;
	}
	//	Compute stride in samples for visiting each pixel area 453
	sampleStride = baseScales[0] * baseScales[1];
	//	Compute multiplicative inverses for baseScales
}

Float HaltonSampler::SampleDimension(int64_t index, int dim) const
{
	if (dim == 0)
		return RadicalInverse(dim, index >> baseExponents[0]);
	else if (dim == 1)
		return RadicalInverse(dim, index / baseScales[1]);
	else
		return ScrambledRadicalInverse(dim, index, PermutationForDimension(dim));
}

std::unique_ptr<Sampler> HaltonSampler::Clone(int seed)
{
	return std::unique_ptr<Sampler>(new HaltonSampler(*this));
}
std::vector<uint16_t> HaltonSampler::radicalInversePermutations;
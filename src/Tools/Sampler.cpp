#include <Tools/Sampler/StratifiedSampler.h>

template <typename T>
void Shuffle(T* samp, int count, int nDimensions) {
	for (int i = 0; i < count; ++i) {
		int other = i + random_int(0, count - i);
		for (int j = 0; j < nDimensions; ++j)
			std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
	}
}

void StratifiedSampler::StartPixel(const Point2i& p)
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
		return vec2{ random_float(), random_float() };
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
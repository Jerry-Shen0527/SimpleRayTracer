#include <algorithm>

#include "CoefficientSpecturm.h"
#include <vector>
static const int sampledLambdaStart = 400;
static const int sampledLambdaEnd = 700;
static  const int nSpectralSamples = 30;

// Spectrum Method Definitions
bool SpectrumSamplesSorted(const float* lambda, const float* vals, int n) {
	for (int i = 0; i < n - 1; ++i)
		if (lambda[i] > lambda[i + 1]) return false;
	return true;
}

void SortSpectrumSamples(float* lambda, float* vals, int n) {
	std::vector<std::pair<float, float>> sortVec;
	sortVec.reserve(n);
	for (int i = 0; i < n; ++i)
		sortVec.push_back(std::make_pair(lambda[i], vals[i]));
	std::sort(sortVec.begin(), sortVec.end());
	for (int i = 0; i < n; ++i) {
		lambda[i] = sortVec[i].first;
		vals[i] = sortVec[i].second;
	}
}

class SampledSpectrum :public CoefficentSpectrum<nSpectralSamples>
{
public:
	SampledSpectrum(float v = 0.f);

	static float AverageSpectrumSamples(const float* lambda, const float* vals, int n, float lambda0, float lambda1);

	static SampledSpectrum FromSampled(const float* lambda, const float* v,
		int n) {
		// Sort samples if unordered, use sorted for returned spectrum
		if (!SpectrumSamplesSorted(lambda, v, n)) {
			std::vector<float> slambda(&lambda[0], &lambda[n]);
			std::vector<float> sv(&v[0], &v[n]);
			SortSpectrumSamples(&slambda[0], &sv[0], n);
			return FromSampled(&slambda[0], &sv[0], n);
		}
		SampledSpectrum r;
		for (int i = 0; i < nSpectralSamples; ++i) {
			// Compute average value of given SPD over $i$th sample's range
			float lambda0 = Lerp(float(i) / float(nSpectralSamples),
				sampledLambdaStart, sampledLambdaEnd);
			float lambda1 = Lerp(float(i + 1) / float(nSpectralSamples),
				sampledLambdaStart, sampledLambdaEnd);
			r.c[i] = AverageSpectrumSamples(lambda, v, n, lambda0, lambda1);
		}
		return r;
	}
};

inline SampledSpectrum::SampledSpectrum(float v)
{
	for (int i = 0; i < nSpectralSamples; ++i)
	{
		c[i] = v;
	}
}

inline float SampledSpectrum::AverageSpectrumSamples(const float* lambda, const float* vals, int n, float lambdaStart,
	float lambdaEnd)
{
	if (lambdaEnd <= lambda[0]) return vals[0];
	if (lambdaStart >= lambda[n - 1]) return vals[n - 1];
	if (n == 1) return vals[0];

	float sum = 0;
	if (lambdaStart < lambda[0]) sum += vals[0] * (lambda[0] - lambdaStart);
	if (lambdaEnd > lambda[n - 1]) sum += vals[n - 1] * (lambdaEnd - lambda[n - 1]);

	int i = 0;
	while (lambdaStart > lambda[i + 1])++i;

	auto interp = [lambda, vals](float w, int i) {
		return Lerp((w - lambda[i]) / (lambda[i + 1] - lambda[i]), vals[i],
			vals[i + 1]);
	};
	for (; i + 1 < n && lambdaEnd >= lambda[i]; ++i) {
		float segLambdaStart = std::max(lambdaStart, lambda[i]);
		float segLambdaEnd = std::min(lambdaEnd, lambda[i + 1]);
		sum += 0.5 * (interp(segLambdaStart, i) + interp(segLambdaEnd, i)) *
			(segLambdaEnd - segLambdaStart);
	}
	return sum / (lambdaEnd - lambdaStart);
}

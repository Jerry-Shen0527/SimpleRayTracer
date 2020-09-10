#include <algorithm>

#include "CoefficientSpecturm.h"
#include "RGBSpectrum.h"
#include <vector>
static const int sampledLambdaStart = 400;
static const int sampledLambdaEnd = 700;
static  const int nSpectralSamples = 30;

class SampledSpectrum :public CoefficientSpectrum<nSpectralSamples>
{
public:
	SampledSpectrum(float v = 0.f) : CoefficientSpectrum(v) {}
	SampledSpectrum(const CoefficientSpectrum<nSpectralSamples>& v)
		: CoefficientSpectrum<nSpectralSamples>(v) {}
	SampledSpectrum(const RGBSpectrum& r, SpectrumType t)
	{
		float rgb[3];
		r.ToRGB(rgb);
		*this = SampledSpectrum::FromRGB(rgb, t);
	}

	static float AverageSpectrumSamples(const float* lambda, const float* vals, int n, float lambda0, float lambda1);
	static SampledSpectrum FromSampled(const float* lambda, const float* v, int n);
	static SampledSpectrum FromRGB(const float rgb[3], SpectrumType type = SpectrumType::Illuminant);
	static SampledSpectrum FromXYZ(const float xyz[3], SpectrumType type = SpectrumType::Reflectance);

	static void Init();

	void ToXYZ(float xyz[3]) const;
	void ToRGB(float rgb[3]) const;
	float y() const;

private:
	static SampledSpectrum X, Y, Z;
	static float yint;

	static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
	static SampledSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
	static SampledSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
	static SampledSpectrum rgbRefl2SpectBlue;
	static SampledSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
	static SampledSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
	static SampledSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
	static SampledSpectrum rgbIllum2SpectBlue;
};

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

inline SampledSpectrum SampledSpectrum::FromSampled(const float* lambda, const float* v, int n)
{
	// Sort samples if unordered, use sorted for returned spectrum
	if (!SpectrumSamplesSorted(lambda, v, n))
	{
		std::vector<float> slambda(&lambda[0], &lambda[n]);
		std::vector<float> sv(&v[0], &v[n]);
		SortSpectrumSamples(&slambda[0], &sv[0], n);
		return FromSampled(&slambda[0], &sv[0], n);
	}
	SampledSpectrum r;
	for (int i = 0; i < nSpectralSamples; ++i)
	{
		// Compute average value of given SPD over $i$th sample's range
		float lambda0 = Lerp(float(i) / float(nSpectralSamples),
			sampledLambdaStart, sampledLambdaEnd);
		float lambda1 = Lerp(float(i + 1) / float(nSpectralSamples),
			sampledLambdaStart, sampledLambdaEnd);
		r.c[i] = AverageSpectrumSamples(lambda, v, n, lambda0, lambda1);
	}
	return r;
}

inline SampledSpectrum SampledSpectrum::FromRGB(const float rgb[3], SpectrumType type)
{
	SampledSpectrum r;
	if (type == SpectrumType::Reflectance) {
		// Convert reflectance spectrum to RGB
		if (rgb[0] <= rgb[1] && rgb[0] <= rgb[2]) {
			// Compute reflectance _SampledSpectrum_ with _rgb[0]_ as minimum
			r += rgb[0] * rgbRefl2SpectWhite;
			if (rgb[1] <= rgb[2]) {
				r += (rgb[1] - rgb[0]) * rgbRefl2SpectCyan;
				r += (rgb[2] - rgb[1]) * rgbRefl2SpectBlue;
			}
			else {
				r += (rgb[2] - rgb[0]) * rgbRefl2SpectCyan;
				r += (rgb[1] - rgb[2]) * rgbRefl2SpectGreen;
			}
		}
		else if (rgb[1] <= rgb[0] && rgb[1] <= rgb[2]) {
			// Compute reflectance _SampledSpectrum_ with _rgb[1]_ as minimum
			r += rgb[1] * rgbRefl2SpectWhite;
			if (rgb[0] <= rgb[2]) {
				r += (rgb[0] - rgb[1]) * rgbRefl2SpectMagenta;
				r += (rgb[2] - rgb[0]) * rgbRefl2SpectBlue;
			}
			else {
				r += (rgb[2] - rgb[1]) * rgbRefl2SpectMagenta;
				r += (rgb[0] - rgb[2]) * rgbRefl2SpectRed;
			}
		}
		else {
			// Compute reflectance _SampledSpectrum_ with _rgb[2]_ as minimum
			r += rgb[2] * rgbRefl2SpectWhite;
			if (rgb[0] <= rgb[1]) {
				r += (rgb[0] - rgb[2]) * rgbRefl2SpectYellow;
				r += (rgb[1] - rgb[0]) * rgbRefl2SpectGreen;
			}
			else {
				r += (rgb[1] - rgb[2]) * rgbRefl2SpectYellow;
				r += (rgb[0] - rgb[1]) * rgbRefl2SpectRed;
			}
		}
		r *= .94;
	}
	else {
		// Convert illuminant spectrum to RGB
		if (rgb[0] <= rgb[1] && rgb[0] <= rgb[2]) {
			// Compute illuminant _SampledSpectrum_ with _rgb[0]_ as minimum
			r += rgb[0] * rgbIllum2SpectWhite;
			if (rgb[1] <= rgb[2]) {
				r += (rgb[1] - rgb[0]) * rgbIllum2SpectCyan;
				r += (rgb[2] - rgb[1]) * rgbIllum2SpectBlue;
			}
			else {
				r += (rgb[2] - rgb[0]) * rgbIllum2SpectCyan;
				r += (rgb[1] - rgb[2]) * rgbIllum2SpectGreen;
			}
		}
		else if (rgb[1] <= rgb[0] && rgb[1] <= rgb[2]) {
			// Compute illuminant _SampledSpectrum_ with _rgb[1]_ as minimum
			r += rgb[1] * rgbIllum2SpectWhite;
			if (rgb[0] <= rgb[2]) {
				r += (rgb[0] - rgb[1]) * rgbIllum2SpectMagenta;
				r += (rgb[2] - rgb[0]) * rgbIllum2SpectBlue;
			}
			else {
				r += (rgb[2] - rgb[1]) * rgbIllum2SpectMagenta;
				r += (rgb[0] - rgb[2]) * rgbIllum2SpectRed;
			}
		}
		else {
			// Compute illuminant _SampledSpectrum_ with _rgb[2]_ as minimum
			r += rgb[2] * rgbIllum2SpectWhite;
			if (rgb[0] <= rgb[1]) {
				r += (rgb[0] - rgb[2]) * rgbIllum2SpectYellow;
				r += (rgb[1] - rgb[0]) * rgbIllum2SpectGreen;
			}
			else {
				r += (rgb[1] - rgb[2]) * rgbIllum2SpectYellow;
				r += (rgb[0] - rgb[1]) * rgbIllum2SpectRed;
			}
		}
		r *= .86445f;
	}
	return r.Clamp();
}

inline SampledSpectrum SampledSpectrum::FromXYZ(const float xyz[3], SpectrumType type)
{
	float rgb[3];
	XYZToRGB(xyz, rgb);
	return FromRGB(rgb, type);
}

inline void SampledSpectrum::Init()
{
	for (int i = 0; i < nSpectralSamples; ++i) {
		float wl0 = Lerp(float(i) / float(nSpectralSamples),
			sampledLambdaStart, sampledLambdaEnd);
		float wl1 = Lerp(float(i + 1) / float(nSpectralSamples),
			sampledLambdaStart, sampledLambdaEnd);
		X.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_X, nCIESamples, wl0,
			wl1);
		Y.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Y, nCIESamples, wl0,
			wl1);
		Z.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Z, nCIESamples, wl0,
			wl1);
		yint += Y.c[i];
	}
}

inline void SampledSpectrum::ToXYZ(float xyz[3]) const
{
	xyz[0] = xyz[1] = xyz[2] = 0.f;
	for (int i = 0; i < nSpectralSamples; ++i)
	{
		xyz[0] += X.c[i] * c[i];
		xyz[1] += Y.c[i] * c[i];
		xyz[2] += Z.c[i] * c[i];
	}
	xyz[0] /= yint;
	xyz[1] /= yint;
	xyz[2] /= yint;
}

inline void SampledSpectrum::ToRGB(float rgb[3]) const
{
	float xyz[3];
	ToXYZ(xyz);
	XYZToRGB(xyz, rgb);
}

inline float SampledSpectrum::y() const
{
	float yy = 0.f;
	for (int i = 0; i < nSpectralSamples; ++i)
	{
		yy += Y.c[i] * c[i];
	}
	return yy / yint;
}

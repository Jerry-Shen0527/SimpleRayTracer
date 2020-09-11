#pragma once
#include <vector>

#include "CoefficientSpecturm.h"
#include <Tools/Math/math_tools.h>

class RGBSpectrum :public CoefficientSpectrum<3>
{
public:
	RGBSpectrum(float v = 0.f) :CoefficientSpectrum<3>(v) {}
	RGBSpectrum(const CoefficientSpectrum<3>& v) :CoefficientSpectrum<3>(v) {}

	static RGBSpectrum FromRGB(const float rgb[3], SpectrumType type = SpectrumType::Illuminant);
	static RGBSpectrum FromXYZ(const float xyz[3], SpectrumType type = SpectrumType::Illuminant);
	static RGBSpectrum FromSampled(const float* lambda, const float* v, int n);

	static float InterpolateSpectrumSamples(const float* lambda, const float* p, int i, const float x);

	void ToRGB(float* rgb) const;
	float y() const;

	const RGBSpectrum& ToRGBSpectrum() const;

	void ToXYZ(float* xyz) const;
};

inline RGBSpectrum RGBSpectrum::FromRGB(const float rgb[3], SpectrumType type)
{
	RGBSpectrum s;
	s.c[0] = rgb[0];
	s.c[1] = rgb[1];
	s.c[2] = rgb[2];
	return s;
}

inline RGBSpectrum RGBSpectrum::FromXYZ(const float xyz[3], SpectrumType type)
{
	float rgb[3];

	XYZToRGB(xyz, rgb);
	return FromRGB(rgb, type);
}

inline float RGBSpectrum::InterpolateSpectrumSamples(const float* lambda, const float* vals, int n, const float l)
{
	if (l <= lambda[0]) return vals[0];
	if (l >= lambda[n - 1]) return vals[n - 1];
	for (int i = 0; i < n - 1; ++i)
	{
		if (l >= lambda[i] && l <= lambda[i + 1])
		{
			float t = (l - lambda[i]) / (lambda[i + 1] - lambda[i]);
			return Lerp(t, vals[i], vals[i + 1]);
		}
	}
}

inline RGBSpectrum RGBSpectrum::FromSampled(const float* lambda, const float* v, int n)
{
	// Sort samples if unordered, use sorted for returned spectrum
	if (!SpectrumSamplesSorted(lambda, v, n))
	{
		std::vector<float> slambda(&lambda[0], &lambda[n]);
		std::vector<float> sv(&v[0], &v[n]);
		SortSpectrumSamples(&slambda[0], &sv[0], n);
		return FromSampled(&slambda[0], &sv[0], n);
	}
	float xyz[3] = { 0, 0, 0 };
	float yint = 0.f;
	for (int i = 0; i < nCIESamples; ++i)
	{
		yint += CIE_Y[i];
		float val = InterpolateSpectrumSamples(lambda, v, n, CIE_lambda[i]);
		xyz[0] += val * CIE_X[i];
		xyz[1] += val * CIE_Y[i];
		xyz[2] += val * CIE_Z[i];
	}
	xyz[0] /= yint;
	xyz[1] /= yint;
	xyz[2] /= yint;
	return FromXYZ(xyz);
}

inline void RGBSpectrum::ToRGB(float* rgb) const
{
	rgb[0] = c[0];
	rgb[1] = c[1];
	rgb[2] = c[2];
}

inline float RGBSpectrum::y() const
{
	float xyz[3];
	float rgb[3];
	ToRGB(rgb);

	RGBToXYZ(rgb, xyz);
	return xyz[1];
}

inline const RGBSpectrum& RGBSpectrum::ToRGBSpectrum() const
{
	return *this;
}

inline void RGBSpectrum::ToXYZ(float* xyz) const
{
	float rgb[3];
	ToRGB(rgb);
	RGBToXYZ(rgb, xyz);
}

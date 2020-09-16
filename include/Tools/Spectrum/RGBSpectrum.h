#pragma once
#include <vector>

#include "CoefficientSpecturm.h"
#include <Tools/Math/math_tools.h>

class RGBSpectrum :public CoefficientSpectrum<3>
{
public:
	RGBSpectrum(double v = 0.f) :CoefficientSpectrum<3>(v) {}
	RGBSpectrum(const CoefficientSpectrum<3>& v) :CoefficientSpectrum<3>(v) {}

	static RGBSpectrum FromRGB(const vec3& rgb, SpectrumType type = SpectrumType::Illuminant);
	static RGBSpectrum FromXYZ(const vec3& xyz, SpectrumType type = SpectrumType::Illuminant);
	static RGBSpectrum FromSampled(const double* lambda, const double* v, int n);

	static double InterpolateSpectrumSamples(const double* lambda, const double* p, int i, const double x);

	void ToRGB(vec3& rgb) const;
	double y() const;

	const RGBSpectrum& ToRGBSpectrum() const;

	void ToXYZ(vec3& xyz) const;
};

inline RGBSpectrum RGBSpectrum::FromRGB(const vec3& rgb, SpectrumType type)
{
	RGBSpectrum s;
	s.c[0] = rgb[0];
	s.c[1] = rgb[1];
	s.c[2] = rgb[2];
	return s;
}

inline RGBSpectrum RGBSpectrum::FromXYZ(const vec3& xyz, SpectrumType type)
{
	vec3 rgb;

	XYZToRGB(xyz, rgb);
	return FromRGB(rgb, type);
}

inline double RGBSpectrum::InterpolateSpectrumSamples(const double* lambda, const double* vals, int n, const double l)
{
	if (l <= lambda[0]) return vals[0];
	if (l >= lambda[n - 1]) return vals[n - 1];
	for (int i = 0; i < n - 1; ++i)
	{
		if (l >= lambda[i] && l <= lambda[i + 1])
		{
			double t = (l - lambda[i]) / (lambda[i + 1] - lambda[i]);
			return Lerp(t, vals[i], vals[i + 1]);
		}
	}
}

inline RGBSpectrum RGBSpectrum::FromSampled(const double* lambda, const double* v, int n)
{
	// Sort samples if unordered, use sorted for returned spectrum
	if (!SpectrumSamplesSorted(lambda, v, n))
	{
		std::vector<double> slambda(&lambda[0], &lambda[n]);
		std::vector<double> sv(&v[0], &v[n]);
		SortSpectrumSamples(&slambda[0], &sv[0], n);
		return FromSampled(&slambda[0], &sv[0], n);
	}
	vec3 xyz = { 0, 0, 0 };
	double yint = 0.f;
	for (int i = 0; i < nCIESamples; ++i)
	{
		yint += CIE_Y[i];
		double val = InterpolateSpectrumSamples(lambda, v, n, CIE_lambda[i]);
		xyz[0] += val * CIE_X[i];
		xyz[1] += val * CIE_Y[i];
		xyz[2] += val * CIE_Z[i];
	}
	xyz[0] /= yint;
	xyz[1] /= yint;
	xyz[2] /= yint;
	return FromXYZ(xyz);
}

inline void RGBSpectrum::ToRGB(vec3& rgb) const
{
	rgb[0] = c[0];
	rgb[1] = c[1];
	rgb[2] = c[2];
}

inline double RGBSpectrum::y() const
{
	vec3 xyz;
	vec3 rgb;
	ToRGB(rgb);

	RGBToXYZ(rgb, xyz);
	return xyz[1];
}

inline const RGBSpectrum& RGBSpectrum::ToRGBSpectrum() const
{
	return *this;
}

inline void RGBSpectrum::ToXYZ(vec3& xyz) const
{
	vec3 rgb;
	ToRGB(rgb);
	RGBToXYZ(rgb, xyz);
}

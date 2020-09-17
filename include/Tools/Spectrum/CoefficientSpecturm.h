#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <Tools/Math/vec3.h>

#include "BRDF/BxDF_Utility.h"

template<int nSamples>
class CoefficientSpectrum
{
public:

	CoefficientSpectrum(double v = 0.f)
	{
		for (int i = 0; i < nSamples; ++i)
		{
			c[i] = v;
		}
	}

	CoefficientSpectrum& operator+=(const CoefficientSpectrum& s2);
	CoefficientSpectrum operator+(const CoefficientSpectrum& s2) const;
	CoefficientSpectrum& operator-=(const CoefficientSpectrum& s2);
	CoefficientSpectrum operator-(const CoefficientSpectrum& s2) const;
	CoefficientSpectrum& operator*=(const CoefficientSpectrum& s2);
	CoefficientSpectrum& operator*=(double f);
	CoefficientSpectrum operator*(const CoefficientSpectrum& s2) const;
	CoefficientSpectrum operator/(const CoefficientSpectrum& s2) const;
	CoefficientSpectrum operator*(double x) const;
	CoefficientSpectrum operator/(double x) const;

	CoefficientSpectrum Clamp(double low = 0, double high = INFINITY) const;

	void print() const;
	bool is_black() const;
	bool has_nans() const;

	template <int nSamples> friend CoefficientSpectrum<nSamples> Sqrt(const CoefficientSpectrum<nSamples>& s);
	template <int nSamples> friend CoefficientSpectrum<nSamples> Exp(const CoefficientSpectrum<nSamples>& s);
	template <int nSamples> friend CoefficientSpectrum<nSamples> Pow(const CoefficientSpectrum<nSamples>& s, double x);
	template <int nSamples> friend CoefficientSpectrum<nSamples> operator*(double x, const CoefficientSpectrum<nSamples>& s);

protected:
	double c[nSamples];
};

template <int nSamples>
CoefficientSpectrum<nSamples>& CoefficientSpectrum<nSamples>::operator+=(const CoefficientSpectrum& s2)
{
	for (int i = 0; i < nSamples; ++i)
	{
		c[i] += s2.c[i];
	}
	return *this;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator+(const CoefficientSpectrum& s2) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] += s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples>& CoefficientSpectrum<nSamples>::operator-=(const CoefficientSpectrum& s2)
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] -= s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator-(const CoefficientSpectrum& s2) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] -= s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples>& CoefficientSpectrum<nSamples>::operator*=(const CoefficientSpectrum& s2)
{
	for (int i = 0; i < nSamples; ++i)
	{
		c[i] *= s2.c[i];
	}
	return *this;
}

template <int nSamples>
CoefficientSpectrum<nSamples>& CoefficientSpectrum<nSamples>::operator*=(double f)
{
	for (int i = 0; i < nSamples; ++i)
	{
		c[i] *= f;
	}
	return *this;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator*(const CoefficientSpectrum& s2) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] *= s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator/(const CoefficientSpectrum& s2) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] /= s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator*(double x) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] *= x;
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator/(double x) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] /= x;
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::Clamp(double low, double high) const
{
	CoefficientSpectrum ret;
	for (int i = 0; i < nSamples; ++i)
		ret.c[i] = clamp(c[i], low, high);
	return ret;
}

template <int nSamples>
void CoefficientSpectrum<nSamples>::print() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		std::cerr << c[i] << ' ';
	}
}

template <int nSamples>
bool CoefficientSpectrum<nSamples>::is_black() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		if (c[i] != 0.) return false;

		return true;
	}
}

template <int nSamples>
bool CoefficientSpectrum<nSamples>::has_nans() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		if (isnan(c[i])) return true;
		return false;
	}
}

template <int nSamples>
CoefficientSpectrum<nSamples> Sqrt(const CoefficientSpectrum<nSamples>& s)
{
	CoefficientSpectrum<nSamples> rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = sqrtf(s.c[i]);
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> Pow(const CoefficientSpectrum<nSamples>& s, double x)
{
	CoefficientSpectrum<nSamples> rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = powf(s.c[i], x);
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> Exp(const CoefficientSpectrum<nSamples>& s)
{
	CoefficientSpectrum<nSamples> rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = expf(s.c[i]);
	}
	return rst;
}

template<int nSamples>
inline CoefficientSpectrum<nSamples> Lerp(double t, const CoefficientSpectrum<nSamples>& s1, const CoefficientSpectrum<nSamples>& s2)
{
	return (1.f - t) * s1 + t * s2;
}
template <int nSamples>
CoefficientSpectrum<nSamples> operator*(double x, const CoefficientSpectrum<nSamples>& s)
{
	return s * x;
}

enum class SpectrumType { Reflectance, Illuminant };

inline void XYZToRGB(const vec3& xyz, vec3& rgb) {
	rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
	rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
	rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const vec3& rgb, vec3& xyz) {
	xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
	xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
	xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

// Spectrum Method Definitions
inline bool SpectrumSamplesSorted(const double* lambda, const double* vals, int n) {
	for (int i = 0; i < n - 1; ++i)
		if (lambda[i] > lambda[i + 1]) return false;
	return true;
}

inline void SortSpectrumSamples(double* lambda, double* vals, int n) {
	std::vector<std::pair<double, double>> sortVec;
	sortVec.reserve(n);
	for (int i = 0; i < n; ++i)
		sortVec.push_back(std::make_pair(lambda[i], vals[i]));
	std::sort(sortVec.begin(), sortVec.end());
	for (int i = 0; i < n; ++i) {
		lambda[i] = sortVec[i].first;
		vals[i] = sortVec[i].second;
	}
}

//CIE data. CIE: basis of eye response functions
static const int nCIESamples = 471;
extern const double CIE_X[nCIESamples];
extern const double CIE_Y[nCIESamples];
extern const double CIE_Z[nCIESamples];
extern const double CIE_lambda[nCIESamples];

//RGB to spectrum data
static const int nRGB2SpectSamples = 32;
extern const double RGB2SpectLambda[nRGB2SpectSamples];
extern const double RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const double RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const double RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const double RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const double RGBRefl2SpectRed[nRGB2SpectSamples];
extern const double RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const double RGBRefl2SpectBlue[nRGB2SpectSamples];
//Illumination conversion can be achieved using this data
extern const double RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const double RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const double RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const double RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const double RGBIllum2SpectRed[nRGB2SpectSamples];
extern const double RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const double RGBIllum2SpectBlue[nRGB2SpectSamples];

void Blackbody(const Float* lambda, int n, Float T, Float* Le) {
	const Float c = 299792458;
	const Float h = 6.62606957e-34;
	const Float kb = 1.3806488e-23;
	for (int i = 0; i < n; ++i) {
		Float l = lambda[i] * 1e-9;
		Float lambda5 = (l * l) * (l * l) * l;
		Le[i] = (2 * h * c * c) /
			(lambda5 * (std::exp((h * c) / (l * kb * T)) - 1));
	}
}

void BlackbodyNormalized(const Float* lambda, int n, Float T,
	Float* Le) {
	Blackbody(lambda, n, T, Le);

	Float lambdaMax = 2.8977721e-3 / T * 1e9;
	Float maxL;
	Blackbody(&lambdaMax, 1, T, &maxL);
	for (int i = 0; i < n; ++i)
		Le[i] /= maxL;
}
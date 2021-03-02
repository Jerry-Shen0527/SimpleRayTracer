#pragma once
#include <vector>
#include <Tools/Mueller.h>


#include "config.h"
#include <Tools/Math/math_tools.h>

inline Float clamp(Float x, Float min, Float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

template<int nSamples>
class CoefficientSpectrum
{
public:

	CoefficientSpectrum(Float v = 0.f)
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
	CoefficientSpectrum& operator*=(Float f);
	CoefficientSpectrum& operator/=(Float f);
	CoefficientSpectrum operator*(const CoefficientSpectrum& s2) const;
	CoefficientSpectrum operator/(const CoefficientSpectrum& s2) const;
	CoefficientSpectrum operator*(Float x) const;
	CoefficientSpectrum operator/(Float x) const;
	CoefficientSpectrum operator-() const;

	Float& operator[](int idx) { return c[idx]; }

	const Float& operator[](int idx) const { return c[idx]; }

	CoefficientSpectrum Clamp(Float low = 0, Float high = INFINITY) const;

	void print() const;
	bool IsBlack() const;
	bool has_nans() const;

	template <int nSamples> friend CoefficientSpectrum<nSamples> Sqrt(const CoefficientSpectrum<nSamples>& s);
	template <int nSamples> friend CoefficientSpectrum<nSamples> Exp(const CoefficientSpectrum<nSamples>& s);
	template <int nSamples> friend CoefficientSpectrum<nSamples> Pow(const CoefficientSpectrum<nSamples>& s, Float x);
	template <int nSamples> friend CoefficientSpectrum<nSamples> operator*(Float x, const CoefficientSpectrum<nSamples>& s);


	MuellerMatrix mueller_spectrum;
	
protected:
	Float c[nSamples];
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
CoefficientSpectrum<nSamples>& CoefficientSpectrum<nSamples>::operator*=(Float f)
{
	for (int i = 0; i < nSamples; ++i)
	{
		c[i] *= f;
	}
	return *this;
}

template <int nSamples>
CoefficientSpectrum<nSamples>& CoefficientSpectrum<nSamples>::operator/=(Float f)
{
	for (int i = 0; i < nSamples; ++i)
	{
		c[i] /= f;
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
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator*(Float x) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] *= x;
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator/(Float x) const
{
	CoefficientSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] /= x;
	}
	return rst;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::operator-() const
{
	return Spectrum() - *this;
}

template <int nSamples>
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>::Clamp(Float low, Float high) const
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
bool CoefficientSpectrum<nSamples>::IsBlack() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		if (c[i] != 0.) return false;
	}
	return true;
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
CoefficientSpectrum<nSamples> Pow(const CoefficientSpectrum<nSamples>& s, Float x)
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
inline CoefficientSpectrum<nSamples> Lerp(Float t, const CoefficientSpectrum<nSamples>& s1, const CoefficientSpectrum<nSamples>& s2)
{
	return (1.f - t) * s1 + t * s2;
}
template <int nSamples>
CoefficientSpectrum<nSamples> operator*(Float x, const CoefficientSpectrum<nSamples>& s)
{
	return s * x;
}

enum class SpectrumType { Reflectance, Illuminant };

inline void XYZToRGB(const Float xyz[3], Float rgb[3]) {
	rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
	rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
	rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const Float rgb[3], Float xyz[3]) {
	xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
	xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
	xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

// Spectrum Method Definitions
inline bool SpectrumSamplesSorted(const Float* lambda, const Float* vals, int n) {
	for (int i = 0; i < n - 1; ++i)
		if (lambda[i] > lambda[i + 1]) return false;
	return true;
}

inline void SortSpectrumSamples(Float* lambda, Float* vals, int n) {
	std::vector<std::pair<Float, Float>> sortVec;
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
extern const Float CIE_X[nCIESamples];
extern const Float CIE_Y[nCIESamples];
extern const Float CIE_Z[nCIESamples];
extern const Float CIE_lambda[nCIESamples];

//RGB to spectrum data
static const int nRGB2SpectSamples = 32;
extern const Float RGB2SpectLambda[nRGB2SpectSamples];
extern const Float RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const Float RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const Float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const Float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const Float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const Float RGBRefl2SpectBlue[nRGB2SpectSamples];
//Illumination conversion can be achieved using this data
extern const Float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const Float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const Float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const Float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const Float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const Float RGBIllum2SpectBlue[nRGB2SpectSamples];

inline void Blackbody(const Float* lambda, int n, Float T, Float* Le) {
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

inline void BlackbodyNormalized(const Float* lambda, int n, Float T,
	Float* Le) {
	Blackbody(lambda, n, T, Le);

	Float lambdaMax = 2.8977721e-3 / T * 1e9;
	Float maxL;
	Blackbody(&lambdaMax, 1, T, &maxL);
	for (int i = 0; i < n; ++i)
		Le[i] /= maxL;
}
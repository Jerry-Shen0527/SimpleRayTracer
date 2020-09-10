#pragma once
#include <iostream>
#include <Tools/Math/math_tools.h>

template<int nSamples>
class CoefficentSpectrum
{
public:

	CoefficentSpectrum(float v = 0.f)
	{
		for (int i = 0; i < nSamples; ++i)
		{
			c[i] = v;
		}
	}

	CoefficentSpectrum& operator+=(const CoefficentSpectrum& s2);
	CoefficentSpectrum operator+(const CoefficentSpectrum& s2) const;
	CoefficentSpectrum operator*(const CoefficentSpectrum& s2) const;
	CoefficentSpectrum operator*(float x) const;

	CoefficentSpectrum Clamp(float low = 0, float high = INFINITY) const;

	void print() const;
	bool is_black() const;
	bool has_nans() const;

	template <int nSamples> friend CoefficentSpectrum<nSamples> Sqrt(const CoefficentSpectrum<nSamples>& s);
	template <int nSamples> friend CoefficentSpectrum<nSamples> Exp(const CoefficentSpectrum<nSamples>& s);
	template <int nSamples> friend CoefficentSpectrum<nSamples> Pow(const CoefficentSpectrum<nSamples>& s, float x);
	template <int nSamples> friend CoefficentSpectrum<nSamples> operator*(float x, const CoefficentSpectrum<nSamples>& s);

protected:
	float c[nSamples];
};

template <int nSamples>
CoefficentSpectrum<nSamples>& CoefficentSpectrum<nSamples>::operator+=(const CoefficentSpectrum& s2)
{
	for (int i = 0; i < nSamples; ++i)
	{
		c[i] += s2.c[i];
	}
	return *this;
}

template <int nSamples>
CoefficentSpectrum<nSamples> CoefficentSpectrum<nSamples>::operator+(const CoefficentSpectrum& s2) const
{
	CoefficentSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] += s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficentSpectrum<nSamples> CoefficentSpectrum<nSamples>::operator*(const CoefficentSpectrum& s2) const
{
	CoefficentSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] *= s2.c[i];
	}
	return rst;
}

template <int nSamples>
CoefficentSpectrum<nSamples> CoefficentSpectrum<nSamples>::operator*(float x) const
{
	CoefficentSpectrum rst = *this;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] *= x;
	}
	return rst;
}

template <int nSamples>
CoefficentSpectrum<nSamples> CoefficentSpectrum<nSamples>::Clamp(float low, float high) const
{
	CoefficentSpectrum rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = clamp(c[i], low, high);
	}
	return rst;
}

template <int nSamples>
void CoefficentSpectrum<nSamples>::print() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		std::cerr << c[i] << ' ';
	}
}

template <int nSamples>
bool CoefficentSpectrum<nSamples>::is_black() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		if (c[i] != 0.) return false;

		return true;
	}
}

template <int nSamples>
bool CoefficentSpectrum<nSamples>::has_nans() const
{
	for (int i = 0; i < nSamples; ++i)
	{
		if (isnan(c[i])) return true;
		return false;
	}
}

template <int nSamples>
CoefficentSpectrum<nSamples> Sqrt(const CoefficentSpectrum<nSamples>& s)
{
	CoefficentSpectrum<nSamples> rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = sqrtf(s.c[i]);
	}
	return rst;
}

template <int nSamples>
CoefficentSpectrum<nSamples> Pow(const CoefficentSpectrum<nSamples>& s, float x)
{
	CoefficentSpectrum<nSamples> rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = powf(s.c[i], x);
	}
	return rst;
}

template <int nSamples>
CoefficentSpectrum<nSamples> Exp(const CoefficentSpectrum<nSamples>& s)
{
	CoefficentSpectrum<nSamples> rst;
	for (int i = 0; i < nSamples; ++i)
	{
		rst.c[i] = expf(s.c[i]);
	}
	return rst;
}

template<int nSamples>
inline CoefficentSpectrum<nSamples> Lerp(float t, const CoefficentSpectrum<nSamples>& s1, const CoefficentSpectrum<nSamples>& s2)
{
	return (1.f - t) * s1 + t * s2;
}
template <int nSamples>
CoefficentSpectrum<nSamples> operator*(float x, const CoefficentSpectrum<nSamples>& s)
{
	return s * x;
}
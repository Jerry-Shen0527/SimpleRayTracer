#pragma once
#include <BRDF/BxDF_Utility.h>
template < typename T, int n> class Bounds;

template <typename T, int n> bool Inside(const vec<T, n>& p, const Bounds<T, n>& b);

template < typename T, int n> class Bounds {
public:
	Bounds() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = vec<T, n>(maxNum);
		pMax = vec<T, n>(minNum);
	}
	Bounds(const vec<T, n>& p) : pMin(p), pMax(p) { }

	Bounds(const vec<T, n>& p1, const vec<T, n>& p2) : pMin(minimum(p1, p2)), pMax(maximum(p1, p2)) {	}

	vec<T, n> Diagonal() const { return pMax - pMin; }

	T SurfaceArea() const;
	T Volume() const;

	int MaximumExtent() const;

	vec<T, n> Lerp(const vec<T, n>& t) const {
		return t * pMin + (vec<T, n>(1) - t) * pMax;
	}

	vec<T, n> Offset(const vec<T, n>& p) const {
		vec<T, n>o = p - pMin;
		return o / (pMax - pMin);
	}

	void BoundingSphere(vec<T, n>* center, Float* radius) const {
		*center = (pMin + pMax) / 2;
		*radius = Inside(*center, *this) ? (*center - pMax).length() : 0;
	}

	const vec<T, n>& operator[](int i) const;
	vec<T, n>& operator[](int i);

	vec<T, n> Corner(int corner) const;

	vec<T, n> pMin;
	vec<T, n> pMax;
};

template <typename T, int n>
T Bounds<T, n>::SurfaceArea() const
{
	vec<T, n> d = Diagonal();
	T rst = 0;
	for (int i = 0; i < n; ++i)
	{
		T temp = 1;
		for (int j = 0; j < n; ++j)
			if (j != i)	temp *= d[j];
		rst += temp;
	}
	return 2 * rst;
}

template <typename T, int n>
T Bounds<T, n>::Volume() const
{
	vec<T, n> d = Diagonal();
	T temp = 1;
	for (int i = 0; i < n; ++i)
	{
		temp *= d[i];
	}
	return temp;
}

template <typename T, int n>
int Bounds<T, n>::MaximumExtent() const
{
	vec<T, n> d = Diagonal();
	int i = 0;
	T temp = d[0];
	for (int j = 0; j < n; ++j)
	{
		if (d[j] > temp)
		{
			temp = d[j];
			i = j;
		}
	}
	return i;
}

template <typename T, int n>
const vec<T, n>& Bounds<T, n>::operator[](int i) const
{
	if (i == 0)
		return pMin;
	return  pMax;
}

template <typename T, int n>
vec<T, n>& Bounds<T, n>::operator[](int i)
{
	if (i == 0)
		return pMin;
	return  pMax;
}

template <typename T, int n>
vec<T, n> Bounds<T, n>::Corner(int corner) const
{
	int bit = 1;
	vec<T, n> temp(0);
	for (int i = 0; i < n; ++i)
	{
		temp[i] = (*this)[(corner & bit) ? 1 : 0][i];
		bit *= 2;
	}
	return temp;
}

template <typename T, int n>
Bounds<T, n> Union(const Bounds<T, n>& b, const vec<T, n>& p)
{
	return Bounds<T, n>(minimum(b.pMin, p), maximum(b.pMax, p));
}

template <typename T, int n>
Bounds<T, n> Union(const Bounds<T, n>& b1, const Bounds<T, n>& b2)
{
	return Bounds<T, n>(minimum(b1.pMin, b2.pMin), maximum(b1.pMax, b2.pMax));
}

template <typename T, int n>
Bounds<T, n> Intersect(const Bounds<T, n>& b1, const Bounds<T, n>& b2)
{
	return Bounds<T, n>(maximum(b1.pMin, b2.pMin), minimum(b1.pMax, b2.pMax));
}

template <typename T, int n>
bool Overlaps(const Bounds<T, n>& b1, const Bounds<T, n>& b2)
{
	for (int i = 0; i < n; ++i)
	{
		if (!(b1.pMax[i] >= b2.pMin[i]) && (b1.pMin[i] <= b2.pMax[i]))
			return  false;
	}
	return true;
}

template <typename T, int n>
bool Inside(const vec<T, n>& p, const Bounds<T, n>& b)
{
	for (int i = 0; i < n; ++i)
	{
		if (!(p[i] >= b.pMin[i] && p[i] <= b.pMax[i])) return false;
	}
	return true;
}

template <typename T, int n>
bool InsideExclusive(const vec<T, n>& p, const Bounds<T, n>& b)
{
	for (int i = 0; i < n; ++i)
	{
		if (!(p[i] > b.pMin[i] && p[i] < b.pMax[i])) return false;
	}
	return true;
}

template <typename T, typename U, int n>
inline Bounds<T, n>	Expand(const Bounds<T, n>& b, U delta) {
	return Bounds<T, n>(b.pMin - vec<T, n>(delta),
		b.pMax + vec<T, n>(delta));
}

typedef Bounds<Float, 2> Bounds2f;
typedef Bounds<int, 2> Bounds2i;
typedef Bounds<Float, 3> Bounds3f;
typedef Bounds<int, 3> Bounds3i;
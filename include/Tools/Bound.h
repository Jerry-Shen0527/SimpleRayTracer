#pragma once
#include <BRDF/BxDF_Utility.h>
template < typename T, int n> class Bounds {
public:
	Bounds() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = vec<T, n>(maxNum);
		pMax = vec<T, n>(minNum);
	}
	Bounds(const vec<T, n>& p) : pMin(p), pMax(p) { }

	Bounds(const vec<T, n>& p1, const vec<T, n>& p2)
		: pMin(minimum(p1, p2)), pMax(maximum(p1, p2)) {
	}

	vec<T, n> Diagonal() const { return pMax - pMin; }

	T SurfaceArea() const;
	T Volume() const;

	int MaximumExtent() const {
		vec<T, n> d = Diagonal();
		if (d.x > d.y && d.x > d.z)
			return 0;
		else if (d.y > d.z)
			return 1;
		else
			return 2;
	}

	vec<T, n> Lerp(const Point3f& t) const {
		return vec<T, n>(::Lerp(t.x, pMin.x, pMax.x),
			::Lerp(t.y, pMin.y, pMax.y),
			::Lerp(t.z, pMin.z, pMax.z));
	}

	vec<T, n> Offset(const vec<T, n>& p) const {
		vec<T, n>o = p - pMin;
		if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
		if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
		if (pMax.z > pMin.z) o.z /= pMax.z - pMin.z;
		return o;
	}

	void BoundingSphere(vec<T, n>* center, Float* radius) const {
		*center = (pMin + pMax) / 2;
		*radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
	}

	const vec<T, n>& operator[](int i) const;
	vec<T, n>& operator[](int i);

	vec<T, n> Corner(int corner) const {
		return vec<T, n>((*this)[(corner & 1)].x,
			(*this)[(corner & 2) ? 1 : 0].y,
			(*this)[(corner & 4) ? 1 : 0].z);
	}

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
	bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
	bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
	bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
	return (x && y && z);
}

template <typename T, int n>
bool Inside(const vec<T, n>& p, const Bounds<T, n>& b)
{
	return (p.x >= b.pMin.x && p.x <= b.pMax.x &&
		p.y >= b.pMin.y && p.y <= b.pMax.y &&
		p.z >= b.pMin.z && p.z <= b.pMax.z);
}

template <typename T, int n>
bool InsideExclusive(const vec<T, n>& p, const Bounds<T, n>& b)
{
	return (p.x >= b.pMin.x && p.x < b.pMax.x&&
		p.y >= b.pMin.y && p.y < b.pMax.y&&
		p.z >= b.pMin.z && p.z < b.pMax.z);
}

template <typename T, typename U, int n>
inline Bounds<T, n>	Expand(const Bounds<T, n>& b, U delta) {
	return Bounds<T, n>(b.pMin - vec<T, n>(delta, delta, delta),
		b.pMax + vec<T, n>(delta, delta, delta));
}

typedef Bounds< Float, 2> Bounds2f;
typedef Bounds< int, 2> Bounds2i;
typedef Bounds< Float, 3> Boundsf;
typedef Bounds< int, 3> Boundsi;
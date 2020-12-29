#pragma once
#include <BRDF/BxDF_Utility.h>

#include "ray.h"
#include "Geometry/Vector3.h"

template<typename T, int n> class Bounds;
template<typename T, int n> bool Inside(const Point<T, n>& p, const Bounds<T, n>& b);

template < typename T, int n> class Bounds {
public:
	Bounds() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Point<T, n>(maxNum);
		pMax = Point<T, n>(minNum);
	}
	template<typename U>
	Bounds(const Bounds<U, n>& b) : pMin(b.pMin), pMax(b.pMax) { }

	Bounds(const Point<T, n>& p) : pMin(p), pMax(p) { }

	Bounds(const Point<T, n>& p1, const Point<T, n>& p2) : pMin(minimum(p1, p2)), pMax(maximum(p1, p2)) {	}

	Vector<T, n> Diagonal() const { return pMax - pMin; }

	T SurfaceArea() const;
	T Volume() const;

	int MaximumExtent() const;

	Point<T, n> Lerp(const Point<T, n>& t) const {
		return t * pMin + (Point<T, n>(1) - t) * pMax;
	}

	Point<T, n> Offset(const Point<T, n>& p) const {
		Point<T, n>o = p - pMin;
		return o / (pMax - pMin);
	}

	void BoundingSphere(Point<T, n>* center, Float* radius) const {
		*center = (pMin + pMax) / 2;
		*radius = Inside(*center, *this) ? (*center - pMax).length() : 0;
	}

	const Point<T, n>& operator[](int i) const;
	Point<T, n>& operator[](int i);
	Point<T, n> Corner(int corner) const;

	bool IntersectP(const Ray& ray, Float* hitt0, Float* hitt1) const;
	bool IntersectP(const Ray& ray, const Point3f& invDir, const int dirIsNeg[3]) const;

	Point<T, n> pMin;
	Point<T, n> pMax;
};

template <typename T, int n>
T Bounds<T, n>::SurfaceArea() const
{
	Point<T, n> d = Diagonal();
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
	Point<T, n> d = Diagonal();
	T temp = 1;
	for (int i = 0; i < n; ++i)
		temp *= d[i];
	return temp;
}

template <typename T, int n>
int Bounds<T, n>::MaximumExtent() const
{
	Point<T, n> d = Diagonal();
	int i = 0;
	T temp = d[0];
	for (int j = 0; j < n; ++j)
		if (d[j] > temp) {
			temp = d[j];
			i = j;
		}
	return i;
}

template <typename T, int n>
const Point<T, n>& Bounds<T, n>::operator[](int i) const
{
	if (i == 0)
		return pMin;
	return  pMax;
}

template <typename T, int n>
Point<T, n>& Bounds<T, n>::operator[](int i)
{
	if (i == 0)
		return pMin;
	return  pMax;
}

template <typename T, int n>
Point<T, n> Bounds<T, n>::Corner(int corner) const
{
	int bit = 1;
	Point<T, n> temp(0);
	for (int i = 0; i < n; ++i)
	{
		temp[i] = (*this)[(corner & bit) ? 1 : 0][i];
		bit *= 2;
	}
	return temp;
}

template <typename T, int n>
bool Bounds<T, n>::IntersectP(const Ray& ray, Float* hitt0, Float* hitt1) const
{
	assert(n == 3);

	Float t0 = 0, t1 = ray.tMax;
	for (int i = 0; i < 3; ++i)
	{
		//Update interval for ith bounding box slab 128
		Float invRayDir = 1 / ray.d[i];
		Float tNear = (pMin[i] - ray.o[i]) * invRayDir;
		Float tFar = (pMax[i] - ray.o[i]) * invRayDir;

		if (tNear > tFar) std::swap(tNear, tFar);
		//Update tFar to ensure robust ray¨Cbounds intersection 221
		tFar *= 1 + 2 * gamma(3);
		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar < t1 ? tFar : t1;
		if (t0 > t1) return false;
	}
	if (hitt0) *hitt0 = t0;
	if (hitt1) *hitt1 = t1;
	return true;
}

template <typename T, int n>
bool Bounds<T, n>::IntersectP(const Ray& ray, const Point3f& invDir, const int dirIsNeg[3]) const
{
	const Bounds& bounds = *this;
	// Check for ray intersection against $x$ and $y$ slabs
	Float tMin = (bounds[dirIsNeg[0]].x() - ray.o.x()) * invDir.x();
	Float tMax = (bounds[1 - dirIsNeg[0]].x() - ray.o.x()) * invDir.x();
	Float tyMin = (bounds[dirIsNeg[1]].y() - ray.o.y()) * invDir.y();
	Float tyMax = (bounds[1 - dirIsNeg[1]].y() - ray.o.y()) * invDir.y();

	// Update _tMax_ and _tyMax_ to ensure robust bounds intersection
	tMax *= 1 + 2 * gamma(3);
	tyMax *= 1 + 2 * gamma(3);
	if (tMin > tyMax || tyMin > tMax) return false;
	if (tyMin > tMin) tMin = tyMin;
	if (tyMax < tMax) tMax = tyMax;

	// Check for ray intersection against $z$ slab
	Float tzMin = (bounds[dirIsNeg[2]].z() - ray.o.z()) * invDir.z();
	Float tzMax = (bounds[1 - dirIsNeg[2]].z() - ray.o.z()) * invDir.z();

	// Update _tzMax_ to ensure robust bounds intersection
	tzMax *= 1 + 2 * gamma(3);
	if (tMin > tzMax || tzMin > tMax) return false;
	if (tzMin > tMin) tMin = tzMin;
	if (tzMax < tMax) tMax = tzMax;
	return (tMin < ray.tMax) && (tMax > 0);
}

template <typename T, int n>
Bounds<T, n> Union(const Bounds<T, n>& b, const Point<T, n>& p)
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
		if (!(b1.pMax[i] >= b2.pMin[i]) && (b1.pMin[i] <= b2.pMax[i]))
			return  false;
	return true;
}

template <typename T, int n>
bool Inside(const Point<T, n>& p, const Bounds<T, n>& b)
{
	for (int i = 0; i < n; ++i)
	{
		if (!(p[i] >= b.pMin[i] && p[i] <= b.pMax[i])) return false;
	}
	return true;
}

template <typename T, int n>
bool InsideExclusive(const Point<T, n>& p, const Bounds<T, n>& b)
{
	for (int i = 0; i < n; ++i)
	{
		if (!(p[i] > b.pMin[i] && p[i] < b.pMax[i])) return false;
	}
	return true;
}

template <typename T, typename U, int n>
inline Bounds<T, n>	Expand(const Bounds<T, n>& b, U delta) {
	return Bounds<T, n>(b.pMin - Point<T, n>(delta),
		b.pMax + Point<T, n>(delta));
}

template<int n>
using Boundsf = Bounds<Float, n>;

template<int n>
using Boundsi = Bounds<int, n>;

using Bounds2f = Boundsf<2>;
using Bounds2i = Boundsi<2>;

using Bounds3f = Boundsf<3>;
using Bounds3i = Boundsi<3>;

class Bounds2iIterator : public std::forward_iterator_tag {
public:
	Bounds2iIterator(const Bounds2i& b, const Point2i& pt)
		: p(pt), bounds(&b) {}
	Bounds2iIterator operator++() {
		advance();
		return *this;
	}
	Bounds2iIterator operator++(int) {
		Bounds2iIterator old = *this;
		advance();
		return old;
	}
	bool operator==(const Bounds2iIterator& bi) const {
		return p == bi.p && bounds == bi.bounds;
	}
	bool operator!=(const Bounds2iIterator& bi) const {
		return p != bi.p || bounds != bi.bounds;
	}

	Point2i operator*() const { return p; }

private:
	void advance() {
		++p.x();
		if (p.x() == bounds->pMax.x()) {
			p.x() = bounds->pMin.x();
			++p.y();
		}
	}
	Point2i p;
	const Bounds2i* bounds;
};

inline Bounds2iIterator begin(const Bounds2i& b) {
	return Bounds2iIterator(b, b.pMin);
}

inline Bounds2iIterator end(const Bounds2i& b) {
	// Normally, the ending point is at the minimum x value and one past
	// the last valid y value.
	Point2i pEnd(b.pMin.x(), b.pMax.y());
	// However, if the bounds are degenerate, override the end point to
	// equal the start point so that any attempt to iterate over the bounds
	// exits out immediately.
	if (b.pMin.x() >= b.pMax.x() || b.pMin.y() >= b.pMax.y())
		pEnd = b.pMin;
	return Bounds2iIterator(b, pEnd);
}
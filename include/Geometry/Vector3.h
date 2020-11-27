#pragma once

#include "config.h"
#include "Vector.h"

template<typename T>
using Vector2 = Vector<T, 2>;

template<typename T>
using Vector3 = Vector<T, 3>;

template<typename T>
using Vector4 = Vector<T, 4>;

using Vector4f = Vector4<Float>;
using Vector3f = Vector3<Float>;
using Vector2f = Vector2<Float>;

using Vector4i = Vector4<int>;
using Vector3i = Vector3<int>;
using Vector2i = Vector2<int>;

inline Vector3f Cross(const Vector3f& u, const Vector3f& v) {
	return Vector3f(u.data[1] * v.data[2] - u.data[2] * v.data[1],
		u.data[2] * v.data[0] - u.data[0] * v.data[2],
		u.data[0] * v.data[1] - u.data[1] * v.data[0]);
}

template <typename T> class Normal3 :public Vector<T, 3>
{
public:
	Normal3() :Vector() {}
	Normal3(const Vector<T, 3>& vec)
	{
		for (int i = 0; i < 3; ++i)
		{
			data[i] = vec[i];
		}
	}
	Normal3(T x, T y, T z) { data[0] = x; data[1] = y; data[2] = z; }
};
using Normal3f = Normal3<float>;

template<typename T, int n>
class Point :public Vector<T, n>
{
public:
	Point() :Vector() {}

	Point(T num) :Vector(num) {}
	template<typename U>
	Point(const Point<U, n>& vec)
	{
		for (int i = 0; i < n; ++i)
		{
			data[i] = vec[i];
		}
	}

	template<typename U>
	Point(const Vector<U, n>& vec)
	{
		for (int i = 0; i < n; ++i)
		{
			data[i] = vec[i];
		}
	}

	Point(T x, T y) { data[0] = x; data[1] = y; }
	Point(T x, T y, T z) { data[0] = x; data[1] = y; data[2] = z; }
	Point(T x, T y, T z, T w) { data[0] = x; data[1] = y; data[2] = z; data[3] = w; }
};

template<typename T, int n>
inline Point<T, n> operator+(const Point<T, n>& u, const Vector<T, n>& v) {
	Point<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] + v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline Point<T, n> operator-(const Point<T, n>& u, const Vector<T, n>& v) {
	Point<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] - v.data[i];
	}
	return rst;
}

template<typename T>
using Point3 = Point<T, 3>;

template<typename T>
using Point2 = Point<T, 2>;

using Point2i = Point2<int>;
using Point2f = Point2<Float>;
using Point3i = Point3<int>;
using Point3f = Point3<Float>;

using Color = Vector3f;

void Vector3f_test();

template <typename T>
Vector<T, 3> Permute(const Vector<T, 3>& v, int x, int y, int z) {
	return Vector<T, 3>(v[x], v[y], v[z]);
}

template <typename T, int n>
int MaxDimension(const Vector<T, n>& v)
{
	int i = 0;
	T max = v[0];
	for (int j = 0; j < n; ++j)
		if (v[j] > v[0])
		{
			max = v[j];
			i = j;
		}
	return i;
}

template <typename T> inline void
CoordinateSystem(const Vector<T, 3>& v1, Vector<T, 3>* v2, Vector<T, 3>* v3) {
	if (std::abs(v1.x()) > std::abs(v1.y()))
		*v2 = Vector3<T>(-v1.z(), 0, v1.x()) /
		std::sqrt(v1.x() * v1.x() + v1.z() * v1.z());
	else
		*v2 = Vector3<T>(0, v1.z(), -v1.y()) /
		std::sqrt(v1.y() * v1.y() + v1.z() * v1.z());
	*v3 = Cross(v1, *v2);
}
template <typename T>
Point<int, 2> Floor(const Point<T, 2>& p) {
	return Point<T, 2>(std::floor(p.x()), std::floor(p.y()));
}

template <typename T>
Point<int, 2> Ceil(const Point<T, 2>& p) {
	return Point<T, 2>(std::ceil(p.x()), std::ceil(p.y()));
}

template <typename T>
Point<T, 2> Lerp(Float t, const Point<T, 2>& v0, const Point<T, 2>& v1) {
	return (1 - t) * v0 + t * v1;
}

template <typename T>
Point<int, 2> Min(const Point<T, 2>& pa, const Point<T, 2>& pb) {
	return Point<T, 2>(std::min(pa.x(), pb.x()), std::min(pa.y(), pb.y()));
}

template <typename T>
Point<int, 2> Max(const Point<T, 2>& pa, const Point<T, 2>& pb) {
	return Point<T, 2>(std::max(pa.x(), pb.x()), std::max(pa.y(), pb.y()));
}
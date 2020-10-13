#pragma once

#include "config.h"
#include "Vector.h"

using Vector4f = Vector<Float, 4>;
using Vector3f = Vector<Float, 3>;
using Vector2f = Vector<Float, 2>;

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
	Point(const Vector<T, n>& vec)
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

using Point2i = Point<int, 2>;
using Point2f = Point<Float, 2>;
using Point3i = Point<int, 3>;
using Point3f = Point<Float, 3>;

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
	if (std::abs(v1.x) > std::abs(v1.y))
		*v2 = Vector3<T>(-v1.z, 0, v1.x) /
		std::sqrt(v1.x * v1.x + v1.z * v1.z);
	else
		*v2 = Vector3<T>(0, v1.z, -v1.y) /
		std::sqrt(v1.y * v1.y + v1.z * v1.z);
	*v3 = Cross(v1, *v2);
}
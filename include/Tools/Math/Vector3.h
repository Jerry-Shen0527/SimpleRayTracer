#pragma once

#include <cassert>

#include "config.h"
#include "Vector.h"

using Vector3f = Vector<Float, 3>;
using Vector2f = Vector<Float, 2>;

inline Vector3f cross(const Vector3f& u, const Vector3f& v) {
	return Vector3f(u.data[1] * v.data[2] - u.data[2] * v.data[1],
		u.data[2] * v.data[0] - u.data[0] * v.data[2],
		u.data[0] * v.data[1] - u.data[1] * v.data[0]);
}

template <typename T> class Normal3 :public Vector<T, 3>
{
	Normal3(Vector<T, 3>& vec) { static_cast<Vector<T, 3>>(*this) = vec; }
};
using Normal3f = Normal3<float>;

template<typename T, int n>
class Point :public Vector<T, n>
{
public:
	Point(Vector<T, 3>& vec) { static_cast<Vector<T, n>>(*this) = vec; }
};

using Point2i = Point<int, 2>;
using Point2f = Point<Float, 2>;
using Point3i = Point<int, 3>;
using Point3f = Point<Float, 3>;

using Color = Vector3f;

void Vector3f_test();

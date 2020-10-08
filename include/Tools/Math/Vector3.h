#ifndef VEC3_H
#define VEC3_H

#include "Vector.h"

template<typename T>
class Vector3 :public Vector<T, 3>
{
public:
	Vector3() : Vector<T, 3>() {}
	Vector3(const Vector<T, 3>& in_vec) :Vector<T, 3>(in_vec) {}

	Vector3(const Normal3<T>& n)
	{
		data[0] = n.x();
		data[1] = n.y();
		data[2] = n.z();
		assert(!n.HasNaNs());
	}
};
using Vector3f = Vector3<float>;

template<typename T>
inline Vector3<T> cross(const Vector3<T>& u, const Vector3<T>& v) {
	return Vector3f(u.data[1] * v.data[2] - u.data[2] * v.data[1],
		u.data[2] * v.data[0] - u.data[0] * v.data[2],
		u.data[0] * v.data[1] - u.data[1] * v.data[0]);
}

template <typename T> class Normal3 :Vector3<T>
{
};
using Normal3f = Normal3<float>;

void Vector3f_test();

#endif

#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
using std::make_shared;

#include "Tools/Math/rng.h"

using std::sqrt;

template<typename T, int n>
class vec {
public:
	vec() : data{ 0 } {}
	vec(std::initializer_list<T> list);

	inline static vec random();
	inline static vec random(T min, T max);

	vec operator-() const;
	T operator[](int i) const { return data[i]; }
	T& operator[](int i) { return data[i]; }

	vec& operator+=(const vec& v);
	vec& operator*=(const T t);
	vec& operator/=(const T t);
	bool operator==(const vec& t)const;
	bool operator!=(const vec& t)const;
	vec normalize() const;
	vec abs() const;

	float length() const;
	float length_squared() const;

	T min();
	T max();

	T x() const;
	T& x();
	T y() const;
	T& y();
	T z() const;
	T& z();
	T w() const;
	T& w();
public:
	T data[n];
};

template<typename T, int n>
vec<T, n>::vec(std::initializer_list<T> list)
{
	assert(list.size() == n);

	int i = 0;
	for (auto iter = list.begin(); iter != list.end(); iter++, i++)
	{
		data[i] = *iter;
	}
}

template<typename T, int n>
vec<T, n> vec<T, n>::random()
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = static_cast<T> (random_float());
	}
	return rst;
}

template<typename T, int n>
vec<T, n> vec<T, n>::random(T min, T max)
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = random_float(min, max);
	}
	return rst;
}

template<typename T, int n>
vec<T, n> vec<T, n>::operator-() const
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = -data[i];
	}
	return rst;
}

template<typename T, int n>
vec<T, n>& vec<T, n>::operator+=(const vec<T, n>& v)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] += v.data[i];
	}
	return *this;
}

template<typename T, int n>
vec<T, n>& vec<T, n>::operator*=(const T t)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] *= t;
	}
	return *this;
}

template<typename T, int n>
vec<T, n>& vec<T, n>::operator/=(const T t)
{
	return *this *= 1 / t;
}

template<typename T, int n>
bool vec<T, n>::operator==(const vec& t) const
{
	for (int i = 0; i < n; ++i)
	{
		if (data[i] != t.data[i])
		{
			return false;
		}
	}
	return  true;
}

template<typename T, int n>
bool vec<T, n>::operator!=(const vec& t) const
{
	return !(*this == t);
}

template<typename T, int n>
vec<T, n> vec<T, n>::normalize() const
{
	return *this / length();
}

template<typename T, int n>
vec<T, n> vec<T, n>::abs() const
{
	vec temp;
	for (int i = 0; i < n; ++i)
	{
		temp.data[i] = std::abs(data[i]);
	}
	return temp;
}

template<typename T, int n>
float vec<T, n>::length() const
{
	return sqrt(length_squared());
}

template<typename T, int n>
float vec<T, n>::length_squared() const
{
	float rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += data[i] * data[i];
	}
	return rst;
}

template <typename T, int n>
T vec<T, n>::min()
{
	T temp = data[0];
	for (int i = 0; i < n; ++i)
	{
		if (data[i] < temp) temp = data[i];
	}
	return  temp;
}

template <typename T, int n>
T vec<T, n>::max()
{
	T temp = data[0];
	for (int i = 0; i < n; ++i)
	{
		if (data[i] > temp) temp = data[i];
	}
	return  temp;
}

template<typename T, int n>
T vec<T, n>::x() const
{
	static_assert(n >= 1);
	return data[0];
}

template<typename T, int n>
T& vec<T, n>::x()
{
	static_assert(n >= 1);
	return data[0];
}

template<typename T, int n>
T vec<T, n>::y() const
{
	static_assert(n >= 2);
	return data[1];
}

template<typename T, int n>
T& vec<T, n>::y()
{
	static_assert(n >= 2);
	return data[1];
}

template<typename T, int n>
T vec<T, n>::z() const
{
	static_assert(n >= 3);
	return data[2];
}

template<typename T, int n>
T& vec<T, n>::z()
{
	static_assert(n >= 3);
	return data[2];
}

template<typename T, int n>
T vec<T, n>::w() const
{
	static_assert(n >= 4);
	return data[3];
}

template<typename T, int n>
T& vec<T, n>::w()
{
	static_assert(n >= 4);
	return data[3];
}

template<typename T, int n>
inline std::ostream& operator<<(std::ostream& out, const vec<T, n>& v) {
	for (int i = 0; i < n; ++i)
	{
		out << v.data[i] << ' ';
	}
	return out;
}

template<typename T, int n>
inline vec<T, n> operator+(const vec<T, n>& u, const vec<T, n>& v) {
	vec<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] + v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline vec<T, n> operator-(const vec<T, n>& u, const vec<T, n>& v) {
	vec<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] - v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline vec<T, n> operator*(const vec<T, n>& u, const vec<T, n>& v) {
	vec<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] * v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline vec<T, n> operator*(float t, const vec<T, n>& v) {
	vec<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = t * v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline vec<T, n> operator*(const vec<T, n>& v, float t) {
	return t * v;
}

template<typename T, int n>
inline vec<T, n> operator/(vec<T, n> v, float t) {
	return (1 / t) * v;
}

template<typename T, int n>
inline float dot(const vec<T, n>& u, const vec<T, n>& v) {
	float rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += u.data[i] * v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline vec<T, n> unit_vector(vec<T, n> v) {
	return v / v.length();
}

template<typename T, int n>
inline vec<T, n> minimum(const vec<T, n>& v1, const vec<T, n>& v2)
{
	vec<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst = v1[i] < v2[i] ? v1[i] : v2[i];
	}
	return  rst;
}

template<typename T, int n>
inline vec<T, n> maximum(const vec<T, n>& v1, const vec<T, n>& v2)
{
	vec<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst = v1[i] < v2[i] ? v1[i] : v2[i];
	}
	return  rst;
}
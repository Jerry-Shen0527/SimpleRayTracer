#pragma once

#include <cmath>
#include <iostream>
using std::make_shared;

#include "Tools/Math/rng.h"

using std::sqrt;

template<int n>
class vec {
public:
	vec() : data{ 0.f } {}
	vec(std::initializer_list<float> list);

	inline static vec random();
	inline static vec random(float min, float max);

	vec operator-() const;
	float operator[](int i) const { return data[i]; }
	float& operator[](int i) { return data[i]; }

	vec& operator+=(const vec<n>& v);
	vec& operator*=(const float t);
	vec& operator/=(const float t);

	float length() const;
	float length_squared() const;

	float x() const;
	float& x();
	float y() const;
	float& y();
	float z() const;
	float& z();

	float w() const;
	float& w();
public:
	float data[n];
};

template <int n>
vec<n>::vec(std::initializer_list<float> list)
{
	static_assert(list.size() == n);
	for (int i = 0; i < n; ++i)
	{
		data[i] = list[i];
	}
}

template <int n>
vec<n> vec<n>::random()
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = random_float();
	}
	return rst;
}

template <int n>
vec<n> vec<n>::random(float min, float max)
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = random_float(min, max);
	}
	return rst;
}

template <int n>
vec<n> vec<n>::operator-() const
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = -data[i];
	}
	return rst;
}

template <int n>
vec<n>& vec<n>::operator+=(const vec<n>& v)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] += v.data[i];
	}
	return *this;
}

template <int n>
vec<n>& vec<n>::operator*=(const float t)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] *= t;
	}
	return *this;
}

template <int n>
vec<n>& vec<n>::operator/=(const float t)
{
	return *this *= 1 / t;
}

template <int n>
float vec<n>::length() const
{
	return sqrt(length_squared());
}

template <int n>
float vec<n>::length_squared() const
{
	float rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += data[i] * data[i];
	}
	return rst;
}

template <int n>
float vec<n>::x() const
{
	static_assert(n >= 1);
	return data[0];
}

template <int n>
float& vec<n>::x()
{
	static_assert(n >= 1);
	return data[0];
}

template <int n>
float vec<n>::y() const
{
	static_assert(n >= 2);
	return data[1];
}

template <int n>
float& vec<n>::y()
{
	static_assert(n >= 2);
	return data[1];
}

template <int n>
float vec<n>::z() const
{
	static_assert(n >= 3);
	return data[2];
}

template <int n>
float& vec<n>::z()
{
	static_assert(n >= 3);
	return data[2];
}

template <int n>
float vec<n>::w() const
{
	static_assert(n >= 4);
	return data[3];
}

template <int n>
float& vec<n>::w()
{
	static_assert(n >= 4);
	return data[3];
}

template<int n>
inline std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
	for (int i = 0; i < n; ++i)
	{
		out << v.data[i] << ' ';
	}
	return out;
}

template<int n>
inline vec<n> operator+(const vec<n>& u, const vec<n>& v) {
	vec<n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] + v.data[i];
	}
	return rst;
}

template<int n>
inline vec<n> operator-(const vec<n>& u, const vec<n>& v) {
	vec<n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] - v.data[i];
	}
	return rst;
}

template<int n>
inline vec<n> operator*(const vec<n>& u, const vec<n>& v) {
	vec<n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] * v.data[i];
	}
	return rst;
}

template<int n>
inline vec<n> operator*(float t, const vec<n>& v) {
	vec<n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = t * v.data[i];
	}
	return rst;
}

template<int n>
inline vec<n> operator*(const vec<n>& v, float t) {
	return t * v;
}

template<int n>
inline vec<n> operator/(vec<n> v, float t) {
	return (1 / t) * v;
}

template<int n>
inline float dot(const vec<n>& u, const vec<n>& v) {
	float rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += u.data[i] * v.data[i];
	}
	return rst;
}

template<int n>
inline vec<n> unit_vector(vec<n> v) {
	return v / v.length();
}

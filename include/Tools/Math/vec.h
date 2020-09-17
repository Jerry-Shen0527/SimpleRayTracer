#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
using std::make_shared;

#include "Tools/Math/rng.h"

using std::sqrt;

template<int n>
class vec {
public:
	vec() : data{ 0.f } {}
	vec(std::initializer_list<double> list);

	inline static vec random();
	inline static vec random(double min, double max);

	vec operator-() const;
	double operator[](int i) const { return data[i]; }
	double& operator[](int i) { return data[i]; }

	vec& operator+=(const vec<n>& v);
	vec& operator*=(const double t);
	vec& operator/=(const double t);
	vec normalize() const;

	double length() const;
	double length_squared() const;

	double x() const;
	double& x();
	double y() const;
	double& y();
	double z() const;
	double& z();
	double w() const;
	double& w();
public:
	double data[n];
};

template <int n>
vec<n>::vec(std::initializer_list<double> list)
{
	assert(list.size() == n);

	int i = 0;
	for (auto iter = list.begin(); iter != list.end(); iter++, i++)
	{
		data[i] = *iter;
	}
}

template <int n>
vec<n> vec<n>::random()
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = random_double();
	}
	return rst;
}

template <int n>
vec<n> vec<n>::random(double min, double max)
{
	vec rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = random_double(min, max);
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
vec<n>& vec<n>::operator*=(const double t)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] *= t;
	}
	return *this;
}

template <int n>
vec<n>& vec<n>::operator/=(const double t)
{
	return *this *= 1 / t;
}

template <int n>
vec<n> vec<n>::normalize() const
{
	return this /= length();
}

template <int n>
double vec<n>::length() const
{
	return sqrt(length_squared());
}

template <int n>
double vec<n>::length_squared() const
{
	double rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += data[i] * data[i];
	}
	return rst;
}

template <int n>
double vec<n>::x() const
{
	static_assert(n >= 1);
	return data[0];
}

template <int n>
double& vec<n>::x()
{
	static_assert(n >= 1);
	return data[0];
}

template <int n>
double vec<n>::y() const
{
	static_assert(n >= 2);
	return data[1];
}

template <int n>
double& vec<n>::y()
{
	static_assert(n >= 2);
	return data[1];
}

template <int n>
double vec<n>::z() const
{
	static_assert(n >= 3);
	return data[2];
}

template <int n>
double& vec<n>::z()
{
	static_assert(n >= 3);
	return data[2];
}

template <int n>
double vec<n>::w() const
{
	static_assert(n >= 4);
	return data[3];
}

template <int n>
double& vec<n>::w()
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
inline vec<n> operator*(double t, const vec<n>& v) {
	vec<n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = t * v.data[i];
	}
	return rst;
}

template<int n>
inline vec<n> operator*(const vec<n>& v, double t) {
	return t * v;
}

template<int n>
inline vec<n> operator/(vec<n> v, double t) {
	return (1 / t) * v;
}

template<int n>
inline double dot(const vec<n>& u, const vec<n>& v) {
	double rst = 0.f;
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

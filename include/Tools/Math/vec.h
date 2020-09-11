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

	inline static vec random() {
		vec rst;
		for (int i = 0; i < n; ++i)
		{
			rst.data[i] = random_float();
		}
		return rst;
	}

	inline static vec random(float min, float max) {
		vec rst;
		for (int i = 0; i < n; ++i)
		{
			rst.data[i] = random_float(min, max);
		}
		return rst;
	}

	vec operator-() const
	{
		vec rst;
		for (int i = 0; i < n; ++i)
		{
			rst.data[i] = -data[i];
		}
		return rst;
	}
	float operator[](int i) const { return data[i]; }
	float& operator[](int i) { return data[i]; }

	vec& operator+=(const vec<n>& v) {
		for (int i = 0; i < n; ++i)
		{
			data[i] += v.data[i];
		}
		return *this;
	}

	vec& operator*=(const float t) {
		for (int i = 0; i < n; ++i)
		{
			data[i] *= t;
		}
		return *this;
	}

	vec& operator/=(const float t) {
		return *this *= 1 / t;
	}

	float length() const {
		return sqrt(length_squared());
	}

	float length_squared() const {
		float rst = 0.f;
		for (int i = 0; i < n; ++i)
		{
			rst += data[i] * data[i];
		}
		return rst;
	}

public:
	float data[n];
};

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
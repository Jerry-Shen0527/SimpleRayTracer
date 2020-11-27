#pragma once
#include <ostream>

template<typename T, int n>
class Vector {
public:
	Vector() : data{ 0 } {}
	Vector(T x)
	{
		for (int i = 0; i < n; ++i)
		{
			data[i] = x;
		}
	}

	Vector(T x, T y) { data[0] = x; data[1] = y; }
	Vector(T x, T y, T z) { data[0] = x; data[1] = y; data[2] = z; }
	Vector(T x, T y, T z, T w) { data[0] = x; data[1] = y; data[2] = z; data[3] = w; }

	inline static Vector random();
	inline static Vector random(T min, T max);

	Vector operator-() const;
	T operator[](int i) const { return data[i]; }
	T& operator[](int i) { return data[i]; }

	Vector& operator=(const Vector& rhs) {
		for (int i = 0; i < n; ++i)
		{
			data[i] = rhs[i];
		}
		return *this;
	}

	Vector& operator+=(const Vector& v);
	Vector& operator-=(const Vector& v);
	Vector& operator*=(const T t);
	Vector& operator/=(const T t);
	bool operator==(const Vector& t)const;
	bool operator!=(const Vector& t)const;
	Vector normalize() const;
	Vector abs() const;

	float length() const;
	float LengthSquared() const;

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
private:
	void set_value(T x, T y, T z);
	void set_value(T x, T y);
};

template <typename T, int n>
void Vector<T, n>::set_value(T x, T y)
{
	data[0] = x;
	data[1] = y;
}

template <typename T, int n>
void Vector<T, n>::set_value(T x, T y, T z)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

template<typename T, int n>
Vector<T, n> Vector<T, n>::random()
{
	Vector rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = static_cast<T> (random_float());
	}
	return rst;
}

template<typename T, int n>
Vector<T, n> Vector<T, n>::random(T min, T max)
{
	Vector rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = random_float(min, max);
	}
	return rst;
}

template<typename T, int n>
Vector<T, n> Vector<T, n>::operator-() const
{
	Vector rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = -data[i];
	}
	return rst;
}

template<typename T, int n>
Vector<T, n>& Vector<T, n>::operator+=(const Vector<T, n>& v)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] += v.data[i];
	}
	return *this;
}

template <typename T, int n>
Vector<T, n>& Vector<T, n>::operator-=(const Vector& v)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] -= v.data[i];
	}
	return *this;
}

template<typename T, int n>
Vector<T, n>& Vector<T, n>::operator*=(const T t)
{
	for (int i = 0; i < n; ++i)
	{
		data[i] *= t;
	}
	return *this;
}

template<typename T, int n>
Vector<T, n>& Vector<T, n>::operator/=(const T t)
{
	return *this *= 1 / t;
}

template<typename T, int n>
bool Vector<T, n>::operator==(const Vector& t) const
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
bool Vector<T, n>::operator!=(const Vector& t) const
{
	return !(*this == t);
}

template<typename T, int n>
Vector<T, n> Vector<T, n>::normalize() const
{
	return *this / length();
}

template<typename T, int n>
Vector<T, n> Normalize(const Vector<T, n>& rhs)
{
	return rhs.normalize();
}

template<typename T, int n>
Vector<T, n> Vector<T, n>::abs() const
{
	Vector temp;
	for (int i = 0; i < n; ++i)
	{
		temp.data[i] = std::abs(data[i]);
	}
	return temp;
}

template<typename T, int n>
Vector<T, n> Abs(const Vector<T, n>& v)
{
	return  v.abs();
}

template<typename T, int n>
float Vector<T, n>::length() const
{
	return sqrt(LengthSquared());
}

template<typename T, int n>
float Vector<T, n>::LengthSquared() const
{
	float rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += data[i] * data[i];
	}
	return rst;
}

template <typename T, int n>
T Vector<T, n>::min()
{
	T temp = data[0];
	for (int i = 0; i < n; ++i)
	{
		if (data[i] < temp) temp = data[i];
	}
	return  temp;
}

template <typename T, int n>
T Vector<T, n>::max()
{
	T temp = data[0];
	for (int i = 0; i < n; ++i)
	{
		if (data[i] > temp) temp = data[i];
	}
	return  temp;
}

template<typename T, int n>
T Vector<T, n>::x() const
{
	static_assert(n >= 1);
	return data[0];
}

template<typename T, int n>
T& Vector<T, n>::x()
{
	static_assert(n >= 1);
	return data[0];
}

template<typename T, int n>
T Vector<T, n>::y() const
{
	static_assert(n >= 2);
	return data[1];
}

template<typename T, int n>
T& Vector<T, n>::y()
{
	static_assert(n >= 2);
	return data[1];
}

template<typename T, int n>
T Vector<T, n>::z() const
{
	static_assert(n >= 3);
	return data[2];
}

template<typename T, int n>
T& Vector<T, n>::z()
{
	static_assert(n >= 3);
	return data[2];
}

template<typename T, int n>
T Vector<T, n>::w() const
{
	static_assert(n >= 4);
	return data[3];
}

template<typename T, int n>
T& Vector<T, n>::w()
{
	static_assert(n >= 4);
	return data[3];
}

template<typename T, int n>
inline std::ostream& operator<<(std::ostream& out, const Vector<T, n>& v) {
	for (int i = 0; i < n; ++i)
	{
		out << v.data[i] << ' ';
	}
	return out;
}

template<typename T, int n>
inline Vector<T, n> operator+(const Vector<T, n>& u, const Vector<T, n>& v) {
	Vector<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] + v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline Vector<T, n> operator-(const Vector<T, n>& u, const Vector<T, n>& v) {
	Vector<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] - v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline Vector<T, n> operator*(const Vector<T, n>& u, const Vector<T, n>& v) {
	Vector<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = u.data[i] * v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline Vector<T, n> operator*(float t, const Vector<T, n>& v) {
	Vector<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst.data[i] = t * v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline Vector<T, n> operator*(const Vector<T, n>& v, float t) {
	return t * v;
}

template<typename T, int n>
inline Vector<T, n> operator/(Vector<T, n> v, float t) {
	return (1 / t) * v;
}

template<typename T, int n>
inline float Dot(const Vector<T, n>& u, const Vector<T, n>& v) {
	float rst = 0.f;
	for (int i = 0; i < n; ++i)
	{
		rst += u.data[i] * v.data[i];
	}
	return rst;
}

template<typename T, int n>
inline Vector<T, n> unit_vector(Vector<T, n> v) {
	return v / v.length();
}

template<typename T, int n>
inline Vector<T, n> minimum(const Vector<T, n>& v1, const Vector<T, n>& v2)
{
	Vector<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst[i] = v1[i] < v2[i] ? v1[i] : v2[i];
	}
	return  rst;
}

template<typename T, int n>
inline Vector<T, n> maximum(const Vector<T, n>& v1, const Vector<T, n>& v2)
{
	Vector<T, n> rst;
	for (int i = 0; i < n; ++i)
	{
		rst[i] = v1[i] < v2[i] ? v1[i] : v2[i];
	}
	return  rst;
}
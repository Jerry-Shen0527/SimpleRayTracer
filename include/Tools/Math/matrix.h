#pragma once
#include "EFloat.h"

template<typename T>
struct Matrix4x4 {
	Matrix4x4();

	Matrix4x4(T val) :Matrix4x4() { *this = *this * val; }

	Matrix4x4(T mat[4][4]);

	Matrix4x4(T t00, T t01, T t02, T t03,
		T t10, T t11, T t12, T t13,
		T t20, T t21, T t22, T t23,
		T t30, T t31, T t32, T t33);

	Matrix4x4 Transpose(const Matrix4x4& m);

	bool operator==(const Matrix4x4& m2) const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j] != m2.m[i][j]) return false;
		return true;
	}
	bool operator!=(const Matrix4x4& m2) const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j] != m2.m[i][j]) return true;
		return false;
	}

	Matrix4x4 operator*(T scalar) const
	{
		Matrix4x4 ret = *this;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				ret.m[i][j] *= scalar;

		return ret;
	}

	Matrix4x4 operator*=(const Matrix4x4& rhs)
	{
		Matrix4x4 ret = *this;
		*this = Mul(*this, rhs);
		return *this;
	}

	Matrix4x4 operator/(T scalar) const
	{
		Matrix4x4 ret = *this;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				ret.m[i][j] /= scalar;

		return ret;
	}

	//Matrix4x4 operator*=(const Matrix4x4& rhs)
	//{
	//	Matrix4x4 ret = *this;
	//	*this = Mul(*this, rhs);
	//	return *this;
	//}

	bool IsBlack() const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j].IsBlack()) return true;
		return false;
	}

	Matrix4x4 operator-()
	{
		Matrix4x4 ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				ret.m[i][j] *= -m[i][j];

		return ret;
	}

	Matrix4x4 operator+=(const Matrix4x4& rhs)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				m[i][j] += rhs.m[i][j];

		return *this;
	}

	Matrix4x4 operator+(const Matrix4x4& rhs) const
	{
		Matrix4x4 ret = *this;
		return ret += rhs;
	}

	Matrix4x4 operator-=(const Matrix4x4& rhs)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				m[i][j] -= rhs.m[i][j];

		return *this;
	}

	Matrix4x4 operator-(const Matrix4x4& rhs) const
	{
		Matrix4x4 ret = *this;
		return ret -= rhs;
	}

	static Matrix4x4 Mul(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 r;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				r.m[i][j] = m1.m[i][0] * m2.m[0][j] +
				m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] +
				m1.m[i][3] * m2.m[3][j];
		return r;
	}

	Float y() { return m[0][0].y(); }

	T m[4][4];
	template<typename T>
	friend Matrix4x4 Inverse(const Matrix4x4&);
};
template<typename T>
Matrix4x4<T> operator*(const Matrix4x4<T>& lhs, const Matrix4x4<T>& rhs)
{
	return Matrix4x4<T>::Mul(lhs, rhs);
}
template<typename T>
inline Matrix4x4<T>::Matrix4x4()
{
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] =
		m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.f;
}
template<typename T>
inline Matrix4x4<T>::Matrix4x4(T mat[4][4])
{
	memcpy(m, mat, 16 * sizeof(T));
}
template<typename T>
inline Matrix4x4<T>::Matrix4x4(T t00, T t01, T t02, T t03, T t10,
	T t11, T t12, T t13, T t20, T t21,
	T t22, T t23, T t30, T t31, T t32,
	T t33) {
	m[0][0] = t00;
	m[0][1] = t01;
	m[0][2] = t02;
	m[0][3] = t03;
	m[1][0] = t10;
	m[1][1] = t11;
	m[1][2] = t12;
	m[1][3] = t13;
	m[2][0] = t20;
	m[2][1] = t21;
	m[2][2] = t22;
	m[2][3] = t23;
	m[3][0] = t30;
	m[3][1] = t31;
	m[3][2] = t32;
	m[3][3] = t33;
}
template<typename T>
inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4& m)
{
	return Matrix4x4(m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
}
template<typename T>
inline Matrix4x4<T> Transpose(const Matrix4x4<T>& m) {
	return Matrix4x4(m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0], m.m[0][1],
		m.m[1][1], m.m[2][1], m.m[3][1], m.m[0][2], m.m[1][2],
		m.m[2][2], m.m[3][2], m.m[0][3], m.m[1][3], m.m[2][3],
		m.m[3][3]);
}
template<typename T>
inline  Matrix4x4<T> Inverse(const Matrix4x4<T>& m) {
	int indxc[4], indxr[4];
	int ipiv[4] = { 0, 0, 0, 0 };
	T minv[4][4];
	memcpy(minv, m.m, 4 * 4 * sizeof(T));
	for (int i = 0; i < 4; i++) {
		int irow = 0, icol = 0;
		T big = 0.f;
		// Choose pivot
		for (int j = 0; j < 4; j++) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; k++) {
					if (ipiv[k] == 0) {
						if (std::abs(minv[j][k]) >= big) {
							big = T(std::abs(minv[j][k]));
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
						std::cerr << "Singular matrix in MatrixInvert" << std::endl;
				}
			}
		}
		++ipiv[icol];
		// Swap rows _irow_ and _icol_ for pivot
		if (irow != icol) {
			for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (minv[icol][icol] == 0.f) std::cerr << "Singular matrix in MatrixInvert" << std::endl;

		// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
		T pivinv = 1. / minv[icol][icol];
		minv[icol][icol] = 1.;
		for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

		// Subtract this row from others to zero out their columns
		for (int j = 0; j < 4; j++) {
			if (j != icol) {
				T save = minv[j][icol];
				minv[j][icol] = 0;
				for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
			}
		}
	}
	// Swap columns to reflect permutation
	for (int j = 3; j >= 0; j--) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; k++)
				std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
		}
	}
	return Matrix4x4(minv);
}

using Matrix4x4f = Matrix4x4<Float>;
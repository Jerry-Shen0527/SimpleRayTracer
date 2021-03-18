#pragma once
#include <complex>

#include "Math/matrix.h"
#include <Tools/Spectrum/SampledSpectrum.h>

template<typename T>
inline MuellerMatrix<T> linear_polarizer(Float value = 1.f) {
	T a = value * .5f;
	return MuellerMatrix(
		a, a, 0, 0,
		a, a, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	);
}

template<typename T>
inline MuellerMatrix<T> linear_polarizer_slant(Float value = 1.f) {
	Float a = value * .5f;
	return MuellerMatrix(
		a, -a, 0, 0,
		-a, a, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	);
}

template<typename T>
inline MuellerMatrix<T> linear_retarder(Float phase) {
	Float s, c;
	s = sin(phase);
	c = cos(phase);
	return MuellerMatrix(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, c, -s,
		0, 0, s, c
	);
}

inline std::tuple<float, float> sincos_arg_diff(const std::complex<float>& a, const std::complex<float>& b)
{
	auto delta = arg(a / b);
	return std::forward_as_tuple(sin(delta), cos(delta));
}
template<typename T>
inline MuellerMatrix<T> rotator(Float theta) {
	auto s = sin(2.f * theta);
	auto c = cos(2.f * theta);
	return MuellerMatrix(
		1, 0, 0, 0,
		0, c, s, 0,
		0, -s, c, 0,
		0, 0, 0, 1
	);
}

inline Float unit_angle(const Vector3f& a, const Vector3f& b) {
	Float dot_uv = Dot(a, b);

	Float temp = 2.f * asin(.5f * (b - a * (dot_uv > 0 ? 1 : -1)).Length());

	return dot_uv >= 0 ? temp : Pi - temp;
}
template<typename T>
inline MuellerMatrix<T> rotate_stokes_basis(const Vector3f& forward, const Vector3f& basis_current, const Vector3f& basis_target) {
	Float theta = unit_angle(Normalize(basis_current), Normalize(basis_target));

	if (Dot(forward, Cross(basis_current, basis_target)) < 0) theta *= -1.f;
	return rotator(theta);
}
template<typename T>
inline MuellerMatrix<T> rotate_mueller_basis(const MuellerMatrix<T>& M,
	const Vector3f& in_forward,
	const Vector3f& in_basis_current,
	const Vector3f& in_basis_target,
	const Vector3f& out_forward,
	const Vector3f& out_basis_current,
	const Vector3f& out_basis_target) {
	MuellerMatrix R_in = rotate_stokes_basis(in_forward, in_basis_current, in_basis_target);
	MuellerMatrix R_out = rotate_stokes_basis(out_forward, out_basis_current, out_basis_target);
	return R_out * M * Transpose(R_in);
}

std::tuple<std::complex<Float>, std::complex<Float>, Float, std::complex<Float>, std::complex<Float>>
fresnel_polarized(Float cos_theta_i, std::complex<Float> eta);

std::tuple<std::complex<Float>, std::complex<Float>, Float, Float, Float>
fresnel_polarized(Float cos_theta_i, Float eta);

template<typename T, typename Eta>
inline MuellerMatrix<T> specular_reflection(T cos_theta_i, Eta eta) {
	std::complex<T> a_s, a_p;

	std::tie(a_s, a_p, std::ignore, std::ignore, std::ignore) = fresnel_polarized(cos_theta_i, eta);

	T sin_delta, cos_delta;
	std::tie(sin_delta, cos_delta) = sincos_arg_diff(a_s, a_p);

	T r_s = abs(a_s * a_s);
	T r_p = abs(a_p * a_p);
	T a = .5f * (r_s + r_p);
	T b = .5f * (r_s - r_p);
	T c = sqrt(r_s * r_p);

	if (c == 0.f) sin_delta = 0.f; // avoid issues with NaNs
	if (c == 0.f) cos_delta = 0.f;

	return MuellerMatrix(
		a, b, 0, 0,
		b, a, 0, 0,
		0, 0, c * cos_delta, c * sin_delta,
		0, 0, -c * sin_delta, c * cos_delta
	);
}
template<typename Spectrum, typename Eta>
inline MuellerMatrix<Spectrum> specular_transmission(Float cos_theta_i, Eta eta) {
	std::complex<Spectrum> a_s, a_p;
	Spectrum cos_theta_t, eta_it, eta_ti;

	std::tie(a_s, a_p, cos_theta_t, eta_it, eta_ti) =
		fresnel_polarized(cos_theta_i, eta);

	// Unit conversion factor
	Spectrum factor = -eta_it * (abs(cos_theta_i) > 1e-8f ? cos_theta_t / cos_theta_i : 0.f);

	// Compute transmission amplitudes
	Spectrum a_s_r = real(a_s) + 1.f,
		a_p_r = (1.f - real(a_p)) * eta_ti;

	Spectrum t_s = a_s_r * a_s_r;
	Spectrum t_p = a_p_r * a_p_r;
	Spectrum a = .5f * factor * (t_s + t_p);
	Spectrum b = .5f * factor * (t_s - t_p);
	Spectrum c = factor * sqrt(t_s * t_p);

	return MuellerMatrix(
		a, b, 0, 0,
		b, a, 0, 0,
		0, 0, c, 0,
		0, 0, 0, c
	);
}
template<typename T>
inline MuellerMatrix<T> reverse(const MuellerMatrix<T>& M) {
	return MuellerMatrix<T>::Mul(MuellerMatrix(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, -1
	), M);
}

inline Vector3f stokes_basis(const Vector3f& w) {
	Vector3f s, p;
	CoordinateSystem(w, &s, &p);
	return s;
}

template<typename T>
inline MuellerMatrix<T> absorber(T value) {
	return MuellerMatrix<T>(
		value, 0, 0, 0,
		0, value, 0, 0,
		0, 0, value, 0,
		0, 0, 0, value
		);
}
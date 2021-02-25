#pragma once
#include <complex>
#include <crt/math_functions.h>

#include "Math/matrix.h"
#include "Spectrum/SampledSpectrum.h"

using MuellerMatrix = Matrix4x4;

namespace Mueller
{
	MuellerMatrix linear_polarizer(Float value = 1.f) {
		Float a = value * .5f;
		return MuellerMatrix(
			a, a, 0, 0,
			a, a, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		);
	}

	MuellerMatrix linear_retarder(Float phase) {
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

	MuellerMatrix<Float> specular_reflection(Float cos_theta_i, Spectrum eta) {
		std::complex<Float> a_s, a_p;

		fresnel_polarized(cos_theta_i, eta);

		Float sin_delta, cos_delta;
		std::tie(sin_delta, cos_delta) = sincos_arg_diff(a_s, a_p);

		Float r_s = abs(sqrt(a_s)),
			r_p = abs(sqrt(a_p)),
			a = .5f * (r_s + r_p),
			b = .5f * (r_s - r_p),
			c = sqrt(r_s * r_p);

		masked(sin_delta, eq(c, 0.f)) = 0.f; // avoid issues with NaNs
		masked(cos_delta, eq(c, 0.f)) = 0.f;

		return MuellerMatrix<Float>(
			a, b, 0, 0,
			b, a, 0, 0,
			0, 0, c * cos_delta, c * sin_delta,
			0, 0, -c * sin_delta, c * cos_delta
			);
	}
}

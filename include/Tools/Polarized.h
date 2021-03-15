#pragma once

template<typename T> class Matrix4x4;

template<typename Spectrum>
struct UnpolarizedT;

template<typename Spectrum>
struct UnpolarizedT
{
	using UnpolarizedType = Spectrum;
};

template<typename Spectrum>
struct UnpolarizedT<Matrix4x4<Spectrum>>
{
	using UnpolarizedType = Spectrum;
};

template<typename Spectrum>
struct SpectrumTrait
{
	static constexpr bool polarized = false;
};

template<typename Spectrum>
struct SpectrumTrait<Matrix4x4<Spectrum>>
{
	static constexpr bool polarized = true;
};

template<typename Spectrum>
constexpr bool is_polarized_t() { return SpectrumTrait<Spectrum>::polarized; }

template<typename Spectrum>
constexpr bool is_polarized_v(Spectrum spectrum) { return SpectrumTrait<Spectrum>::polarized; }

template<typename Spectrum>
using Unpolarize = typename UnpolarizedT<Spectrum>::UnpolarizedType;

template<typename Spectrum>
Unpolarize<Spectrum> unpolarize_v(const Spectrum& spectrum)
{
	if constexpr (is_polarized_v(spectrum))
	{
		return spectrum.m[0][0];
	}
	else
	{
		return spectrum;
	}
}
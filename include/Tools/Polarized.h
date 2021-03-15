#pragma once
#include <xtr1common>

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
	using polarized = std::false_type;
};

template<typename Spectrum>
struct SpectrumTrait<Matrix4x4<Spectrum>>
{
	using polarized = std::true_type;
};

template<typename Spectrum>
constexpr bool is_polarized_t(){ return SpectrumTrait<Spectrum>::polarized::value; }


template<typename Spectrum>
constexpr bool is_polarized_v(Spectrum spectrum) { return SpectrumTrait<Spectrum>::polarized::value; }

template<typename Spectrum>
using Unpolarize = typename UnpolarizedT<Spectrum>::UnpolarizedType;

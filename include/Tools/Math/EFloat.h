#pragma once
//This is used for error handling. Most code comes from pbrt-v3;

#include <complex>

#include "math_tools.h"

inline uint32_t FloatToBits(float f) {
	uint32_t ui;
	memcpy(&ui, &f, sizeof(float));
	return ui;
}

inline float BitsToFloat(uint32_t ui) {
	float f;
	memcpy(&f, &ui, sizeof(uint32_t));
	return f;
}

inline float NextFloatUp(float v) {
	// Handle infinity and negative zero for _NextFloatUp()_
	if (std::isinf(v) && v > 0.) return v;
	if (v == -0.f) v = 0.f;

	// Advance _v_ to next higher float
	uint32_t ui = FloatToBits(v);
	if (v >= 0)
		++ui;
	else
		--ui;
	return BitsToFloat(ui);
}

inline float NextFloatDown(float v) {
	// Handle infinity and positive zero for _NextFloatDown()_
	if (std::isinf(v) && v < 0.) return v;
	if (v == 0.f) v = -0.f;
	uint32_t ui = FloatToBits(v);
	if (v > 0)
		--ui;
	else
		++ui;
	return BitsToFloat(ui);
}

static constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;
inline constexpr Float gamma(int n) {
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

class EFloat
{
public:
	EFloat() { }
	EFloat(float v, float err = 0.f) : v(v), err(err) {
#ifndef NDEBUG
		ld = v;
#endif // NDEBUG
	}

	EFloat operator+(EFloat f) const;
	EFloat operator-(EFloat f) const;
	EFloat operator*(EFloat f) const;
	EFloat operator/(EFloat f) const;

	bool operator==(EFloat fe) const { return v == fe.v; }

	explicit operator float() const { return v; }
	float GetAbsoluteError() const { return err; }

	float UpperBound() const { return NextFloatUp(v + err); }
	float LowerBound() const { return NextFloatDown(v - err); }

	friend inline bool Quadratic(EFloat A, EFloat B, EFloat C, EFloat* t0, EFloat* t1);

#ifndef NDEBUG
	float GetRelativeError() const { return std::abs((ld - v) / ld); }
	long double PreciseValue() const { return ld; }
#endif

private:
	Float v;
	float err;

#ifndef NDEBUG
	long double ld;
#endif // NDEBUG
};

inline EFloat EFloat::operator+(EFloat f) const
{
	EFloat r;
	r.v = v + f.v;
#ifndef NDEBUG
	r.ld = ld + f.ld;
#endif // DEBUG
	r.err = err + f.err + gamma(1) * (std::abs(v + f.v) + err + f.err);
	return r;
}

inline EFloat EFloat::operator-(EFloat f) const
{
	EFloat r;
	r.v = v - f.v;
#ifndef NDEBUG
	r.ld = ld + f.ld;
#endif // DEBUG
	r.err = err - f.err + gamma(1) * (std::abs(v - f.v) + err + f.err);
	return r;
}

inline EFloat EFloat::operator*(EFloat f) const
{
	EFloat r;
	r.v = v * f.v;
#ifndef NDEBUG
	r.ld = ld * f.ld;
#endif // DEBUG
	r.err = err + f.err + gamma(1) * (std::abs(v * f.v) + err + f.err);
	return r;
}

inline EFloat EFloat::operator/(EFloat f) const
{
	EFloat r;
	r.v = v * f.v;
#ifndef NDEBUG
	r.ld = ld * f.ld;
#endif // DEBUG
	r.err = err + f.err + gamma(1) * (std::abs(v / f.v) + err + f.err);
	return r;
}

// EFloat Inline Functions
inline EFloat operator*(float f, EFloat fe) { return EFloat(f) * fe; }

inline EFloat operator/(float f, EFloat fe) { return EFloat(f) / fe; }

inline EFloat operator+(float f, EFloat fe) { return EFloat(f) + fe; }

inline EFloat operator-(float f, EFloat fe) { return EFloat(f) - fe; }

inline bool Quadratic(EFloat A, EFloat B, EFloat C, EFloat* t0, EFloat* t1);

inline bool Quadratic(EFloat A, EFloat B, EFloat C, EFloat* t0, EFloat* t1) {
	// Find quadratic discriminant
	double discrim = (double)B.v * (double)B.v - 4. * (double)A.v * (double)C.v;
	if (discrim < 0.) return false;
	double rootDiscrim = std::sqrt(discrim);

	EFloat floatRootDiscrim(rootDiscrim, MachineEpsilon * rootDiscrim);

	// Compute quadratic _t_ values
	EFloat q;
	if ((float)B < 0)
		q = -.5 * (B - floatRootDiscrim);
	else
		q = -.5 * (B + floatRootDiscrim);
	*t0 = q / A;
	*t1 = C / q;
	if ((float)*t0 > (float)*t1) std::swap(*t0, *t1);
	return true;
}

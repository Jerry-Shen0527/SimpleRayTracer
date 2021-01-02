#pragma once
#include "math_tools.h"
#include <vector>
#include <Tools/Math/rng.h>

#include "Geometry/Vector3.h"

//Uniformly sample in a disk
inline Point2f ConcentricSampleDisk(const Point2f& u)
{
	// Map uniform random numbers to $[-1,1]^2$
	Point2f uOffset = 2.f * u - Vector2f(1, 1);

	// Handle degeneracy at the origin
	if (uOffset.x() == 0 && uOffset.y() == 0) return Point2f(0, 0);

	// Apply concentric mapping to point
	Float theta, r;
	if (std::abs(uOffset.x()) > std::abs(uOffset.y())) {
		r = uOffset.x();
		theta = PiOver4 * (uOffset.y() / uOffset.x());
	}
	else {
		r = uOffset.y();
		theta = PiOver2 - PiOver4 * (uOffset.x() / uOffset.y());
	}
	return r * Point2f(std::cos(theta), std::sin(theta));
}

inline Vector3f UniformSampleHemisphere(const Point2f& u) {
	Float z = u[0];
	Float r = std::sqrt(std::max((Float)0, (Float)1. - z * z));
	Float phi = 2 * Pi * u[1];
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

inline Float UniformHemispherePdf() { return Inv2Pi; }

inline Vector3f UniformSampleSphere(const Point2f& u) {
	Float z = 1 - 2 * u[0];
	Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
	Float phi = 2 * Pi * u[1];
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

struct Distribution1D {
	//Distribution1D Public Methods 758
	Distribution1D(const Float* f, int n) : func(f, f + n), cdf(n + 1)
	{
		//Compute integral of step function at xi 758
		cdf[0] = 0;
		for (int i = 1; i < n + 1; ++i)
			cdf[i] = cdf[i - 1] + func[i - 1] / n;
		//	Transform step function integral into CDF 759
		funcInt = cdf[n];
		if (funcInt == 0) {
			for (int i = 1; i < n + 1; ++i)
				cdf[i] = Float(i) / Float(n);
		}
		else {
			for (int i = 1; i < n + 1; ++i)
				cdf[i] /= funcInt;
		}
	}
	int Count() const { return func.size(); }

	Float SampleContinuous(Float u, Float* pdf, int* off = nullptr) const {
		//Find surrounding CDF segmentsand offset 759
		int offset = FindInterval(cdf.size(), [&](int index) { return cdf[index] <= u; });
		if (off) *off = offset;
		//Compute offset along CDF segment 759
		Float du = u - cdf[offset];
		if ((cdf[offset + 1] - cdf[offset]) > 0)
			du /= (cdf[offset + 1] - cdf[offset]);
		//	Compute PDF for sampled offset 759
		if (pdf) *pdf = func[offset] / funcInt;
		//	Return x ∈[0, 1) corresponding to sample 760
		return (offset + du) / Count();
	}
	//Distribution1D Public Data 758
	int SampleDiscrete(Float u, Float* pdf = nullptr,
		Float* uRemapped = nullptr) const {
		//Find surrounding CDF segmentsand offset 759
		int offset = FindInterval(cdf.size(), [&](int index) { return cdf[index] <= u; });
		if (pdf) *pdf = func[offset] / (funcInt * Count());
		if (uRemapped)
			*uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
		return offset;
	}

	Float DiscretePDF(int index) const {
		return func[index] / (funcInt * Count());
	}
public:
	std::vector<Float> func;
	std::vector<Float> cdf;
	Float funcInt;
};
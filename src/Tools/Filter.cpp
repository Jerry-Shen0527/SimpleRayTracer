#include <Tools/Filter.h>

Float GaussianFilter::Evaluate(const Point2f& p) const {
	return Gaussian(p.x(), expX) * Gaussian(p.y(), expY);
}

Float BoxFilter::Evaluate(const Point2f& p) const { return 1.; }

Float TriangleFilter::Evaluate(const Point2f& p) const
{
	return std::max((Float)0, radius.x() - std::abs(p.x())) *
		std::max((Float)0, radius.y() - std::abs(p.y()));
}
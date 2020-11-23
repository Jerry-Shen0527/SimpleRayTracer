#pragma once
#include "Math/Vector3.h"

class Filter {
public:
    // Filter Interface
    virtual ~Filter();
    Filter(const Vector2f& radius)
        : radius(radius), invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}
    virtual Float Evaluate(const Point2f& p) const = 0;

    // Filter Public Data
    const Vector2f radius, invRadius;
};


class BoxFilter : public Filter {
public:
    BoxFilter(const Vector2f& radius) : Filter(radius) {}
    Float Evaluate(const Point2f& p) const;
};

class TriangleFilter : public Filter {
public:
    TriangleFilter(const Vector2f& radius) : Filter(radius) { }
    Float Evaluate(const Point2f& p) const override;
};

class GaussianFilter : public Filter {
public:
    // GaussianFilter Public Methods
    GaussianFilter(const Vector2f& radius, Float alpha)
        : Filter(radius),
        alpha(alpha),
        expX(std::exp(-alpha * radius.x * radius.x)),
        expY(std::exp(-alpha * radius.y * radius.y)) {}
    Float Evaluate(const Point2f& p) const;

private:
    // GaussianFilter Private Data
    const Float alpha;
    const Float expX, expY;

    // GaussianFilter Utility Functions
    Float Gaussian(Float d, Float expv) const {
        return std::max((Float)0, Float(std::exp(-alpha * d * d) - expv));
    }
};
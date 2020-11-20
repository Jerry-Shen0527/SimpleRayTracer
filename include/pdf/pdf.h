#pragma once
#include <Tools/Math/onb.h>

#include "Tools/Math/Sampling.h"


class pdf {
public:
	virtual ~pdf() {}

	virtual float value(const Vector3f& direction) const = 0;
	virtual Vector3f generate() const = 0;
};

class cosine_pdf : public pdf {
public:
	cosine_pdf(const Vector3f& w) { uvw.build_from_w(w); }

	virtual float value(const Vector3f& direction) const override {
		auto cosine = Dot(unit_vector(direction), uvw.w());
		return (cosine <= 0) ? 0 : cosine / Pi;
	}

	virtual Vector3f generate() const override {
		return uvw.local(random_cosine_d());
	}

public:
	onb uvw;
};

class mixture_pdf : public pdf {
public:
	mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1, float th = 0.5) {
		p[0] = p0;
		p[1] = p1;

		threshold = th;
	}

	virtual float value(const Vector3f& direction) const override {
		return threshold * p[0]->value(direction) + (1 - threshold) * p[1]->value(direction);
	}

	virtual Vector3f generate() const override {
		if (random_float() < threshold)
			return p[0]->generate();
		else
			return p[1]->generate();
	}

public:
	float threshold;
	std::shared_ptr<pdf> p[2];
};

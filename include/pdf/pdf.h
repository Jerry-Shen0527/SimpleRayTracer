#pragma once
#include <Tools/onb.h>
#include <ray.h>

class pdf;

struct scatter_record {
	ray specular_ray;
	bool is_specular;
	color attenuation;
	std::shared_ptr<pdf> pdf_ptr;
};


class pdf {
public:
	virtual ~pdf() {}

	virtual double value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;
};

class cosine_pdf : public pdf {
public:
	cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

	virtual double value(const vec3& direction) const override {
		auto cosine = dot(unit_vector(direction), uvw.w());
		return (cosine <= 0) ? 0 : cosine / pi;
	}

	virtual vec3 generate() const override {
		return uvw.local(random_cosine_direction());
	}

public:
	onb uvw;
};



class mixture_pdf : public pdf {
public:
	mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1, double th = 0.5) {
		p[0] = p0;
		p[1] = p1;

		threshold = th;
	}

	virtual double value(const vec3& direction) const override {
		return threshold * p[0]->value(direction) + (1 - threshold) * p[1]->value(direction);
	}

	virtual vec3 generate() const override {
		if (random_double() < threshold)
			return p[0]->generate();
		else
			return p[1]->generate();
	}

public:
	double threshold;
	std::shared_ptr<pdf> p[2];
};

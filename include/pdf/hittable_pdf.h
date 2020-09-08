#pragma once
#include "pdf.h"
#include "Geometry/hittable.h"


class hittable_pdf : public pdf {
public:
	hittable_pdf(shared_ptr<hittable> p, const point3& origin) : ptr(p), o(origin) {}

	virtual double value(const vec3& direction) const override {
		return ptr->pdf_value(o, direction);
	}

	virtual vec3 generate() const override {
		return ptr->random(o);
	}

public:
	point3 o;
	shared_ptr<hittable> ptr;
};

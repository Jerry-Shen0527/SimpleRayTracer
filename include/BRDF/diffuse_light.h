#pragma once
#include "material.h"

#include "texture.h"

class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

	virtual Spectrum emitted(const ray& r_in, const hit_record& rec, double u, double v,
		const point3& p) const override
	{
		if (rec.front_face)
			return Spectrum::FromRGB( emit->value(u, v, p));
		else
			return Spectrum(0);
	}

public:
	shared_ptr<texture> emit;
};

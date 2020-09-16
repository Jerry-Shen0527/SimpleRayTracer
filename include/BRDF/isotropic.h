#pragma once

#include <BRDF/material.h>

#include "texture.h"
#include "Tools/Math/Sampling.h"

class isotropic : public material {
public:
	isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
	isotropic(std::shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const override {
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
		srec.specular_ray = ray(rec.p, random_in_unit_sphere(), r_in.time());
		srec.is_specular = true;
		return true;
	}

public:
	std::shared_ptr<texture> albedo;
};

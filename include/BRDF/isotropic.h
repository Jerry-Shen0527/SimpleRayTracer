#pragma once

#include <BRDF/Material.h>

#include "texture.h"
#include "Tools/Math/Sampling.h"

class isotropic : public Material {
public:
	isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
	isotropic(std::shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const surface_hit_record& rec, scatter_record& srec
	) const override {
		srec.attenuation = albedo->value(rec.uv, rec.p);
		srec.update();
		srec.specular_ray = ray(rec.p, random_in_unit_sphere(),infinity, r_in.time());
		srec.is_specular = true;
		return true;
	}

public:
	std::shared_ptr<texture> albedo;
};

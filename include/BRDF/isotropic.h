#pragma once

#include <memory>
#include <BRDF/Material.h>

#include "texture.h"
#include "Tools/Math/Sampling.h"

class isotropic : public Material {
public:
	isotropic(Color c) : albedo(std::make_shared<solid_Color>(c)) {}
	isotropic(std::shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const Ray& r_in, const surface_hit_record& rec, scatter_record& srec
	) const override {
		srec.attenuation = albedo->value(rec.uv, rec.p);
		srec.update();
		srec.specular_ray = Ray(rec.p, random_in_unit_sphere(),infinity, r_in.time());
		srec.is_specular = true;
		return true;
	}

public:
	std::shared_ptr<texture> albedo;
};

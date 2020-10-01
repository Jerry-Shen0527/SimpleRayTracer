#pragma once

#include <ray.h>

#include "metal.h"
#include <pdf/scatter_record.h>
#include <Geometry/hit_record.h>

vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat);
float schlick(float cosine, float ref_idx);

class dielectric : public Material {
public:
	dielectric(float ri) : ref_idx(ri) {}

	virtual bool scatter(
		const ray& r_in, const surface_hit_record& rec, scatter_record& srec
	) const override;

	float ref_idx;
};

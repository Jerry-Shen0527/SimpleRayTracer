#pragma once

#include <ray.h>

#include "metal.h"
#include <pdf/scatter_record.h>
#include <Geometry/surface_hit_record.h>

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat);
double schlick(double cosine, double ref_idx);

class dielectric : public material {
public:
	dielectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(
		const ray& r_in, const surface_hit_record& rec, scatter_record& srec
	) const override;

	double ref_idx;
};

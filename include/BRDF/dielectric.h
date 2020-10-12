#pragma once

#include <ray.h>

#include "metal.h"
#include <pdf/scatter_record.h>
#include <Geometry/hit_record.h>

Vector3f refract(const Vector3f& uv, const Vector3f& n, float etai_over_etat);
float schlick(float cosine, float ref_idx);

class dielectric : public Material {
public:
	dielectric(float ri) : ref_idx(ri) {}

	virtual bool scatter(
		const Ray& r_in, const SurfaceInteraction& rec, scatter_record& srec
	) const override;

	float ref_idx;
};

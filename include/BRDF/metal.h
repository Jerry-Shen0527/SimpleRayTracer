#pragma once
#include "material.h"

vec3 reflect(const vec3& v, const vec3& n);

class metal : public material {
public:
	metal(const color& a) : albedo(a) {}

	metal(const color& a, double f);

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const override;

public:
	color albedo;
	double fuzz;
};

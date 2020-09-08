#pragma once

#include <ray.h>

#include <BRDF/metal.h>

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = dot(-uv, n);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

double schlick(double cosine, double ref_idx) {
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class dielectric : public material {
public:
	dielectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const override;

	double ref_idx;
};

inline bool dielectric::scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const
{
	srec.attenuation = color(1.0, 1.0, 1.0);
	srec.is_specular = true;
	double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx;

	vec3 unit_direction = unit_vector(r_in.direction());

	double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
	double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
	if (etai_over_etat * sin_theta > 1.0)
	{
		vec3 reflected = reflect(unit_direction, rec.normal);
		srec.specular_ray = ray(rec.p, reflected);
		return true;
	}

	double reflect_prob = schlick(cos_theta, etai_over_etat);
	if (random_double() < reflect_prob)
	{
		vec3 reflected = reflect(unit_direction, rec.normal);
		srec.specular_ray = ray(rec.p, reflected);
		return true;
	}

	vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
	srec.specular_ray = ray(rec.p, refracted);
	return true;
}

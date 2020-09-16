#include <BRDF/metal.h>

#include "Tools/Math/Sampling.h"

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

metal::metal(const color& a, double f): albedo(a), fuzz(f < 1 ? f : 1)
{
}

bool metal::scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const
{
	vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
	srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
	srec.attenuation = albedo;
	srec.is_specular = true;
	srec.pdf_ptr = nullptr;
	return true;
}

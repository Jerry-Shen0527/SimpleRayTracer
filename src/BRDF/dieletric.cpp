#include <BRDF/dielectric.h>

Vector3f refract(const Vector3f& uv, const Vector3f& n, float etai_over_etat) {
	auto cos_theta = Dot(-uv, n);
	Vector3f r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vector3f r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}


float schlick(float cosine, float ref_idx) {
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool dielectric::scatter(const Ray& r_in, const surface_hit_record& rec, scatter_record& srec) const
{
	srec.attenuation = Color(1.0, 1.0, 1.0);
	srec.update();
	srec.is_specular = true;
	float etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx;

	Vector3f unit_direction = unit_vector(r_in.direction());

	float cos_theta = fmin(Dot(-unit_direction, rec.normal), 1.0);
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
	if (etai_over_etat * sin_theta > 1.0)
	{
		Vector3f reflected = reflect(unit_direction, rec.normal);
		srec.specular_ray = Ray(rec.p, reflected);
		return true;
	}

	float reflect_prob = schlick(cos_theta, etai_over_etat);
	if (random_float() < reflect_prob)
	{
		Vector3f reflected = reflect(unit_direction, rec.normal);
		srec.specular_ray = Ray(rec.p, reflected);
		return true;
	}

	Vector3f refracted = refract(unit_direction, rec.normal, etai_over_etat);
	srec.specular_ray = Ray(rec.p, refracted);
	return true;
}

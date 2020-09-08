#pragma once

class lambertian : public material {
public:

	lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
	lambertian(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const override {
		srec.is_specular = false;
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared< cosine_pdf>(rec.normal);
		return true;
	}

	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
	{
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 : cosine / pi;
	}

public:
	shared_ptr<texture> albedo;
};

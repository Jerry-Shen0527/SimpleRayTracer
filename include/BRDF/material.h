#ifndef MATERIAL_H
#define MATERIAL_H

#include <ray.h>
#include <pdf/pdf.h>

class material;

struct hit_record {
	point3 p;
	vec3 normal;
	double u;
	double v;
	double t;
	bool front_face;
	std::shared_ptr<material> mat_ptr;
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class material {
public:
	virtual ~material() = default;

	virtual color emitted(
		const ray& r_in, const hit_record& rec, double u, double v, const point3& p
	) const {
		return color(0, 0, 0);
	}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const {
		return false;
	}

	virtual double scattering_pdf(
		const ray& r_in, const hit_record& rec, const ray& scattered
	) const {
		return 0;
	}
};

#endif
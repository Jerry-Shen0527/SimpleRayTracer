#ifndef SPHERE_H
#define SPHERE_H

#include <Geometry/hittable.h>
#include <Tools/Math/onb.h>

class sphere : public hittable {
public:
	sphere() {}
	sphere(point3 cen, double r, std::shared_ptr<material> m, bool pdf = false)
		: center(cen), radius(r), mat_ptr(m) {
		pdf_enabled = pdf;
	}

	virtual bool hit(
		const ray& r, double tmin, double tmax, surface_hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

	void get_sphere_uv(const vec3& p, vec2& uv) const;
	double pdf_value(const point3& o, const vec3& v) const override;
	vec3 random(const point3& o) const override;

public:

	point3 center;
	double radius;
	std::shared_ptr<material> mat_ptr;
};

#endif

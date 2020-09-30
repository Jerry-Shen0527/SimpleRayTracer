#include <Geometry/sphere.h>
#include <Geometry/bvh.h>

#include "Tools/Math/math_tools.h"
#include "Tools/Math/Sampling.h"

bool sphere::hit(const ray& r, surface_hit_record& rec) const {
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;

	if (discriminant > 0) {
		auto root = sqrt(discriminant);

		auto temp = (-half_b - root) / a;
		if (temp < r.tMax) {
			//if (temp < r.tMax && temp > t_min) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			rec.normal = (rec.p - center) / radius;
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r.direction(), outward_normal);
			get_sphere_uv((rec.p - center) / radius, rec.uv);
			rec.mat_ptr = mat_ptr;
			return true;
		}

		temp = (-half_b + root) / a;
		if (temp < r.tMax) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			rec.normal = (rec.p - center) / radius;
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r.direction(), outward_normal);
			get_sphere_uv((rec.p - center) / radius, rec.uv);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& output_box) const
{
	output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

void sphere::get_sphere_uv(const vec3& p, vec2& uv) const
{
	auto phi = atan2(p.z(), p.x());
	auto theta = asin(p.y());
	uv.x() = 1 - (phi + pi) / (2 * pi);
	uv.y() = (theta + pi / 2) / pi;
}

float sphere::pdf_value(const point3& o, const vec3& v) const {
	surface_hit_record rec;
	if (!this->hit(ray(o, v), rec))
		return 0;

	auto cos_theta_max = sqrt(1 - radius * radius / (center - o).length_squared());
	auto solid_angle = 2 * pi * (1 - cos_theta_max);

	return  1 / solid_angle;
}

vec3 sphere::random(const point3& o) const {
	vec3 direction = center - o;
	auto distance_squared = direction.length_squared();
	onb uvw;
	uvw.build_from_w(direction);
	return uvw.local(random_to_sphere(radius, distance_squared));
}
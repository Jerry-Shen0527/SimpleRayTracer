#include <Geometry/hittable_list.h>
#include <Geometry/MovingSphere.h>
#include <Geometry/bvh.h>

point3 moving_sphere::center(float time) const {
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}
bool moving_sphere::bounding_box(float t0, float t1, aabb& output_box) const
{
	aabb box0(
		center(t0) - vec3(radius, radius, radius),
		center(t0) + vec3(radius, radius, radius));
	aabb box1(
		center(t1) - vec3(radius, radius, radius),
		center(t1) + vec3(radius, radius, radius));
	output_box = surrounding_box(box0, box1);
	return true;
}

bool moving_sphere::hit(
	const ray& r, surface_hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;

	auto discriminant = half_b * half_b - a * c;

	if (discriminant > 0) {
		auto root = sqrt(discriminant);

		auto temp = (-half_b - root) / a;
		if (temp < r.tMax && temp > 0.0001) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			auto outward_normal = (rec.p - center(r.time())) / radius;
			rec.set_face_normal(r.direction(), outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}

		temp = (-half_b + root) / a;
		if (temp < r.tMax && temp > 0.0001) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			auto outward_normal = (rec.p - center(r.time())) / radius;
			rec.set_face_normal(r.direction(), outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}
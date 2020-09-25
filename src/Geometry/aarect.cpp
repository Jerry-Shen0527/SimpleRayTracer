#include <Geometry/aarect.h>

#include "Tools/Math/math_tools.h"

bool xy_rect::hit(const ray& r, float t0, float t1, surface_hit_record& rec) const {
	auto t = (k - r.origin().z()) / r.direction().z();
	if (t < t0 || t > t1)
		return false;
	auto x = r.origin().x() + t * r.direction().x();
	auto y = r.origin().y() + t * r.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.uv.x() = (x - x0) / (x1 - x0);
	rec.uv.y() = (y - y0) / (y1 - y0);
	rec.t = t;
	auto outward_normal = vec3(0, 0, 1);
	rec.set_face_normal(r.direction(), outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	return true;
}

bool xy_rect::bounding_box(float t0, float t1, aabb& output_box) const
{
	// The bounding box must have non-zero width in each dimension, so pad the Z
	// dimension a small amount.
	output_box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
	return true;
}

bool xz_rect::hit(const ray& r, float t0, float t1, surface_hit_record& rec) const {
	auto t = (k - r.origin().y()) / r.direction().y();
	if (t < t0 || t > t1)
		return false;
	auto x = r.origin().x() + t * r.direction().x();
	auto z = r.origin().z() + t * r.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.uv.x() = (x - x0) / (x1 - x0);
	rec.uv.y() = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = vec3(0, 1, 0);
	rec.set_face_normal(r.direction(), outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	return true;
}

float xz_rect::pdf_value(const point3& origin, const vec3& v) const
{
	surface_hit_record rec;
	if (!this->hit(ray(origin, v), 0.001, infinity, rec))
		return 0;

	auto area = (x1 - x0) * (z1 - z0);
	auto distance_squared = rec.t * rec.t * v.length_squared();
	auto cosine = fabs(dot(v, rec.normal) / v.length());

	return distance_squared / (cosine * area);
}

vec3 xz_rect::random(const point3& origin) const
{
	auto random_point = point3(random_float(x0, x1), k, random_float(z0, z1));
	return random_point - origin;
}

bool yz_rect::hit(const ray& r, float t0, float t1, surface_hit_record& rec) const {
	auto t = (k - r.origin().x()) / r.direction().x();
	if (t < t0 || t > t1)
		return false;
	auto y = r.origin().y() + t * r.direction().y();
	auto z = r.origin().z() + t * r.direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.uv.x() = (y - y0) / (y1 - y0);
	rec.uv.y() = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = vec3(1, 0, 0);
	rec.set_face_normal(r.direction(), outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	return true;
}

bool yz_rect::bounding_box(float t0, float t1, aabb& output_box) const
{
	// The bounding box must have non-zero width in each dimension, so pad the X
	// dimension a small amount.
	output_box = aabb(point3(k - 0.0001, y0, z0), point3(k + 0.0001, y1, z1));
	return true;
}

bool flip_face::hit(const ray& r, float t_min, float t_max, surface_hit_record& rec) const
{
	if (!ptr->hit(r, t_min, t_max, rec))
		return false;

	rec.front_face = !rec.front_face;
	return true;
}

bool flip_face::bounding_box(float t0, float t1, aabb& output_box) const
{
	return ptr->bounding_box(t0, t1, output_box);
}

bool flip_face::get_pdf_enabled()
{
	return ptr->get_pdf_enabled();
}
float flip_face::pdf_value(const point3& o, const vec3& v) const
{
	return ptr->pdf_value(o, v);
}

vec3 flip_face::random(const vec3& o) const
{
	return ptr->random(o);
}
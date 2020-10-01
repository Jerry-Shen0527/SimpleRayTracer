#include <Geometry/translation.h>

#include "Tools/Math/math_tools.h"

Transform::Transform(const Float mat[4][4])
{
	m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
	mInv = Inverse(m);
}

Transform::Transform(const Matrix4x4& m, const Matrix4x4& mInv) : m(m), mInv(mInv)
{
}

bool Transform::HasScale() const
{
	Float la2 = (*this)(Vector3f(1, 0, 0)).length_squared();
	Float lb2 = (*this)(Vector3f(0, 1, 0)).length_squared();
	Float lc2 = (*this)(Vector3f(0, 0, 1)).length_squared();
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
	return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
#undef NOT_ONE
}

bool Transform::SwapsHandedness() const
{
	Float det =
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) -
		m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0]) +
		m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]);
	return det < 0;
}

bool translate::hit(const ray& r, surface_hit_record& rec) const {
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (!ptr->hit(moved_r, rec))
		return false;

	rec.p += offset;
	rec.set_face_normal(moved_r.direction(), rec.normal);

	return true;
}

bool translate::bounding_box(float t0, float t1, aabb& output_box) const {
	if (!ptr->bounding_box(t0, t1, output_box))
		return false;

	output_box = aabb(output_box.min() + offset, output_box.max() + offset);

	return true;
}

rotate_y::rotate_y(std::shared_ptr<hittable> p, float angle) : ptr(p) {
	auto radians = degrees_to_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r,  surface_hit_record& rec) const {
	auto origin = r.origin();
	auto direction = r.direction();

	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	ray rotated_r(origin, direction,infinity, r.time());

	if (!ptr->hit(rotated_r, rec))
		return false;

	auto p = rec.p;
	auto normal = rec.normal;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotated_r.direction(), normal);

	return true;
}
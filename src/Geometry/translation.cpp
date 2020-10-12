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

Bounds3f Transform::operator()(const Bounds3f& b) const
{
	const Transform& M = *this;
	Bounds3f ret(M(Point3f(b.pMin.x(), b.pMin.y(), b.pMin.z())));
	ret = Union(ret, M(Point3f(b.pMax.x(), b.pMin.y(), b.pMin.z())));
	ret = Union(ret, M(Point3f(b.pMin.x(), b.pMax.y(), b.pMin.z())));
	ret = Union(ret, M(Point3f(b.pMin.x(), b.pMin.y(), b.pMax.z())));
	ret = Union(ret, M(Point3f(b.pMin.x(), b.pMax.y(), b.pMax.z())));
	ret = Union(ret, M(Point3f(b.pMax.x(), b.pMax.y(), b.pMin.z())));
	ret = Union(ret, M(Point3f(b.pMax.x(), b.pMin.y(), b.pMax.z())));
	ret = Union(ret, M(Point3f(b.pMax.x(), b.pMax.y(), b.pMax.z())));
	return ret;
}

Ray Transform::operator()(const Ray& r) const
{
	Vector3f oError;
	Point3f o = (*this)(r.orig);
	Vector3f d = (*this)(r.d);
	//TODO:Offset ray origin to edge of error boundsand compute tMax 233
	//Float lengthSquared = d.length_squared();
	//Float tMax = r.tMax;
	//if (lengthSquared > 0) {
	//	Float dt = dot(abs(d), oError) / lengthSquared;
	//	o += d * dt;
	//	tMax -= dt;
	//}
	return Ray(o, d, r.tMax, r.time(), r.medium);
}

SurfaceInteraction Transform::operator()(const SurfaceInteraction& si) const
{
	SurfaceInteraction ret;
	// Transform _p_ and _pError_ in _SurfaceInteraction_
	ret.p = (*this)(si.p);

	// Transform remaining members of _SurfaceInteraction_
	const Transform& t = *this;
	ret.n = (t(si.n)).normalize();
	ret.ray_in = (t(si.ray_in)).normalize();
	ret.time = si.time;
	ret.mediumInterface = si.mediumInterface;
	ret.uv = si.uv;
	ret.shape = si.shape;
	ret.dpdu = t(si.dpdu);
	ret.dpdv = t(si.dpdv);
	ret.dndu = t(si.dndu);
	ret.dndv = t(si.dndv);
	ret.shading.n = t(si.shading.n).normalize();
	ret.shading.dpdu = t(si.shading.dpdu);
	ret.shading.dpdv = t(si.shading.dpdv);
	ret.shading.dndu = t(si.shading.dndu);
	ret.shading.dndv = t(si.shading.dndv);
	ret.dudx = si.dudx;
	ret.dvdx = si.dvdx;
	ret.dudy = si.dudy;
	ret.dvdy = si.dvdy;
	ret.dpdx = t(si.dpdx);
	ret.dpdy = t(si.dpdy);
	ret.bsdf = si.bsdf;
	ret.bssrdf = si.bssrdf;
	ret.primitive = si.primitive;
	//    ret.n = Faceforward(ret.n, ret.shading.n);
	ret.shading.n = Faceforward(ret.shading.n, ret.n);
	ret.faceIndex = si.faceIndex;
	return ret;
}

bool Transform::HasScale() const
{
	Float la2 = (*this)(Vector3f(1, 0, 0)).LengthSquared();
	Float lb2 = (*this)(Vector3f(0, 1, 0)).LengthSquared();
	Float lc2 = (*this)(Vector3f(0, 0, 1)).LengthSquared();
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

bool Transform::IsIdentity() const
{
	return (m.m[0][0] == 1.f && m.m[0][1] == 0.f && m.m[0][2] == 0.f &&
		m.m[0][3] == 0.f && m.m[1][0] == 0.f && m.m[1][1] == 1.f &&
		m.m[1][2] == 0.f && m.m[1][3] == 0.f && m.m[2][0] == 0.f &&
		m.m[2][1] == 0.f && m.m[2][2] == 1.f && m.m[2][3] == 0.f &&
		m.m[3][0] == 0.f && m.m[3][1] == 0.f && m.m[3][2] == 0.f &&
		m.m[3][3] == 1.f);
}

bool translate::hit(const Ray& r, SurfaceInteraction& rec) const {
	Ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (!ptr->hit(moved_r, rec))
		return false;

	rec.p += offset;
	rec.set_face_normal(moved_r.direction(), rec.n);

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

	Point3f min(infinity, infinity, infinity);
	Point3f max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				Vector3f tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = aabb(min, max);
}

bool rotate_y::hit(const Ray& r, SurfaceInteraction& rec) const {
	auto origin = r.origin();
	auto direction = r.direction();

	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	Ray rotated_r(origin, direction, infinity, r.time());

	if (!ptr->hit(rotated_r, rec))
		return false;

	auto p = rec.p;
	auto normal = rec.n;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.n[0] + sin_theta * rec.n[2];
	normal[2] = -sin_theta * rec.n[0] + cos_theta * rec.n[2];

	rec.p = p;
	rec.set_face_normal(rotated_r.direction(), normal);

	return true;
}
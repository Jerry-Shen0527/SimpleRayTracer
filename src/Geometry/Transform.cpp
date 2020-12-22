#include "Geometry/Interaction.h"
#include "Geometry/Transform.h"

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
	Point3f o = (*this)(r.o, &oError);
	Vector3f d = (*this)(r.d);
	// Offset ray origin to edge of error bounds and compute _tMax_
	Float lengthSquared = d.LengthSquared();
	Float tMax = r.tMax;
	if (lengthSquared > 0) {
		Float dt = Dot(Abs(d), oError) / lengthSquared;
		o += d * dt;
		tMax -= dt;
	}
	return Ray(o, d, tMax, r.time, r.medium);
}

Transform Transform::operator*(const Transform& t2) const
{
	return Transform(Matrix4x4::Mul(m, t2.m), Matrix4x4::Mul(t2.mInv, mInv));
}

SurfaceInteraction Transform::operator()(const SurfaceInteraction& si) const
{
	SurfaceInteraction ret;
	// Transform _p_ and _pError_ in _SurfaceInteraction_
	ret.p = (*this)(si.p);

	// Transform remaining members of _SurfaceInteraction_
	const Transform& t = *this;
	ret.n = (t(si.n)).Normalize();
	ret.wo = (t(si.wo)).Normalize();
	ret.time = si.time;
	ret.mediumInterface = si.mediumInterface;
	ret.uv = si.uv;
	ret.shape = si.shape;
	ret.dpdu = t(si.dpdu);
	ret.dpdv = t(si.dpdv);
	ret.dndu = t(si.dndu);
	ret.dndv = t(si.dndv);
	ret.shading.n = t(si.shading.n).Normalize();
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
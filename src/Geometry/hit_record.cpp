#include <Geometry/hit_record.h>
#include <Tools/Light/AreaLight.h>

Spectrum SurfaceInteraction::Le(const Vector3f& w) const
{
	const AreaLight* area = mat_ptr->GetAreaLight();
	return area ? area->L(*this, w) : Spectrum(0.f);
}

Ray Interaction::SpawnRayTo(const Interaction& h2) const
{
	return SpawnRayTo(h2.p);
}

Ray Interaction::SpawnRay(const Vector3f& d) const
{
	Point3f o = OffsetRayOrigin(p, pError, n, d);
	return Ray(o, d, Infinity, t, GetMedium(d));
}

Ray Interaction::SpawnRayTo(const Point3f& p2) const
{
	Point3f origin = OffsetRayOrigin(p, pError, n, p2 - p);
	Vector3f d = p2 - p;
	return Ray(origin, d, 1 - ShadowEpsilon, t, GetMedium(d));
}


void SurfaceInteraction::set_face_normal(const Vector3f& r_in, const Normal3f& outward_normal)
{
	front_face = Dot(r_in, outward_normal) < 0;
	n = front_face ? outward_normal : -outward_normal;
}

SurfaceInteraction::SurfaceInteraction(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, Float time,const Shape * shape) : Interaction(p, Normal3f((Cross(dpdu, dpdv)).normalize()), pError, wo, time), uv(uv), dpdu(dpdu), dpdv(dpdv), dndu(dndu), dndv(dndv)
{
	set_face_normal(wo, n);

	shading.n = n;
	shading.dpdu = dpdu;
	shading.dpdv = dpdv;
	shading.dndu = dndu;
	shading.dndv = dndv;
}
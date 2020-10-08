#include <Geometry/hit_record.h>
#include <Tools/Light/AreaLight.h>

Spectrum surface_hit_record::Le(const Vector3f& w) const
{
	const AreaLight* area = mat_ptr->GetAreaLight();
	return area ? area->L(*this, w) : Spectrum(0.f);
}

ray hit_record::SpawnRayTo(const hit_record& h2) const
{
	return SpawnRayTo(h2.p);
}

ray hit_record::SpawnRay(const Vector3f& d) const
{
	Point3f o = OffsetRayOrigin(p, pError, normal, d);
	return ray(o, d, infinity, t, GetMedium(d));
}

ray hit_record::SpawnRayTo(const Point3f& p2) const
{
	Point3f origin = OffsetRayOrigin(p, pError, normal, p2 - p);
	Vector3f d = p2 - p;
	return ray(origin, d, 1 - ShadowEpsilon, t, GetMedium(d));
}


void surface_hit_record::set_face_normal(const Vector3f& r_in, const Normal3f& outward_normal)
{
	front_face = dot(r_in, outward_normal) < 0;
	normal = front_face ? outward_normal : -outward_normal;
}

surface_hit_record::surface_hit_record(const Point3f& p, const Vector3f& pError, const Point2f& uv, const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, Float time,const Shape * shape) : hit_record(p, Normal3f((cross(dpdu, dpdv)).normalize()), pError, wo, time), uv(uv), dpdu(dpdu), dpdv(dpdv), dndu(dndu), dndv(dndv)
{
	set_face_normal(wo, normal);

	shading.n = normal;
	shading.dpdu = dpdu;
	shading.dpdv = dpdv;
	shading.dndu = dndu;
	shading.dndv = dndv;
}
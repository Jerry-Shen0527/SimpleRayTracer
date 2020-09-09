#include <Geometry/hittable.h>
#include <Geometry/bvh.h>

bool hittable::bounding_box(double t0, double t1, aabb& output_box) const
{
	output_box = aabb(vec3(-DBL_MAX, -DBL_MAX, -DBL_MAX), vec3(DBL_MAX, DBL_MAX, DBL_MAX));
	return false;
}

bool hittable::get_pdf_enabled()
{
	return pdf_enabled;
}
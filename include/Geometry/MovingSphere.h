#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "hittable.h"

class moving_sphere : public hittable {
public:
	moving_sphere() {}
	moving_sphere(
		Point3f cen0, Point3f cen1, float t0, float t1, float r, shared_ptr<material> m)
		: center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
	{};

	virtual bool hit(
		const Ray& r,  SurfaceInteraction& rec) const override;

	Point3f center(float time) const;

	bool bounding_box(float t0, float t1, aabb& output_box) const override;
	Point3f center0, center1;
	float time0, time1;
	float radius;
	shared_ptr<material> mat_ptr;
};

#endif
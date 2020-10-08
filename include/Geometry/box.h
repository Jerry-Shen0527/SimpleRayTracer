#ifndef BOX_H
#define BOX_H

#include "aarect.h"
#include "hittable_list.h"


class box : public hittable {
public:
	box() {}
	box(const Point3f& p0, const Point3f& p1, shared_ptr<Material> ptr);

	virtual bool hit(const ray& r,  surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override {
		output_box = aabb(box_min, box_max);
		return true;
	}

public:
	Point3f box_min;
	Point3f box_max;
	hittable_list sides;
};


#endif
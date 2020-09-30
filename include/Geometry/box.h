#ifndef BOX_H
#define BOX_H

#include "aarect.h"
#include "hittable_list.h"


class box : public hittable {
public:
	box() {}
	box(const point3& p0, const point3& p1, shared_ptr<material> ptr);

	virtual bool hit(const ray& r,  surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override {
		output_box = aabb(box_min, box_max);
		return true;
	}

public:
	point3 box_min;
	point3 box_max;
	hittable_list sides;
};


#endif
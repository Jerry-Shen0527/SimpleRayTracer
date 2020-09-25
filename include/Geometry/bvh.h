#ifndef BVH_H
#define BVH_H

#include <ray.h>
#include <vector>


#undef min
#undef max

struct surface_hit_record;
#include "hittable.h"

class aabb {
public:
	aabb() {}
	aabb(const point3& a, const point3& b) { _min = a; _max = b; }

	point3 min() const { return _min; }
	point3 max() const { return _max; }

	bool hit(const ray& r, float tmin, float tmax) const;

	point3 _min;
	point3 _max;
};

class hittable_list;
//this serves as a decorator for simple hittable list
class bvh_node : public hittable
{
public:

	bvh_node(hittable_list& list, float time0, float time1);


	bvh_node(
		std::vector<std::shared_ptr<hittable>>& objects,
		size_t start, size_t end, float time0, float time1);

	virtual bool hit(
		const ray& r, float tmin, float tmax, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> left;
	std::shared_ptr<hittable> right;
	aabb box;
};


#endif
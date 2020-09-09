#ifndef BVH_H
#define BVH_H

#include <ray.h>
#include <vector>

#include <Geometry/hittable_list.h>



#undef min
#undef max

class aabb {
public:
	aabb() {}
	aabb(const point3& a, const point3& b) { _min = a; _max = b; }

	point3 min() const { return _min; }
	point3 max() const { return _max; }

	bool hit(const ray& r, double tmin, double tmax) const;

	point3 _min;
	point3 _max;
};

class hittable_list;
//this serves as a decorator for simple hittable list
class bvh_node : public hittable
{
public:

	bvh_node(hittable_list& list, double time0, double time1)
		: bvh_node(list.objects, 0, list.objects.size(), time0, time1)
	{}

	bvh_node(
		std::vector<std::shared_ptr<hittable>>& objects,
		size_t start, size_t end, double time0, double time1);

	virtual bool hit(
		const ray& r, double tmin, double tmax, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> left;
	std::shared_ptr<hittable> right;
	aabb box;
};

aabb surrounding_box(aabb box0, aabb box1);

#endif
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
#include <memory>
#include <vector>

#include <Geometry/hittable.h>


using std::shared_ptr;
using std::make_shared;

aabb surrounding_box(aabb box0, aabb box1);

class hittable_list : public hittable {
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object);

	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;
	double pdf_value(const point3& o, const vec3& v) const override;
	vec3 random(const vec3& o) const override;

	std::vector<shared_ptr<hittable>> objects;
	std::vector<shared_ptr<hittable>> pdf_objects;
};

#endif

#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include <BRDF/isotropic.h>
class constant_medium : public hittable {
public:
	constant_medium(std::shared_ptr<hittable> b, float d, std::shared_ptr<texture> a);

	constant_medium(std::shared_ptr<hittable> b, float d, color c);

	virtual bool hit(const ray& r,  surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> boundary;
	std::shared_ptr<material> phase_function;
	float neg_inv_density;
};

#endif
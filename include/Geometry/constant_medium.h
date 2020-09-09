#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include <BRDF/isotropic.h>
class constant_medium : public hittable {
public:
	constant_medium(std::shared_ptr<hittable> b, double d, std::shared_ptr<texture> a);

	constant_medium(std::shared_ptr<hittable> b, double d, color c);

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> boundary;
	std::shared_ptr<material> phase_function;
	double neg_inv_density;
};

#endif
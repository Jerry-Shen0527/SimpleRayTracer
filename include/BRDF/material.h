#ifndef MATERIAL_H
#define MATERIAL_H

#include <ray.h>
#include <Geometry/hittable.h>
#include <Geometry/hittable.h>

struct hit_record;

class material {
public:
	virtual color emitted(double u, double v, const point3& p) const {
		return color(0, 0, 0);
	}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const = 0;
};



#endif
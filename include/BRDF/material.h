#ifndef MATERIAL_H
#define MATERIAL_H

#include <ray.h>
#include <Geometry/hit_record.h>
#include <pdf/scatter_record.h>

#include "common.h"

class material {
public:
	virtual ~material() = default;

	virtual Spectrum emitted(
		const ray& r_in, const hit_record& rec, double u, double v, const point3& p
	) const {
		return Spectrum(0);
	}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const {
		return false;
	}

	virtual double scattering_pdf(
		const ray& r_in, const hit_record& rec, const ray& scattered
	) const {
		return 0;
	}
};

#endif
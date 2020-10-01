#ifndef MATERIAL_H
#define MATERIAL_H

#include <ray.h>
#include <pdf/scatter_record.h>

class surface_hit_record;
class AreaLight;


class Material {
public:
	virtual ~Material() = default;

	virtual color emitted(const ray& r_in, const surface_hit_record& rec,const point2& uv, const point3& p) const {
		return color(0, 0, 0);
	}

	virtual bool scatter(const ray& r_in, const surface_hit_record& rec, scatter_record& srec) const {
		return false;
	}

	virtual float scattering_pdf(
		const ray& r_in, const surface_hit_record& rec, const ray& scattered
	) const {
		return 0;
	}

	virtual  AreaLight* GetAreaLight() { return  nullptr; }
};

#endif

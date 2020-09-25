#pragma once
#include "material.h"

#include "texture.h"

class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

	virtual color emitted(const ray& r_in, const surface_hit_record& rec, const vec2& uv, const point3& p) const override
	{
		if (rec.front_face)
			return  emit->value(uv, p);
		else
			return color(0, 0, 0);
	}

public:
	shared_ptr<texture> emit;
};

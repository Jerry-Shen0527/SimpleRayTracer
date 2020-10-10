#pragma once
#include "Material.h"

#include "texture.h"

class diffuse_light : public Material {
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(Color c) : emit(std::make_shared<solid_Color>(c)) {}

	virtual Color emitted(const Ray& r_in, const surface_hit_record& rec, const Point2f& uv, const Point3f& p) const override
	{
		if (rec.front_face)
			return  emit->value(uv, p);
		else
			return Color(0, 0, 0);
	}

public:
	shared_ptr<texture> emit;
};

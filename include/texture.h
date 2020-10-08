#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <memory>
#include <Tools/perlin.h>

#include "Tools/Math/Vector3.h"

class texture {
public:
	virtual Color value(const Vector2f& uv, const Point3f& p) const = 0;
};

class solid_Color : public texture {
public:
	solid_Color() {}
	solid_Color(Color c) : Color_value(c) {}

	solid_Color(float red, float green, float blue)
		: solid_Color(Color(red, green, blue)) {}

	virtual Color value(const Vector2f& uv, const Point3f& p) const override {
		return Color_value;
	}

private:
	Color Color_value;
};

class checker_texture : public texture {
public:
	checker_texture() {}

	checker_texture(std::shared_ptr<texture> t0, std::shared_ptr<texture> t1)
		: even(t0), odd(t1) {}

	checker_texture(Color c1, Color c2)
		: even(std::make_shared<solid_Color>(c1)), odd(std::make_shared<solid_Color>(c2)) {}

	virtual Color value(const Vector2f& uv, const Point3f& p) const override;

public:
	std::shared_ptr<texture> odd;
	std::shared_ptr<texture> even;
};

class noise_texture : public texture {
public:
	noise_texture() {}

	noise_texture(float sc) : scale(sc) {}

	virtual Color value(const Vector2f& uv, const Point3f& p) const override;

public:
	perlin noise;
	float scale;
};

class image_texture : public texture {
public:
	const static int bytes_per_pixel = 3;

	image_texture()
		: data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	image_texture(const std::string& filename);

	~image_texture() {
		delete data;
	}

	virtual Color value(const Vector2f& uv, const Point3f& p) const override;

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};

#endif
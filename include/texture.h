#ifndef TEXTURE_H
#define TEXTURE_H

#include <Tools/perlin.h>

#include "Tools/Math/vec3.h"


class texture {
public:
	virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
	solid_color() {}
	solid_color(color c) : color_value(c) {}

	solid_color(double red, double green, double blue)
		: solid_color(color(red, green, blue)) {}

	virtual color value(double u, double v, const vec3& p) const override {
		return color_value;
	}

private:
	color color_value;
};

class checker_texture : public texture {
public:
	checker_texture() {}

	checker_texture(std::shared_ptr<texture> t0, std::shared_ptr<texture> t1)
		: even(t0), odd(t1) {}

	checker_texture(color c1, color c2)
		: even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	virtual color value(double u, double v, const point3& p) const override;

public:
	std::shared_ptr<texture> odd;
	std::shared_ptr<texture> even;
};

class noise_texture : public texture {
public:
	noise_texture() {}

	noise_texture(double sc) : scale(sc) {}

	virtual color value(double u, double v, const point3& p) const override;

public:
	perlin noise;
	double scale;
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

	virtual color value(double u, double v, const vec3& p) const override;

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};

#endif
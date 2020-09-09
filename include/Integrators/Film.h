#pragma once
#include <vec3.h>
#include <vector>

class Film
{
public:

	Film(int w, int h);

	void write_color(int i, int j, color pixel_color, int samples_per_pixel);
	int pixelcount;

	int width;
	int height;

	std::vector<unsigned char> image;
};

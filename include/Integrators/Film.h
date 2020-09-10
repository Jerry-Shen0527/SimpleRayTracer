#pragma once
#include <vector>

#include "Tools/Math/vec3.h"

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

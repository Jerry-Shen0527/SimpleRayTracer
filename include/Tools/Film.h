#pragma once

#include <vector>

#include "Math/Vector3.h"


class Film
{
public:

	Film(int w, int h);

	void write_Color(int i, int j, Color pixel_Color, int samples_per_pixel);
	int pixelcount;

	int width;
	int height;

	std::vector<unsigned char> image;
};

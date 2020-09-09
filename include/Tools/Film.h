#pragma once
#include <vector>

#include "common.h"

class Film
{
public:

	Film(int w, int h) :width(w), height(h)
	{
		pixelcount = width * height;
		image.resize(pixelcount);
	}

	void write_color(int i, int j, color pixel_color, int samples_per_pixel);
	int pixelcount;

	int width;
	int height;


	std::vector<unsigned> image;
};

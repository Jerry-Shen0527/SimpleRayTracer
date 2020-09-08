#pragma once

#include "Tools/math_tools.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Tools/stb_image_write.h>

#include "common.h"
#include "pdf/hittable_pdf.h"
#include "Scene/WorldFactory.h"
#include "Tools/math_tools.h"

void write_color(unsigned char image[], int i, int j, int width, color pixel_color, int samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	auto scale = 1.0 / samples_per_pixel;

	// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	// Divide the color by the number of samples and gamma-correct for gamma=2.0.
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	// Write the translated [0,255] value of each color component.
	image[(j) * 3 * width + i * 3 + 0] = static_cast <unsigned char>(256 * clamp(r, 0.0, 0.999));
	image[(j) * 3 * width + i * 3 + 1] = static_cast <unsigned char>(256 * clamp(g, 0.0, 0.999));
	image[(j) * 3 * width + i * 3 + 2] = static_cast <unsigned char>(256 * clamp(b, 0.0, 0.999));
}

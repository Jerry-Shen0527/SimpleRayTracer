#include "Tools/Film.h"


#include "Tools/Math/math_tools.h"

Film::Film(int w, int h): width(w), height(h)
{
	pixelcount = width * height;
	image.resize(pixelcount * 3);
}

void Film::write_Color(int i, int j, Color pixel_Color, int samples_per_pixel)
{
	auto r = pixel_Color.x();
	auto g = pixel_Color.y();
	auto b = pixel_Color.z();

	auto scale = 1.0 / samples_per_pixel;

	// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	// Divide the Color by the number of samples and gamma-correct for gamma=2.0.
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	// Write the translated [0,255] value of each Color component.
	image[j * 3 * width + i * 3 + 0] = static_cast<unsigned char>(256 * Clamp(r, 0.0, 0.999));
	image[j * 3 * width + i * 3 + 1] = static_cast<unsigned char>(256 * Clamp(g, 0.0, 0.999));
	image[j * 3 * width + i * 3 + 2] = static_cast<unsigned char>(256 * Clamp(b, 0.0, 0.999));
}

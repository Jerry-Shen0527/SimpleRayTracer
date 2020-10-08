#include <texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Tools/Files/rtw_stb_image.h>

#include "Tools/Math/math_tools.h"

Color checker_texture::value(const Vector2f& uv, const Point3f& p) const
{
	auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
	if (sines < 0)
		return odd->value(uv, p);
	else
		return even->value(uv, p);
}

Color noise_texture::value(const Vector2f& uv, const Point3f& p) const
{
	return Color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
}

image_texture::image_texture(const std::string& filename)
{
	auto components_per_pixel = bytes_per_pixel;

	data = stbi_load(
		filename.c_str(), &width, &height, &components_per_pixel, components_per_pixel);

	if (!data)
	{
		std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
		width = height = 0;
	}

	bytes_per_scanline = bytes_per_pixel * width;
}

Color image_texture::value(const Vector2f& uv, const Point3f& p) const
{
	// If we have no texture data, then return solid cyan as a debugging aid.
	if (data == nullptr)
		return Color(0, 1, 1);

	// Clamp input texture coordinates to [0,1] x [1,0]
	auto x = Clamp(uv.x(), 0.0, 1.0);
	auto y = 1.0 - Clamp(uv.y(), 0.0, 1.0); // Flip V to image coordinates

	auto i = static_cast<int>(x * width);
	auto j = static_cast<int>(y * height);

	// Clamp integer mapping, since actual coordinates should be less than 1.0
	if (i >= width) i = width - 1;
	if (j >= height) j = height - 1;

	const auto Color_scale = 1.0 / 255.0;
	auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

	return Color(Color_scale * pixel[0], Color_scale * pixel[1], Color_scale * pixel[2]);
}

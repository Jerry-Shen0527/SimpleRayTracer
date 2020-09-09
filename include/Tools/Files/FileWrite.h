#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <string>
#include <Tools/stb_image_write.h>
#include <Integrators/Film.h>

void film_to_file(std::string file_name, Film& film)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	stbi_write_png((file_name + " " + std::to_string(sys.wMonth) + "_" + std::to_string(sys.wDay) + " " + std::to_string(sys.wHour) + "_" + std::to_string(sys.wMinute) + "_" + std::to_string(sys.wSecond) + ".png").c_str(), film.width, film.height, 3, &film.image[0], 0);
}

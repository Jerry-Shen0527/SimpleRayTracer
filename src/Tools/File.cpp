#include <Tools/Files/FileWrite.h>
#include <Windows.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Tools/Files/stb_image_write.h>

void film_to_file(std::string file_name, Film& film)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	stbi_write_png((file_name + " " + std::to_string(sys.wMonth) + "_" + std::to_string(sys.wDay) + " " + std::to_string(sys.wHour) + "_" + std::to_string(sys.wMinute) + "_" + std::to_string(sys.wSecond) + ".png").c_str(), film.width, film.height, 3, &film.image[0], 0);
}
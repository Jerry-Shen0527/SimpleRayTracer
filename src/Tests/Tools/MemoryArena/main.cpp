#include <iostream>
#include <Tools/MemoryArena.h>

int main()
{
	std::cout << (4095 >> 12) << std::endl;
	std::cout << (30964 >> 12) << std::endl;
	std::cout << (46502 >> 12) << std::endl;
	std::cout << (15000 >> 12) << std::endl;
	std::cout << (7193 >> 12) << std::endl;
	std::cout << (4096 >> 12) << std::endl;
	std::cout << (8912 >> 12);
}
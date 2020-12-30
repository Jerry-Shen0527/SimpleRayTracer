#include <iostream>
#include <Tools/MemoryArena.h>

int main()
{
	for (int i = 0; i < 8; ++i)
	{
		std::cout << (i >>2)%2 << (i >> 1)%2  << (i >> 0)%2 << std::endl;
	}
}
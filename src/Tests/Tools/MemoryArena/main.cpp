#include <iostream>
#include <Tools/MemoryArena.h>

#include "Tools/Math/EFloat.h"

int main()
{
	for (int i = 0; i < 8; ++i)
	{
		Float x_v = (i >> 2) % 2 ? 1 : 0;
		Float y_v = (i >> 1) % 2 ? 1 : 0;
		Float z_v = (i >> 0) % 2 ? 1 : 0;
		std::cout << x_v << y_v << z_v << std::endl;
	}
}

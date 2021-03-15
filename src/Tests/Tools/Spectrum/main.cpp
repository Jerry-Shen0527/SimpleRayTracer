#include <Tools/Polarized.h>
#include <Tools/Math/matrix.h>

#include "Tools/Spectrum/SampledSpectrum.h"

int main()
{
	std::cout << is_polarized_v(Spectrum()) << std::endl;
	auto mat = Unpolarize<Spectrum>();

	
	
	std::cout << is_polarized_v(mat) << std::endl;

}

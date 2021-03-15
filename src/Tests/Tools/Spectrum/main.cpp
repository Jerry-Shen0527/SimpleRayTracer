#include <Tools/Polarized.h>
#include <Tools/Math/matrix.h>

int main()
{
	std::cout << is_polarized_v(Spectrum()) << std::endl;
	auto mat = UnpolarizedSpectrum(1.0);

	std::cout << is_polarized_v(mat) << std::endl;

}

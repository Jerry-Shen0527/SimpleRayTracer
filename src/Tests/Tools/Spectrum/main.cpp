#include <Tools/Polarized.h>

int main()
{
	std::cout << is_polarized_v(Spectrum(1.0)) << std::endl;
	auto mat = MuellerMatrix(Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0), Spectrum(1.0));

	std::cout << is_polarized_v(mat) << std::endl;

}
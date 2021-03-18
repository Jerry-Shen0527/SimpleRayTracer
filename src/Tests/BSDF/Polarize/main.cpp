#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif
#include "config.h"
#include "Tools/Math/matrix.h"
#include "Tools/Spectrum/SampledSpectrum.h"


int main()
{
	Spectrum spectrum(UnpolarizedSpectrum(1.0));

	std::cout << spectrum;

	std::cout << unpolarize_v(spectrum) << std::endl;

	std::cout << (spectrum*=UnpolarizedSpectrum(2.0));

	
	
#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}

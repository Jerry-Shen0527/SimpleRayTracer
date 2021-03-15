#pragma once
#include <Declarations.h>
#define IMPORT_TYPES \
	using UnpolarizedSpectrum =          Unpolarize<Spectrum>;\
	using BxDF                =          BxDF<Spectrum>;\
	using Medium=Medium<Spectrum>;
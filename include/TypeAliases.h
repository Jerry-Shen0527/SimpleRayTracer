#pragma once
#include <Declarations.h>

#define IMPORT_TYPES_L1 \
	using UnpolarizedSpectrum = Unpolarize<Spectrum>;\
	using BxDF                =          BxDF<Spectrum>;

#define IMPORT_TYPES_L2\
	IMPORT_TYPES_L1\
	using Medium=Medium<Spectrum>;\
	using BSDF=BSDF<Spectrum>;

#define IMPORT_TYPES_L3\
	IMPORT_TYPES_L2\
	using MediumInterface=MediumInterface<Spectrum>;	\
	using LambertianReflection=LambertianReflection<Spectrum>;\
	using OrenNayar=OrenNayar<Spectrum>;
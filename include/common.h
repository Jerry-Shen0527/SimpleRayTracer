#pragma once
#include <string>
#include <memory>
#include <Tools/Spectrum/SampledSpectrum.h>

using std::shared_ptr;
const std::string path = "C:/Users/Jerry/WorkSpace/SimpleRayTracer/resources/";


static const Float OneMinusEpsilon = 0x1.fffffep-1;
#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

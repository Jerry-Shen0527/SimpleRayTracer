#pragma once
#include <string>
#include <memory>
#include <Tools/Spectrum/SampledSpectrum.h>

class SampledSpectrum;
using std::shared_ptr;
const std::string path = "C:/Users/Jerry/WorkSpace/SimpleRayTracer/resources/";
typedef SampledSpectrum Spectrum;

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))
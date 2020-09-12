#pragma once
#include <string>
#include <memory>

class SampledSpectrum;
using std::shared_ptr;
const std::string path = "C:/Users/Jerry/WorkSpace/SimpleRayTracer/resources/";
typedef SampledSpectrum Spectrum;

enum class TransportMode { Radiance, Importance };

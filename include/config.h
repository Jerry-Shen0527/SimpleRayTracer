#pragma once
#include <iostream>
#include <Tools/Polarized.h>
#include <Declarations.h>
#include <TypeAliases.h>

using Float = float;

enum class TransportMode { Radiance, Importance };

template<typename T>
using MuellerMatrix = Matrix4x4<T>;

using Spectrum = MuellerMatrix<SampledSpectrum>;

class AnimatedTransform;
class RayDifferential;
class Ray;
struct CameraSample;
class Film;

using std::shared_ptr;
const std::string path = "C:/Users/Jerry/WorkSpace/SimpleRayTracer/resources/";

static const Float OneMinusEpsilon = 0x1.fffffep-1;
#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

constexpr Float Infinity = std::numeric_limits<Float>::infinity();
constexpr Float Pi = 3.1415926535897932385f;
constexpr Float InvPi = 1 / Pi;
constexpr Float Inv2Pi = 1 / Pi / 2.0;
constexpr Float PiOver4 = Pi / 4.0;
constexpr Float PiOver2 = Pi / 2.0;

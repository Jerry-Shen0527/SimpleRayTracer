#pragma once
#include <iostream>

using Float = float;

enum class TransportMode { Radiance, Importance };

class Primitive;
class MemoryArena;
class Shape;
class BSDF;
class BSSRDF;
class MediumInterface;
class Material;
class Interaction;
class SurfaceInteraction;
class Scene;
class VisibilityTester;
class SampledSpectrum;
using Spectrum = SampledSpectrum;

class AnimatedTransform;
class RayDifferential;
class Ray;
struct CameraSample;
class Medium;
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

extern bool polarized;
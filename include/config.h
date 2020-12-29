#pragma once
#include <cmath>
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

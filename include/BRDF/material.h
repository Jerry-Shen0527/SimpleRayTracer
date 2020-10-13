#ifndef MATERIAL_H
#define MATERIAL_H

#include <ray.h>
#include <pdf/scatter_record.h>

enum class TransportMode;
class MemoryArena;
class SurfaceInteraction;
class AreaLight;

class material {
public:
	virtual ~material() = default;

	virtual Color emitted(const Ray& r_in, const SurfaceInteraction& rec, const Point2f& uv, const Point3f& p) const {
		return Color(0, 0, 0);
	}

	virtual bool scatter(const Ray& r_in, const SurfaceInteraction& rec, scatter_record& srec) const {
		return false;
	}

	virtual float scattering_pdf(
		const Ray& r_in, const SurfaceInteraction& rec, const Ray& scattered
	) const {
		return 0;
	}

	virtual  AreaLight* GetAreaLight() { return  nullptr; }
};

class Material
{
public:
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
};

#endif

#pragma once
#include "Geometry/hit_record.h"

class MatteMaterial
{
public:
	MatteMaterial(const std::shared_ptr<Texture<Spectrum>>& Kd,
		const std::shared_ptr<Texture<Float>>& sigma,
		const std::shared_ptr<Texture<Float>>& bumpMap)
		: Kd(Kd), sigma(sigma), bumpMap(bumpMap) { }

	void MatteMaterial::ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const {
		//Perform bump mapping with bumpMap, if present 579
		//	Evaluate textures for MatteMaterial materialand allocate BRDF 579
	}

private:
	std::shared_ptr<Texture<Spectrum>> Kd;
	std::shared_ptr<Texture<Float>> sigma, bumpMap;
};

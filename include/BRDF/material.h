#ifndef MATERIAL_H
#define MATERIAL_H

#include <Tools/Texture.h>
#include <Tools/MemoryArena.h>

class Material {
public:
	// Material Interface
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
	virtual ~Material();
	static void Bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si);
};

class MatteMaterial : public Material {
public:
	// MatteMaterial Public Methods
	MatteMaterial(const std::shared_ptr<Texture<Spectrum>>& Kd, const std::shared_ptr<Texture<Float>>& sigma, const std::shared_ptr<Texture<Float>>& bumpMap)
		: Kd(Kd), sigma(sigma), bumpMap(bumpMap) {}
	void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena,
		TransportMode mode,
		bool allowMultipleLobes) const override;

private:
	// MatteMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> Kd;
	std::shared_ptr<Texture<Float>> sigma, bumpMap;
};

class GlassMaterial : public Material {
public:
	// GlassMaterial Public Methods
	GlassMaterial(const std::shared_ptr<Texture<Spectrum>>& Kr,
		const std::shared_ptr<Texture<Spectrum>>& Kt,
		const std::shared_ptr<Texture<Float>>& uRoughness,
		const std::shared_ptr<Texture<Float>>& vRoughness,
		const std::shared_ptr<Texture<Float>>& index,
		const std::shared_ptr<Texture<Float>>& bumpMap,
		bool remapRoughness)
		: Kr(Kr),
		Kt(Kt),
		uRoughness(uRoughness),
		vRoughness(vRoughness),
		index(index),
		bumpMap(bumpMap),
		remapRoughness(remapRoughness) {}
	void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena,
		TransportMode mode,
		bool allowMultipleLobes) const;

private:
	// GlassMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> Kr, Kt;
	std::shared_ptr<Texture<Float>> uRoughness, vRoughness;
	std::shared_ptr<Texture<Float>> index;
	std::shared_ptr<Texture<Float>> bumpMap;
	bool remapRoughness;
};

#endif

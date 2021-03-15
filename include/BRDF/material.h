#ifndef MATERIAL_H
#define MATERIAL_H

#include <Tools/Texture.h>

class Material {
public:
		// Material Interface
		virtual void ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
	virtual ~Material() {}
	static void Bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si);
};

class MatteMaterial : public Material {
public:
	IMPORT_TYPES_L3

	// MatteMaterial Public Methods
	MatteMaterial(const std::shared_ptr<Texture<Spectrum>>& Kd, const std::shared_ptr<Texture<Float>>& sigma, const std::shared_ptr<Texture<Float>>& bumpMap)
		: Kd(Kd), sigma(sigma), bumpMap(bumpMap) {}
	void ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si,
		MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;

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
	void ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si,
		MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const;

private:
	// GlassMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> Kr, Kt;
	std::shared_ptr<Texture<Float>> uRoughness, vRoughness;
	std::shared_ptr<Texture<Float>> index;
	std::shared_ptr<Texture<Float>> bumpMap;
	bool remapRoughness;
};
extern const int nSpectralSamples;
class PhysicalGlassMaterial :public Material
{
	// GlassMaterial Public Methods
public:
	PhysicalGlassMaterial(const std::shared_ptr<Texture<Spectrum>>& Kr,
		const std::shared_ptr<Texture<Spectrum>>& Kt,
		const std::shared_ptr<Texture<Float>>& uRoughness,
		const std::shared_ptr<Texture<Float>>& vRoughness,
		const std::shared_ptr<Texture<Spectrum>>& index,
		const std::shared_ptr<Texture<Float>>& bumpMap,
		bool remapRoughness, int lambdagroup = nSpectralSamples)
		: Kr(Kr),
		Kt(Kt),
		uRoughness(uRoughness),
		vRoughness(vRoughness),
		index(index),
		bumpMap(bumpMap),
		remapRoughness(remapRoughness), lambda_group(lambdagroup) {}
	void ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si,
		MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;

private:
	// GlassMaterial Private Data
	const int lambda_group;

	std::shared_ptr<Texture<Spectrum>> Kr, Kt;
	std::shared_ptr<Texture<Float>> uRoughness, vRoughness;
	std::shared_ptr<Texture<Spectrum>> index;
	std::shared_ptr<Texture<Float>> bumpMap;
	bool remapRoughness;
};

class MetalMaterial : public Material {
public:
	IMPORT_TYPES_L3
	// MetalMaterial Public Methods
	MetalMaterial(const std::shared_ptr<Texture<UnpolarizedSpectrum>>& eta,
		const std::shared_ptr<Texture<UnpolarizedSpectrum>>& k,
		const std::shared_ptr<Texture<Float>>& rough,
		const std::shared_ptr<Texture<Float>>& urough,
		const std::shared_ptr<Texture<Float>>& vrough,
		const std::shared_ptr<Texture<Float>>& bump,
		bool remapRoughness);
	void ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si,
		MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const;

private:
	// MetalMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> eta, k;
	std::shared_ptr<Texture<Float>> roughness, uRoughness, vRoughness;
	std::shared_ptr<Texture<Float>> bumpMap;
	bool remapRoughness;
};

#endif

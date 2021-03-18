#include <BRDF/BSDF.h>
#include "BRDF/Material.h"

#include <Tools/MemoryArena.h>

#include "BRDF/MicrofacetDistribution.h"
#include "BRDF/Specular.h"

void Material::Bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si)
{
	// Compute offset positions and evaluate displacement texture
	SurfaceInteraction siEval = *si;

	// Shift _siEval_ _du_ in the $u$ direction
	Float du = .5f * (std::abs(si->dudx) + std::abs(si->dudy));
	// The most common reason for du to be zero is for ray that start from
	// light sources, where no differentials are available. In this case,
	// we try to choose a small enough du so that we still get a decently
	// accurate bump value.
	if (du == 0) du = .0005f;
	siEval.p = si->p + du * si->shading.dpdu;
	siEval.uv = si->uv + Vector2f(du, 0.f);
	siEval.n = Normalize((Normal3f)Cross(si->shading.dpdu, si->shading.dpdv) +
		du * si->dndu);
	Float uDisplace = d->Evaluate(siEval);

	// Shift _siEval_ _dv_ in the $v$ direction
	Float dv = .5f * (std::abs(si->dvdx) + std::abs(si->dvdy));
	if (dv == 0) dv = .0005f;
	siEval.p = si->p + dv * si->shading.dpdv;
	siEval.uv = si->uv + Vector2f(0.f, dv);
	siEval.n = Normalize((Normal3f)Cross(si->shading.dpdu, si->shading.dpdv) +
		dv * si->dndv);
	Float vDisplace = d->Evaluate(siEval);
	Float displace = d->Evaluate(*si);

	// Compute bump-mapped differential geometry
	Vector3f dpdu = si->shading.dpdu +
		(uDisplace - displace) / du * Vector3f(si->shading.n) +
		displace * Vector3f(si->shading.dndu);
	Vector3f dpdv = si->shading.dpdv +
		(vDisplace - displace) / dv * Vector3f(si->shading.n) +
		displace * Vector3f(si->shading.dndv);
	si->SetShadingGeometry(dpdu, dpdv, si->shading.dndu, si->shading.dndv,
		false);
}

void MatteMaterial::ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
	// Perform bump mapping with _bumpMap_, if present
	if (bumpMap) Bump(bumpMap, si);

	// Evaluate textures for _MatteMaterial_ material and allocate BRDF
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	Spectrum r = Kd->Evaluate(*si).Clamp();
	Float sig = Clamp(sigma->Evaluate(*si), 0, 90);
	if (!r.IsBlack()) {
		if (sig == 0)
			si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(r));
		else
			si->bsdf->Add(ARENA_ALLOC(arena, OrenNayar)(r, sig));
	}
}

void GlassMaterial::ComputeScatteringFunctions(const Spectrum& spectrum,
	SurfaceInteraction* si,
	MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const {
	// Perform bump mapping with _bumpMap_, if present
	if (bumpMap) Bump(bumpMap, si);
	Float eta = index->Evaluate(*si);
	Float urough = uRoughness->Evaluate(*si);
	Float vrough = vRoughness->Evaluate(*si);
	UnpolarizedSpectrum R = Kr->Evaluate(*si).Clamp();
	UnpolarizedSpectrum T = Kt->Evaluate(*si).Clamp();
	// Initialize _bsdf_ for smooth or rough dielectric
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si, eta);

	if (R.IsBlack() && T.IsBlack()) return;

	bool isSpecular = urough == 0 && vrough == 0;
	if (isSpecular && allowMultipleLobes) {
		si->bsdf->Add(ARENA_ALLOC(arena, FresnelSpecular)(R, T, 1.f, eta, mode));
	}
	else {
		if (remapRoughness) {
			urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
			vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
		}
		MicrofacetDistribution* distrib =
			isSpecular ? nullptr
			: ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(urough, vrough);
		if (!R.IsBlack()) {
			Fresnel* fresnel = ARENA_ALLOC(arena, FresnelDielectric)(1.f, eta);
			if (isSpecular)
				si->bsdf->Add(
					ARENA_ALLOC(arena, SpecularReflection)(R, fresnel));
			else
				si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetReflection)(R, distrib, fresnel));
		}
		if (!T.IsBlack()) {
			if (isSpecular)
				si->bsdf->Add(ARENA_ALLOC(arena, SpecularTransmission)(
					T, 1.f, eta, mode));
			else
				si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetTransmission)(T, distrib, 1.f, eta, mode));
		}
	}
}

void PhysicalGlassMaterial::ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	// Perform bump mapping with _bumpMap_, if present
	if (bumpMap) Bump(bumpMap, si);
	Spectrum eta = index->Evaluate(*si);

	Float beta_accumulate[nSpectralSamples + 1];
	beta_accumulate[0] = 0;

	for (int i = 1; i < nSpectralSamples + 1; ++i)
	{
		beta_accumulate[i] = beta_accumulate[i - 1] + unpolarize_v(spectrum)[i - 1];
	}

	auto beta_sum = beta_accumulate[nSpectralSamples];

	auto sample = random_float(0, beta_sum);

	int index = 0;

	while (sample >= beta_accumulate[index])
	{
		index++;
	}

	index = (index - 1) / lambda_group * lambda_group;

	Spectrum R(0.f);
	Spectrum T(0.f);

	Float beta_part = beta_accumulate[index + lambda_group] - beta_accumulate[index];

	assert(beta_part != 0);

	for (int i = index; i < index + lambda_group; ++i)
	{
		unpolarize_v(R)[i] = 1.0f / beta_part * beta_sum;
		unpolarize_v(T)[i] = 1.0f / beta_part * beta_sum;
	}
	index = index + (lambda_group - 1) / 2;

	Float urough = uRoughness->Evaluate(*si);
	Float vrough = vRoughness->Evaluate(*si);
	R *= Kr->Evaluate(*si).Clamp();
	T *= Kt->Evaluate(*si).Clamp();
	// Initialize _bsdf_ for smooth or rough dielectric
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si, unpolarize_v(eta)[index]);

	if (R.IsBlack() && T.IsBlack()) return;

	bool isSpecular = urough == 0 && vrough == 0;
	if (isSpecular && allowMultipleLobes) {
		si->bsdf->Add(ARENA_ALLOC(arena, FresnelSpecular)(R, T, 1.f, unpolarize_v(eta)[index], mode));
	}
	else {
		if (remapRoughness) {
			urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
			vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
		}
		MicrofacetDistribution* distrib =
			isSpecular ? nullptr
			: ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(urough, vrough);
		if (!R.IsBlack()) {
			Fresnel* fresnel = ARENA_ALLOC(arena, FresnelDielectric)(1.f, unpolarize_v(eta)[index]);
			if (isSpecular)
				si->bsdf->Add(
					ARENA_ALLOC(arena, SpecularReflection)(R, fresnel));
			else
				si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetReflection)(R, distrib, fresnel));
		}
		if (!T.IsBlack()) {
			if (isSpecular)
				si->bsdf->Add(ARENA_ALLOC(arena, SpecularTransmission)(T, 1.f, unpolarize_v(eta)[index], mode));
			else
				si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetTransmission)(T, distrib, 1.f, unpolarize_v(eta)[index], mode));
		}
	}
}

MetalMaterial::MetalMaterial(const std::shared_ptr<Texture<UnpolarizedSpectrum>>& eta, const std::shared_ptr<Texture<UnpolarizedSpectrum>>& k,
	const std::shared_ptr<Texture<Float>>& rough, const std::shared_ptr<Texture<Float>>& urough,
	const std::shared_ptr<Texture<Float>>& vrough, const std::shared_ptr<Texture<Float>>& bump, bool remapRoughness)
	: eta(eta), k(k), roughness(rough), uRoughness(urough), vRoughness(vrough), bumpMap(bump), remapRoughness(remapRoughness) {}

void MetalMaterial::ComputeScatteringFunctions(const Spectrum& spectrum, SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
	// Perform bump mapping with _bumpMap_, if present
	if (bumpMap) Bump(bumpMap, si);
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);

	Float uRough =
		uRoughness ? uRoughness->Evaluate(*si) : roughness->Evaluate(*si);
	Float vRough =
		vRoughness ? vRoughness->Evaluate(*si) : roughness->Evaluate(*si);
	if (remapRoughness) {
		uRough = TrowbridgeReitzDistribution::RoughnessToAlpha(uRough);
		vRough = TrowbridgeReitzDistribution::RoughnessToAlpha(vRough);
	}
	Fresnel* frMf = ARENA_ALLOC(arena, FresnelConductor)(Spectrum(1.), eta->Evaluate(*si), k->Evaluate(*si));
	MicrofacetDistribution* distrib =
		ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(uRough, vRough);
	si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetReflection)(Spectrum(1.), distrib, frMf, mode));
}
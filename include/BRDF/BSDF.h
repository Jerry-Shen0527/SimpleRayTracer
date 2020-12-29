#pragma once
#include "BxDF.h"
#include "Geometry/Interaction.h"


class BSDF
{
public:
	BSDF(const SurfaceInteraction& si, Float eta = 1) : eta(eta), ns(si.shading.n), ng(si.n), ss(Normalize(si.shading.dpdu)), ts(Cross(ns, ss)) { }

	void Add(BxDF* b) {
		assert(nBxDFs < MaxBxDFs);
		bxdfs[nBxDFs++] = b;
	}

	int NumComponents(BxDFType flags = BSDF_ALL) const;

	Vector3f WorldToLocal(const Vector3f& v) const {
		return Vector3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
	}

	Vector3f LocalToWorld(const Vector3f& v) const {
		return Vector3f(ss.x() * v.x() + ts.x() * v.y() + ns.x() * v.z(),
			ss.y() * v.x() + ts.y() * v.y() + ns.y() * v.z(),
			ss.z() * v.x() + ts.z() * v.y() + ns.z() * v.z());
	}

	Spectrum f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags) const {
		Vector3f wi = WorldToLocal(wiW), wo = WorldToLocal(woW);
		bool reflect = Dot(wiW, ng) * Dot(woW, ng) > 0;
		Spectrum f(0.f);
		for (int i = 0; i < nBxDFs; ++i)
			if (bxdfs[i]->MatchesFlags(flags) &&
				(reflect && bxdfs[i]->type & BSDF_REFLECTION || !reflect && bxdfs[i]->type & BSDF_TRANSMISSION))
				f += bxdfs[i]->f(wo, wi);
		return f;
	}

	Float Pdf(const Vector3f& woWorld, const Vector3f& wiWorld, BxDFType flags = BSDF_ALL) const;
	Spectrum Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u, Float* pdf, BxDFType type = BSDF_ALL, BxDFType* sampledType = nullptr) const;

	Spectrum rho(int nSamples, const Point2f* samples1, const Point2f* samples2, BxDFType flags = BSDF_ALL) const;
	Spectrum rho(const Vector3f& wo, int nSamples, const Point2f* samples, BxDFType flags = BSDF_ALL) const;

public:
	const Float eta;
private:
	const Normal3f ns, ng;
	const Vector3f ss, ts;

	int nBxDFs = 0;
	static constexpr int MaxBxDFs = 8;
	BxDF* bxdfs[MaxBxDFs];
};

inline int BSDF::NumComponents(BxDFType flags) const
{
	int num = 0;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(flags)) ++num;
	return num;
}

inline Float BSDF::Pdf(const Vector3f& woWorld, const Vector3f& wiWorld, BxDFType flags) const
{
	if (nBxDFs == 0.f) return 0.f;
	Vector3f wo = WorldToLocal(wo), wi = WorldToLocal(wi);
	if (wo.z() == 0) return 0.;
	Float pdf = 0.f;
	int matchingComps = 0;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(flags)) {
			++matchingComps;
			pdf += bxdfs[i]->Pdf(wo, wi);
		}
	Float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
	return v;
}

inline Spectrum BSDF::Sample_f(const Vector3f& woWorld, Vector3f* wiWorld, const Point2f& u, Float* pdf, BxDFType type,
	BxDFType* sampledType) const
{
	// Choose which _BxDF_ to sample
	int matchingComps = NumComponents(type);
	if (matchingComps == 0) {
		*pdf = 0;
		if (sampledType) *sampledType = BxDFType(0);
		return Spectrum(0);
	}
	int comp =
		std::min((int)std::floor(u[0] * matchingComps), matchingComps - 1);

	// Get _BxDF_ pointer for chosen component
	BxDF* bxdf = nullptr;
	int count = comp;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(type) && count-- == 0) {
			bxdf = bxdfs[i];
			break;
		}

	// Remap _BxDF_ sample _u_ to $[0,1)^2$
	Point2f uRemapped(std::min(u[0] * matchingComps - comp, OneMinusEpsilon),
		u[1]);

	// Sample chosen _BxDF_
	Vector3f wi, wo = WorldToLocal(woWorld);
	if (wo.z() == 0) return 0.;
	*pdf = 0;
	if (sampledType) *sampledType = bxdf->type;
	Spectrum f = bxdf->Sample_f(wo, &wi, uRemapped, pdf, sampledType);
	if (*pdf == 0) {
		if (sampledType) *sampledType = BxDFType(0);
		return 0;
	}
	*wiWorld = LocalToWorld(wi);

	// Compute overall PDF with all matching _BxDF_s
	if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
		for (int i = 0; i < nBxDFs; ++i)
			if (bxdfs[i] != bxdf && bxdfs[i]->MatchesFlags(type))
				*pdf += bxdfs[i]->Pdf(wo, wi);
	if (matchingComps > 1) *pdf /= matchingComps;

	// Compute value of BSDF for sampled direction
	if (!(bxdf->type & BSDF_SPECULAR)) {
		bool reflect = Dot(*wiWorld, ng) * Dot(woWorld, ng) > 0;
		f = 0.;
		for (int i = 0; i < nBxDFs; ++i)
			if (bxdfs[i]->MatchesFlags(type) &&
				((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
					(!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
				f += bxdfs[i]->f(wo, wi);
	}
	return f;
}

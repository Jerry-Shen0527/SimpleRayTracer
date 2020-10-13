#pragma once
#include "BxDF.h"
#include "Geometry/hit_record.h"
#include "Tools/Math/Vector3.h"

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
		return Vector3f(ss.x * v.x + ts.x * v.y + ns.x * v.z,
			ss.y * v.x + ts.y * v.y + ns.y * v.z,
			ss.z * v.x + ts.z * v.y + ns.z * v.z);
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

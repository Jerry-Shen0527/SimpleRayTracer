#include <BRDF/Fresnel.h>
#include <BRDF/Specular.h>
#include <Tools/Mueller.h>

Spectrum SpecularReflection::Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType)  const
{
	*wi = Vector3f(-wo.x(), -wo.y(), wo.z());

	*pdf = 1;
	return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}

bool Refract(const Vector3f& wi, const Normal3f& n, Float eta, Vector3f* wt)
{
	//Compute cos ¦Èt using Snell¡¯s law 531

	Float cosThetaI = Dot(n, wi);
	Float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
	Float sin2ThetaT = eta * eta * sin2ThetaI;

	//Handle total internal reflection for transmission 531
	if (sin2ThetaT >= 1) return false;

	Float cosThetaT = std::sqrt(1 - sin2ThetaT);
	*wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
	return true;
}

Spectrum SpecularTransmission::Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType)  const
{
	//Figure out which ¦Ç is incidentand which is transmitted 529
	bool entering = CosTheta(wo) > 0;
	Float etaI = entering ? etaA : etaB;
	Float etaT = entering ? etaB : etaA;
	//Compute ray direction for specular transmission 529
	if (!Refract(wo, Faceforward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
		return 0;
	*pdf = 1;
	Spectrum ft = T * (Spectrum(1.) - fresnel.Evaluate(CosTheta(*wi)));
	//Account for non - symmetry with transmission to different medium 961
	return ft / AbsCosTheta(*wi);
}

Spectrum FresnelSpecular::f(const Vector3f& wo, const Vector3f& wi) const
{
	return Spectrum(0.f);
}

Float FresnelSpecular::Pdf(const Vector3f& wo, const Vector3f& wi) const
{
	return 0;
}

Spectrum FresnelSpecular::Sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample, float* pdf, BxDFType* sampledType) const
{
	bool has_reflection = HasFlags(BSDF_REFLECTION);
	bool has_transmission = HasFlags(BSDF_TRANSMISSION);

	Float F = FrDielectric(CosTheta(wo), etaA, etaB);
	Float eta_ti = CosTheta(wo) > 0.f ? etaA / etaB : etaB / etaA;

	UnpolarizedSpectrum reflectance = 1.f, transmittance = 1.f;

	Spectrum weight;

	Float m_eta = etaB / etaA;

	auto selected_r = sample[0] <= F;

	if (selected_r)
	{
		*pdf = F;
		*wi = Vector3f(-wo.x(), -wo.y(), wo.z());
	}
	else
	{
		*pdf = 1 - F;
		bool entering = CosTheta(wo) > 0;
		Float etaI = entering ? etaA : etaB;
		Float etaT = entering ? etaB : etaA;
		if (!Refract(wo, Faceforward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
			return 0;
	}

	if constexpr (is_polarized_t<Spectrum>()) {
		Vector3f wi_hat = mode == TransportMode::Radiance ? wo : *wi;
		Vector3f wo_hat = mode == TransportMode::Radiance ? *wi : wo;

		/* BSDF weights are Mueller matrices now. */
		Float cos_theta_i_hat = CosTheta(wi_hat);
		Spectrum R_mat = specular_reflection(UnpolarizedSpectrum(cos_theta_i_hat), UnpolarizedSpectrum(m_eta)),
			T_mat = specular_transmission<UnpolarizedSpectrum, Float>(cos_theta_i_hat, m_eta);

		if (has_reflection && has_transmission) {
			weight = (selected_r ? R_mat : T_mat) * (1 / *pdf);
		}
		else if (has_reflection || has_transmission) {
			weight = has_reflection ? R_mat : T_mat;
			*pdf = 1.f;
		}
		/* Apply frame reflection, according to "Stellar Polarimetry" by
		   David Clarke, Appendix A.2 (A26) */
		weight = reverse(weight);

		/* The Stokes reference frame vector of this matrix lies in the plane
		   of reflection / refraction. */
		Vector3f n(0, 0, 1);
		Vector3f s_axis_in = Normalize(Cross(n, -wi_hat));
		Vector3f p_axis_in = Normalize(Cross(-wi_hat, s_axis_in));
		Vector3f s_axis_out = Normalize(Cross(n, wo_hat));
		Vector3f p_axis_out = Normalize(Cross(wo_hat, s_axis_out));

		/* Rotate in/out reference vector of weight s.t. it aligns with the
		   implicit Stokes bases of -wi_hat & wo_hat. */
		weight = rotate_mueller_basis(weight, -wi_hat, p_axis_in, stokes_basis(-wi_hat), wo_hat, p_axis_out, stokes_basis(wo_hat));

		if (selected_r)
		{
			weight = weight * absorber(reflectance);
			if (sampledType)
				*sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
			auto ret = Spectrum(R);
			ret = ret * weight;
			ret = ret / AbsCosTheta(*wi) * F;
			return ret;
		}
		else
		{
			weight = weight * absorber(transmittance);
			bool entering = CosTheta(wo) > 0;
			Float etaI = entering ? etaA : etaB;
			Float etaT = entering ? etaB : etaA;

			//Compute ray direction for specular transmission 529

			Spectrum ft = Spectrum(T * (1 - F)) * weight;
			//Account for non - symmetry with transmission to different medium 961
			if (mode == TransportMode::Radiance)
				ft = ft * (etaI * etaI) / (etaT * etaT);
			if (sampledType)
				*sampledType = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
			ft = ft / AbsCosTheta(*wi);
			return ft;
		}
	}
	else
	{
		if (selected_r) {
			//Compute specular reflection for FresnelSpecular 817
			if (sampledType)
				*sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
			return  R * F / AbsCosTheta(*wi);
		}
		else {
			//Compute specular transmission for FresnelSpecular 817

			Spectrum ft = T * (1 - F);
			//Account for non - symmetry with transmission to different medium 961
			bool entering = CosTheta(wo) > 0;
			Float etaI = entering ? etaA : etaB;
			Float etaT = entering ? etaB : etaA;
			if (mode == TransportMode::Radiance)
				ft = ft * (etaI * etaI) / (etaT * etaT);
			if (sampledType)
				*sampledType = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
			return ft / AbsCosTheta(*wi);
		}
	}
}
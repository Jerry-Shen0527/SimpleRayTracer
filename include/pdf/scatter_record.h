#pragma once
#include <ray.h>

#include "common.h"

class pdf;
struct scatter_record {
	Ray specular_ray;
	bool is_specular;
	Color attenuation;
	std::shared_ptr<pdf> pdf_ptr;
	Spectrum sp_attenuation;
	virtual  void update() {}
};

struct spectrum_scatter_record :public scatter_record
{
	void update() override;
};

inline void spectrum_scatter_record::update()
{
	sp_attenuation = Spectrum::FromRGB(attenuation, SpectrumType::Reflectance);
}

#pragma once
#include <ray.h>

class pdf;
struct scatter_record {
	ray specular_ray;
	bool is_specular;
	color attenuation;
	std::shared_ptr<pdf> pdf_ptr;
};
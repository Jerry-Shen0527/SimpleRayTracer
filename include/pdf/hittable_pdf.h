#pragma once
#include "pdf.h"
#include "Geometry/hittable.h"


//class hittable_pdf : public pdf {
//public:
//	hittable_pdf(shared_ptr<hittable> p, const Point3f& origin) : ptr(p), o(origin) {}
//
//	virtual float value(const Vector3f& direction) const override {
//		return ptr->pdf_value(o, direction);
//	}
//
//	virtual Vector3f generate() const override {
//		return ptr->random(o);
//	}
//
//public:
//	Point3f o;
//	shared_ptr<hittable> ptr;
//};

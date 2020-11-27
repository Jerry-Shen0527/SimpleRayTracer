#ifndef CAMERA_H
#define CAMERA_H
#include <config.h>
#include <Geometry/AnimatedTransform.h>

class Camera {
public:
	// Camera Interface
	Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
		Float shutterClose, Film* film, const Medium* medium);
	virtual ~Camera();
	virtual Float GenerateRay(const CameraSample& sample, Ray* ray) const = 0;
	virtual Float GenerateRayDifferential(const CameraSample& sample, RayDifferential* rd) const;
	virtual Spectrum We(const Ray& ray, Point2f* pRaster2 = nullptr) const;
	virtual void Pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const;
	virtual Spectrum Sample_Wi(const Interaction& ref, const Point2f& u,
		Vector3f* wi, Float* pdf, Point2f* pRaster,
		VisibilityTester* vis) const;

	// Camera Public Data
	AnimatedTransform CameraToWorld;
	const Float shutterOpen, shutterClose;
	Film* film;
	const Medium* medium;
};

#endif

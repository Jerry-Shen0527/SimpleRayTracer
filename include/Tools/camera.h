#ifndef CAMERA_H
#define CAMERA_H
#include <config.h>
#include <Geometry/AnimatedTransform.h>

#include "Film.h"

class Camera {
public:
	// Camera Interface
	IMPORT_TYPES_L2
	Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
		Float shutterClose, Film* film, const Medium* medium);
	virtual ~Camera() {};
	virtual Float GenerateRay(const CameraSample& sample, Ray* ray) const = 0;
	virtual Float GenerateRayDifferential(const CameraSample& sample, RayDifferential* rd) const;
	virtual Spectrum We(const Ray& ray, Point2f* pRaster2 = nullptr) const
	{
		return Spectrum(0.);
	}
	//virtual void Pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const;
	//virtual Spectrum Sample_Wi(const Interaction& ref, const Point2f& u,
	//	Vector3f* wi, Float* pdf, Point2f* pRaster,
	//	VisibilityTester* vis) const;

	// Camera Public Data
	AnimatedTransform CameraToWorld;
	const Float shutterOpen, shutterClose;
	Film* film;
	const Medium* medium;
};

class ProjectiveCamera : public Camera {
public:
	// ProjectiveCamera Public Methods
	ProjectiveCamera(const AnimatedTransform& CameraToWorld,
		const Transform& CameraToScreen,
		const Bounds2f& screenWindow, Float shutterOpen,
		Float shutterClose, Float lensr, Float focald, Film* film,
		const Medium* medium)
		: Camera(CameraToWorld, shutterOpen, shutterClose, film, medium),
		CameraToScreen(CameraToScreen) {
		// Initialize depth of field parameters
		lensRadius = lensr;
		focalDistance = focald;

		// Compute projective camera transformations

		// Compute projective camera screen transformations
		ScreenToRaster =
			Scale(film->fullResolution.x(), film->fullResolution.y(), 1) *
			Scale(1 / (screenWindow.pMax.x() - screenWindow.pMin.x()),
				1 / (screenWindow.pMin.y() - screenWindow.pMax.y()), 1) *
			Translate(Vector3f(-screenWindow.pMin.x(), -screenWindow.pMax.y(), 0));
		RasterToScreen = Inverse(ScreenToRaster);
		RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
	}

protected:
	// ProjectiveCamera Protected Data
	Transform CameraToScreen, RasterToCamera;
	Transform ScreenToRaster, RasterToScreen;
	Float lensRadius, focalDistance;
};

class PerspectiveCamera : public ProjectiveCamera {
public:

	// PerspectiveCamera Public Methods
	PerspectiveCamera(const AnimatedTransform& CameraToWorld,
		const Bounds2f& screenWindow, Float shutterOpen,
		Float shutterClose, Float lensRadius, Float focalDistance,
		Float fov, Film* film, const Medium* medium);
	Float GenerateRay(const CameraSample& sample, Ray*) const override;
	Float GenerateRayDifferential(const CameraSample& sample,
		RayDifferential* ray) const override;
	Spectrum We(const Ray& ray, Point2f* pRaster2 = nullptr) const;
	void Pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const;
	Spectrum Sample_Wi(const Interaction& ref, const Point2f& sample,
		Vector3f* wi, Float* pdf, Point2f* pRaster,
		VisibilityTester* vis) const;

private:
	// PerspectiveCamera Private Data
	Vector3f dxCamera, dyCamera;
	Float A;
};

#endif

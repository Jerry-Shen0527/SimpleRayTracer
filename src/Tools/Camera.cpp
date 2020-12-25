#include <Tools/camera.h>

#include "Tools/Sampler.h"

PerspectiveCamera::PerspectiveCamera(const AnimatedTransform& CameraToWorld, const Bounds2f& screenWindow, Float shutterOpen, Float shutterClose, Float lensRadius, Float focalDistance, Float fov, Film* film, const Medium* medium)
	: ProjectiveCamera(CameraToWorld, Perspective(fov, 1e-2f, 1000.f),
		screenWindow, shutterOpen, shutterClose, lensRadius,
		focalDistance, film, medium) {
	// Compute differential changes in origin for perspective camera rays
	dxCamera =
		(RasterToCamera(Point3f(1, 0, 0)) - RasterToCamera(Point3f(0, 0, 0)));
	dyCamera =
		(RasterToCamera(Point3f(0, 1, 0)) - RasterToCamera(Point3f(0, 0, 0)));

	// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
	Point2i res = film->fullResolution;
	Point3f pMin = RasterToCamera(Point3f(0, 0, 0));
	Point3f pMax = RasterToCamera(Point3f(res.x(), res.y(), 0));
	pMin /= pMin.z();
	pMax /= pMax.z();
	A = std::abs((pMax.x() - pMin.x()) * (pMax.y() - pMin.y()));
}

Float Camera::GenerateRayDifferential(const CameraSample& sample, RayDifferential* rd) const
{
	Float wt = GenerateRay(sample, rd);
	if (wt == 0) return 0;

	// Find camera ray after shifting a fraction of a pixel in the $x$ direction
	Float wtx;
	for (Float eps : { .05, -.05 }) {
		CameraSample sshift = sample;
		sshift.pFilm.x() += eps;
		Ray rx;
		wtx = GenerateRay(sshift, &rx);
		rd->rxOrigin = rd->o + (rx.o - rd->o) / eps;
		rd->rxDirection = rd->d + (rx.d - rd->d) / eps;
		if (wtx != 0)
			break;
	}
	if (wtx == 0)
		return 0;

	// Find camera ray after shifting a fraction of a pixel in the $y$ direction
	Float wty;
	for (Float eps : { .05, -.05 }) {
		CameraSample sshift = sample;
		sshift.pFilm.y() += eps;
		Ray ry;
		wty = GenerateRay(sshift, &ry);
		rd->ryOrigin = rd->o + (ry.o - rd->o) / eps;
		rd->ryDirection = rd->d + (ry.d - rd->d) / eps;
		if (wty != 0)
			break;
	}
	if (wty == 0)
		return 0;

	rd->hasDifferentials = true;
	return wt;
}

Float PerspectiveCamera::GenerateRay(const CameraSample& sample, Ray* ray) const
{
	Point3f pFilm = Point3f(sample.pFilm.x(), sample.pFilm.y(), 0);
	Point3f pCamera = RasterToCamera(pFilm);
	*ray = Ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)));
	// Modify ray for depth of field
	if (lensRadius > 0) {
		// Sample point on lens
		Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

		// Compute point on plane of focus
		Float ft = focalDistance / ray->d.z();
		Point3f pFocus = (*ray)(ft);

		// Update ray for effect of lens
		ray->o = Point3f(pLens.x(), pLens.y(), 0);
		ray->d = Normalize(pFocus - ray->o);
	}
	ray->time = Lerp(sample.time, shutterOpen, shutterClose);
	ray->medium = medium;
	*ray = CameraToWorld(*ray);
	return 1;
}
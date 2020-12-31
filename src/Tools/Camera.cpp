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

Camera::Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen, Float shutterClose, Film* film, const Medium* medium)
	: CameraToWorld(CameraToWorld), shutterOpen(shutterOpen), shutterClose(shutterClose), film(film), medium(medium)
{
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

Float PerspectiveCamera::GenerateRayDifferential(const CameraSample& sample, RayDifferential* ray) const
{
	// Compute raster and camera sample positions
	Point3f pFilm = Point3f(sample.pFilm.x(), sample.pFilm.y(), 0);
	Point3f pCamera = RasterToCamera(pFilm);
	Vector3f dir = Normalize(Vector3f(pCamera.x(), pCamera.y(), pCamera.z()));
	*ray = RayDifferential(Point3f(0, 0, 0), dir);
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

	// Compute offset rays for _PerspectiveCamera_ ray differentials
	if (lensRadius > 0) {
		// Compute _PerspectiveCamera_ ray differentials accounting for lens

		// Sample point on lens
		Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);
		Vector3f dx = Normalize(Vector3f(pCamera + dxCamera));
		Float ft = focalDistance / dx.z();
		Point3f pFocus = Point3f(0, 0, 0) + (ft * dx);
		ray->rxOrigin = Point3f(pLens.x(), pLens.y(), 0);
		ray->rxDirection = Normalize(pFocus - ray->rxOrigin);

		Vector3f dy = Normalize(Vector3f(pCamera + dyCamera));
		ft = focalDistance / dy.z();
		pFocus = Point3f(0, 0, 0) + (ft * dy);
		ray->ryOrigin = Point3f(pLens.x(), pLens.y(), 0);
		ray->ryDirection = Normalize(pFocus - ray->ryOrigin);
	}
	else {
		ray->rxOrigin = ray->ryOrigin = ray->o;
		ray->rxDirection = Normalize(Vector3f(pCamera) + dxCamera);
		ray->ryDirection = Normalize(Vector3f(pCamera) + dyCamera);
	}
	ray->time = Lerp(sample.time, shutterOpen, shutterClose);
	ray->medium = medium;
	*ray = CameraToWorld(*ray);
	ray->hasDifferentials = true;
	return 1;
}

Spectrum PerspectiveCamera::We(const Ray& ray, Point2f* pRaster2) const
{
	//Interpolate camera matrix and check if ¦Ø is forward - facing 950
	Transform c2w;
	CameraToWorld.Interpolate(ray.time, &c2w);
	Float cosTheta = Dot(ray.d, c2w(Vector3f(0, 0, 1)));
	if (cosTheta <= 0)
		return 0;
	//	Map ray(p, ¦Ø) onto the raster grid 950
	Point3f pFocus = ray((lensRadius > 0 ? focalDistance : 1) / cosTheta);
	Point3f pRaster = Inverse(RasterToCamera)(Inverse(c2w)(pFocus));
	//	Return raster position if requested 950
	if (pRaster2) *pRaster2 = Point2f(pRaster.x(), pRaster.y());
	//	Return zero importance for out of bounds points 951
	Bounds2i sampleBounds = film->GetSampleBounds();
	if (pRaster.x() < sampleBounds.pMin.x() || pRaster.x() >= sampleBounds.pMax.x() ||
		pRaster.y() < sampleBounds.pMin.y() || pRaster.y() >= sampleBounds.pMax.y())
		return 0;
	//	Compute lens area of perspective camera 953
	Float lensArea = lensRadius != 0 ? (Pi * lensRadius * lensRadius) : 1;
	//	Return importance for point on image plane 953
	Float cos2Theta = cosTheta * cosTheta;
	return Spectrum(1 / (A * lensArea * cos2Theta * cos2Theta));
}

void PerspectiveCamera::Pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const
{
	
}

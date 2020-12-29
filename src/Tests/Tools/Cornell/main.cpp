#include <Geometry/TriangleMesh.h>
#include <Geometry/bvh.h>

#include "Tools/camera.h"
#include "Tools/Parallel.h"
#include "Tools/Sampler.h"

int indices[] = {
	0,2,3,
	0,1,3,
	2,3,6,
	3,6,7,
	4,5,7,
	4,6,7,
	1,3,5,
	3,5,7,
	0,1,4,4,5,1 };
Point3f points[] = {
	Point3f(0, 0, 0),Point3f(0, 0, 555),
	Point3f(0, 555, 0),Point3f(0, 555, 555),
	Point3f(555, 0, 0),Point3f(555, 0, 555),
	Point3f(555, 555, 0),Point3f(555, 555, 555),
};
Float rgbcolor[] = { 0,256,256 };

int main()
{
	SampledSpectrum::Init();
	auto t = Translate(Vector3f(-278, -278, 0));
	auto wto = Inverse(t);
	auto mesh = CreateTriangleMesh(&wto, &t, true, 10, indices, 8, points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	for (auto triangle : mesh)
	{
		primitives.emplace_back(make_shared<GeometricPrimitive>(triangle, nullptr, nullptr));
	}

	BVHAccel bvh(primitives, 3, SplitMethod::Middle);

	SurfaceInteraction isect;

	Film film(Point2i(600, 600), Bounds2f(Point2f(0, 0), Point2f(1, 1)), std::make_unique<BoxFilter>(Vector2f(1.0, 1.0)), 1, "test.png", 1);
	Transform trans = Translate(Vector3f(0, 0, -3000));
	AnimatedTransform transform(&trans, 0, &trans, 0);
	PerspectiveCamera camera(transform, Bounds2f(Point2f(0, 0), Point2f(1, 1)), 0, 0.0, 0, 10.0, 40.0, &film, nullptr);

	StratifiedSampler sampler(1, 1, true, 2);

	Bounds2i sampleBounds = camera.film->GetSampleBounds();
	Vector2i sampleExtent = sampleBounds.Diagonal();
	const int tileSize = 16;
	//(+tilesize-1)/tileSize: max groups
	Point2i nTiles((sampleExtent.x() + tileSize - 1) / tileSize, (sampleExtent.y() + tileSize - 1) / tileSize);
	ParallelFor2D(
		[&](Point2i tile) {
			MemoryArena arena;

			int seed = tile.y() * nTiles.x() + tile.x();
			std::unique_ptr<Sampler> tileSampler = sampler.Clone(seed);

			int x0 = sampleBounds.pMin.x() + tile.x() * tileSize;
			int x1 = std::min(x0 + tileSize, sampleBounds.pMax.x());
			int y0 = sampleBounds.pMin.y() + tile.y() * tileSize;
			int y1 = std::min(y0 + tileSize, sampleBounds.pMax.y());
			Bounds2i tileBounds(Point2i(x0, y0), Point2i(x1, y1));

			std::unique_ptr<FilmTile> filmTile = camera.film->GetFilmTile(tileBounds);

			for (Point2i pixel : tileBounds) {
				tileSampler->StartPixel(pixel);
				do {
					//Initialize CameraSample for current sample 30
					CameraSample cameraSample = tileSampler->GetCameraSample(pixel);
					//	Generate camera ray for current sample 31
					RayDifferential ray;
					Float rayWeight = camera.GenerateRayDifferential(cameraSample, &ray);
					ray.ScaleDifferentials(1 / std::sqrt(tileSampler->samplesPerPixel));
					//	Evaluate radiance along camera ray 31
					Spectrum L(0.f);
					if (rayWeight > 0)
						if (bvh.Intersect(ray, &isect))
							L = Spectrum::FromRGB(rgbcolor);

					//	Add camera ray¡¯s contribution to image 32
					filmTile->AddSample(cameraSample.pFilm, L, rayWeight);

					//	Free MemoryArena memory from computing image sample value 32
					arena.Reset();
				} while (tileSampler->StartNextSample());
			}
			camera.film->MergeFilmTile(std::move(filmTile));
		}, nTiles);

	camera.film->WriteImage(1, true);
}
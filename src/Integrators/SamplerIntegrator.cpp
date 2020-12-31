#include <Integrators/SamplerIntegrator.h>
#include <Tools/Film.h>
#include <Tools/MemoryArena.h>
#include <Tools/Parallel.h>
#include <Tools/Sampler.h>

void SamplerIntegrator::Render(const Scene& scene)
{
	ParallelInit();
	Preprocess(scene, *sampler);
	Bounds2i sampleBounds = camera->film->GetSampleBounds();
	Vector2i sampleExtent = sampleBounds.Diagonal();
	const int tileSize = 16;
	//(+tilesize-1)/tileSize: max groups
	Point2i nTiles((sampleExtent.x() + tileSize - 1) / tileSize, (sampleExtent.y() + tileSize - 1) / tileSize);
	
	ParallelFor2D(
		[&](Point2i tile) {
			MemoryArena arena;

			int seed = tile.y() * nTiles.x() + tile.x();
			std::unique_ptr<Sampler> tileSampler = sampler->Clone(seed);

			int x0 = sampleBounds.pMin.x() + tile.x() * tileSize;
			int x1 = std::min(x0 + tileSize, sampleBounds.pMax.x());
			int y0 = sampleBounds.pMin.y() + tile.y() * tileSize;
			int y1 = std::min(y0 + tileSize, sampleBounds.pMax.y());
			Bounds2i tileBounds(Point2i(x0, y0), Point2i(x1, y1));

			std::unique_ptr<FilmTile> filmTile = camera->film->GetFilmTile(tileBounds);

			for (Point2i pixel : tileBounds) {
				tileSampler->StartPixel(pixel);
				do {
					//Initialize CameraSample for current sample 30
					CameraSample cameraSample = tileSampler->GetCameraSample(pixel);
					//	Generate camera ray for current sample 31
					RayDifferential ray;
					Float rayWeight = camera->GenerateRayDifferential(cameraSample, &ray);
					ray.ScaleDifferentials(1 / std::sqrt(tileSampler->samplesPerPixel));
					//	Evaluate radiance along camera ray 31
					Spectrum L(0.f);
					if (rayWeight > 0)
						L = Li(ray, scene, *tileSampler, arena);

					//	Add camera ray¡¯s contribution to image 32
					filmTile->AddSample(cameraSample.pFilm, L, rayWeight);

					//	Free MemoryArena memory from computing image sample value 32
					arena.Reset();
				} while (tileSampler->StartNextSample());
			}
			camera->film->MergeFilmTile(std::move(filmTile));
		}, nTiles);
	ParallelCleanup();
	camera->film->WriteImage();
}


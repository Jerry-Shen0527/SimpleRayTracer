#include <Integrators/SPPM.h>

#include "BRDF/BSDF.h"
#include "Integrators/DirectLightIntegrator.h"
#include "Tools/MemoryArena.h"
#include "Tools/Parallel.h"
#include "Tools/Sampler.h"
#include "Tools/Files/stb_image_write.h"

struct SPPMPixel {
	//SPPMPixel Public Methods
		//SPPMPixel Public Data 974
	SPPMPixel() : M(0) {}

	Float radius = 0;
	Spectrum Ld;

	struct VisiblePoint {
		// VisiblePoint Public Methods
		VisiblePoint() {}
		VisiblePoint(const Point3f& p, const Vector3f& wo, const BSDF* bsdf,
			const Spectrum& beta)
			: p(p), wo(wo), bsdf(bsdf), beta(beta) {}
		Point3f p;
		Vector3f wo;
		const BSDF* bsdf = nullptr;
		Spectrum beta;
	} vp;

	AtomicFloat Phi[nSpectralSamples];

	//number of photons collected
	std::atomic<int> M;

	Float N = 0;
	Spectrum tau;
};

struct SPPMPixelListNode {
	SPPMPixel* pixel;
	SPPMPixelListNode* next;
};

inline std::unique_ptr<Distribution1D> ComputeLightPowerDistribution(
	const Scene& scene) {
	std::vector<Float> lightPower;
	for (const auto& light : scene.lights)
		lightPower.push_back(light->Power().y());
	return std::unique_ptr<Distribution1D>(
		new Distribution1D(&lightPower[0], lightPower.size()));
}

inline unsigned int hash(const Point3i& p, int hashSize) {
	return (unsigned int)((p.x() * 73856093) ^ (p.y() * 19349663) ^ (p.z() * 83492791)) % hashSize;
}

void SPPMIntegrator::Render(const Scene& scene, bool benchmark)
{
	//Initialize pixelBoundsand pixels array for SPPM 973
	Bounds2i pixelBounds = camera->film->croppedPixelBounds;
	int nPixels = pixelBounds.Volume();
	std::unique_ptr<SPPMPixel[]> pixels(new SPPMPixel[nPixels]);
	for (int i = 0; i < nPixels; ++i)
		pixels[i].radius = initialSearchRadius;
	const Float invSqrtSPP = 1.f / std::sqrt(nIterations);

	//Compute lightDistr for sampling lights proportional to power 974
	std::unique_ptr<Distribution1D> lightDistr = ComputeLightPowerDistribution(scene);

	//Perform nIterations of SPPM integration 975
	HaltonSampler sampler(nIterations, pixelBounds);
	//Compute number of tiles to use for SPPM camera pass 976
	Vector2i pixelExtent = pixelBounds.Diagonal();
	const int tileSize = 16;
	Point2i nTiles((pixelExtent.x() + tileSize - 1) / tileSize, (pixelExtent.y() + tileSize - 1) / tileSize);
	ParallelInit();
	std::vector<MemoryArena> perThreadArenas(NumSystemCores());

	for (int iter = 0; iter < nIterations; ++iter) {
		//Generate SPPM visible points 976
		ParallelFor2D(
			[&](Point2i tile) {
				MemoryArena& arena = perThreadArenas[ThreadIndex];

				//Follow camera paths for tile in image for SPPM 976
				int tileIndex = tile.y() * nTiles.x() + tile.x();
				std::unique_ptr<Sampler> tileSampler = sampler.Clone(tileIndex);

				//Compute tileBounds for SPPM tile
				int x0 = pixelBounds.pMin.x() + tile.x() * tileSize;
				int x1 = std::min(x0 + tileSize, pixelBounds.pMax.x());
				int y0 = pixelBounds.pMin.y() + tile.y() * tileSize;
				int y1 = std::min(y0 + tileSize, pixelBounds.pMax.y());
				Bounds2i tileBounds(Point2i(x0, y0), Point2i(x1, y1));

				for (Point2i pPixel : tileBounds) {
					//Prepare tileSampler for pPixel 977
					tileSampler->StartPixel(pPixel);
					tileSampler->SetSampleNumber(iter);

					//	Generate camera ray for pixel for SPPM 977
					CameraSample cameraSample = tileSampler->GetCameraSample(pPixel);
					RayDifferential ray;
					Spectrum beta = camera->GenerateRayDifferential(cameraSample, &ray);
					if (beta.IsBlack())
						continue;
					ray.ScaleDifferentials(invSqrtSPP);

					//	Follow camera ray path until a visible point is created 977
					Point2i pPixelO = Point2i(pPixel - pixelBounds.pMin);
					int pixelOffset = pPixelO.x() + pPixelO.y() * (pixelBounds.pMax.x() - pixelBounds.pMin.x());
					SPPMPixel& pixel = pixels[pixelOffset];
					bool specularBounce = false;

					for (int depth = 0; depth < maxDepth; ++depth) {
						SurfaceInteraction isect;
						if (!scene.Intersect(ray, &isect)) {
							//Accumulate light contributions for ray with no intersection 977
							for (const auto& light : scene.lights)
								pixel.Ld += beta * light->Le(ray);
							break;
						}
						//Process SPPM camera ray intersection 978

						//Compute BSDF at SPPM camera ray intersection 978
						isect.ComputeScatteringFunctions(ray, arena, true);
						if (!isect.bsdf) {
							ray = isect.SpawnRay(ray.d);
							--depth;
							continue;
						}
						const BSDF& bsdf = *isect.bsdf;

						//	Accumulate direct illumination at SPPM camera ray intersection 978
						Vector3f wo = -ray.d;
						if (depth == 0 || specularBounce)
							pixel.Ld += beta * isect.Le(wo);
						pixel.Ld += beta *
							UniformSampleOneLight(isect, scene, arena, *tileSampler);

						//	Possibly create visible point and end camera path 979
						bool isDiffuse =
							bsdf.NumComponents(BxDFType(BSDF_DIFFUSE | BSDF_REFLECTION | BSDF_TRANSMISSION)) > 0;
						bool isGlossy =
							bsdf.NumComponents(BxDFType(BSDF_GLOSSY | BSDF_REFLECTION | BSDF_TRANSMISSION)) > 0;
						if (isDiffuse || (isGlossy && depth == maxDepth - 1)) {
							pixel.vp = { isect.p, wo, &bsdf, beta };
							break;
						}

						//	Spawn ray from SPPM camera path vertex
						if (depth < maxDepth - 1) {
							Float pdf;
							Vector3f wi;
							BxDFType type;
							Spectrum f = bsdf.Sample_f(wo, &wi, tileSampler->Get2D(), &pdf, BSDF_ALL, &type);

							if (pdf == 0. || f.IsBlack()) break;
							specularBounce = (type & BSDF_SPECULAR) != 0;
							beta *= f * AbsDot(wi, isect.shading.n) / pdf;

							if (beta.y() < 0.25) {
								Float continueProb =
									std::min((Float)1, beta.y());
								if (tileSampler->Get1D() > continueProb) break;
								beta /= continueProb;
							}
							ray = (RayDifferential)isect.SpawnRay(wi);
						}
					}
				}
			}, nTiles);
		//Create grid of all SPPM visible points 979
		int hashSize = nPixels;
		std::vector<std::atomic<SPPMPixelListNode*>> grid(hashSize);

		Bounds3f gridBounds;
		Float maxRadius = 0.;
		for (int i = 0; i < nPixels; ++i) {
			const SPPMPixel& pixel = pixels[i];
			if (pixel.vp.beta.IsBlack())
				continue;
			Bounds3f vpBound = Expand(Bounds3f(pixel.vp.p), pixel.radius);
			gridBounds = Union(gridBounds, vpBound);
			maxRadius = std::max(maxRadius, pixel.radius);
		}

		Vector3f diag = gridBounds.Diagonal();
		Float maxDiag = diag.max();
		int baseGridRes = (int)(maxDiag / maxRadius);
		int gridRes[3];
		for (int i = 0; i < 3; ++i)
			gridRes[i] = std::max((int)(baseGridRes * diag[i] / maxDiag), 1);

		ParallelFor(
			[&](int pixelIndex) {
				MemoryArena& arena = perThreadArenas[ThreadIndex];
				SPPMPixel& pixel = pixels[pixelIndex];
				if (!pixel.vp.beta.IsBlack()) {
					//Add pixel¡¯s visible point to applicable grid cells 982
					Float radius = pixel.radius;
					Point3i pMin, pMax;
					ToGrid(pixel.vp.p - Vector3f(radius, radius, radius), gridBounds, gridRes, &pMin);
					ToGrid(pixel.vp.p + Vector3f(radius, radius, radius), gridBounds, gridRes, &pMax);
					for (int z = pMin.z(); z <= pMax.z(); ++z)
						for (int y = pMin.y(); y <= pMax.y(); ++y)
							for (int x = pMin.x(); x <= pMax.x(); ++x) {
								//Add visible point to grid cell(x, y, z) 982
								int h = hash(Point3i(x, y, z), hashSize);
								SPPMPixelListNode* node = arena.Alloc<SPPMPixelListNode>();
								node->pixel = &pixel;

								//Atomically add node to the start of grid[h]¡¯s linked list
								node->next = grid[h];
								while (grid[h].compare_exchange_weak(node->next, node) == false)
									;
							}
				}
			}, nPixels, 4096);

		//	Trace photons and accumulate contributions 983
		{
			std::vector<MemoryArena> photonShootArenas(NumSystemCores());
			ParallelFor(
				[&](int photonIndex) {
					MemoryArena& arena = photonShootArenas[ThreadIndex];

					//Follow photon path for photonIndex 984
					uint64_t haltonIndex = (uint64_t)iter * (uint64_t)photonsPerIteration + photonIndex;
					int haltonDim = 0;

					//Choose light to shoot photon from 985
					Float lightPdf;
					Float lightSample = RadicalInverse(haltonDim++, haltonIndex);
					int lightNum = lightDistr->SampleDiscrete(lightSample, &lightPdf);
					const std::shared_ptr<Light>& light = scene.lights[lightNum];

					//	Compute sample values for photon ray leaving light source 985
					Point2f uLight0(RadicalInverse(haltonDim, haltonIndex), RadicalInverse(haltonDim + 1, haltonIndex));
					Point2f uLight1(RadicalInverse(haltonDim + 2, haltonIndex), RadicalInverse(haltonDim + 3, haltonIndex));
					Float uLightTime = Lerp(RadicalInverse(haltonDim + 4, haltonIndex), camera->shutterOpen, camera->shutterClose);
					haltonDim += 5;

					//	Generate photonRay from light source and initialize beta 985
					RayDifferential photonRay;
					Normal3f nLight;
					Float pdfPos, pdfDir;
					Spectrum Le = light->Sample_Le(uLight0, uLight1, uLightTime, &photonRay, &nLight, &pdfPos, &pdfDir);

					if (pdfPos == 0 || pdfDir == 0 || Le.IsBlack()) return;
					//Spectrum beta = (AbsDot(nLight, photonRay.d) * Le) / (lightPdf * pdfPos * pdfDir);
					Spectrum beta = (AbsDot(nLight, photonRay.d) * Le) / (lightPdf * pdfPos * pdfDir);

					if (beta.IsBlack())
						return;

					//	Follow photon path through scene and record intersections 986
					SurfaceInteraction isect;
					for (int depth = 0; depth < maxDepth; ++depth) {
						if (!scene.Intersect(photonRay, &isect))
							break;
						if (depth > 0) {
							//Add photon contribution to nearby visible points 986
							Point3i photonGridIndex;
							if (ToGrid(isect.p, gridBounds, gridRes, &photonGridIndex)) {
								int h = hash(photonGridIndex, hashSize);
								//Add photon contribution to visible points in grid[h] 986
								for (SPPMPixelListNode* node = grid[h].load(std::memory_order_relaxed);
									node != nullptr; node = node->next) {
									SPPMPixel& pixel = *node->pixel;
									Float radius = pixel.radius;
									if ((pixel.vp.p - isect.p).LengthSquared() > radius * radius)
										continue;

									//Update pixel and M for nearby photon 987
									Vector3f wi = -photonRay.d;
									Spectrum Phi = beta * pixel.vp.bsdf->f(pixel.vp.wo, wi);
									for (int i = 0; i < nSpectralSamples; ++i)
										pixel.Phi[i].Add(Phi[i]);
									++pixel.M;
								}
							}
						}
						//Sample new photon ray direction 987

						//Compute BSDF at photon intersection point 988
						isect.ComputeScatteringFunctions(photonRay, arena, true,
							TransportMode::Importance);
						if (!isect.bsdf) {
							--depth;
							photonRay = isect.SpawnRay(photonRay.d);
							continue;
						}
						const BSDF& photonBSDF = *isect.bsdf;
						//	Sample BSDF fr and direction wi for reflected photon 988
						Vector3f wi, wo = -photonRay.d;
						Float pdf;
						BxDFType flags;
						//Generate bsdfSample for outgoing photon sample 988

						Point2f bsdfSample(RadicalInverse(haltonDim, haltonIndex), RadicalInverse(haltonDim + 1, haltonIndex));
						haltonDim += 2;

						Spectrum fr = photonBSDF.Sample_f(wo, &wi, bsdfSample, &pdf, BSDF_ALL, &flags);
						if (fr.IsBlack() || pdf == 0.f) break;

						Spectrum bnew = beta * fr * AbsDot(wi, isect.shading.n) / pdf;
						//Possibly terminate photon path with Russian roulette 989
						Float q = std::max((Float)0, 1 - bnew.y() / beta.y());
						if (RadicalInverse(haltonDim++, haltonIndex) < q)
							break;
						beta = bnew / (1 - q);
						photonRay = (RayDifferential)isect.SpawnRay(wi);
					}
					arena.Reset();
				}, photonsPerIteration, 8192);
		}
		//	Update pixel values from this pass¡¯s photons 989
		//

		ParallelFor([&](int i) {
			SPPMPixel& p = pixels[i];
			if (p.M > 0) {
				// Update pixel photon count, search radius, and $\tau$ from
				// photons
				Float gamma = (Float)2 / (Float)3;
				Float Nnew = p.N + gamma * p.M;
				Float Rnew = p.radius * std::sqrt(Nnew / (p.N + p.M));
				Spectrum Phi;
				for (int j = 0; j < nSpectralSamples; ++j)
					Phi[j] = p.Phi[j];
				p.tau = (p.tau + p.vp.beta * Phi) * (Rnew * Rnew) /
					(p.radius * p.radius);
				p.N = Nnew;
				p.radius = Rnew;
				p.M = 0;
				for (int j = 0; j < nSpectralSamples; ++j)
					p.Phi[j] = (Float)0;
			}
			// Reset _VisiblePoint_ in pixel
			p.vp.beta = 0.;
			p.vp.bsdf = nullptr;
			}, nPixels, 4096);

		//	Periodically store SPPM image in film and write image
		if (iter + 1 == nIterations || ((iter + 1) % writeFrequency == 0)) {
			int x0 = pixelBounds.pMin.x();
			int x1 = pixelBounds.pMax.x();
			uint64_t Np = (uint64_t)(iter + 1) * (uint64_t)photonsPerIteration;
			std::unique_ptr<Spectrum[]> image(new Spectrum[pixelBounds.Volume()]);
			int offset = 0;
			for (int y = pixelBounds.pMin.y(); y < pixelBounds.pMax.y(); ++y) {
				for (int x = x0; x < x1; ++x) {
					// Compute radiance _L_ for SPPM pixel _pixel_
					const SPPMPixel& pixel =
						pixels[(y - pixelBounds.pMin.y()) * (x1 - x0) + (x - x0)];
					Spectrum L = pixel.Ld / (iter + 1);
					L += pixel.tau / (Np * Pi * pixel.radius * pixel.radius);
					image[offset++] = L;
				}
			}
			camera->film->SetImage(image.get());
			camera->film->WriteImage(1, true);
		}
		for (int i = 0; i < perThreadArenas.size(); ++i)
			perThreadArenas[i].Reset();
	}
	ParallelCleanup();
}

bool SPPMIntegrator::ToGrid(const Point3f& p, const Bounds3f& bounds, const int gridRes[3], Point3i* pi)
{
	bool inBounds = true;
	Vector3f pg = bounds.Offset(p);
	for (int i = 0; i < 3; ++i)
	{
		(*pi)[i] = (int)(gridRes[i] * pg[i]);
		inBounds &= ((*pi)[i] >= 0 && (*pi)[i] < gridRes[i]);
		(*pi)[i] = Clamp((*pi)[i], 0, gridRes[i] - 1);
	}
	return inBounds;
}
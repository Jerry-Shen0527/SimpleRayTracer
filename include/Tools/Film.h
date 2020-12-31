#pragma once

#include <vector>

#include "Atomic.h"
#include "Bound.h"
#include <Tools/Filter.h>
#include <mutex>

#include "Spectrum/SampledSpectrum.h"

struct FilmTilePixel {
	Spectrum contribSum = 0.f;
	Float filterWeightSum = 0.f;
};

class FilmTile;

class Film {
public:
	// Film Public Methods
	Film(const Point2i& resolution, const Bounds2f& cropWindow,
		std::unique_ptr<Filter> filter, Float diagonal,
		const std::string& filename, Float scale,
		Float maxSampleLuminance = Infinity);
	Bounds2i GetSampleBounds() const;
	Bounds2f GetPhysicalExtent() const;
	std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i& sampleBounds);
	void MergeFilmTile(std::unique_ptr<FilmTile> tile);
	void WriteImage(Float splatScale = 1, bool timeStamp = false);
	void SetImage(const Spectrum* img) const;
	void AddSplat(const Point2f& p, Spectrum v);
	void Clear();

	// Film Public Data
	const Point2i fullResolution;
	const Float diagonal;
	std::unique_ptr<Filter> filter;
	const std::string filename;
	Bounds2i croppedPixelBounds;

private:
	// Film Private Data

	struct Pixel {
		Pixel() { xyz[0] = xyz[1] = xyz[2] = filterWeightSum = 0; }
		Float xyz[3];
		Float filterWeightSum;
		AtomicFloat splatXYZ[3];
		Float pad;
	};

	std::unique_ptr<Pixel[]> pixels;
	static constexpr int filterTableWidth = 16;
	Float filterTable[filterTableWidth * filterTableWidth];
	std::mutex mutex;
	const Float scale;
	const Float maxSampleLuminance;

	// Film Private Methods
	Pixel& GetPixel(const Point2i& p) {
		int width = croppedPixelBounds.pMax.x() - croppedPixelBounds.pMin.x();
		int offset = (p.x() - croppedPixelBounds.pMin.x()) +
			(p.y() - croppedPixelBounds.pMin.y()) * width;
		return pixels[offset];
	}
};

class FilmTile {
public:
	// FilmTile Public Methods
	FilmTile(const Bounds2i& pixelBounds, const Vector2f& filterRadius,
		const Float* filterTable, int filterTableSize,
		Float maxSampleLuminance)
		: pixelBounds(pixelBounds),
		filterRadius(filterRadius),
		invFilterRadius(1 / filterRadius.x(), 1 / filterRadius.y()),
		filterTable(filterTable),
		filterTableSize(filterTableSize),
		maxSampleLuminance(maxSampleLuminance) {
		pixels = std::vector<FilmTilePixel>(std::max(0, pixelBounds.Volume()));
	}

	void AddSample(const Point2f& pFilm, Spectrum L, Float sampleWeight = 1.);

	FilmTilePixel& GetPixel(const Point2i& p);

	const FilmTilePixel& GetPixel(const Point2i& p) const;
	Bounds2i GetPixelBounds() const { return pixelBounds; }

private:
	// FilmTile Private Data
	const Bounds2i pixelBounds;
	const Vector2f filterRadius, invFilterRadius;
	const Float* filterTable;
	const int filterTableSize;
	std::vector<FilmTilePixel> pixels;
	const Float maxSampleLuminance;
	friend class Film;
};
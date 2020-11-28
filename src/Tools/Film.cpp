#include "Tools/Film.h"

Film::Film(const Point2i& resolution, const Bounds2f& cropWindow,
	std::unique_ptr<Filter> filt, Float diagonal,
	const std::string& filename, Float scale, Float maxSampleLuminance)
	: fullResolution(resolution),
	diagonal(diagonal * .001),
	filter(std::move(filt)),
	filename(filename),
	scale(scale),
	maxSampleLuminance(maxSampleLuminance) {
	// Compute film image bounds
	croppedPixelBounds =
		Bounds2i(Point2i(std::ceil(fullResolution.x() * cropWindow.pMin.x()),
			std::ceil(fullResolution.y() * cropWindow.pMin.y())),
			Point2i(std::ceil(fullResolution.x() * cropWindow.pMax.x()),
				std::ceil(fullResolution.y() * cropWindow.pMax.y())));

	// Allocate film image storage
	pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Volume()]);

	// Precompute filter weight table
	int offset = 0;
	for (int y = 0; y < filterTableWidth; ++y) {
		for (int x = 0; x < filterTableWidth; ++x, ++offset) {
			Point2f p;
			p.x() = (x + 0.5f) * filter->radius.x() / filterTableWidth;
			p.y() = (y + 0.5f) * filter->radius.y() / filterTableWidth;
			filterTable[offset] = filter->Evaluate(p);
		}
	}
}

Bounds2i Film::GetSampleBounds() const
{
	Bounds2f floatBounds(Floor(Point2f(croppedPixelBounds.pMin) + Vector2f(0.5f, 0.5f) - filter->radius),
		Ceil(Point2f(croppedPixelBounds.pMax) - Vector2f(0.5f, 0.5f) + filter->radius));
	return (Bounds2i)floatBounds;
}

Bounds2f Film::GetPhysicalExtent() const
{
	Float aspect = (Float)fullResolution.y() / (Float)fullResolution.x();
	Float x = std::sqrt(diagonal * diagonal / (1 + aspect * aspect));
	Float y = aspect * x;
	return Bounds2f(Point2f(-x / 2, -y / 2), Point2f(x / 2, y / 2));
}

std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i& sampleBounds) {
	// Bound image pixels that samples in _sampleBounds_ contribute to
	Vector2f halfPixel = Vector2f(0.5f, 0.5f);
	Bounds2f floatBounds = Bounds2f(sampleBounds.pMin, sampleBounds.pMax);
	Point2i p0 = (Point2i)Ceil(floatBounds.pMin - halfPixel - filter->radius);
	Point2i p1 = (Point2i)Floor(floatBounds.pMax - halfPixel + filter->radius) +
		Point2i(1, 1);
	Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), croppedPixelBounds);
	return std::unique_ptr<FilmTile>(new FilmTile(
		tilePixelBounds, filter->radius, filterTable, filterTableWidth,
		maxSampleLuminance));
}

void FilmTile::AddSample(const Point2f& pFilm, Spectrum L, Float sampleWeight)
{
	if (L.y() > maxSampleLuminance)
		L *= maxSampleLuminance / L.y();
	// Compute sample's raster bounds
	Point2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
	Point2i p0 = Ceil(pFilmDiscrete - filterRadius);
	Point2i p1 = Floor(pFilmDiscrete + filterRadius) + Point2i(1, 1);
	p0 = Max(p0, pixelBounds.pMin);
	p1 = Min(p1, pixelBounds.pMax);

	// Loop over filter support and add sample to pixel arrays

	// Precompute $x$ and $y$ filter table offsets
	int* ifx = ALLOCA(int, p1.x() - p0.x());
	for (int x = p0.x(); x < p1.x(); ++x)
	{
		Float fx = std::abs((x - pFilmDiscrete.x()) * invFilterRadius.x() *
			filterTableSize);
		ifx[x - p0.x()] = std::min((int)std::floor(fx), filterTableSize - 1);
	}
	int* ify = ALLOCA(int, p1.y() - p0.y());
	for (int y = p0.y(); y < p1.y(); ++y)
	{
		Float fy = std::abs((y - pFilmDiscrete.y()) * invFilterRadius.y() *
			filterTableSize);
		ify[y - p0.y()] = std::min((int)std::floor(fy), filterTableSize - 1);
	}
	for (int y = p0.y(); y < p1.y(); ++y)
	{
		for (int x = p0.x(); x < p1.x(); ++x)
		{
			// Evaluate filter value at $(x,y)$ pixel
			int offset = ify[y - p0.y()] * filterTableSize + ifx[x - p0.x()];
			Float filterWeight = filterTable[offset];

			// Update pixel values with filtered sample contribution
			FilmTilePixel& pixel = GetPixel(Point2i(x, y));
			pixel.contribSum += L * sampleWeight * filterWeight;
			pixel.filterWeightSum += filterWeight;
		}
	}
}

FilmTilePixel& FilmTile::GetPixel(const Point2i& p)
{
	int width = pixelBounds.pMax.x() - pixelBounds.pMin.x();
	int offset =
		(p.x() - pixelBounds.pMin.x()) + (p.y() - pixelBounds.pMin.y()) * width;
	return pixels[offset];
}

const FilmTilePixel& FilmTile::GetPixel(const Point2i& p) const
{
	int width = pixelBounds.pMax.x() - pixelBounds.pMin.x();
	int offset =
		(p.x() - pixelBounds.pMin.x()) + (p.y() - pixelBounds.pMin.y()) * width;
	return pixels[offset];
}
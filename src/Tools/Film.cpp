#include "Tools/Film.h"
#include <Tools/Files/rtw_stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Tools/Files/stb_image_write.h"

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

void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (Point2i pixel : tile->GetPixelBounds()) {
		// Merge _pixel_ into _Film::pixels_
		const FilmTilePixel& tilePixel = tile->GetPixel(pixel);
		Pixel& mergePixel = GetPixel(pixel);
		Float xyz[3];
		tilePixel.contribSum.ToXYZ(xyz);
		for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
		mergePixel.filterWeightSum += tilePixel.filterWeightSum;
	}
}

void Film::WriteImage(Float splatScale, bool timeStamp)
{
	std::unique_ptr<Float[]> rgb(new Float[3 * croppedPixelBounds.Volume()]);
	int offset = 0;
	for (Point2i p : croppedPixelBounds) {
		// Convert pixel XYZ color to RGB
		Pixel& pixel = GetPixel(p);
		XYZToRGB(pixel.xyz, &rgb[3 * offset]);

		// Normalize pixel with weight sum
		Float filterWeightSum = pixel.filterWeightSum;
		if (filterWeightSum != 0) {
			Float invWt = (Float)1 / filterWeightSum;
			rgb[3 * offset] = std::max((Float)0, rgb[3 * offset] * invWt);
			rgb[3 * offset + 1] =
				std::max((Float)0, rgb[3 * offset + 1] * invWt);
			rgb[3 * offset + 2] =
				std::max((Float)0, rgb[3 * offset + 2] * invWt);
		}

		// Add splat value at pixel
		Float splatRGB[3];
		Float splatXYZ[3] = { pixel.splatXYZ[0], pixel.splatXYZ[1],
							 pixel.splatXYZ[2] };
		XYZToRGB(splatXYZ, splatRGB);
		rgb[3 * offset] += splatScale * splatRGB[0];
		rgb[3 * offset + 1] += splatScale * splatRGB[1];
		rgb[3 * offset + 2] += splatScale * splatRGB[2];

		// Scale pixel value by _scale_
		rgb[3 * offset] *= scale;
		rgb[3 * offset + 1] *= scale;
		rgb[3 * offset + 2] *= scale;
		++offset;
	}

	std::unique_ptr<char[]> rgb_char(new char[3 * croppedPixelBounds.Volume()]);

	offset = 0;
	for (Point2i p : croppedPixelBounds) {
		// Scale pixel value by _scale_
		rgb_char[3 * offset]     = static_cast<char>(Clamp(rgb[3 * offset], 0, 255.0));
		rgb_char[3 * offset + 1] = static_cast<char>(Clamp(rgb[3 * offset + 1], 0, 255.0));
		rgb_char[3 * offset + 2] = static_cast<char>(Clamp(rgb[3 * offset + 2], 0, 255.0));
		++offset;
	}

	auto out_put = croppedPixelBounds.Diagonal();

	if (timeStamp)
	{
		struct tm* tm = new struct tm;
		auto tme = time(nullptr);
		localtime_s(tm, &tme);
		auto time_s = std::to_string(tm->tm_mon) + "-" + std::to_string(tm->tm_mday) + " " + std::to_string(tm->tm_hour) + "-" + std::to_string(tm->tm_min) + "-" + std::to_string(tm->tm_sec) + "-";
		stbi_write_png((time_s + filename).c_str(), out_put.x(), out_put.y(), 3, &rgb_char[0], 0);
		delete tm;
	}
	else {
		stbi_write_png(filename.c_str(), out_put.x(), out_put.y(), 3, &rgb_char[0], 0);
	}
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
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
		Bounds2i(Point2i(std::ceil(fullResolution.x * cropWindow.pMin.x),
			std::ceil(fullResolution.y * cropWindow.pMin.y)),
			Point2i(std::ceil(fullResolution.x * cropWindow.pMax.x),
				std::ceil(fullResolution.y * cropWindow.pMax.y)));

	// Allocate film image storage
	pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Area()]);
	filmPixelMemory += croppedPixelBounds.Area() * sizeof(Pixel);

	// Precompute filter weight table
	int offset = 0;
	for (int y = 0; y < filterTableWidth; ++y) {
		for (int x = 0; x < filterTableWidth; ++x, ++offset) {
			Point2f p;
			p.x = (x + 0.5f) * filter->radius.x / filterTableWidth;
			p.y = (y + 0.5f) * filter->radius.y / filterTableWidth;
			filterTable[offset] = filter->Evaluate(p);
		}
	}
}
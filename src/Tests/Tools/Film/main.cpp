#include <Tools/Film.h>
#include <iomanip>

int main()
{
	static constexpr int filterTableWidth = 16;
	Float filterTable[filterTableWidth * filterTableWidth];

	Float radius = 3;

	std::unique_ptr<Filter> filter = std::make_unique<TriangleFilter>(radius);

	int offset = 0;

	for (int y = 0; y < filterTableWidth; ++y) {
		for (int x = 0; x < filterTableWidth; ++x, ++offset) {
			Point2f p;
			p.x() = (x + 0.5f) * filter->radius.x() / filterTableWidth;
			p.y() = (y + 0.5f) * filter->radius.y() / filterTableWidth;
			filterTable[offset] = filter->Evaluate(p);
		}
	}

	offset = 0;
	for (int y = 0; y < filterTableWidth; ++y) {
		for (int x = 0; x < filterTableWidth; ++x, ++offset) {
			std::cout << std::setprecision(3) << std::setw(6) << filterTable[offset] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;

	Bounds2i bounds(Point2i(1, 1), Point2i(15, 16));
	Vector2f ra(radius, radius);
	FilmTile film_tile(bounds, ra, filterTable, filterTableWidth, 1);

	film_tile.AddSample(Point2f(5, 4), Spectrum(1.0), 1);

	auto bound = film_tile.GetPixelBounds();

	Vector3f rst;

	SampledSpectrum::Init();

	for (int i = 1; i < 15; ++i)
	{
		for (int j = 1; j < 7; ++j)
		{
			film_tile.GetPixel(Point2i(i, j)).contribSum.ToXYZ(rst);
			std::cout << '(';
			rst.print(std::cout);
			std::cout << ')';
		}
		std::cout << std::endl;
	}

	std::cout << "Pause" << std::endl;
}
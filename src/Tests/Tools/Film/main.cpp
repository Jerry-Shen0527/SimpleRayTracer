#include <Tools/Film.h>

int main()
{
	static constexpr int filterTableWidth = 16;
	Float filterTable[filterTableWidth * filterTableWidth];

	Float radius = 3.0;

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

	Bounds2i bounds(Point2i(1, 1), Point2i(15, 16));
	Vector2f ra(radius, radius);
	FilmTile film_tile(bounds, ra, filterTable, filterTableWidth * filterTableWidth, 1);

	film_tile.AddSample(Point2f(3, 3), Spectrum(0.5), 1);
	std::cout << "Pause" << std::endl;
}
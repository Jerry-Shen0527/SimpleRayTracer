#include <Geometry/Vector3.h>

int main()
{
	Vector3f a(3, 4, 5);
	Vector3f b(4, 5, 6);
	std::cout << a << std::endl;
	std::cout << a[0] << ' ' << a[1] << ' ' << a[2] << std::endl;

	a += b;
	std::cout << 2 * a << std::endl;
	std::cout << -a / 2 << std::endl;
	std::cout << a.length() << std::endl;
	std::cout << a + b << std::endl;
	std::cout << b - a << std::endl;
	std::cout << a * b << std::endl;
	std::cout << Dot(a, b) << std::endl;

	b = a;
	std::cout << b << std::endl;
	std::cout << Cross(a, b) << std::endl;
	std::cout << a.x() << ' ' << a.y() << ' ' << a.z() << std::endl;
	std::cout << a.random() << std::endl;
	std::cout << a.random(1, 2) << std::endl;
}
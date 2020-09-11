#include <Tools/Math/vec3.h>

void vec3_test()
{
	vec3 a(3, 4, 5);
	vec3 b(4, 5, 6);
	std::cout << a << std::endl;
	std::cout << a[0] << ' ' << a[1] << ' ' << a[2] << std::endl;

	a += b;
	std::cout << 2 * a << std::endl;
	std::cout << -a / 2 << std::endl;
	std::cout << a.length() << std::endl;
	std::cout << a + b << std::endl;
	std::cout << b - a << std::endl;
	std::cout << a * b << std::endl;
	std::cout << dot(a, b) << std::endl;

	b = a;
	std::cout << b << std::endl;
	std::cout << cross(a, b) << std::endl;
	std::cout << a.x() << ' ' << a.y() << ' ' << a.z() << std::endl;
	std::cout << a.random() << std::endl;
	std::cout << a.random(1, 2) << std::endl;
	std::cout << random_in_unit_sphere() << std::endl;
	std::cout << random_unit_vector() << "length=" << random_unit_vector().length() << std::endl;

	
}
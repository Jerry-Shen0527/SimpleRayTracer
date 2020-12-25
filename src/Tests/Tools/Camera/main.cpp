#include <Geometry/TriangleMesh.h>
#include <Geometry/bvh.h>

#include "Tools/camera.h"

int indices[] = { 0,1,2 };
Point3f points[] = { Point3f(0,0,0),Point3f(1,0,0),Point3f(0,1,0) };

int main()
{
	Transform identity;
	auto mesh = CreateTriangleMesh(&identity, &identity, false, 1, indices, 3, points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	for (auto triangle : mesh)
	{
		primitives.emplace_back(make_shared<GeometricPrimitive>(triangle, nullptr, nullptr));
	}

	BVHAccel bvh(primitives, 3, SplitMethod::Middle);

	Ray ray(Point3f(0.5, 0.501, -1), Vector3f(0, 0, 1));

	SurfaceInteraction isect;


	Film film(Point2i(600, 600), Bounds2f(1.0, 1.0), std::make_unique<BoxFilter>(Vector2f(1.0, 1.0)), 1, "test", 1);
	Transform t = Translate(Vector3f(278, 278, -800));
	AnimatedTransform transform(&t, 0, &t, 0);
	PerspectiveCamera camera(transform, Bounds2f(Point2f(1, 1)), 0, 1.0, 1, 10.0, 40.0, &film, nullptr);

	//if (bvh.Intersect(ray, &isect))
	//	cout << "Intersect!" << endl;
	//else cout << "Not intersect!" << endl;

	camera.GenerateRay()
	
}
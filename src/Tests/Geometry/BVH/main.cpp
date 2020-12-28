#include <Geometry/TriangleMesh.h>
#include <Geometry/bvh.h>

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

	Ray ray(Point3f(0.25, 0.2501, -1), Vector3f(0, 0, 1));

	SurfaceInteraction isect;

	if (bvh.Intersect(ray, &isect))
		cout << "Intersect!" << endl;
	else cout << "Not intersect!" << endl;
}
#include <Tools/Scenes.h>

#include "BRDF/Material.h"
#include "Geometry/bvh.h"
#include "Geometry/TriangleMesh.h"
#include "Integrators/PathIntegrator.h"

int indices[] = {
	0,2,3,
	0,1,3,
	2,3,6,
	3,6,7,
	4,5,7,
	4,6,7,
	1,3,5,
	3,5,7,
	1,0,4,
	1,4,5
};
Point3f points[] = {
	Point3f(0, 0, 0),Point3f(0, 0, 555),
	Point3f(0, 555, 0),Point3f(0, 555, 555),
	Point3f(555, 0, 0),Point3f(555, 0, 555),
	Point3f(555, 555, 0),Point3f(555, 555, 555),
};
Float rgbcolor[] = { 0,256,256 };

Float red[] = { .65, .05, .05 };
Float white[] = { .73, .73, .73 };
Float green[] = { .12, .45, .15 };

Point3f light_points[] = {
	Point3f(213,554,227),
	Point3f(213,554,343),
	Point3f(332,554,227),
	Point3f(332,554,343)
};

int light_indices[] =
{
	0,1,2,2,1,3
};

Scene CreateCornell()
{
	SampledSpectrum::Init();
	auto t = Rotate(0, Vector3f(0, 1, 0));
	auto wto = Inverse(t);

	auto mesh = CreateTriangleMesh(&t, &wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(&t, &wto, true, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);

	auto Red = make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(red, SpectrumType::Reflectance));
	auto Green = make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(green, SpectrumType::Reflectance));
	auto White = make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(white, SpectrumType::Reflectance));

	auto red_material = make_shared<MatteMaterial>(Red, Zero, nullptr);
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);
	auto green_material = make_shared<MatteMaterial>(Green, Zero, nullptr);

	auto light_t = Transform();

	vector<shared_ptr<Light>> lights;

	auto white_light = BlackBodySpectrum(5000, 20);

	for (auto light_tri : light_mesh)
	{
		auto light_ptr = make_shared<DiffuseAreaLight>(light_t, MediumInterface(), white_light, 1, light_tri);
		lights.push_back(light_ptr);
		primitives.push_back(make_shared<GeometricPrimitive>(light_tri, white_material, light_ptr));
	}

	primitives.push_back(make_shared<GeometricPrimitive>(mesh[0], green_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[1], green_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[2], white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[3], white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[4], red_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[5], red_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[6], white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[7], white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[8], white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[9], white_material, nullptr));

	auto tb1 = Translate(Vector3f(265, 0, 295));
	tb1 = tb1 * Rotate(15, Vector3f(0, 1, 0));

	auto tb2 = Translate(Vector3f(130, 0, 65));
	tb2 = tb2 * Rotate(-18, Vector3f(0, 1, 0));

	auto Box1 = CreateBox(&tb1, &Inverse(tb1), false, Point3f(0, 0, 0), Point3f(165, 330, 165));
	auto Box2 = CreateBox(&tb2, &Inverse(tb2), false, Point3f(0, 0, 0), Point3f(165, 165, 165));

	for (auto&& tri : Box1)
	{
		primitives.push_back(make_shared<GeometricPrimitive>(tri, white_material, nullptr));
	}

	for (auto&& tri : Box2)
	{
		primitives.push_back(make_shared<GeometricPrimitive>(tri, white_material, nullptr));
	}

	auto bvh = make_shared<BVHAccel>(primitives, 3, SplitMethod::Middle);

	//(+tilesize-1)/tileSize: max groups

	Scene scene(bvh, lights);
	return scene;
}
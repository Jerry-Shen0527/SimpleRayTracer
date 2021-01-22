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

Scene CreateCornell(MemoryArena& arena)
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

	auto tb1 = ARENA_ALLOC(arena, Transform);
	auto itb1 = ARENA_ALLOC(arena, Transform);

	*tb1 = Translate(Vector3f(265, 0, 295));
	*tb1 = *tb1 * Rotate(15, Vector3f(0, 1, 0));
	*itb1 = Inverse(*tb1);

	auto tb2 = ARENA_ALLOC(arena, Transform);

	auto itb2 = ARENA_ALLOC(arena, Transform);

	*tb2 = Translate(Vector3f(130, 0, 65));
	*tb2 = *tb2 * Rotate(-18, Vector3f(0, 1, 0));
	*itb2 = Inverse(*tb2);

	auto Box1 = CreateBox(tb1, itb1, false, Point3f(0, 0, 0), Point3f(165, 330, 165));
	auto Box2 = CreateBox(tb2, itb2, false, Point3f(0, 0, 0), Point3f(165, 165, 165));

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

Float lambda[] = { 206.64,208.38,210.14,211.94,213.77,215.63,217.52,219.44,221.4,223.4,225.43,227.5,229.6,231.75,233.93,236.16,238.43,240.75,243.11,245.52,247.97,250.48,253.03,255.64,258.3,261.02,263.8,266.63,269.53,272.49,275.52,278.62,281.78,285.02,288.34,291.73,295.2,298.76,302.4,306.14,309.96,313.89,317.91,322.04,326.28,330.63,335.1,339.69,344.4,349.25,354.24,359.38,364.66,370.11,375.71,381.49,387.45,393.6,399.95,406.51,413.28,420.29,427.54,435.04,442.8,450.86,459.2,467.87,476.87,486.22,495.94,506.06,516.6,527.6,539.07,551.05,563.57,576.68,590.41,604.81,619.93,635.82,652.55,670.19,688.81,708.49,729.32,751.43,774.91,799.9,826.57,855.07,885.61,918.41,953.73,991.88,1033.21,1078.13,1127.14,1180.81,1239.85,1305.11,1377.61,1458.65,1549.81,1653.14,1771.22,1907.46,2066.42,2254.28,2479.7 };

Float eta[] = { 1.422,1.427,1.43,1.431,1.432,1.435,1.438,1.44,.442,.447,1.452,.453,.454,1.458,1.462,1.466,1.47,1.474,1.478,1.481,1.484,1.487,1.49,1.495,.504,1.523,.546,1.572,1.598,1.624,1.648,1.669,1.69,1.717,1.742,1.76,.776,1.795,.812,1.823,1.83,1.837,1.84,1.834,1.824,1.812,.798,1.782,.766,1.753,1.74,1.728,1.716,1.706,1.696,1.685,1.674,.666,1.658,1.647,1.636,1.628,1.616,1.596,.562,1.502,.426,1.346,1.242,1.087,0.916,0.755,.608,.492,0.402,0.346,0.306,0.268,0.236,0.212,0.194,0.178,0.166,0.161,0.16,0.161,0.164,0.17,0.176,.181,0.188,0.198,0.21,0.222,0.236,0.253,10.272,30.291,40.312,10.339,50.372,10.41,10.454,50.503,10.559,40.622,20.696,60.785,20.896,81.034,1.205 };

Float k[] = { 1.306,1.31925,1.334,1.34925,1.364,1.376,1.388,1.403,1.418,1.42963,1.442,1.4595,1.478,1.49375,1.51,1.5295,1.55,1.56963,1.59,1.61163,1.636,1.66675,1.698,1.72463,1.748,1.76675,1.784,1.80338,1.822,1.8375,1.852,1.86775,1.882,1.89175,1.9,1.91,1.918,1.92038,1.92,1.91888,1.916,1.91138,1.904,1.89138,1.878,1.86825,1.86,1.85175,1.846,1.84525,1.848,1.85238,1.862,1.883,1.906,1.9225,1.936,1.94775,1.956,1.95938,1.958,1.95138,1.94,1.9245,1.904,1.87588,1.846,1.81463,1.796,1.79738,1.84,1.9565,2.12,2.32625,2.54,2.73063,2.88,2.94063,2.97,3.015,3.06,3.07,3.15,3.44581,3.8,4.08769,4.357,4.61019,4.86,5.12581,5.39,5.63125,5.88,6.16813,6.47,6.75313,7.07,7.485,7.93,8.40375,8.77,8.7575,8.77,9.20313,9.81,10.4575,11.2,12.03063,13,14.14438,15.5 };

Scene CreateCornellGold(MemoryArena& arena)
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

	auto One = make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));

	auto gold_material = make_shared<MetalMaterial>(make_shared<ConstantTexture<Spectrum>>(Spectrum::FromSampled(lambda, eta, 111)), make_shared<ConstantTexture<Spectrum>>(Spectrum::FromSampled(lambda, k, 111)), make_shared<ConstantTexture<float>>(0.5), nullptr, nullptr, nullptr, false);

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

	auto tb1 = ARENA_ALLOC(arena, Transform);
	auto itb1 = ARENA_ALLOC(arena, Transform);

	*tb1 = Translate(Vector3f(265, 0, 295));
	*tb1 = *tb1 * Rotate(15, Vector3f(0, 1, 0));
	*itb1 = Inverse(*tb1);

	auto tb2 = ARENA_ALLOC(arena, Transform);

	auto itb2 = ARENA_ALLOC(arena, Transform);

	*tb2 = Translate(Vector3f(130, 0, 65));
	*tb2 = *tb2 * Rotate(-18, Vector3f(0, 1, 0));
	*itb2 = Inverse(*tb2);

	auto Box1 = CreateBox(tb1, itb1, false, Point3f(0, 0, 0), Point3f(165, 330, 165));
	auto Box2 = CreateBox(tb2, itb2, false, Point3f(0, 0, 0), Point3f(165, 165, 165));

	for (auto&& tri : Box1)
	{
		primitives.push_back(make_shared<GeometricPrimitive>(tri, gold_material, nullptr));
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

Scene CreateCornellWithBalls(MemoryArena& arena)
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

	auto One = make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));
	auto glass_material = make_shared<GlassMaterial>(One, One, make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<float>>(1.5), nullptr, false);

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

	auto tb1 = ARENA_ALLOC(arena, Transform);
	auto itb1 = ARENA_ALLOC(arena, Transform);

	*tb1 = Translate(Vector3f(265, 0, 295));
	*tb1 = *tb1 * Rotate(15, Vector3f(0, 1, 0));
	*itb1 = Inverse(*tb1);

	auto tb2 = ARENA_ALLOC(arena, Transform);

	auto itb2 = ARENA_ALLOC(arena, Transform);

	*tb2 = Translate(Vector3f(190, 90, 190));
	*tb2 = *tb2 * Rotate(-18, Vector3f(0, 1, 0));
	*itb2 = Inverse(*tb2);

	auto Box1 = CreateBox(tb1, itb1, false, Point3f(0, 0, 0), Point3f(165, 330, 165));
	//auto Box2 = CreateBox(&tb2, &Inverse(tb2), false, Point3f(0, 0, 0), Point3f(165, 165, 165));

	auto sphere = make_shared<Sphere>(tb2, itb2, false, 90, -1000, 1000, 360);

	for (auto&& tri : Box1)
	{
		primitives.push_back(make_shared<GeometricPrimitive>(tri, white_material, nullptr));
	}

	//for (auto&& tri : Box2)
	//{
	//	primitives.push_back(make_shared<GeometricPrimitive>(tri, white_material, nullptr));
	//}

	//primitives.push_back(make_shared<GeometricPrimitive>(sphere, white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(sphere, glass_material, nullptr));

	auto bvh = make_shared<BVHAccel>(primitives, 3, SplitMethod::Middle);

	//(+tilesize-1)/tileSize: max groups

	Scene scene(bvh, lights);
	return scene;
}
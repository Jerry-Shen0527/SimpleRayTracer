#include <Tools/Scenes.h>

#include "BRDF/Material.h"
#include "Geometry/bvh.h"
#include "Geometry/TriangleMesh.h"
#include "Integrators/PathIntegrator.h"

int indices[] = {
	0,2,3,
	0,3,1,
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

Float red[] = { .65, .05, .05 };
Float white[] = { 1., 1., 1. };
Float green[] = { .12, .45, .15 };

//Point3f light_points[] = {
//	Point3f(213,554,227),
//	Point3f(213,554,343),
//	Point3f(332,554,227),
//	Point3f(332,554,343)
//};

Point3f light_points[] = {
	Point3f(0,0,0),
	Point3f(0,0,116),
	Point3f(119,0,0),
	Point3f(119,0,116)
};

int light_indices[] =
{
	0,1,2,2,1,3
};

Scene CreateCornell(MemoryArena& arena)
{
	UnpolarizedSpectrum::Init();
	IMPORT_TYPES_L3
		auto t = ARENA_ALLOC(arena, Transform);
	auto wto = ARENA_ALLOC(arena, Transform);
	*t = Transform();
	*wto = Inverse(*t);

	auto t_light = ARENA_ALLOC(arena, Transform);
	auto wto_light = ARENA_ALLOC(arena, Transform);
	*t_light = Translate(Vector3f(213, 554, 227));
	*wto_light = Inverse(*t_light);

	auto mesh = CreateTriangleMesh(t, wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(t_light, wto_light, true, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);

	auto Red = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(red, SpectrumType::Reflectance));
	auto Green = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(green, SpectrumType::Reflectance));
	auto White = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(white, SpectrumType::Reflectance));

	auto red_material = make_shared<MatteMaterial>(Red, Zero, nullptr);
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);
	auto green_material = make_shared<MatteMaterial>(Green, Zero, nullptr);

	vector<shared_ptr<Light>> lights;

	auto white_light = BlackBodySpectrum(5000, 20);

	for (auto light_tri : light_mesh)
	{
		auto light_ptr = make_shared<DiffuseAreaLight>(*t_light, MediumInterface(), white_light, 1, light_tri);
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

	Scene scene(bvh, lights);
	return scene;
}

Float gold_lambda[] = { 375.71,381.49,387.45,393.6,399.95,406.51,413.28,420.29,427.54,435.04,442.8,450.86,459.2,467.87,476.87,486.22,495.94,506.06,516.6,527.6,539.07,551.05,563.57,576.68,590.41,604.81,619.93,635.82,652.55,670.19,688.81,708.49,729.32 };

Float gold_eta[] = { 1.696,1.685,1.674,.666,1.658,1.647,1.636,1.628,1.616,1.596,.562,1.502,.426,1.346,1.242,1.087,0.916,0.755,.608,.492,0.402,0.346,0.306,0.268,0.236,0.212,0.194,0.178,0.166,0.161,0.16,0.161,0.164 };

Float gold_k[] = { 1.906,1.9225,1.936,1.94775,1.956,1.95938,1.958,1.95138,1.94,1.9245,1.904,1.87588,1.846,1.81463,1.796,1.79738,1.84,1.9565,2.12,2.32625,2.54,2.73063,2.88,2.94063,2.97,3.015,3.06,3.07,3.15,3.44581,3.8,4.08769,4.357 };

Float glass_lambda[] = { 200,205,210,215,220,225,230,235,240,245,250,255,260,265,270,275,280,285,290,295,300,305,310,315,320,325,330,335,340,345,350,355,360,365,370,375,380,385,390,395,400,405,410,415,420,425,430,435,440,445,450,455,460,465,470,475,480,485,490,495,500,505,510,515,520,525,530,535,540,545,550,555,560,565,570,575,580,585,590,595,600,605,610,615,620,625,630,635,640,645,650,655,660,665,670,675,680,685,690,700,710,720,730,740,750,760,770,780,790,800,810,820,830,840,850,860,870,880,890,900,910,920,930,940,950,960,970,980,990,1000,1010,1020,1030,1040,1050,1060,1070,1080,1090,1100,1110,1120,1130,1140,1150,1160,1170,1180,1190,1200,1210,1220,1230,1240,1250,1260,1270,1280,1290,1300,1310,1320,1330,1340,1350,1360,1370,1380,1390,1400,1410,1420,1430,1440,1450,1460,1470,1480,1490,1500,1510,1520,1530,1540,1550,1560,1570,1580,1590,1600,1610,1620,1630,1640,1650,1660,1670,1680,1690,1700,1710,1720,1730,1740,1750,1760,1770,1780,1790,1800,1810,1820,1830,1840,1850,1860,1870,1880,1890,1900,1910,1920,1930,1940,1950,1960,1970,1980,1990,2000 };

Float glass_eta[] = { 1.6479,1.641,1.6347,1.629,1.6238,1.6191,1.6147,1.6107,1.607,1.6035,1.6003,1.5974,1.5946,1.5921,1.5897,1.5874,1.5853,1.5833,1.5814,1.5797,1.578,1.5765,1.575,1.5736,1.5722,1.571,1.5698,1.5686,1.5676,1.5665,1.5655,1.5646,1.5637,1.5628,1.562,1.5612,1.5605,1.5597,1.559,1.5584,1.5577,1.5571,1.5565,1.5559,1.5554,1.5549,1.5544,1.5539,1.5534,1.5529,1.5525,1.5521,1.5516,1.5512,1.5508,1.5505,1.5501,1.5497,1.5494,1.5491,1.5487,1.5484,1.5481,1.5478,1.5475,1.5472,1.547,1.5467,1.5464,1.5462,1.5459,1.5457,1.5455,1.5452,1.545,1.5448,1.5446,1.5444,1.5442,1.544,1.5438,1.5436,1.5434,1.5432,1.543,1.5429,1.5427,1.5425,1.5424,1.5422,1.542,1.5419,1.5417,1.5416,1.5414,1.5413,1.5412,1.541,1.5409,1.5406,1.5404,1.5401,1.5399,1.5396,1.5394,1.5392,1.539,1.5387,1.5385,1.5383,1.5381,1.5379,1.5378,1.5376,1.5374,1.5372,1.537,1.5369,1.5367,1.5365,1.5364,1.5362,1.5361,1.5359,1.5357,1.5356,1.5354,1.5353,1.5351,1.535,1.5349,1.5347,1.5346,1.5344,1.5343,1.5342,1.534,1.5339,1.5337,1.5336,1.5335,1.5333,1.5332,1.5331,1.5329,1.5328,1.5327,1.5326,1.5324,1.5323,1.5322,1.532,1.5319,1.5318,1.5316,1.5315,1.5314,1.5313,1.5311,1.531,1.5309,1.5307,1.5306,1.5305,1.5303,1.5302,1.5301,1.53,1.5298,1.5297,1.5296,1.5294,1.5293,1.5292,1.529,1.5289,1.5288,1.5286,1.5285,1.5284,1.5282,1.5281,1.528,1.5278,1.5277,1.5276,1.5274,1.5273,1.5271,1.527,1.5269,1.5267,1.5266,1.5264,1.5263,1.5262,1.526,1.5259,1.5257,1.5256,1.5254,1.5253,1.5252,1.525,1.5249,1.5247,1.5246,1.5244,1.5243,1.5241,1.524,1.5238,1.5237,1.5235,1.5233,1.5232,1.523,1.5229,1.5227,1.5226,1.5224,1.5222,1.5221,1.5219,1.5218,1.5216,1.5214,1.5213,1.5211,1.5209 };

Scene CreateCornellGold(MemoryArena& arena)
{
	UnpolarizedSpectrum::Init();

	auto t = ARENA_ALLOC(arena, Transform);
	auto wto = ARENA_ALLOC(arena, Transform);
	*t = Transform();
	*wto = Inverse(*t);

	auto t_light = ARENA_ALLOC(arena, Transform);
	auto wto_light = ARENA_ALLOC(arena, Transform);
	*t_light = Translate(Vector3f(213, 554, 227));
	*wto_light = Inverse(*t_light);

	auto mesh = CreateTriangleMesh(t, wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(t_light, wto_light, true, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);

	auto Red = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(red, SpectrumType::Reflectance));
	auto Green = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(green, SpectrumType::Reflectance));
	auto White = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(white, SpectrumType::Reflectance));

	auto red_material = make_shared<MatteMaterial>(Red, Zero, nullptr);
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);
	auto green_material = make_shared<MatteMaterial>(Green, Zero, nullptr);

	auto One = make_shared<ConstantTexture<UnpolarizedSpectrum>>(UnpolarizedSpectrum(1.0));

	auto gold_material = make_shared<MetalMaterial>(make_shared<ConstantTexture<UnpolarizedSpectrum>>(UnpolarizedSpectrum::FromSampled(gold_lambda, gold_eta, 111)), make_shared<ConstantTexture<UnpolarizedSpectrum>>(UnpolarizedSpectrum::FromSampled(gold_lambda, gold_k, 33)), make_shared<ConstantTexture<float>>(0.0), nullptr, nullptr, nullptr, false);
	//auto gold_material = make_shared<MetalMaterial>(make_shared<ConstantTexture<UnpolarizedSpectrum>>(UnpolarizedSpectrum::FromSampled(glass_lambda, glass_eta, 230)), make_shared<ConstantTexture<UnpolarizedSpectrum>>(Spectrum(0.f)), make_shared<ConstantTexture<float>>(0.0), nullptr, nullptr, nullptr, false);

	vector<shared_ptr<Light>> lights;

	auto white_light = BlackBodySpectrum(5000, 20);

	for (auto light_tri : light_mesh)
	{
		auto light_ptr = make_shared<DiffuseAreaLight>(*t_light, MediumInterface<Spectrum>(), Spectrum(white_light), 1, light_tri);
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
	UnpolarizedSpectrum::Init();

	auto t = ARENA_ALLOC(arena, Transform);
	auto wto = ARENA_ALLOC(arena, Transform);
	*t = Transform();
	*wto = Inverse(*t);

	auto t_light = ARENA_ALLOC(arena, Transform);
	auto wto_light = ARENA_ALLOC(arena, Transform);
	*t_light = Translate(Vector3f(213, 554, 227));
	*wto_light = Inverse(*t_light);
	auto mesh = CreateTriangleMesh(t, wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(t_light, wto_light, true, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);

	auto Red = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(red, SpectrumType::Reflectance));
	auto Green = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(green, SpectrumType::Reflectance));
	auto White = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(white, SpectrumType::Reflectance));

	auto red_material = make_shared<MatteMaterial>(Red, Zero, nullptr);
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);
	auto green_material = make_shared<MatteMaterial>(Green, Zero, nullptr);

	auto One = make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));
	auto glass_material = make_shared<GlassMaterial>(One, One, make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<Float>>(1.5), nullptr, true);

	vector<shared_ptr<Light>> lights;

	auto white_light = BlackBodySpectrum(5000, 20);

	for (auto light_tri : light_mesh)
	{
		auto light_ptr = make_shared<DiffuseAreaLight>(*t_light, MediumInterface<Spectrum>(), white_light, 1, light_tri);
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

	//primitives.push_back(make_shared<GeometricPrimitive>(sphere, white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(sphere, glass_material, nullptr));

	auto bvh = make_shared<BVHAccel>(primitives, 3, SplitMethod::Middle);

	//(+tilesize-1)/tileSize: max groups

	Scene scene(bvh, lights);
	return scene;
}

Point3f prism_points[] =
{
	Point3f(0  ,227.5,400),
	Point3f(0  ,327.5,400),
	Point3f(0  ,277.5,313.4),
	Point3f(555,227.5,400),
	Point3f(555,327.5,400),
	Point3f(555,277.5,313.4)
};

Point3f prism_points_[] =
{
	Point3f(400  ,-1  ,227.5),
	Point3f(400  ,-1  ,327.5),
	Point3f(313.4,-1  ,277.5),
	Point3f(400  ,255,227.5),
	Point3f(400  ,255,327.5),
	Point3f(313.4,255,277.5)
};

int prism_idx[] =
{
	0,2,1,
	3,4,5,
	0,2,3,
	2,5,3,
	1,5,2,
	1,4,5,
	3,1,0,
	4,1,3
};

Scene CreatePhysicalGlass(MemoryArena& arena)
{
	UnpolarizedSpectrum::Init();

	auto t = ARENA_ALLOC(arena, Transform);
	auto wto = ARENA_ALLOC(arena, Transform);
	*t = Scale(3, 3, 3);
	*t = *t * Translate(Vector3f(-177, 0, -177));
	*wto = Inverse(*t);

	auto light_t = ARENA_ALLOC(arena, Transform);
	auto light_wto = ARENA_ALLOC(arena, Transform);
	*light_t = Scale(3, 1, 0.2);
	*light_t = Translate(Vector3f(180, 600, 320)) * Rotate(0, Vector3f(1, 0, 0)) * *light_t;
	*light_wto = Inverse(*light_t);

	auto mesh = CreateTriangleMesh(t, wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(light_t, light_wto, true, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);
	auto White = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(white, SpectrumType::Reflectance));
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);

	auto glass_eta_spectrum = UnpolarizedSpectrum::FromSampled(glass_lambda, glass_eta, 230);

	auto One = make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));
	auto glass_material = make_shared<PhysicalGlassMaterial>(One, One, make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<Spectrum>>(glass_eta_spectrum), nullptr, false, 1);

	vector<shared_ptr<Light>> lights;

	auto white_light = BlackBodySpectrum(5300, 1.3);

	auto* identity = ARENA_ALLOC(arena, Transform);
	*identity = Transform();

	auto light = make_shared<DistantLight>(*identity, white_light, Normalize(Vector3f(0, 1, 0.33635)));
	lights.push_back(light);

	auto prism_t = ARENA_ALLOC(arena, Transform);
	auto prism_wto = ARENA_ALLOC(arena, Transform);
	*prism_t = Translate(Vector3f(200, 800, 0));
	*prism_wto = Inverse(*prism_t);

	auto glass_mesh = CreateTriangleMesh(prism_t, prism_wto, false, sizeof(prism_idx) / sizeof(int) / 3, prism_idx, 6, prism_points);

	primitives.push_back(make_shared<GeometricPrimitive>(mesh[8], white_material, nullptr));
	primitives.push_back(make_shared<GeometricPrimitive>(mesh[9], white_material, nullptr));

	for (auto&& tri : glass_mesh)
	{
		primitives.push_back(make_shared<GeometricPrimitive>(tri, glass_material, nullptr));
	}

	auto ball_t = ARENA_ALLOC(arena, Transform);
	auto ball_wto = ARENA_ALLOC(arena, Transform);

	*ball_t = Translate(Vector3f(90, 400, 490));
	*ball_wto = Inverse(*ball_t);

	auto sphere = make_shared<Sphere>(ball_t, ball_wto, false, 90, -1000, 1000, 360);

	primitives.push_back(make_shared<GeometricPrimitive>(sphere, glass_material, nullptr));

	auto bvh = make_shared<BVHAccel>(primitives, 3, SplitMethod::Middle);

	Scene scene(bvh, lights);
	return scene;
}

Scene CreateCornellPolarized(MemoryArena& arena)
{
	UnpolarizedSpectrum::Init();

	auto t = ARENA_ALLOC(arena, Transform);
	auto wto = ARENA_ALLOC(arena, Transform);
	*t = Transform();
	*wto = Inverse(*t);

	auto t_light = ARENA_ALLOC(arena, Transform);
	auto wto_light = ARENA_ALLOC(arena, Transform);
	*t_light = Translate(Vector3f(213, 554, 227));
	*wto_light = Inverse(*t_light);
	auto mesh = CreateTriangleMesh(t, wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(t_light, wto_light, true, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);

	auto Red = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(red, SpectrumType::Reflectance));
	auto Green = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(green, SpectrumType::Reflectance));
	auto White = make_shared<ConstantTexture<Spectrum>>(UnpolarizedSpectrum::FromRGB(white, SpectrumType::Reflectance));

	auto red_material = make_shared<MatteMaterial>(Red, Zero, nullptr);
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);
	auto green_material = make_shared<MatteMaterial>(Green, Zero, nullptr);

	auto One = make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));
	auto glass_material = make_shared<GlassMaterial>(One, One, make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<float>>(0), make_shared<ConstantTexture<Float>>(1.5), nullptr, true);

	vector<shared_ptr<Light>> lights;

	auto white_light = BlackBodySpectrum(5000, 20);

	for (auto light_tri : light_mesh)
	{
		auto light_ptr = make_shared<DiffuseAreaLight>(*t_light, MediumInterface<Spectrum>(), white_light, 1, light_tri);
		lights.push_back(light_ptr);
		primitives.push_back(make_shared<GeometricPrimitive>(light_tri, white_material, light_ptr));
	}
	auto gold_material = make_shared<MetalMaterial>(make_shared<ConstantTexture<UnpolarizedSpectrum>>(UnpolarizedSpectrum::FromSampled(gold_lambda, gold_eta, 111)), make_shared<ConstantTexture<UnpolarizedSpectrum>>(UnpolarizedSpectrum::FromSampled(gold_lambda, gold_k, 33)), make_shared<ConstantTexture<float>>(0.0), nullptr, nullptr, nullptr, false);
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

	*tb2 = Translate(Vector3f(180, 130, 190));
	*tb2 = *tb2 * Rotate(-18, Vector3f(0, 1, 0));
	*itb2 = Inverse(*tb2);

	//auto Box2 = CreateBox(&tb2, &Inverse(tb2), false, Point3f(0, 0, 0), Point3f(165, 165, 165));

	auto prism_t = ARENA_ALLOC(arena, Transform);
	auto prism_wto = ARENA_ALLOC(arena, Transform);
	*prism_t = Translate(Vector3f(0, 0, 0));
	*prism_wto = Inverse(*prism_t);

	auto prism_mesh = CreateTriangleMesh(prism_t, prism_wto, false, sizeof(prism_idx) / sizeof(int) / 3, prism_idx, 6, prism_points);

	auto sphere = make_shared<Sphere>(tb2, itb2, false, 90, -1000, 1000, 360);

	for (auto&& tri : prism_mesh)
	{
		primitives.push_back(make_shared<GeometricPrimitive>(tri, glass_material, nullptr));
	}

	//primitives.push_back(make_shared<GeometricPrimitive>(sphere, glass_material, nullptr));

	auto bvh = make_shared<BVHAccel>(primitives, 3, SplitMethod::Middle);

	//(+tilesize-1)/tileSize: max groups

	Scene scene(bvh, lights);
	return scene;
}
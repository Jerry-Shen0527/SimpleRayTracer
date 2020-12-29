#include <Geometry/TriangleMesh.h>
#include <Geometry/bvh.h>
#include <Integrators/DirectLightIntegrator.h>

#include "BRDF/Material.h"
#include "Tools/camera.h"
#include "Tools/Parallel.h"
#include "Tools/Sampler.h"

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
	Point3f(213,227,554),
	Point3f(213,343,554),
	Point3f(332,227,554),
	Point3f(332,343,554)
};

int light_indices[] =
{
	0,1,2,
	2,3,4
};

int main()
{
	SampledSpectrum::Init();
	auto t = Rotate(0, Vector3f(0, 1, 0));
	auto wto = Inverse(t);

	auto mesh = CreateTriangleMesh(&t, &wto, false, sizeof(indices) / sizeof(int) / 3, indices, 8, points);

	auto light_mesh = CreateTriangleMesh(&t, &wto, false, sizeof(light_indices) / sizeof(int) / 3, light_indices, 8, light_points);

	using namespace  std;
	vector<shared_ptr<Primitive>> primitives;

	auto Zero = make_shared<ConstantTexture<Float>>(0);

	auto Red = make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(red));
	auto Green = make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(green));
	auto White = make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(white));

	auto red_material = make_shared<MatteMaterial>(Red, Zero, nullptr);
	auto white_material = make_shared<MatteMaterial>(White, Zero, nullptr);
	auto green_material = make_shared<MatteMaterial>(Green, Zero, nullptr);

	auto light_t = Transform();

	vector<shared_ptr<Light>> lights;

	for (auto light_tri : light_mesh)
	{
		lights.push_back(make_shared<DiffuseAreaLight>(light_t, MediumInterface(), Spectrum::FromRGB(white), 1, light_tri));
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


	BVHAccel bvh(primitives, 3, SplitMethod::SAH);

	Film film(Point2i(400, 400), Bounds2f(Point2f(0, 0), Point2f(1, 1)), std::make_unique<BoxFilter>(Vector2f(0.5, 0.5)), 1., "test.png", 1.);
	Transform trans = Translate(Vector3f(277.5, 277.5, -800));
	AnimatedTransform transform(&trans, 0, &trans, 0);

	auto camera = make_shared<PerspectiveCamera>(transform, Bounds2f(Point2f(-1, -1), Point2f(1, 1)), 0, 1.0, 0, 10.0, 40.0, &film, nullptr);

	shared_ptr<Sampler> sampler = make_shared<StratifiedSampler>(2, 2, true, 2);

	Bounds2i sampleBounds = camera->film->GetSampleBounds();
	Vector2i sampleExtent = sampleBounds.Diagonal();
	const int tileSize = 16;
	//(+tilesize-1)/tileSize: max groups
	Point2i nTiles((sampleExtent.x() + tileSize - 1) / tileSize, (sampleExtent.y() + tileSize - 1) / tileSize);

	DirectLightingIntegrator integrator(LightStrategy::UniformSampleAll, 50, camera, sampler, Bounds2i(600, 600));

	Scene scene(shared_ptr<Primitive>(&bvh), lights);

	integrator.Render(scene);

	camera->film->WriteImage(1, true);
}
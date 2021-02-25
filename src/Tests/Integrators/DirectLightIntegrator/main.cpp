#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

#include <Integrators/DirectLightIntegrator.h>
#include <Tools/MemoryArena.h>


#include "BRDF/Material.h"
#include "Tools/camera.h"
#include "Tools/Parallel.h"
#include "Tools/Sampler.h"
#include "Tools/Scenes.h"

int main()
{
	Film film(Point2i(1000, 1000), Bounds2f(Point2f(0, 0), Point2f(1, 1)), std::make_unique<BoxFilter>(Vector2f(0.5, 0.5)), 1., "test.png", 1.);
	Transform trans = Translate(Vector3f(277.5, 277.5, -800));
	AnimatedTransform transform(&trans, 0, &trans, 0);

	auto camera = std::make_shared<PerspectiveCamera>(transform, Bounds2f(Point2f(-1, -1), Point2f(1, 1)), 0, 1.0, 0, 10.0, 40.0, &film, nullptr);

	shared_ptr<Sampler> sampler = std::make_shared<StratifiedSampler>(6, 6, true, 10);

	DirectLightingIntegrator integrator(LightStrategy::UniformSampleAll, 1, camera, sampler);
	MemoryArena arena;

	Scene scene = CreateCornell(arena);
	integrator.Render(scene,true);

	camera->film->WriteImage(1, true);

	arena.Reset();
#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}
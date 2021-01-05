#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif
#include <memory>

#include "Integrators/PathIntegrator.h"
#include "Tools/Scenes.h"

int main()
{
	const auto scene = CreateCornell();
	const std::shared_ptr<Sampler> sampler = std::make_shared<StratifiedSampler>(7, 7, true, 10);

	Transform trans = Translate(Vector3f(277.5, 277.5, -800));
	AnimatedTransform transform(&trans, 0, &trans, 0);

	Film film(Point2i(1000, 1000), Bounds2f(Point2f(0, 0), Point2f(1, 1)), std::make_unique<BoxFilter>(Vector2f(1, 1)), 1., "test.png", 1.);

	auto camera = std::make_shared<PerspectiveCamera>(transform, Bounds2f(Point2f(-1, -1), Point2f(1, 1)), 0, 1.0, 0, 10.0, 40.0, &film, nullptr);

	PathIntegrator integrator(20, camera, sampler);

	integrator.Render(scene);

	camera->film->WriteImage(1, true);

#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}
#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

#include <memory>
#include "Integrators/SPPM.h"
#include "Tools/camera.h"
#include "Tools/Parallel.h"
#include "Tools/Scenes.h"

int main()
{
	Film film(Point2i(1000, 1000), Bounds2f(Point2f(0, 0), Point2f(1, 1)), std::make_unique<BoxFilter>(Vector2f(1.5, 1.5)), 1., "test.png", 1.);
	Transform trans = Translate(Vector3f(277.5, 277.5, -800));
	AnimatedTransform transform(&trans, 0, &trans, 0);

	shared_ptr<const Camera> camera = std::make_shared<PerspectiveCamera>(transform, Bounds2f(Point2f(-1, -1), Point2f(1, 1)), 0, 1.0, 0, 10.0, 40.0, &film, nullptr);

	SPPMIntegrator integrator(camera, 20, 10000000, 40, 1., 10);

	Scene scene = CreateCornell();

	integrator.Render(scene);

#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}
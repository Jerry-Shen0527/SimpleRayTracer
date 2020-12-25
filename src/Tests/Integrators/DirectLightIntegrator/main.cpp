#include <memory>
#include <Integrators/DirectLightIntegrator.h>

#include "Tools/Filter.h"

int main()
{
	Film film(Point2i(600, 600), Bounds2f(1.0, 1.0), std::make_unique<BoxFilter>(Vector2f(1.0, 1.0)), 1, "test", 1);
	Transform t = Translate(Vector3f(278, 278, -800));

	AnimatedTransform transform(&t, 0, &t, 0);
	PerspectiveCamera camera(transform, Bounds2f(Point2f(1, 1)), 0, 1.0, 1, 10.0, 40.0, &film, nullptr);
	StratifiedSampler sampler(10, 10, true, 10);
	DirectLightingIntegrator integrator(LightStrategy::UniformSampleOne, 50, std::make_shared<Camera>(camera), std::make_shared<Sampler>(sampler), Bounds2i(600, 600));

	DiffuseAreaLight light(Translate());
	
	Scene scene;
	
	integrator.Render()
}
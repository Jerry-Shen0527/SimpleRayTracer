#pragma once
#include "Geometry/Primitive.h"
#include "Tools/Light/Light.h"

class Scene {
public:
	// Scene Public Methods
	Scene(std::shared_ptr<Primitive> aggregate, const std::vector<std::shared_ptr<Light>>& lights) : lights(lights), aggregate(aggregate)
	{
		// Scene Constructor Implementation
		worldBound = aggregate->WorldBound();
		for (const auto& light : lights) {
			light->Preprocess(*this);
			if (light->flags & (int)LightFlags::Infinite)
				infiniteLights.push_back(light);
		}
	}
	const Bounds3f& WorldBound() const { return worldBound; }
	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool IntersectP(const Ray& ray) const;

	// Scene Public Data
	std::vector<std::shared_ptr<Light>> lights;
	// Store infinite light sources separately for cases where we only want
	// to loop over them.
	std::vector<std::shared_ptr<Light>> infiniteLights;

private:
	// Scene Private Data
	std::shared_ptr<Primitive> aggregate;
	Bounds3f worldBound;
};

inline bool Scene::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	return aggregate->Intersect(ray, isect);
}

inline bool Scene::IntersectP(const Ray& ray) const
{
	return aggregate->IntersectP(ray);
}

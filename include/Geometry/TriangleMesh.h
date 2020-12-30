#pragma once
#include <Geometry/Shape.h>

#include "Tools/Texture.h"

struct TriangleMesh {
	TriangleMesh::TriangleMesh(const Transform& ObjectToWorld,
		int nTriangles, const int* vertexIndices, int nVertices,
		const Point3f* P, const Vector3f* S, const Normal3f* N,
		const Point2f* UV,
		const std::shared_ptr<Texture<Float>>& alphaMask)
		: nTriangles(nTriangles), nVertices(nVertices),
		vertexIndices(vertexIndices, vertexIndices + 3 * nTriangles),
		alphaMask(alphaMask) {
		//Transform mesh vertices to world space 155
		p.reset(new Point3f[nVertices]);
		for (int i = 0; i < nVertices; ++i)
			p[i] = ObjectToWorld(P[i]);
		//Copy UV, N, and S vertex data, if present
			// Copy _UV_, _N_, and _S_ vertex data, if present
		if (UV) {
			uv.reset(new Point2f[nVertices]);
			memcpy(uv.get(), UV, nVertices * sizeof(Point2f));
		}
		if (N) {
			n.reset(new Normal3f[nVertices]);
			for (int i = 0; i < nVertices; ++i) n[i] = ObjectToWorld(N[i]);
		}
		if (S) {
			s.reset(new Vector3f[nVertices]);
			for (int i = 0; i < nVertices; ++i) s[i] = ObjectToWorld(S[i]);
		}
	}

	const int nTriangles, nVertices;
	std::vector<int> vertexIndices;
	std::unique_ptr<Point3f[]> p;
	std::unique_ptr<Normal3f[]> n;
	std::unique_ptr<Vector3f[]> s;
	std::unique_ptr<Point2f[]> uv;
	std::shared_ptr<Texture<Float>> alphaMask;
};

class Triangle : public Shape {
public:
	Triangle(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation,
		const std::shared_ptr<TriangleMesh>& mesh, int triNumber);

	bool Intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect, bool testAlphaTexture) const override;

	Bounds3f ObjectBound() const {
		//Get triangle vertices in p0, p1, and p2 157
		const Point3f& p0 = mesh->p[v[0]];
		const Point3f& p1 = mesh->p[v[1]];
		const Point3f& p2 = mesh->p[v[2]];

		return Union(Bounds3f((*WorldToObject)(p0), (*WorldToObject)(p1)), (*WorldToObject)(p2));
	}

	Bounds3f WorldBound() const {
		//Get triangle vertices in p0, p1, and p2 157
		const Point3f& p0 = mesh->p[v[0]];
		const Point3f& p1 = mesh->p[v[1]];
		const Point3f& p2 = mesh->p[v[2]];
		return Union(Bounds3f(p0, p1), p2);
	}

	Float Area() const override;

private:
	std::shared_ptr<TriangleMesh> mesh;
	const int* v;

	void GetUVs(Point2f uv[3]) const;
public:
	Interaction Sample(const Point2f& u, Float* pdf) const override;
};


inline std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation, int nTriangles, const int* vertexIndices, int nVertices, const Point3f* p,
	const Vector3f* s = nullptr, const Normal3f* n = nullptr, const Point2f* uv = nullptr,
	const std::shared_ptr<Texture<Float>>& alphaMask = nullptr)
{
	std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(*ObjectToWorld, nTriangles, vertexIndices, nVertices, p, s, n, uv, alphaMask);
	std::vector<std::shared_ptr<Shape>> tris;
	for (int i = 0; i < nTriangles; ++i)
		tris.push_back(std::make_shared<Triangle>(ObjectToWorld, WorldToObject, reverseOrientation, mesh, i));
	return tris;
}
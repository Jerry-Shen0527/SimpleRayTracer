#pragma once
#include <Geometry/Shape.h>

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

	std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(
		const Transform* ObjectToWorld, const Transform* WorldToObject,
		bool reverseOrientation, int nTriangles,
		const int* vertexIndices, int nVertices, const Point3f* p,
		const Vector3f* s, const Normal3f* n, const Point2f* uv,
		const std::shared_ptr<Texture<Float>>& alphaMask) {
		std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(
			*ObjectToWorld, nTriangles, vertexIndices, nVertices, p, s, n, uv,
			alphaMask);
		std::vector<std::shared_ptr<Shape>> tris;
		for (int i = 0; i < nTriangles; ++i)
			tris.push_back(std::make_shared<Triangle>(ObjectToWorld,
				WorldToObject, reverseOrientation, mesh, i));
		return tris;
	}

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
};

inline Triangle::Triangle(const Transform* ObjectToWorld, const Transform* WorldToObject, bool reverseOrientation, const std::shared_ptr<TriangleMesh>& mesh, int triNumber) :
	Shape(ObjectToWorld, WorldToObject, reverseOrientation), mesh(mesh)
{
	v = &mesh->vertexIndices[3 * triNumber];
}

inline bool Triangle::Intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect, bool testAlphaTexture) const
{
	//Get triangle vertices in p0, p1, and p2 157
	const Point3f& p0 = mesh->p[v[0]];
	const Point3f& p1 = mesh->p[v[1]];
	const Point3f& p2 = mesh->p[v[2]];
	//	Perform ray¨Ctriangle intersection test 158
	Point3f p0t = p0 - Vector3f(ray.orig);
	Point3f p1t = p1 - Vector3f(ray.orig);
	Point3f p2t = p2 - Vector3f(ray.orig);
	//Transform triangle vertices to ray coordinate space 158
	int kz = MaxDimension(Abs(ray.d));
	int kx = kz + 1;
	if (kx == 3) kx = 0;
	int ky = kx + 1;
	if (ky == 3) ky = 0;
	Vector3f d = Permute(ray.d, kx, ky, kz);
	p0t = Permute(p0t, kx, ky, kz);
	p1t = Permute(p1t, kx, ky, kz);
	p2t = Permute(p2t, kx, ky, kz);
	Float Sx = -d.x() / d.z;
	Float Sy = -d.y / d.z;
	Float Sz = 1.f / d.z;
	p0t.x() += Sx * p0t.z;
	p0t.y += Sy * p0t.z;
	p1t.x() += Sx * p1t.z;
	p1t.y += Sy * p1t.z;
	p2t.x() += Sx * p2t.z;
	p2t.y += Sy * p2t.z;
	//	Compute edge function coefficients e0, e1, and e2 161
	Float e0 = p1t.x() * p2t.y - p1t.y * p2t.x();
	Float e1 = p2t.x() * p0t.y - p2t.y * p0t.x();
	Float e2 = p0t.x() * p1t.y - p0t.y * p1t.x();
	//	Fall back to double - precision test at triangle edges
	//	Perform triangle edge and determinant tests 162
	if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
		return false;
	Float det = e0 + e1 + e2;
	if (det == 0)
		return false;
	//	Compute scaled hit distance to triangle and test against ray t range 162
	p0t.z *= Sz;
	p1t.z *= Sz;
	p2t.z *= Sz;
	Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
	if (det < 0 && (tScaled >= 0 || tScaled < ray.tMax * det))
		return false;
	else if (det > 0 && (tScaled <= 0 || tScaled > ray.tMax * det))
		return false;
	//	Compute barycentric coordinates and t value for triangle intersection 163
	Float invDet = 1 / det;
	Float b0 = e0 * invDet;
	Float b1 = e1 * invDet;
	Float b2 = e2 * invDet;
	Float t = tScaled * invDet;
	//	Ensure that computed triangle t is conservatively greater than zero 234
	//	Compute triangle partial derivatives 164
	Vector3f dpdu, dpdv;
	Point2f uv[3];
	GetUVs(uv);
	//Compute deltas for triangle partial derivatives 164
	Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
	Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
	Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
	if (determinant == 0)
	{
		//Handle zero determinant for triangle partial derivative matrix 164
		CoordinateSystem(Normalize(Cross(p2 - p0, p1 - p0)), &dpdu, &dpdv);
	}
	else
	{
		Float invdet = 1 / determinant;
		dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
		dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
	}
	//	Compute error bounds for triangle intersection 227
	//	Interpolate(u, v) parametric coordinates and hit point 164
	Point3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
	Point2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
	//	Test intersection against alpha texture, if present 165
	if (testAlphaTexture && mesh->alphaMask) {
		SurfaceInteraction isectLocal(pHit, Vector3f(0, 0, 0), uvHit,
			Vector3f(0, 0, 0), dpdu, dpdv, Normal3f(0, 0, 0), Normal3f(0, 0, 0),
			ray.time, this);
		if (mesh->alphaMask->Evaluate(isectLocal) == 0)
			return false;
	}
	//	Fill in SurfaceInteraction from triangle hit 165
	*isect = SurfaceInteraction(pHit, pError, uvHit, -ray.d, dpdu, dpdv, Normal3f(0, 0, 0), Normal3f(0, 0, 0), ray.time, this);
	//Override surface normal in isect for triangle 165
	if (mesh->n || mesh->s) {
		//Initialize Triangle shading geometry 166
		Normal3f ns;
		if (mesh->n)
			ns = Normalize(b0 * mesh->n[v[0]] + b1 * mesh->n[v[1]] + b2 * mesh->n[v[2]]);
		else
			ns = isect->n;
		Vector3f ss;
		if (mesh->s) ss = Normalize(b0 * mesh->s[v[0]] +
			b1 * mesh->s[v[1]] +
			b2 * mesh->s[v[2]]);
		else
			ss = Normalize(isect->dpdu);
		Vector3f ts = Cross(ss, ns);
		if (ts.LengthSquared() > 0.f) {
			ts = Normalize(ts);
			ss = Cross(ts, ns);
		}
		else
			CoordinateSystem((Vector3f)ns, &ss, &ts);
		isect->SetShadingGeometry(ss, ts, dndu, dndv, true);
	}
	//Ensure correct orientation of the geometric normal 166

	*tHit = t;
	return true;
}

inline Float Triangle::Area() const
{
	const Point3f& p0 = mesh->p[v[0]];
	const Point3f& p1 = mesh->p[v[1]];
	const Point3f& p2 = mesh->p[v[2]];
	return 0.5 * Cross(p1 - p0, p2 - p0).length();
}

inline void Triangle::GetUVs(Point2f uv[3]) const
{
	if (mesh->uv)
	{
		uv[0] = mesh->uv[v[0]];
		uv[1] = mesh->uv[v[1]];
		uv[2] = mesh->uv[v[2]];
	}
	else
	{
		uv[0] = Point2f(0, 0);
		uv[1] = Point2f(1, 0);
		uv[2] = Point2f(1, 1);
	}
}

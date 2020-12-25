#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <vector>

#include <Tools/MemoryArena.h>
#include "Tools/Bound.h"

#include "Geometry/Primitive.h"

struct SurfaceInteraction;

enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

struct BVHPrimitiveInfo {
	BVHPrimitiveInfo() {}
	BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f& bounds)
		: primitiveNumber(primitiveNumber), bounds(bounds),
		centroid(.5f * bounds.pMin + .5f * bounds.pMax) { }
	size_t primitiveNumber;
	Bounds3f bounds;
	Point3f centroid;
};

struct LinearBVHNode {
	Bounds3f bounds;
	union {
		int primitivesOffset;   // leaf
		int secondChildOffset;  // interior
	};
	uint16_t nPrimitives;  // 0 -> interior node
	uint8_t axis;          // interior node: xyz
	uint8_t pad[1];        // ensure 32 byte total size
};

struct BVHBuildNode {
	//BVHBuildNode Public Methods 258
	void InitLeaf(int first, int n, const Bounds3f& b) {
		firstPrimOffset = first;
		nPrimitives = n;
		bounds = b;
		children[0] = children[1] = nullptr;
	}

	void InitInterior(int axis, BVHBuildNode* c0, BVHBuildNode* c1) {
		children[0] = c0;
		children[1] = c1;
		bounds = Union(c0->bounds, c1->bounds);
		splitAxis = axis;
		nPrimitives = 0;
	}

	Bounds3f bounds;
	BVHBuildNode* children[2];
	int splitAxis, firstPrimOffset, nPrimitives;
};

struct BucketInfo {
	int count = 0;
	Bounds3f bounds;
};

class BVHAccel : public Aggregate
{
public:
	BVHAccel(const std::vector<std::shared_ptr<Primitive>>& p, int maxPrimsInNode, SplitMethod splitMethod);

	BVHBuildNode* recursiveBuild(MemoryArena& arena,
	                             std::vector<BVHPrimitiveInfo>& primitiveInfo, int start,
	                             int end, int* totalNodes,
	                             std::vector<std::shared_ptr<Primitive>>& orderedPrims);

	Bounds3f WorldBound() const override;
	bool Intersect(const Ray& r, SurfaceInteraction*) const override;
	bool IntersectP(const Ray& r) const override;
	int flattenBVHTree(BVHBuildNode* node, int* offset);
private:
	const int maxPrimsInNode;
	const SplitMethod splitMethod;
	std::vector<std::shared_ptr<Primitive>> primitives;
	LinearBVHNode* nodes = nullptr;
};

#endif
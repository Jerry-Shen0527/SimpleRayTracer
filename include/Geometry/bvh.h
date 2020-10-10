#ifndef BVH_H
#define BVH_H

#include <ray.h>
#include <vector>

#include "Primitive.h"
#include "Tools/Bound.h"

#undef min
#undef max

struct surface_hit_record;
#include "hittable.h"

class aabb {
public:
	aabb() {}
	aabb(const Point3f& a, const Point3f& b) { _min = a; _max = b; }

	Point3f min() const { return _min; }
	Point3f max() const { return _max; }

	bool hit(const Ray& r) const;

	Point3f _min;
	Point3f _max;
};

class hittable_list;
//this serves as a decorator for simple hittable list
class bvh_node : public hittable
{
public:

	bvh_node(hittable_list& list, float time0, float time1);

	bvh_node(
		std::vector<std::shared_ptr<hittable>>& objects,
		size_t start, size_t end, float time0, float time1);

	virtual bool hit(const Ray& r, surface_hit_record& rec) const override;

	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;

public:
	std::shared_ptr<hittable> left;
	std::shared_ptr<hittable> right;
	aabb box;
};

enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

struct BVHPrimitiveInfo {
	BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f& bounds)
		: primitiveNumber(primitiveNumber), bounds(bounds),
		centroid(.5f * bounds.pMin + .5f * bounds.pMax) { }
	size_t primitiveNumber;
	Bounds3f bounds;
	Point3f centroid;
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

class BVHAccel
{
public:
	BVHAccel(const std::vector<std::shared_ptr<Primitive>>& p,
		int maxPrimsInNode, SplitMethod splitMethod)
		: maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p),
		splitMethod(splitMethod) {
		if (primitives.size() == 0)
			return;
		//Build BVH from primitives 257
		std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
		for (size_t i = 0; i < primitives.size(); ++i)
			primitiveInfo[i] = { i, primitives[i]->WorldBound() };

		MemoryArena arena(1024 * 1024);
		int totalNodes = 0;
		std::vector<std::shared_ptr<Primitive>> orderedPrims;
		BVHBuildNode* root;
		if (splitMethod == SplitMethod::HLBVH)
			root = HLBVHBuild(arena, primitiveInfo, &totalNodes, orderedPrims);
		else
			root = recursiveBuild(arena, primitiveInfo, 0, primitives.size(),
				&totalNodes, orderedPrims);
		primitives.swap(orderedPrims);
	}

	BVHBuildNode* recursiveBuild(MemoryArena& arena,
		std::vector<BVHPrimitiveInfo>& primitiveInfo, int start,
		int end, int* totalNodes,
		std::vector<std::shared_ptr<Primitive>>& orderedPrims) {
		BVHBuildNode* node = arena.Alloc<BVHBuildNode>();
		(*totalNodes)++;
		//Compute bounds of all primitives in BVH node 260
		Bounds3f bounds;
		for (int i = start; i < end; ++i)
			bounds = Union(bounds, primitiveInfo[i].bounds);
		int nPrimitives = end - start;
		if (nPrimitives == 1) {
			//Create leaf BVHBuildNode 260
			int firstPrimOffset = orderedPrims.size();
			for (int i = start; i < end; ++i) {
				int primNum = primitiveInfo[i].primitiveNumber;
				orderedPrims.push_back(primitives[primNum]);
			}
			node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
			return node;
		}
		else {
			//Compute bound of primitive centroids, choose split dimension dim 261
			Bounds3f centroidBounds;
			for (int i = start; i < end; ++i)
				centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
			int dim = centroidBounds.MaximumExtent();
			//Partition primitives into two sets and build children 261
			int mid = (start + end) / 2;
			if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {
				//Create leaf BVHBuildNode 260
				int firstPrimOffset = orderedPrims.size();
				for (int i = start; i < end; ++i) {
					int primNum = primitiveInfo[i].primitiveNumber;
					orderedPrims.push_back(primitives[primNum]);
				}
				node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
				return node;
			}
			else {
				//Partition primitives based on splitMethod
				Float pmid = (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]) / 2;
				BVHPrimitiveInfo* midPtr =
					std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
						[dim, pmid](const BVHPrimitiveInfo& pi) {
							return pi.centroid[dim] < pmid;
						});
				mid = midPtr - &primitiveInfo[0];
				if (mid != start && mid != end)
					break;
				node->InitInterior(dim,
					recursiveBuild(arena, primitiveInfo, start, mid,
						totalNodes, orderedPrims),
					recursiveBuild(arena, primitiveInfo, mid, end,
						totalNodes, orderedPrims));
			}
		}
		return node;
	}

private:
	const int maxPrimsInNode;
	const SplitMethod splitMethod;
	std::vector<std::shared_ptr<Primitive>> primitives;
};

#endif
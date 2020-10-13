#include <algorithm>
#include <Geometry/bvh.h>
#include <Geometry/hittable_list.h>

aabb surrounding_box(aabb box0, aabb box1);
using std::shared_ptr;
bool bvh_node::bounding_box(float t0, float t1, aabb& output_box) const {
	output_box = box;
	return true;
}

bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
	aabb box_a;
	aabb box_b;
#undef min
	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min()[axis] < box_b.min()[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 2);
}

aabb surrounding_box(aabb box0, aabb box1) {
	Point3f small(fmin(box0.min().x(), box1.min().x()),
		fmin(box0.min().y(), box1.min().y()),
		fmin(box0.min().z(), box1.min().z()));

	Point3f big(fmax(box0.max().x(), box1.max().x()),
		fmax(box0.max().y(), box1.max().y()),
		fmax(box0.max().z(), box1.max().z()));

	return aabb(small, big);
}

bvh_node::bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end, float time0, float time1) {
	int axis = random_int(0, 2);//select one axis
	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare
		: box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {
		left = right = objects[start];
	}
	else if (object_span == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);
	}

	aabb box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	box = surrounding_box(box_left, box_right);
}

bvh_node::bvh_node(hittable_list& list, float time0, float time1) : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
{}

bool bvh_node::hit(const Ray& r, SurfaceInteraction& rec) const {
	if (!box.hit(r))
		return false;

	bool hit_left = left->hit(r, rec);
	bool hit_right;
	if (hit_left)
	{
		auto r_temp = r;
		r_temp.o = r.at(rec.t);
		hit_right = right->hit(r_temp, rec);
	}
	else
	{
		hit_right = right->hit(r, rec);
	}

	return hit_left || hit_right;
}

inline bool aabb::hit(const Ray& r) const {
	for (int a = 0; a < 3; a++) {
		auto invD = 1.0f / r.direction()[a];
		auto t0 = (min()[a] - r.origin()[a]) * invD;
		auto t1 = (max()[a] - r.origin()[a]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);
		auto tmin = t0;
		auto tmax = t1 < r.tMax ? t1 : r.tMax;
		if (tmax <= tmin)
			return false;
	}
	return true;
}
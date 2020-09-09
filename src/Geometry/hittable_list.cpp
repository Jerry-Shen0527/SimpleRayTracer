#include <Geometry/hittable_list.h>
#include <Geometry/bvh.h>

void hittable_list::add(shared_ptr<hittable> object)
{
	objects.push_back(object);
	if (object->get_pdf_enabled())
	{
		pdf_objects.push_back(object);
	}
}

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for (const auto& object : objects) {
		if (object->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

inline bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const
{
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;

	for (const auto& object : objects) {
		if (!object->bounding_box(t0, t1, temp_box)) return false;
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
		first_box = false;
	}

	return true;
}

double hittable_list::pdf_value(const point3& o, const vec3& v) const {
	auto weight = 1.0 / pdf_objects.size();
	auto sum = 0.0;

	for (const auto& object : pdf_objects)
		sum += weight * object->pdf_value(o, v);

	return sum;
}

vec3 hittable_list::random(const vec3& o) const {
	auto int_size = static_cast<int>(pdf_objects.size());
	return pdf_objects[random_int(0, int_size - 1)]->random(o);
}
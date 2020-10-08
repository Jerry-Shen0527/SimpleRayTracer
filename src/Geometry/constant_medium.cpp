#include <memory>
#include <Geometry/constant_medium.h>

constant_medium::constant_medium(std::shared_ptr<hittable> b, float d, std::shared_ptr<texture> a) : boundary(b),
	neg_inv_density(-1 / d), phase_function(std::make_shared<isotropic>(a))
{
}

constant_medium::constant_medium(std::shared_ptr<hittable> b, float d, Color c) : boundary(b), neg_inv_density(-1 / d), phase_function(std::make_shared<isotropic>(c))
{
}

bool constant_medium::hit(const ray& r, surface_hit_record& rec) const {
	// Print occasional samples when debugging. To enable, set enableDebug true.
	const bool enableDebug = false;
	const bool debugging = enableDebug && random_float() < 0.00001;

	surface_hit_record rec1, rec2;

	if (!boundary->hit(r, rec1))
		return false;

	auto r_temp = r;
	r_temp.orig = r.at(rec1.t);
	if (!boundary->hit(r_temp, rec2))
		return false;

	if (debugging) std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

	//if (rec1.t < t_min) rec1.t = t_min;
	//if (rec2.t > t_max) rec2.t = t_max;

	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)
		rec1.t = 0;

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	const auto hit_distance = neg_inv_density * log(random_float());

	if (hit_distance > distance_inside_boundary)
		return false;

	rec.t = rec1.t + hit_distance / ray_length;
	rec.p = r.at(rec.t);

	if (debugging) {
		std::cerr << "hit_distance = " << hit_distance << '\n'
			<< "rec.t = " << rec.t << '\n'
			<< "rec.p = " << rec.p << '\n';
	}

	rec.normal = Vector3f(1, 0, 0);  // arbitrary
	rec.front_face = true;     // also arbitrary
	rec.mat_ptr = phase_function;

	return true;
}

bool constant_medium::bounding_box(float t0, float t1, aabb& output_box) const
{
	return boundary->bounding_box(t0, t1, output_box);
}
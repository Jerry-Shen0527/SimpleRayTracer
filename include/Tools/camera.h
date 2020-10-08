#ifndef CAMERA_H
#define CAMERA_H

#include <ray.h>

#include "Film.h"

class camera {
public:

	camera() {}

	camera(
		point3 lookfrom,
		point3 lookat,
		Vector3f   vup,
		float vfov, // vertical field-of-view in degrees
		float aspect_ratio,
		float aperture,
		float focus_dist,
		float t0 = 0,
		float t1 = 0,
		shared_ptr<Film> _film = nullptr
	)
	{
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

		lens_radius = aperture / 2;

		time0 = t0;
		time1 = t1;

		film = _film;
	}

	ray get_ray(float s, float t) const {
		Vector3f rd = lens_radius * random_in_unit_disk();
		Vector3f offset = u * rd.x() + v * rd.y();

		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, infinity, random_float(time0, time1));
	}

	shared_ptr<Film> film;

private:
	float time0;
	float time1;
	point3 origin;
	point3 lower_left_corner;
	Vector3f horizontal;
	Vector3f vertical;
	float lens_radius;

	Vector3f w, u, v;
};

#endif

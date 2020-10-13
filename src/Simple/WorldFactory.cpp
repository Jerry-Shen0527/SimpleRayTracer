#include <Scene/WorldFactory.h>

hittable_list WorldFactory::random_scene() {
	hittable_list world;

	auto checker = make_shared<checker_texture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
	world.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	auto ground_material = make_shared<lambertian>(Color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_float();
			Point3f center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

			if ((center - Point3f(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = Color::random() * Color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = Color::random(0.5, 1);
					auto fuzz = random_float(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(Point3f(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(Color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(Point3f(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(Point3f(4, 1, 0), 1.0, material3));

	return hittable_list(make_shared<bvh_node>(world, 0, 0));
	//return  world;
}

hittable_list WorldFactory::two_spheres() {
	hittable_list objects;

	auto checker = make_shared<checker_texture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

	objects.add(make_shared<sphere>(Point3f(0, -10, 0), 10, make_shared<lambertian>(checker)));
	objects.add(make_shared<sphere>(Point3f(0, 10, 0), 10, make_shared<lambertian>(checker)));

	return objects;
}

hittable_list WorldFactory::two_perlin_spheres() {
	hittable_list objects;

	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(Point3f(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	return objects;
}

hittable_list WorldFactory::earth() {
	auto earth_texture = make_shared<image_texture>("../resources/earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(Point3f(0, 0, 0), 2, earth_surface);

	return hittable_list(globe);
}

hittable_list WorldFactory::simple_light() {
	hittable_list objects;

	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(Point3f(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	auto difflight = make_shared<diffuse_light>(Color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

	objects.add(make_shared<sphere>(Point3f(0, 6, 0), 1, difflight));

	return objects;
}

hittable_list WorldFactory::cornell_box() {
	hittable_list world;

	auto red = make_shared<lambertian>(Color(.65, .05, .05));
	auto white = make_shared<lambertian>(Color(.73, .73, .73));
	auto green = make_shared<lambertian>(Color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(Color(15, 15, 15));

	world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	world.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light, true)));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	//shared_ptr<material> aluminum = make_shared<metal>(Color(0.8, 0.85, 0.88), 0.0);
	shared_ptr<hittable> box1 = make_shared<box>(Point3f(0, 0, 0), Point3f(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, Vector3f(265, 0, 295));
	world.add(box1);

	//shared_ptr<hittable> box2 = make_shared<box>(Point3f(0, 0, 0), Point3f(165, 165, 165), white);
	//box2 = make_shared<rotate_y>(box2, -18);
	//box2 = make_shared<translate>(box2, Vector3f(130, 0, 65));
	//world.add(box2);
	shared_ptr<material> glass = make_shared<dielectric>(1.5);
	shared_ptr<hittable> glass_sphere = make_shared<sphere>(Point3f(190, 90, 190), 90.0, glass);

	world.add(glass_sphere);

	return world;
}

hittable_list WorldFactory::cornell_smoke() {
	hittable_list world;

	auto red = make_shared<lambertian>(Color(.65, .05, .05));
	auto white = make_shared<lambertian>(Color(.73, .73, .73));
	auto green = make_shared<lambertian>(Color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(Color(15, 15, 15));

	world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	world.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light, true)));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(Point3f(0, 0, 0), Point3f(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, Vector3f(265, 0, 295));

	shared_ptr<hittable> box2 = make_shared<box>(Point3f(0, 0, 0), Point3f(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, Vector3f(130, 0, 65));

	world.add(make_shared<constant_medium>(box1, 0.01, Color(0, 0, 0)));
	world.add(make_shared<constant_medium>(box2, 0.01, Color(1, 1, 1)));

	return world;
}

hittable_list WorldFactory::final_scene() {
	hittable_list boxes1;
	auto ground = make_shared<lambertian>(Color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_float(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<box>(Point3f(x0, y0, z0), Point3f(x1, y1, z1), ground));
		}
	}

	hittable_list objects;

	objects.add(make_shared<bvh_node>(boxes1, 0, 1));

	auto light = make_shared<diffuse_light>(Color(7, 7, 7));
	objects.add(make_shared<flip_face>(make_shared<xz_rect>(123, 423, 147, 412, 554, light, true)));

	auto center1 = Point3f(400, 400, 200);
	auto center2 = center1 + Vector3f(30, 0, 0);
	auto moving_sphere_material = make_shared<lambertian>(Color(0.7, 0.3, 0.1));
	objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

	objects.add(make_shared<sphere>(Point3f(260, 150, 45), 50, make_shared<dielectric>(1.5)));
	objects.add(make_shared<sphere>(
		Point3f(0, 150, 145), 50, make_shared<metal>(Color(0.8, 0.8, 0.9), 10.0)
		));

	auto boundary = make_shared<sphere>(Point3f(360, 150, 145), 70, make_shared<dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constant_medium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
	boundary = make_shared<sphere>(Point3f(0, 0, 0), 5000, make_shared<dielectric>(1.5));
	objects.add(make_shared<constant_medium>(boundary, .0001, Color(1, 1, 1)));

	auto emat = make_shared<lambertian>(make_shared<image_texture>(path + "earthmap.jpg"));
	objects.add(make_shared<sphere>(Point3f(400, 200, 400), 100, emat));
	auto pertext = make_shared<noise_texture>(0.1);
	objects.add(make_shared<sphere>(Point3f(220, 280, 300), 80, make_shared<lambertian>(pertext)));

	hittable_list boxes2;
	auto white = make_shared<lambertian>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(Point3f::random(0, 165), 10, white));
	}

	objects.add(make_shared<translate>(
		make_shared<rotate_y>(
			make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
		Vector3f(-100, 270, 395)
		)
	);

	return objects;
}

void WorldFactory::get_world(int idx, hittable_list& world, camera& cam, Color& background)
{
	Vector3f vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	Point3f lookfrom;
	Point3f lookat;
	auto vfov = 40.0;
	auto aperture = 0.0;

	float aspect_ratio = 1.0;

	switch (idx) {
	case 1:
		world = random_scene();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3f(13, 2, 3);
		lookat = Point3f(0, 0, 0);
		vfov = 20.0;
		aperture = 0.1;
		break;

	case 2:
		world = two_spheres();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3f(13, 2, 3);
		lookat = Point3f(0, 0, 0);
		vfov = 20.0;
		break;
		//default:

	case 3:
		world = two_perlin_spheres();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3f(13, 2, 3);
		lookat = Point3f(0, 0, 0);
		vfov = 20.0;
		break;

	case 4:
		world = earth();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3f(13, 2, 3);
		lookat = Point3f(0, 0, 0);
		vfov = 20.0;
		break;

	case 5:
		world = simple_light();
		background = Color(0, 0, 0);
		lookfrom = Point3f(26, 3, 6);
		lookat = Point3f(0, 2, 0);
		vfov = 20.0;
		break;

	case 6:
		world = cornell_box();
		background = Color(0, 0, 0);
		lookfrom = Point3f(278, 278, -800);
		lookat = Point3f(278, 278, 0);
		vfov = 40.0;
		break;

	case 7:
		world = cornell_smoke();
		lookfrom = Point3f(278, 278, -800);
		lookat = Point3f(278, 278, 0);
		vfov = 40.0;
		break;
	default:

	case 8:
		world = final_scene();
		background = Color(0, 0, 0);
		lookfrom = Point3f(478, 278, -600);
		lookat = Point3f(278, 278, 0);
		vfov = 40.0;
		aspect_ratio = 1.6;
		break;
	}

	cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0, 1.0);
	int image_height = 1024;
	int image_width = static_cast<int>(image_height * aspect_ratio);
	cam.film = make_shared<Film>(image_width, image_height);
}
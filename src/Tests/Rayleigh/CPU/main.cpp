#include "ray.h"
#include "Tools/Bound.h"
#include "Tools/Film.h"
#include "Tools/Filter.h"
#include "Tools/Parallel.h"
#include "Tools/Sampler.h"

const float z = 1000;
const Point3f bound(1000, 1000, z);

//auto bound = Bounds3f(Point3f(0, 0, 0), Point3f(bound_x, bound_y, 1000));

//Special version for this kind of intersection. Different logic to intersecting the bounding box.
bool Intersect(const Ray& ray, Float* hitt0, Float* hitt1)
{
	Float t0 = 0, t1 = ray.tMax;
	bool flag = false;
	for (int i = 0; i < 3; ++i)
	{
		//Update interval for ith bounding box slab 128
		Float invRayDir = 1 / ray.d[i];
		Float tNear = (0 - ray.o[i]) * invRayDir;
		Float tFar = (bound[i] - ray.o[i]) * invRayDir;

		if (tNear > tFar) std::swap(tNear, tFar);
		//Update tFar to ensure robust ray¨Cbounds intersection 221
		tFar *= 1 + 2 * gamma(3);

		if (ray.tMax > tFar)
		{
			if (!flag || tFar < t0)
			{
				t0 = tFar;
			}
			flag = true;
		}
	}
	if (hitt0)
	{
		*hitt0 = t0;
	}
	return flag;
}

const static Float average_free_length = 700;

bool scatter(const Ray& r_in, Ray& out, Float spectrum)
{
	auto probability = random_float();
	r_in.tMax = -log(probability) * average_free_length * pow(spectrum / sampledLambdaStart, 4);
	Float t1, t2;
	if (Intersect(r_in, &t1, &t2))
	{
		out = Ray(r_in.at(t1), r_in.d);
		return false;
	}
	else
	{
		auto x = random_float();
		auto temp = pow(2 * x + sqrt(1 + 4 * x * x), 1 / 3.0);
		auto costheta = temp - 1 / temp;
		auto sintheta = sqrt(1 - costheta * costheta);
		auto random_phi = random_float() * 2 * Pi;
		costheta *= random_float() > 0.5 ? 1.0 : -1.0;
		Vector3f v_out(cos(random_phi) * sintheta, sin(random_phi) * sintheta, costheta);

		Vector3f x1(Cross(Normalize(r_in.d), Normalize(r_in.auxiliary_axis)));
		Vector3f y1(Cross(Normalize(r_in.d), x1));
		Vector3f z1 = Normalize(r_in.d);

		Vector3f d_out = v_out.x() * x1 + v_out.y() * y1 + v_out.z() * z1;

		out = Ray(r_in.at(r_in.tMax), d_out);
		return true;
	}
}

const Point2i resolution(bound[0], bound[1]);

const long samples = 100000000;

int count = 0;

int main()
{
	srand(time(0));

	UnpolarizedSpectrum::Init();
	ParallelInit();

	Film film(resolution, Bounds2f(Point2f(0, 0), Point2f(1, 1)), std::make_unique<GaussianFilter>(4, 1), 1., "test.png", 1.);

	auto filmtile = film.GetFilmTile(Bounds2i(Point2i(0, 0), resolution));

	auto sun_spec = BlackBodySpectrum(6100, 20);

	ParallelFor([&](int j) {
		for (int i = 0; i < nSpectralSamples; ++i)
		{
			UnpolarizedSpectrum spectrum(0.);
			spectrum[i] = sun_spec[i];

			auto r = Ray(Point3f(random_float(0, bound[0]), random_float(0, bound[1]), 1), Normalize(Vector3f(0, 0, 20) + UniformSampleSphere(Point2f(random_float(), random_float()))));
			while (scatter(r, r, sampledLambdaStart + i * (sampledLambdaEnd - sampledLambdaStart) / nSpectralSamples));

			if (r.o.z() > (z-0.2))
			{
				auto position = Vector2f(resolution / 2) + (500 / r.d.z() * Vector2f(r.d.x(), r.d.y()));

				if (position.x() >= 0 && position.y() >= 0 && position.x() < resolution.x() && position.y() < resolution.y())
				{
					filmtile->AddSample(position, spectrum, 1);
					count++;
				}
			}
		}}, samples, 4096, true
	);

	ParallelCleanup();

	std::cout << count;

	film.MergeFilmTile(std::move(filmtile));

	film.WriteImage(1, true);
}
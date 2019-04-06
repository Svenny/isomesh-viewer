#include "primitives.h"
#include <cmath>
#include <iostream>
#include <algorithm>

double Plane::value(double x, double y, double z) const noexcept
{
	return plane.x*x + plane.y*y + plane.z*z;
}

glm::dvec3 Plane::grad(double x, double y, double z) const noexcept
{
	return plane;
}

double Ellipsoid::value(double x, double y, double z) const noexcept
{
	const glm::dvec3 divisor = 1.0 / (radius * radius);
	const glm::dvec3 p = {x, y, z};
	return glm::dot (p * p, divisor) - 1.0;
}

glm::dvec3 Ellipsoid::grad(double x, double y, double z) const noexcept
{
	const glm::dvec3 divisor = 1.0 / (radius * radius);
	const glm::dvec3 p = {x, y, z};
	return p * (2.0 * divisor);
}

double Box::value(double x, double y, double z) const noexcept
{
	glm::dvec3 p = glm::abs (glm::dvec3(x, y, z)) - boxSize * 0.5;
	return glm::max (glm::max (p.x, p.y), p.z);
}

glm::dvec3 Box::grad(double x, double y, double z) const noexcept
{
	glm::dvec3 p(x, y, z);
	glm::dvec3 grad = glm::sign (p);
	p = glm::abs (p) - boxSize * 0.5;
	double mxval = glm::max (glm::max (p.x, p.y), p.z);
	if (glm::abs (p.x - mxval) > 1e-5)
		grad.x = 0;
	if (glm::abs (p.y - mxval) > 1e-5)
		grad.y = 0;
	if (glm::abs (p.z - mxval) > 1e-5)
		grad.z = 0;
	return grad;
}

double Waves::value(double x, double y, double z) const noexcept
{
	return y +
		amp1 * sin (freq1 * x) + amp2 * sin (freq2 * x) +
		amp1 * sin (freq1 * z) + amp2 * sin (freq2 * z);
}

glm::dvec3 Waves::grad(double x, double y, double z) const noexcept
{
	double dx =
		amp1 * freq1 * cos (freq1 * x) +
		amp2 * freq2 * cos (freq2 * x);
	double dy = 1;
	double dz =
		amp1 * freq1 * cos (freq1 * z) +
		amp2 * freq2 * cos (freq2 * z);
	return glm::dvec3 (dx, dy, dz);
}

double TwoSpheres::value(double x, double y, double z) const noexcept
{
	const glm::dvec3 center_right (radius + gap * 0.5, 0, 0);
	const glm::dvec3 center_left (-radius - gap * 0.5, 0, 0);
	const double radius2 = radius * radius;

	glm::dvec3 p(x,y,z);

	if (p.x < 0)
		p -= center_left;
	else
		p -= center_right;
	return glm::dot (p, p) - radius2;
}

glm::dvec3 TwoSpheres::grad(double x, double y, double z) const noexcept
{
	const glm::dvec3 center_right (radius + gap * 0.5, 0, 0);
	const glm::dvec3 center_left (-radius - gap * 0.5, 0, 0);

	glm::dvec3 p(x,y,z);

	if (p.x < 0)
		p -= center_left;
	else
		p -= center_right;
	return 2.0 * p;
}


double PerlinNoise::fade(double t) const noexcept {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) const noexcept {
	return a + t * (b - a);
}
double PerlinNoise::grad(int hash, double x, double y, double z) const noexcept {
	int h = hash & 15;
	double u = h<8 ? x : y;
	double v = h<4 ? y : h==12||h==14 ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

void PerlinNoise::reseed(uint32_t seed) noexcept {
	for (int32_t i = 0; i < 256; i++)
		p[i] = i;
	std::shuffle(p.begin(), p.begin() + 256, std::default_random_engine(seed));
	for (size_t i = 0; i < 256; ++i)
		p[256 + i] = p[i];
}

double PerlinNoise::value(double x, double y, double z) const noexcept
{
	x *= 0.05;y *= 0.05;z *= 0.05;

	const int32_t X = (int32_t)std::floor(x) & 255;
	const int32_t Y = (int32_t)std::floor(y) & 255;
	const int32_t Z = (int32_t)std::floor(z) & 255;

	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);

	const double u = fade(x);
	const double v = fade(y);
	const double w = fade(z);

	const int32_t A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z;
	const int32_t B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;

	return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                     grad(p[BA  ], x-1, y  , z   )),
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
                                     grad(p[BA+1], x-1, y  , z-1 )),
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
}

glm::dvec3 PerlinNoise::grad(double x, double y, double z) const noexcept
{
	const double h = 0.1;

	const double x1 = value(x - h, y, z);
	const double x2 = value(x + h, y, z);
	const double y1 = value(x, y - h, z);
	const double y2 = value(x, y + h, z);
	const double z1 = value(x, y, z - h);
	const double z2 = value(x, y, z + h);

	return {(x2 - x1)/2/h, (y2 - y1)/2/h, (z2 - z1)/2/h};
}

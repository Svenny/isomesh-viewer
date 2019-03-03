#include "primitives.h"

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

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <isomesh/field/scalar_field.hpp>

struct Plane: public isomesh::ScalarField {
	glm::dvec3 plane = {1, 1, 1};

	double value(double x, double y, double z) const noexcept override;
	glm::dvec3 grad(double x, double y, double z) const noexcept override;
};

struct Ellipsoid: public isomesh::ScalarField {
	glm::dvec3 radius = {8, 8, 8};

	double value(double x, double y, double z) const noexcept override;
	glm::dvec3 grad(double x, double y, double z) const noexcept override;
};

struct Box: public isomesh::ScalarField {
	glm::dvec3 boxSize = glm::dvec3 (8, 8, 8);

	double value(double x, double y, double z) const noexcept override;
	glm::dvec3 grad(double x, double y, double z) const noexcept override;
};

struct Waves: public isomesh::ScalarField {
	double freq1 = 0.1;
	double freq2 = 0.2;
	double amp1 = 1.1;
	double amp2 = 1.2;

	double value(double x, double y, double z) const noexcept override;
	glm::dvec3 grad(double x, double y, double z) const noexcept override;
};

struct TwoSpheres: public isomesh::ScalarField {
	double radius = 8;
	double gap = 0.5;

	double value(double x, double y, double z) const noexcept override;
	glm::dvec3 grad(double x, double y, double z) const noexcept override;
};

#endif // PRIMITIVES_H

#include "functionselector.h"

#include <QDebug>

isomesh::SurfaceFunction FunctionSelector::selectFunction () {
	switch (m_usedFunction) {
	case FunPlane:
		return selectPlane ();
	case FunEllipsoid:
		return selectEllipsoid ();
	case FunBox:
		return selectBox ();
	case FunWaves:
		return selectWaves ();
	case FunPerlin:
		return selectPerlin ();
	case FunMultifractal:
		return selectMultifractal ();
	}
	qDebug () << "Selected invalid function";
	isomesh::SurfaceFunction fun;
	fun.f = [](glm::dvec3) { return 1; };
	fun.grad = [](glm::dvec3) { return glm::dvec3 (0, 1, 0); };
	return fun;
}

isomesh::SurfaceFunction FunctionSelector::selectPlane () {
	isomesh::SurfaceFunction fun;
	glm::dvec3 normal (m_planeA, m_planeB, m_planeC);
	fun.f = [=](glm::dvec3 p) {
		return glm::dot (normal, p);
	};
	fun.grad = [=](glm::dvec3 p) {
		return normal;
	};
	return fun;
}

isomesh::SurfaceFunction FunctionSelector::selectEllipsoid () {
	isomesh::SurfaceFunction fun;
	double mult_x = 1.0 / (m_ellipsoidA * m_ellipsoidA);
	double mult_y = 1.0 / (m_ellipsoidB * m_ellipsoidB);
	double mult_z = 1.0 / (m_ellipsoidC * m_ellipsoidC);
	glm::dvec3 mult (mult_x, mult_y, mult_z);
	fun.f = [=](glm::dvec3 p) {
		return glm::dot (p * p, mult) - 1.0;
	};
	fun.grad = [=](glm::dvec3 p) {
		return p * (2.0 * mult);
	};
	return fun;
}

isomesh::SurfaceFunction FunctionSelector::selectBox () {
	isomesh::SurfaceFunction fun;
	// TODO: implement this
	return fun;
}

isomesh::SurfaceFunction FunctionSelector::selectWaves () {
	isomesh::SurfaceFunction fun;
	fun.f = [=](glm::dvec3 p) {
		return p.y +
			m_ampX1 * sin (m_freqX1 * p.x) + m_ampX2 * sin (m_freqX2 * p.x) +
			m_ampZ1 * sin (m_freqZ1 * p.z) + m_ampZ2 * sin (m_freqZ2 * p.z);
	};
	fun.grad = [=](glm::dvec3 p) {
		double dx =
			m_ampX1 * m_freqX1 * cos (m_freqX1 * p.x) +
			m_ampX2 * m_freqX2 * cos (m_freqX2 * p.x);
		double dy = 1;
		double dz =
			m_ampZ1 * m_freqZ1 * cos (m_freqZ1 * p.z) +
			m_ampZ2 * m_freqZ2 * cos (m_freqZ2 * p.z);
		return glm::dvec3 (dx, dy, dz);
	};
	return fun;
}

isomesh::SurfaceFunction FunctionSelector::selectPerlin () {
	isomesh::SurfaceFunction fun;
	// TODO: implement this
	return fun;
}

isomesh::SurfaceFunction FunctionSelector::selectMultifractal () {
	isomesh::SurfaceFunction fun;
	// TODO: implement this
	return fun;
}

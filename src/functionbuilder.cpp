#include "functionbuilder.h"

#include <QDebug>

isomesh::SurfaceFunction FunctionBuilder::buildFunction () {
	switch (m_usedFunction) {
	case FunPlane:
		return buildPlaneFunction ();
	case FunEllipsoid:
		return buildEllipsoidFunction ();
	case FunBox:
		return buildBoxFunction ();
	case FunWaves:
		return buildWavesFunction ();
	case FunPerlin:
		return buildPerlinFunction ();
	case FunMultifractal:
		return buildMultifractalFunction ();
	case FunTwoSpheres:
		return buildTwoSpheresFunction ();
	}
	qDebug () << "Selected invalid function";
	isomesh::SurfaceFunction fun;
	fun.f = [](glm::dvec3) { return 1; };
	fun.grad = [](glm::dvec3) { return glm::dvec3 (0, 1, 0); };
	return fun;
}

isomesh::SurfaceFunction FunctionBuilder::buildPlaneFunction () {
	isomesh::SurfaceFunction fun;
	const glm::dvec3 dir = m_planeDir;
	fun.f = [=](glm::dvec3 p) {
		return glm::dot (dir, p);
	};
	fun.grad = [=](glm::dvec3 p) {
		return dir;
	};
	return fun;
}

isomesh::SurfaceFunction FunctionBuilder::buildEllipsoidFunction () {
	isomesh::SurfaceFunction fun;
	const glm::dvec3 divisor = 1.0 / (m_ellipsoidRadii * m_ellipsoidRadii);
	fun.f = [=](glm::dvec3 p) {
		return glm::dot (p * p, divisor) - 1.0;
	};
	fun.grad = [=](glm::dvec3 p) {
		return p * (2.0 * divisor);
	};
	return fun;
}

isomesh::SurfaceFunction FunctionBuilder::buildBoxFunction () {
	isomesh::SurfaceFunction fun;
	const glm::dvec3 sub = m_boxSize * 0.5;
	fun.f = [=](glm::dvec3 p) {
		p = glm::abs (p) - sub;
		return glm::max (glm::max (p.x, p.y), p.z);
	};
	fun.grad = [=](glm::dvec3 p) {
		glm::dvec3 grad = glm::sign (p);
		p = glm::abs (p) - sub;
		double mxval = glm::max (glm::max (p.x, p.y), p.z);
		if (glm::abs (p.x - mxval) > 1e-5)
			grad.x = 0;
		if (glm::abs (p.y - mxval) > 1e-5)
			grad.y = 0;
		if (glm::abs (p.z - mxval) > 1e-5)
			grad.z = 0;
		return grad;
	};
	return fun;
}

isomesh::SurfaceFunction FunctionBuilder::buildWavesFunction () {
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

isomesh::SurfaceFunction FunctionBuilder::buildPerlinFunction () {
	isomesh::SurfaceFunction fun;
	// TODO: implement this
	return fun;
}

isomesh::SurfaceFunction FunctionBuilder::buildMultifractalFunction () {
	isomesh::SurfaceFunction fun;
	// TODO: implement this
	return fun;
}

isomesh::SurfaceFunction FunctionBuilder::buildTwoSpheresFunction () {
	isomesh::SurfaceFunction fun;
	// Center of right sphere
	const double x_right = m_twoSpheresRadius + m_twoSpheresGap * 0.5;
	const glm::dvec3 center_right (x_right, 0, 0);
	// Center of left sphere
	const double x_left = -x_right;
	const glm::dvec3 center_left (x_left, 0, 0);
	const double radius2 = m_twoSpheresRadius * m_twoSpheresRadius;
	fun.f = [=](glm::dvec3 p) {
		if (p.x < 0)
			p -= center_left;
		else
			p -= center_right;
		return glm::dot (p, p) - radius2;
	};
	fun.grad = [=](glm::dvec3 p) {
		if (p.x < 0)
			p -= center_left;
		else
			p -= center_right;
		return 2.0 * p;
	};
	return fun;
}

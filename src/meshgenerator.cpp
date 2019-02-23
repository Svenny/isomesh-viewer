#include "meshgenerator.h"

#include <chrono>

#include <QDebug>

#include <isomesh/isomesh.hpp>

void MeshGenerator::generateMesh () {
	qDebug () << "generate mesh requested";
	if (!m_doGenerateCalled) {
		m_doGenerateCalled = true;
		QMetaObject::invokeMethod (this, "doGenerateMesh", Qt::QueuedConnection);
	}
}

void MeshGenerator::doGenerateMesh () {
	m_doGenerateCalled = false;
	qDebug () << "regenerating mesh...";
	isomesh::Mesh result;
	// compute...
	auto t1 = std::chrono::high_resolution_clock::now ();
	{
		isomesh::BisectionZeroFinder solver;
		// Generate a surface chunk from this function,
		// then check it against a manually validated result
		isomesh::UniformGrid G (m_chunkSize, glm::dvec3 (m_xOffset, m_yOffset, m_zOffset), m_chunkScale);
		G.fill (m_function, solver, isomesh::TrivialMaterialSelector ());
		if (m_usedAlgorithm == AlgoMarchingCubes)
			result = isomesh::marchingCubes (G);
		else if (m_usedAlgorithm == AlgoDualContouring) {
			isomesh::AnyNonemptyMaterialFilter filter;
			isomesh::GradientDescentQefSolver3D solver;
			result = isomesh::dualContouring (G, filter, solver);
		}
		else qDebug () << "Invalid algorithm used";
	}
	auto t2 = std::chrono::high_resolution_clock::now ();
	auto dur = std::chrono::duration_cast<std::chrono::duration<double> > (t2 - t1);
	qDebug () << "... done in" << dur.count () * 1000.0 << " ms";
	emit meshGenerated (QSharedPointer<isomesh::Mesh>::create (std::move (result)));
}

void MeshGenerator::setUsedFunction (isomesh::SurfaceFunction fun) { m_function = fun; }



void MeshGenerator::setUsedAlgorithm (UsedAlgorithm algo) { m_usedAlgorithm = algo; }

// ------------------------------------------
// Common parameters
// ------------------------------------------

void MeshGenerator::setChunkSize (int size) {
	if (size < 4 || size > 128)
		return; // Out of grid's allowed range
	if (size & (size - 1))
		return; // Not a power of two
	m_chunkSize = size;
}

void MeshGenerator::setChunkScale (double scale) {
	if (scale <= 0)
		return;
	m_chunkScale = scale;
}

void MeshGenerator::setXOffset (double value) { m_xOffset = value; }
void MeshGenerator::setYOffset (double value) { m_yOffset = value; }
void MeshGenerator::setZOffset (double value) { m_zOffset = value; }

#include "meshgenerator.h"

#include <chrono>

#include <QDebug>

#include <isomesh/algo/marching_cubes.hpp>
#include <isomesh/data/grid.hpp>
#include <isomesh/util/material_selector.hpp>
#include <isomesh/util/zero_finder.hpp>

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
		isomesh::SurfaceFunction F = m_funBuilder.buildFunction ();
		isomesh::BisectionZeroFinder solver;
		// Generate a surface chunk from this function,
		// then check it against a manually validated result
		isomesh::UniformGrid G (m_chunkSize, glm::dvec3 (m_xOffset, m_yOffset, m_zOffset), m_chunkScale);
		G.fill (F, solver, isomesh::TrivialMaterialSelector ());
		result = isomesh::marchingCubes (G);
	}
	auto t2 = std::chrono::high_resolution_clock::now ();
	auto dur = std::chrono::duration_cast<std::chrono::duration<double> > (t2 - t1);
	qDebug () << "... done in" << dur.count () * 1000.0 << " ms";
	emit meshGenerated (QSharedPointer<isomesh::Mesh>::create (std::move (result)));
}

void MeshGenerator::setUsedFunction (UsedFunction fun) { m_funBuilder.setUsedFunction (fun); }



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

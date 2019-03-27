#include "meshgenerator.h"

#include <chrono>

#include <QDebug>
#include <QTime>

#include <isomesh/isomesh.hpp>

void MeshGenerator::generateMesh () {
	qDebug () << "generate mesh requested in" << QTime::currentTime().toString(Qt::SystemLocaleShortDate).toStdString().data();
	if (!m_doGenerateCalled) {
		m_doGenerateCalled = true;
		QMetaObject::invokeMethod (this, "doGenerateMesh", Qt::QueuedConnection);
	}
}

void MeshGenerator::doGenerateMesh () {
	m_doGenerateCalled = false;
	qDebug () << "regenerating mesh...";
	isomesh::Mesh result;
	auto t1 = std::chrono::high_resolution_clock::now ();
	{
		if (m_usedAlgorithm == AlgoDualMarchingCubes) {
			isomesh::TrivialMaterialSelector selector;
			isomesh::GradientDescentQefSolver4D solver;
			isomesh::DMC_Octree octree (m_chunkSize, glm::dvec3 (m_xOffset, m_yOffset, m_zOffset), m_chunkScale);
			octree.build (*m_function, selector, solver, m_epsilon,
			              m_dmcUseSimpleSplitPolicy, m_dmcUseRandomSampling, m_dmcUseEarlyStop);
			result = octree.contour ();
		}
		else if (m_usedAlgorithm == AlgoMarchingCubes) {
			isomesh::BisectionZeroFinder solver;
			isomesh::UniformGrid G (m_chunkSize, glm::dvec3 (m_xOffset, m_yOffset, m_zOffset), m_chunkScale);
			G.fill (*m_function, solver, isomesh::TrivialMaterialSelector ());
			result = isomesh::marchingCubes (G);
		}
		else if (m_usedAlgorithm == AlgoDualContouring) {
			isomesh::BisectionZeroFinder solver;
			isomesh::UniformGrid G (m_chunkSize, glm::dvec3 (m_xOffset, m_yOffset, m_zOffset), m_chunkScale);
			G.fill (*m_function, solver, isomesh::TrivialMaterialSelector ());
			isomesh::AnyNonemptyMaterialFilter filter;
			if (m_dcUseSimplification) {
				isomesh::QrQefSolver3D qef_solver;
				isomesh::DC_Octree octree (m_chunkSize, glm::dvec3 (m_xOffset, m_yOffset, m_zOffset), m_chunkScale);
				octree.build (G, qef_solver, m_epsilon, true);
				result = octree.contour (filter);
			}
			else {
				isomesh::GradientDescentQefSolver3D qef_solver;
				result = isomesh::dualContouring (G, filter, qef_solver);
			}
		}
		else qDebug () << "Invalid algorithm used";
	}
	auto t2 = std::chrono::high_resolution_clock::now ();
	auto dur = std::chrono::duration_cast<std::chrono::duration<double> > (t2 - t1);
	qDebug () << "... done in" << dur.count () * 1000.0 << " ms";
	qDebug () << "mesh has" << result.vertexCount () << "vertices and" << result.indexCount () << "indices";
	emit meshGenerated (QSharedPointer<isomesh::Mesh>::create (std::move (result)));
}

void MeshGenerator::setUsedFunction (QSharedPointer<isomesh::ScalarField> fun) { m_function = fun; }

void MeshGenerator::setEpsilon(float epsilon) { m_epsilon = epsilon; }

void MeshGenerator::setUsedAlgorithm (UsedAlgorithm algo) { m_usedAlgorithm = algo; }

// ------------------------------------------
// Common parameters
// ------------------------------------------

void MeshGenerator::setChunkSize (int size) {
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

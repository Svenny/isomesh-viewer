#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include <QObject>
#include <QSharedPointer>

#include <isomesh/field/scalar_field.hpp>
#include <isomesh/data/mesh.hpp>

enum UsedAlgorithm : int {
	AlgoMarchingCubes,
	AlgoDualContouring
};

class MeshGenerator : public QObject {
	Q_OBJECT

public slots:
	void generateMesh ();
	// Function parameters
	void setUsedFunction (isomesh::ScalarField* field);
	// Algorithm parameters
	void setUsedAlgorithm (UsedAlgorithm algo);
	// Common parameters
	void setChunkSize (int size);
	void setXOffset (double value);
	void setYOffset (double value);
	void setZOffset (double value);
	void setChunkScale (double value);

signals:
	void meshGenerated (QSharedPointer<isomesh::Mesh> mesh);

private slots:
	// Does actual generation
	void doGenerateMesh ();

private:
	bool m_doGenerateCalled = false;
	isomesh::ScalarField* m_function;
	// Algorithm parameters
	UsedAlgorithm m_usedAlgorithm = AlgoMarchingCubes;
	// Common parameters
	int m_chunkSize = 32;
	double m_chunkScale = 1;
	double m_xOffset = 0, m_yOffset = 0, m_zOffset = 0;
};

#endif // MESHGENERATOR_H

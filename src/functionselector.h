#ifndef FUNCTIONSELECTOR_H
#define FUNCTIONSELECTOR_H

#include <isomesh/common.hpp>

enum UsedFunction : int {
	FunPlane,
	FunEllipsoid,
	FunBox,
	FunWaves,
	FunPerlin,
	FunMultifractal
};

class FunctionSelector {
public:
	// Builds function from set parameters
	isomesh::SurfaceFunction selectFunction ();
	void setUsedFunction (UsedFunction fun) { m_usedFunction = fun; }
	// Plane settings
	void setPlaneA (double value) { m_planeA = value; }
	void setPlaneB (double value) { m_planeB = value; }
	void setPlaneC (double value) { m_planeC = value; }

	
private:
	UsedFunction m_usedFunction = FunWaves;
	// Plane settings
	double m_planeA = 1, m_planeB = 1, m_planeC = 1;
	// Ellipsoid settings
	double m_ellipsoidA = 8, m_ellipsoidB = 8, m_ellipsoidC = 8;
	// Box settings
	double m_boxA = 8, m_boxB = 8, m_boxC = 8;
	// Waves settings
	double m_freqX1 = 0.1, m_freqX2 = 0.2, m_freqZ1 = 0.3, m_freqZ2 = 0.4;
	double m_ampX1 = 1.1, m_ampX2 = 1.2, m_ampZ1 = 1.3, m_ampZ2 = 1.4;
	// Multifractal settings
	int m_mfrOctaves = 8;
	double m_mfrFrequency = 0.3, m_mfrAmplitude = 1.5;
	double m_mfrLacunarity = 2.0, m_mfrGain = 0.5;
	
	isomesh::SurfaceFunction selectPlane ();
	isomesh::SurfaceFunction selectEllipsoid ();
	isomesh::SurfaceFunction selectBox ();
	isomesh::SurfaceFunction selectWaves ();
	isomesh::SurfaceFunction selectPerlin ();
	isomesh::SurfaceFunction selectMultifractal ();
};

#endif // FUNCTIONSELECTOR_H

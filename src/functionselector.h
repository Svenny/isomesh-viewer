#ifndef FUNCTIONSELECTOR_H
#define FUNCTIONSELECTOR_H

#include <isomesh/common.hpp>

enum UsedFunction : int {
	FunPlane,
	FunEllipsoid,
	FunBox,
	FunWaves,
	FunPerlin,
	FunMultifractal,
	FunTwoSpheres
};

class FunctionBuilder {
public:
	// Builds function from set parameters
	isomesh::SurfaceFunction buildFunction ();
	void setUsedFunction (UsedFunction fun) { m_usedFunction = fun; }
	// Plane settings
	void setPlaneDirX (double value) { m_planeDir.x = value; }
	void setPlaneDirY (double value) { m_planeDir.y = value; }
	void setPlaneDirZ (double value) { m_planeDir.z = value; }
	// Ellipsoid settings
	void setEllipsoidRadiusX (double value) { m_ellipsoidRadii.x = value; }
	void setEllipsoidRadiusY (double value) { m_ellipsoidRadii.y = value; }
	void setEllipsoidRadiusZ (double value) { m_ellipsoidRadii.z = value; }
	// Box settings
	void setBoxSizeX (double value) { m_boxSize.x = value; }
	void setBoxSizeY (double value) { m_boxSize.y = value; }
	void setBoxSizeZ (double value) { m_boxSize.z = value; }
	// Waves settings
	void setWavesFrequencyX1 (double value) { m_freqX1 = value; }
	void setWavesFrequencyX2 (double value) { m_freqX2 = value; }
	void setWavesFrequencyZ1 (double value) { m_freqZ1 = value; }
	void setWavesFrequencyZ2 (double value) { m_freqZ2 = value; }
	void setWavesAmplitudeX1 (double value) { m_ampX1 = value; }
	void setWavesAmplitudeX2 (double value) { m_ampX2 = value; }
	void setWavesAmplitudeZ1 (double value) { m_ampZ1 = value; }
	void setWavesAmplitudeZ2 (double value) { m_ampZ2 = value; }
	// Multifractal settings
	void setMultifractalOctaves (int value) { m_mfrOctaves = value; }
	void setMultifractalFrequency (double value) { m_mfrFrequency = value; }
	void setMultifractalAmplitude (double value) { m_mfrAmplitude = value; }
	void setMultifractalLacunarity (double value) { m_mfrLacunarity = value; }
	void setMultifractalGain (double value) { m_mfrGain = value; }
	// Two spheres settings
	void setTwoSpheresRadius (double value) { m_twoSpheresRadius = value; }
	void setTwoSpheresGap (double value) { m_twoSpheresGap = value; }
	
private:
	UsedFunction m_usedFunction = FunWaves;
	// Plane settings (direction vector)
	glm::dvec3 m_planeDir = glm::dvec3 (1, 1, 1);
	// Ellipsoid settings (base radii)
	glm::dvec3 m_ellipsoidRadii = glm::dvec3 (8, 8, 8);
	// Box settings (side lengths)
	glm::dvec3 m_boxSize = glm::dvec3 (8, 8, 8);
	// Waves settings
	double m_freqX1 = 0.1, m_freqX2 = 0.2, m_freqZ1 = 0.3, m_freqZ2 = 0.4;
	double m_ampX1 = 1.1, m_ampX2 = 1.2, m_ampZ1 = 1.3, m_ampZ2 = 1.4;
	// Multifractal settings
	int m_mfrOctaves = 8;
	double m_mfrFrequency = 0.3, m_mfrAmplitude = 1.5;
	double m_mfrLacunarity = 2.0, m_mfrGain = 0.5;
	// Two spheres settings
	double m_twoSpheresRadius = 8;
	double m_twoSpheresGap = 0.5;
	
	isomesh::SurfaceFunction buildPlaneFunction ();
	isomesh::SurfaceFunction buildEllipsoidFunction ();
	isomesh::SurfaceFunction buildBoxFunction ();
	isomesh::SurfaceFunction buildWavesFunction ();
	isomesh::SurfaceFunction buildPerlinFunction ();
	isomesh::SurfaceFunction buildMultifractalFunction ();
	isomesh::SurfaceFunction buildTwoSpheresFunction ();
};

#endif // FUNCTIONSELECTOR_H

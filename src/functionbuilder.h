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
	FunTwoSpheres,
	FunHeightmap
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
	void setWavesFrequency1 (double value) { m_freq1 = value; }
	void setWavesFrequency2 (double value) { m_freq2 = value; }
	void setWavesAmplitude1 (double value) { m_amp1 = value; }
	void setWavesAmplitude2 (double value) { m_amp2 = value; }
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
	double m_freq1 = 0.1, m_freq2 = 0.2;
	double m_amp1 = 1.1, m_amp2 = 1.2;
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

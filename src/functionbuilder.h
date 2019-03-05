#ifndef FUNCTIONSELECTOR_H
#define FUNCTIONSELECTOR_H

#include <QSharedPointer>

#include <isomesh/common.hpp>
#include <isomesh/field/heightmap.hpp>
#include "primitives.h"

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
	FunctionBuilder();
	QSharedPointer<isomesh::ScalarField> buildFunction(UsedFunction fun);

public:
	Plane plane;
	Ellipsoid ellipsoid;
	Box box;
	Waves waves;
	TwoSpheres twoSpheres;
	isomesh::Heightmap heightmap;
};

#endif // FUNCTIONSELECTOR_H

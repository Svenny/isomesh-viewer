#include "functionbuilder.h"

#include <QDebug>

isomesh::ScalarField* FunctionBuilder::buildFunction(UsedFunction fun) {
	switch (fun) {
	case FunPlane:
		return &plane;
	case FunEllipsoid:
		return &ellipsoid;
	case FunBox:
		return &box;
	case FunWaves:
		return &waves;
	case FunPerlin:
		//TODO
		break;
	case FunMultifractal:
		//TODO
		break;
	case FunTwoSpheres:
		return &twoSpheres;
	case FunHeightmap:
		return &heightmap;
	}
	qDebug () << "Selected invalid function";
	return nullptr;
}

FunctionBuilder::FunctionBuilder():
	heightmap({0, 3.4}, {0,0,0}, 0.1)
{
}

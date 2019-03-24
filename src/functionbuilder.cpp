#include "functionbuilder.h"

#include <QDebug>

QSharedPointer<isomesh::ScalarField> FunctionBuilder::buildFunction(UsedFunction fun) {
	switch (fun) {
	case FunPlane:
		return QSharedPointer<Plane>::create(plane);
	case FunEllipsoid:
		return QSharedPointer<Ellipsoid>::create(ellipsoid);
	case FunBox:
		return QSharedPointer<Box>::create(box);
	case FunWaves:
		return QSharedPointer<Waves>::create(waves);
	case FunPerlin:
		//TODO
		break;
	case FunMultifractal:
		//TODO
		break;
	case FunTwoSpheres:
		return QSharedPointer<TwoSpheres>::create(twoSpheres);
	case FunHeightmap:
		return QSharedPointer<isomesh::Heightmap>::create(heightmap);
	case FunModel:
		return QSharedPointer<isomesh::MeshField>::create(meshField);
	}
	qDebug () << "Selected invalid function";
	return nullptr;
}

FunctionBuilder::FunctionBuilder():
	heightmap({0, 3.4}, {0,0,0}, 0.1)
{
}

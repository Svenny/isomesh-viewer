#include <QApplication>
#include <QSharedPointer>
#include <QSurfaceFormat>

#include <isomesh/data/mesh.hpp>

#include "meshgenerator.h"

#include "mainwindow.h"

Q_DECLARE_METATYPE (QSharedPointer<isomesh::Mesh>)
Q_DECLARE_METATYPE (UsedFunction)
Q_DECLARE_METATYPE (UsedAlgorithm)

int main (int argc, char *argv[]) {
	qRegisterMetaType<QSharedPointer<isomesh::Mesh> > ();
	qRegisterMetaType<UsedFunction> ();
	qRegisterMetaType<UsedAlgorithm> ();

	QSurfaceFormat fmt;
	fmt.setRenderableType (QSurfaceFormat::OpenGL);
	fmt.setVersion (3, 3);
	fmt.setProfile (QSurfaceFormat::CoreProfile);
	fmt.setSwapBehavior (QSurfaceFormat::DoubleBuffer);
	fmt.setDepthBufferSize (32);
	fmt.setSamples (2);
	QSurfaceFormat::setDefaultFormat (fmt);

	QApplication app (argc, argv);

	MainWindow w;
	w.show ();

	return app.exec ();
}

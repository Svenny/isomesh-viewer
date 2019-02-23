#include <QApplication>
#include <QSharedPointer>
#include <QSurfaceFormat>
#include <QTranslator>

#include <isomesh/common.hpp>
#include <isomesh/data/mesh.hpp>

#include "meshgenerator.h"

#include "mainwindow.h"

Q_DECLARE_METATYPE (QSharedPointer<isomesh::Mesh>)
Q_DECLARE_METATYPE (UsedFunction)
Q_DECLARE_METATYPE (UsedAlgorithm)
Q_DECLARE_METATYPE (isomesh::SurfaceFunction);

int main (int argc, char *argv[]) {
	qRegisterMetaType<QSharedPointer<isomesh::Mesh> > ();
	qRegisterMetaType<UsedFunction> ();
	qRegisterMetaType<UsedAlgorithm> ();
	qRegisterMetaType<isomesh::SurfaceFunction> ();

	QSurfaceFormat fmt;
	fmt.setRenderableType (QSurfaceFormat::OpenGL);
	fmt.setVersion (3, 3);
	fmt.setProfile (QSurfaceFormat::CoreProfile);
	fmt.setSwapBehavior (QSurfaceFormat::DoubleBuffer);
	fmt.setDepthBufferSize (24);
	fmt.setSamples (2);
	QSurfaceFormat::setDefaultFormat (fmt);

	QApplication app (argc, argv);

	QTranslator translator;
	if (translator.load(QLocale::system(), QLatin1String("isomeshviewer"), QLatin1String("_"), QLatin1String(":/translations")))
		app.installTranslator(&translator);

	MainWindow w;
	w.show ();

	return app.exec ();
}

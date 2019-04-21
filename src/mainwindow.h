#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLocale>
#include <QMainWindow>
#include <QThread>

#include <memory>

#include "meshgenerator.h"
#include "functionbuilder.h"

class QLineEdit;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow (QWidget *parent = nullptr);
	virtual ~MainWindow () override;

signals:
	void regenerateMesh ();

private slots:
	void checkQLineEditData();
	void selectedFunctionChanged(int idx);
	void selectedAlgoChanged(int idx);
	void generateMesh();
	void exportMeshToPly();
	void setPathToHeightmap();
	void lightingStatusChanged(int status);
	void textureStatusChanged(int status);
	void textureScaleChanged();
	void setPathToTexture();
	void normalColorStatusChanged(int status);
	void lightDirChanged();
	void setPathToModel();
	void showOriginalModelStatusChanged(int status);
	void updateBoundCube();

private:
	Ui::MainWindow *ui;
	QThread m_workerThread;
	MeshGenerator *m_meshGen;
	FunctionBuilder m_builder;
	QSharedPointer<isomesh::Mesh> m_originalModel;
	QString m_modelFilepath;
	QSharedPointer<isomesh::Mesh> m_storedModel;
	QLocale m_locale;

	void initFunctionParams ();
	void initAlgorithmParams ();
	bool updateFunctionParams();
	bool updateAlgoParams();
	bool hasInvalidInput(std::initializer_list<QLineEdit*> widgets);
	double parseDouble(QLineEdit* edit);
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLocale>
#include <QMainWindow>
#include <QThread>

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
	void generateMesh();

private:
	Ui::MainWindow *ui;
	QThread m_workerThread;
	MeshGenerator *m_meshGen;
	FunctionBuilder m_builder;
	QLocale m_locale;

	void initFunctionParams ();
	void initAlgorithmParams ();
	bool updateFunctionParams();
	bool hasInvalidInput(std::initializer_list<QLineEdit*> widgets);
	double parseDouble(QLineEdit* edit);
};

#endif // MAINWINDOW_H

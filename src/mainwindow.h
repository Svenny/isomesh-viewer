#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLocale>
#include <QMainWindow>
#include <QThread>

#include "meshgenerator.h"

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

	void generateMesh();

private:
	Ui::MainWindow *ui;
	QThread m_workerThread;
	MeshGenerator *m_meshGen;
	QLocale m_locale;

	void initFunctionParams ();
	void initAlgorithmParams ();
	double parseDouble(QLineEdit* edit);
};

#endif // MAINWINDOW_H

#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QPalette>
#include <QMessageBox>

Q_DECLARE_METATYPE (UsedFunction)
Q_DECLARE_METATYPE (UsedAlgorithm)

MainWindow::MainWindow (QWidget *parent) :
	QMainWindow (parent), ui (new Ui::MainWindow), m_locale (QLocale::system ()) {
	ui->setupUi (this);
	// Setup mesh generator running in a separate thread
	m_meshGen = new MeshGenerator;
	m_meshGen->moveToThread (&m_workerThread);
	connect (&m_workerThread, SIGNAL (finished ()), m_meshGen, SLOT (deleteLater ()));
	//connect (ui->generateButton, SIGNAL (clicked ()), m_meshGen, SLOT (generateMesh ()));
	//connect (ui->generateButton, &QPushButton::clicked, this, [this]() {
	//	double num = m_locale.toDouble (ui->chunkScaleEdit->text ());
	//	qDebug () << num;
	//});
	connect (m_meshGen, &MeshGenerator::meshGenerated, ui->viewer, &ViewerWidget::setMesh);
	m_workerThread.start ();

	auto scaleValidator = new QDoubleValidator (1e-5, 1e5, 100, ui->chunkScaleEdit);
	scaleValidator->setNotation (QDoubleValidator::StandardNotation);
	ui->chunkScaleEdit->setValidator (scaleValidator);

	auto offsetValidator = new QDoubleValidator (this);
	offsetValidator->setNotation (QDoubleValidator::StandardNotation);
	ui->xOffsetEdit->setValidator (offsetValidator);
	ui->yOffsetEdit->setValidator (offsetValidator);
	ui->zOffsetEdit->setValidator (offsetValidator);

	initFunctionParams ();
	initAlgorithmParams ();
}

MainWindow::~MainWindow () {
	m_workerThread.quit ();
	m_workerThread.wait ();
	delete ui;
}

void MainWindow::initFunctionParams () {
	ui->funSelectorBox->addItem (tr ("Plane"), FunPlane);
	ui->funSelectorBox->addItem (tr ("Ellipsoid"), FunEllipsoid);
	ui->funSelectorBox->addItem (tr ("Box"), FunBox);
	ui->funSelectorBox->addItem (tr ("Waves"), FunWaves);
	ui->funSelectorBox->addItem (tr ("Perlin noise"), FunPerlin);
	ui->funSelectorBox->addItem (tr ("Multifractal"), FunMultifractal);
	ui->funSelectorBox->addItem (tr ("Two spheres"), FunTwoSpheres);
	connect (ui->funSelectorBox, QOverload<int>::of (&QComboBox::currentIndexChanged), this, [this](int idx) {
		auto fun = ui->funSelectorBox->itemData(idx).value<UsedFunction>();
		QMetaObject::invokeMethod (m_meshGen, "setUsedFunction", Q_ARG (UsedFunction, fun));
	});
	ui->funSelectorBox->setCurrentIndex (3);
}

void MainWindow::initAlgorithmParams () {
	ui->algoSelectorBox->addItem (tr ("Marching Cubes"), AlgoMarchingCubes);
	ui->algoSelectorBox->addItem (tr ("Dual Contouring"), AlgoDualContouring);
	connect (ui->algoSelectorBox, QOverload<int>::of (&QComboBox::currentIndexChanged), this, [this](int idx) {
		auto algo = ui->algoSelectorBox->itemData(idx).value<UsedAlgorithm>();
		QMetaObject::invokeMethod (m_meshGen, "setUsedAlgorithm", Q_ARG (UsedAlgorithm, algo));
	});
	ui->algoSelectorBox->setCurrentIndex (0);
}

void MainWindow::checkQLineEditData() {
	QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
	if (edit) {
		QPalette palette = edit->palette();
		if (edit->hasAcceptableInput() || edit->text().isEmpty())
			palette.setColor(QPalette::Base, Qt::white);
		else
			palette.setColor(QPalette::Base, Qt::red);
		edit->setPalette(palette);
	}
}

void MainWindow::generateMesh() {
	bool allInputValid
		=  ui->chunkScaleEdit->hasAcceptableInput()
		&& ui->xOffsetEdit->hasAcceptableInput()
		&& ui->yOffsetEdit->hasAcceptableInput()
		&& ui->zOffsetEdit->hasAcceptableInput();

	qDebug() << allInputValid;
	if (!allInputValid) {
		QMessageBox::warning(
			this,
			tr("Isomesh Viewer"),
			tr("Some inputs fields contains invalid information or empty (the fields with invalid input marks by red color)")
		);
		return;
	}

	QMetaObject::invokeMethod (m_meshGen, "setChunkSize", Q_ARG (int, ui->chunkSizeBox->currentText().toInt()));
	QMetaObject::invokeMethod (m_meshGen, "setChunkScale", Q_ARG (double, parseDouble(ui->chunkScaleEdit)));
	QMetaObject::invokeMethod (m_meshGen, "setXOffset", Q_ARG (double, parseDouble(ui->xOffsetEdit)));
	QMetaObject::invokeMethod (m_meshGen, "setYOffset", Q_ARG (double, parseDouble(ui->yOffsetEdit)));
	QMetaObject::invokeMethod (m_meshGen, "setZOffset", Q_ARG (double, parseDouble(ui->zOffsetEdit)));
	QMetaObject::invokeMethod (m_meshGen, "generateMesh");
}

double MainWindow::parseDouble(QLineEdit* edit)
{
	return m_locale.toDouble(edit->text());
}


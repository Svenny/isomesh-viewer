#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow (QWidget *parent) :
	QMainWindow (parent), ui (new Ui::MainWindow), m_locale (QLocale::system ()) {
	ui->setupUi (this);
	// Setup mesh generator running in a separate thread
	m_meshGen = new MeshGenerator;
	m_meshGen->moveToThread (&m_workerThread);
	connect (&m_workerThread, SIGNAL (finished ()), m_meshGen, SLOT (deleteLater ()));
	connect (ui->generateButton, SIGNAL (clicked ()), m_meshGen, SLOT (generateMesh ()));
	//connect (ui->generateButton, &QPushButton::clicked, this, [this]() {
	//	double num = m_locale.toDouble (ui->chunkScaleEdit->text ());
	//	qDebug () << num;
	//});
	connect (m_meshGen, &MeshGenerator::meshGenerated, ui->viewer, &ViewerWidget::setMesh);
	m_workerThread.start ();
	
	initFunctionParams ();
	initAlgorithmParams ();
	initCommonParams ();
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
	connect (ui->funSelectorBox, QOverload<int>::of (&QComboBox::currentIndexChanged), this, [this](int idx) {
		auto fun = UsedFunction (ui->funSelectorBox->itemData (idx).toInt ());
		QMetaObject::invokeMethod (m_meshGen, "setUsedFunction", Q_ARG (UsedFunction, fun));
	});
	ui->funSelectorBox->setCurrentIndex (3);
}

void MainWindow::initAlgorithmParams () {
	ui->algoSelectorBox->addItem (tr ("Marching Cubes"), AlgoMarchingCubes);
	ui->algoSelectorBox->addItem (tr ("Dual Contouring"), AlgoDualContouring);
	connect (ui->algoSelectorBox, QOverload<int>::of (&QComboBox::currentIndexChanged), this, [this](int idx) {
		auto algo = UsedAlgorithm (ui->algoSelectorBox->itemData (idx).toInt ());
		QMetaObject::invokeMethod (m_meshGen, "setUsedAlgorithm", Q_ARG (UsedAlgorithm, algo));
	});
	ui->algoSelectorBox->setCurrentIndex (0);
}

void MainWindow::initCommonParams () {
	// Chunk size
	ui->chunkSizeBox->addItem ("4");
	ui->chunkSizeBox->addItem ("8");
	ui->chunkSizeBox->addItem ("16");
	ui->chunkSizeBox->addItem ("32");
	ui->chunkSizeBox->addItem ("64");
	ui->chunkSizeBox->addItem ("128");
	connect (ui->chunkSizeBox, QOverload<const QString&>::of (&QComboBox::currentIndexChanged),
	         this, [this](const QString &sz) {
		QMetaObject::invokeMethod (m_meshGen, "setChunkSize", Q_ARG (int, sz.toInt ()));
	});
	ui->chunkSizeBox->setCurrentIndex (3);
	// Chunk scale
	auto scaleValidator = new QDoubleValidator (1e-5, 1e5, 100, ui->chunkScaleEdit);
	scaleValidator->setNotation (QDoubleValidator::StandardNotation);
	ui->chunkScaleEdit->setValidator (scaleValidator);
	connect (ui->chunkScaleEdit, &QLineEdit::textEdited, this, [this](const QString &text) {
		double scale = m_locale.toDouble (text);
		if (scale < 1e-5)
			scale = 1;
		QMetaObject::invokeMethod (m_meshGen, "setChunkScale", Q_ARG (double, scale));
	});
	// Offsets
	auto offsetValidator = new QDoubleValidator (this);
	offsetValidator->setNotation (QDoubleValidator::StandardNotation);
	// X
	ui->xOffsetEdit->setValidator (offsetValidator);
	connect (ui->xOffsetEdit, &QLineEdit::textEdited, this, [this](const QString &text) {
		double offset = m_locale.toDouble (text);
		QMetaObject::invokeMethod (m_meshGen, "setXOffset", Q_ARG (double, offset));
	});
	// Y
	ui->yOffsetEdit->setValidator (offsetValidator);
	connect (ui->yOffsetEdit, &QLineEdit::textEdited, this, [this](const QString &text) {
		double offset = m_locale.toDouble (text);
		QMetaObject::invokeMethod (m_meshGen, "setYOffset", Q_ARG (double, offset));
	});
	// Z
	ui->zOffsetEdit->setValidator (offsetValidator);
	connect (ui->zOffsetEdit, &QLineEdit::textEdited, this, [this](const QString &text) {
		double offset = m_locale.toDouble (text);
		QMetaObject::invokeMethod (m_meshGen, "setZOffset", Q_ARG (double, offset));
	});
}

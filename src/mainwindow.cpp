#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <limits>

#include <QPalette>
#include <QMessageBox>
#include <QFileDialog>

Q_DECLARE_METATYPE (UsedFunction)
Q_DECLARE_METATYPE (UsedAlgorithm)

MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
		, ui (new Ui::MainWindow)
		, m_locale (QLocale::system ())
{
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

	auto doubleValidator = new QDoubleValidator (this);
	doubleValidator->setNotation (QDoubleValidator::StandardNotation);
	ui->xOffsetEdit->setValidator (doubleValidator);
	ui->yOffsetEdit->setValidator (doubleValidator);
	ui->zOffsetEdit->setValidator (doubleValidator);

	auto positiveDoubleValidator = new QDoubleValidator(this);
	positiveDoubleValidator->setBottom(0);
	positiveDoubleValidator->setNotation(QDoubleValidator::StandardNotation);

	ui->wavesFreq1Edit->setValidator (positiveDoubleValidator);
	ui->wavesFreq1Edit->setText(m_locale.toString(0.1));
	ui->wavesFreq2Edit->setValidator (positiveDoubleValidator);
	ui->wavesFreq2Edit->setText(m_locale.toString(0.2));
	ui->wavesAmp1Edit->setValidator (positiveDoubleValidator);
	ui->wavesAmp1Edit->setText(m_locale.toString(1.1));
	ui->wavesAmp2Edit->setValidator (positiveDoubleValidator);
	ui->wavesAmp2Edit->setText(m_locale.toString(1.2));

	ui->hmapPixelSizeEdit->setValidator(positiveDoubleValidator);
	ui->hmapPixelSizeEdit->setText(m_locale.toString(0.1));
	ui->hmapMaxHEdit->setValidator(doubleValidator);
	ui->hmapMaxHEdit->setText(m_locale.toString(3.4));
	ui->hmapMinHEdit->setValidator(doubleValidator);

	auto zeroGreaterValidator = new QDoubleValidator(this);
	zeroGreaterValidator->setBottom(std::numeric_limits<double>::epsilon());
	zeroGreaterValidator->setNotation(QDoubleValidator::StandardNotation);

	ui->boxXEdit->setValidator(zeroGreaterValidator);
	ui->boxYEdit->setValidator(zeroGreaterValidator);
	ui->boxZEdit->setValidator(zeroGreaterValidator);

	ui->ellipsoidXEdit->setValidator(zeroGreaterValidator);
	ui->ellipsoidYEdit->setValidator(zeroGreaterValidator);
	ui->ellipsoidZEdit->setValidator(zeroGreaterValidator);

	ui->dmcEpsilonEdit->setValidator(zeroGreaterValidator);
	ui->dmcEpsilonEdit->setText(m_locale.toString(0.025));

	ui->twoSpheresRadiusEdit->setValidator(zeroGreaterValidator);
	ui->twoSpheresGapEdit->setValidator(zeroGreaterValidator);
	ui->twoSpheresGapEdit->setText(m_locale.toString(0.5));

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
	ui->funSelectorBox->addItem (tr ("Heightmap"), FunHeightmap);

	ui->funSelectorBox->setCurrentIndex (3);
}

void MainWindow::initAlgorithmParams () {
	ui->algoSelectorBox->addItem (tr ("Marching Cubes"), AlgoMarchingCubes);
	ui->algoSelectorBox->addItem (tr ("Dual Contouring"), AlgoDualContouring);
	ui->algoSelectorBox->addItem (tr ("Dual Marching Cubes"), AlgoDualMarchingCubes);
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
	if (hasInvalidInput({ui->chunkScaleEdit, ui->xOffsetEdit, ui->yOffsetEdit, ui->zOffsetEdit}))
		return;

	QMetaObject::invokeMethod (m_meshGen, "setChunkSize", Q_ARG (int, ui->chunkSizeBox->currentText().toInt()));
	QMetaObject::invokeMethod (m_meshGen, "setChunkScale", Q_ARG (double, parseDouble(ui->chunkScaleEdit)));
	QMetaObject::invokeMethod (m_meshGen, "setXOffset", Q_ARG (double, parseDouble(ui->xOffsetEdit)));
	QMetaObject::invokeMethod (m_meshGen, "setYOffset", Q_ARG (double, parseDouble(ui->yOffsetEdit)));
	QMetaObject::invokeMethod (m_meshGen, "setZOffset", Q_ARG (double, parseDouble(ui->zOffsetEdit)));

	if (updateAlgoParams() && updateFunctionParams()) {
		auto fun = ui->funSelectorBox->currentData().value<UsedFunction>();
		if (fun == UsedFunction::FunHeightmap && !m_builder.heightmap.isDataLoaded()) {
			QMessageBox::warning(this, tr("Isomesh Viewer"), tr("Heigthmap file not loaded"));
			return;
		}
		QMetaObject::invokeMethod (m_meshGen, "setUsedFunction", Q_ARG (QSharedPointer<isomesh::ScalarField>, m_builder.buildFunction(fun)));
		QMetaObject::invokeMethod (m_meshGen, "generateMesh");
		ui->viewer->setFocus();
	}
}

double MainWindow::parseDouble(QLineEdit* edit)
{
	return m_locale.toDouble(edit->text());
}

void MainWindow::selectedFunctionChanged(int idx)
{
	ui->stackedWidget->setCurrentIndex(idx);
}

void MainWindow::selectedAlgoChanged(int idx)
{
	ui->stackedWidgetAlgo->setCurrentIndex(idx);
}

bool MainWindow::updateFunctionParams()
{
	auto fun = ui->funSelectorBox->currentData().value<UsedFunction>();
	switch (fun)
	{
		case UsedFunction::FunBox: {
			if (hasInvalidInput({ui->boxXEdit, ui->boxYEdit, ui->boxZEdit}))
				break;

			m_builder.box.boxSize.x = parseDouble(ui->boxXEdit);
			m_builder.box.boxSize.y = parseDouble(ui->boxYEdit);
			m_builder.box.boxSize.z = parseDouble(ui->boxZEdit);
			return true;
		}

		case UsedFunction::FunEllipsoid: {
			if (hasInvalidInput({ui->ellipsoidXEdit, ui->ellipsoidYEdit, ui->ellipsoidZEdit}))
				break;

			m_builder.ellipsoid.radius.x = parseDouble(ui->ellipsoidXEdit);
			m_builder.ellipsoid.radius.y = parseDouble(ui->ellipsoidYEdit);
			m_builder.ellipsoid.radius.z = parseDouble(ui->ellipsoidZEdit);
			return true;
		}

		case UsedFunction::FunPlane: {
			if (hasInvalidInput({ui->planeXEdit, ui->planeYEdit, ui->planeZEdit}))
				break;

			m_builder.plane.plane.x = parseDouble(ui->planeXEdit);
			m_builder.plane.plane.y = parseDouble(ui->planeYEdit);
			m_builder.plane.plane.z = parseDouble(ui->planeZEdit);
			return true;
		}

		case UsedFunction::FunWaves: {
			if (hasInvalidInput({ui->wavesFreq1Edit, ui->wavesFreq2Edit, ui->wavesAmp1Edit, ui->wavesAmp2Edit}))
				break;

			m_builder.waves.freq1 = parseDouble(ui->wavesFreq1Edit);
			m_builder.waves.freq2 = parseDouble(ui->wavesFreq2Edit);
			m_builder.waves.amp1 = parseDouble(ui->wavesAmp1Edit);
			m_builder.waves.amp2 = parseDouble(ui->wavesAmp2Edit);
			return true;
		}

		case UsedFunction::FunHeightmap: {
			if (hasInvalidInput({ui->hmapMaxHEdit, ui->hmapPixelSizeEdit, ui->hmapMinHEdit}))
				break;

			m_builder.heightmap.setPixelSize(parseDouble(ui->hmapPixelSizeEdit));
			m_builder.heightmap.setHeightRange({parseDouble(ui->hmapMinHEdit), parseDouble(ui->hmapMaxHEdit)});
			return true;
		}

		case UsedFunction::FunTwoSpheres: {
			if (hasInvalidInput({ui->twoSpheresGapEdit, ui->twoSpheresRadiusEdit}))
				break;

			m_builder.twoSpheres.radius = parseDouble(ui->twoSpheresRadiusEdit);
			m_builder.twoSpheres.gap = parseDouble(ui->twoSpheresGapEdit);
			return true;
		}
	}

	return false;
}

bool MainWindow::updateAlgoParams()
{
	auto algo = ui->algoSelectorBox->currentData().value<UsedAlgorithm>();
	switch(algo) {
		case UsedAlgorithm::AlgoMarchingCubes:
			[[fallthrough]];
		case UsedAlgorithm::AlgoDualContouring:
			return true;

		case UsedAlgorithm::AlgoDualMarchingCubes: {
			if (hasInvalidInput({ui->dmcEpsilonEdit}))
				break;

			m_meshGen->setEpsilon(static_cast<float>(parseDouble(ui->dmcEpsilonEdit)));
			return true;
		}
	}

	return false;
}

bool MainWindow::hasInvalidInput(std::initializer_list<QLineEdit*> widgets)
{
	for (const QLineEdit* edit : widgets) {
		if (!edit->hasAcceptableInput()) {
			QMessageBox::warning(
				this,
				tr("Isomesh Viewer"),
				tr("Some inputs fields contains invalid information or empty (the fields with invalid input marks by red color)")
			);
			return true;
		}
	}
	return false;
}

void MainWindow::setPathToHeightmap()
{
	const QString& filename = QFileDialog::getOpenFileName(this, tr("Load heightmap"), QString(), tr("Heightmap Files (*.png *.jpg *.bmp)"), nullptr, QFileDialog::DontUseNativeDialog);
	try {
		if (!filename.isEmpty()) {
			m_builder.heightmap.loadGrayscaleMap(filename.toStdString());
		}
	} catch (std::runtime_error e) {
		QMessageBox::critical(this, tr("Isomesh Viewer"), tr("Loading file ends with error: %1").arg(tr(e.what())));
	}
}

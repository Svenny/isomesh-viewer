#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QPalette>
#include <QMessageBox>
#include <QFileDialog>

Q_DECLARE_METATYPE (UsedFunction)
Q_DECLARE_METATYPE (UsedAlgorithm)

MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
		, ui (new Ui::MainWindow)
		, m_locale (QLocale::system ())
		, m_heigthmap(new isomesh::HeightMapImporter({0, 3.4}, glm::dvec3(0,0,0), 0.1)) 
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
	ui->boxXEdit->setValidator (positiveDoubleValidator);
	ui->boxYEdit->setValidator (positiveDoubleValidator);
	ui->boxZEdit->setValidator (positiveDoubleValidator);

	ui->ellipsoidXEdit->setValidator (positiveDoubleValidator);
	ui->ellipsoidYEdit->setValidator (positiveDoubleValidator);
	ui->ellipsoidZEdit->setValidator (positiveDoubleValidator);

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

	if (updateFunctionParams()) {
		auto fun = ui->funSelectorBox->currentData().value<UsedFunction>();
		if (fun == UsedFunction::FunHeightmap) {
			if (!m_heigthmap->isDataLoaded()) {
				QMessageBox::warning(this, tr("Isomesh Viewer"), tr("Heigthmap file not loaded"));
				return;
			}
			QMetaObject::invokeMethod (m_meshGen, "setUsedFunction", Q_ARG (isomesh::SurfaceFunction, isomesh::HeightMapImporter::buildSurfaceFunction(m_heigthmap)));
		} else {
			QMetaObject::invokeMethod (m_meshGen, "setUsedFunction", Q_ARG (isomesh::SurfaceFunction,
 m_builder.buildFunction()));
		}
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
	auto fun = ui->funSelectorBox->itemData(idx).value<UsedFunction>();
	m_builder.setUsedFunction(fun);
	ui->stackedWidget->setCurrentIndex(ui->funSelectorBox->currentIndex());
}

bool MainWindow::updateFunctionParams()
{
	auto fun = ui->funSelectorBox->currentData().value<UsedFunction>();
	switch (fun)
	{
		case UsedFunction::FunBox: {
			if (hasInvalidInput({ui->boxXEdit, ui->boxYEdit, ui->boxZEdit}))
				break;

			m_builder.setBoxSizeX(parseDouble(ui->boxXEdit));
			m_builder.setBoxSizeY(parseDouble(ui->boxYEdit));
			m_builder.setBoxSizeZ(parseDouble(ui->boxZEdit));
			return true;
		}

		case UsedFunction::FunEllipsoid: {
			if (hasInvalidInput({ui->ellipsoidXEdit, ui->ellipsoidYEdit, ui->ellipsoidZEdit}))
				break;

			m_builder.setEllipsoidRadiusX(parseDouble(ui->ellipsoidXEdit));
			m_builder.setEllipsoidRadiusY(parseDouble(ui->ellipsoidYEdit));
			m_builder.setEllipsoidRadiusZ(parseDouble(ui->ellipsoidZEdit));
			return true;
		}

		case UsedFunction::FunPlane: {
			if (hasInvalidInput({ui->planeXEdit, ui->planeYEdit, ui->planeZEdit}))
				break;

			m_builder.setPlaneDirX(parseDouble(ui->planeXEdit));
			m_builder.setPlaneDirY(parseDouble(ui->planeYEdit));
			m_builder.setPlaneDirZ(parseDouble(ui->planeZEdit));
			return true;
		}

		case UsedFunction::FunWaves: {
			if (hasInvalidInput({ui->wavesFreq1Edit, ui->wavesFreq2Edit, ui->wavesAmp1Edit, ui->wavesAmp2Edit}))
				break;

			m_builder.setWavesFrequency1(parseDouble(ui->wavesFreq1Edit));
			m_builder.setWavesFrequency2(parseDouble(ui->wavesFreq2Edit));
			m_builder.setWavesAmplitude1(parseDouble(ui->wavesAmp1Edit));
			m_builder.setWavesAmplitude2(parseDouble(ui->wavesAmp2Edit));
			return true;
		}

		case UsedFunction::FunHeightmap: {
			if (hasInvalidInput({ui->hmapMaxHEdit, ui->hmapPixelSizeEdit, ui->hmapMinHEdit}))
				break;

			m_heigthmap->setPixelSize(parseDouble(ui->hmapPixelSizeEdit));
			m_heigthmap->setHeightRange({parseDouble(ui->hmapMinHEdit), parseDouble(ui->hmapMaxHEdit)});
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
	const QString& filename = QFileDialog::getOpenFileName(this, tr("Load heightmap"), QString(), tr("Heightmap Files (*.png *.bmp)"), nullptr, QFileDialog::DontUseNativeDialog);
	try {
		if (!filename.isEmpty()) {
			m_heigthmap->loadGrayscale8bitMap(filename.toStdString());
		}
	} catch (std::runtime_error e) {
		QMessageBox::critical(this, tr("Isomesh Viewer"), tr("Loading file ends with error: %1").arg(tr(e.what())));
	}
}

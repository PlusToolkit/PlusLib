#include "VolumeReconstructionToolbox.h"

#include "vtkFreehandController.h"
#include "VolumeReconstructionController.h"

#include <QFileDialog>

#include "vtkFileFinder.h"

//-----------------------------------------------------------------------------

VolumeReconstructionToolbox::VolumeReconstructionToolbox(QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
{
	ui.setupUi(this);

	// Initialize toolbox controller
	VolumeReconstructionController* toolboxController = VolumeReconstructionController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Volume reconstruction toolbox controller is not initialized!");
		return;
	}

	toolboxController->SetToolbox(this);

	// Connect events
	connect( ui.pushButton_OpenVolumeReconstructionConfig, SIGNAL( clicked() ), this, SLOT( OpenVolumeReconstructionConfigClicked() ) );
	connect( ui.pushButton_OpenInputImage, SIGNAL( clicked() ), this, SLOT( OpenInputImageClicked() ) );
	connect( ui.pushButton_Reconstruct, SIGNAL( clicked() ), this, SLOT( ReconstructClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveClicked() ) );
}

//-----------------------------------------------------------------------------

VolumeReconstructionToolbox::~VolumeReconstructionToolbox()
{
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::RefreshToolboxContent()
{
	//LOG_TRACE("VolumeReconstructionToolbox::RefreshToolboxContent");

	VolumeReconstructionController* toolboxController = VolumeReconstructionController::GetInstance();

	// If initialization failed
	if (toolboxController->State() == ToolboxState_Uninitialized) {
		ui.label_Instructions->setText("N/A");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
	} else
	// If initialized
	if (toolboxController->State() == ToolboxState_Idle) {
		ui.label_Instructions->setText(tr("N/A"));
		if (! toolboxController->GetVolumeReconstructionConfigFileLoaded()) {
			ui.label_Instructions->setText(tr("Volume reconstruction config XML has to be loaded"));
			ui.pushButton_Reconstruct->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
		} else if (ui.lineEdit_InputImage->text().length() == 0) {
			ui.label_Instructions->setText(tr("Input image has to be selected"));
			ui.pushButton_Reconstruct->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
		} else {
			ui.label_Instructions->setText(tr("Press Reconstruct button start reconstruction"));
			ui.pushButton_Reconstruct->setEnabled(true);
			ui.pushButton_Save->setEnabled(false);
		}

		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));
	} else
	// If in progress
	if (toolboxController->State() == ToolboxState_InProgress) {
		ui.label_Instructions->setText("");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
	} else
	// If done
	if (toolboxController->State() == ToolboxState_Done) {
		ui.label_Instructions->setText("Reconstructed volume saved successfully");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(true);
	} else
	// If error occured
	if (toolboxController->State() == ToolboxState_Error) {
		ui.label_Instructions->setText("Error occured!");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
	}

	// Needed for forced refreshing the UI (without this, no progress is shown)
	QApplication::processEvents();
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Stop()
{
	LOG_TRACE("VolumeReconstructionToolbox::Stop");

	// No action
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Clear()
{
	LOG_TRACE("VolumeReconstructionToolbox::Clear");

	// No action
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::OpenVolumeReconstructionConfigClicked()
{
	LOG_TRACE("VolumeReconstructionToolbox::OpenVolumeReconstructionConfigClicked"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open volume reconstruction configuration XML" ) ), vtkFileFinder::GetInstance()->GetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	ui.lineEdit_VolumeReconstructionConfig->setText(fileName);
	ui.lineEdit_VolumeReconstructionConfig->setToolTip(fileName);

	// Load volume reconstruction configuration xml
	if (VolumeReconstructionController::GetInstance()->LoadVolumeReconstructionConfigFromFile(fileName.toStdString()) != PLUS_SUCCESS) {
		LOG_ERROR("Failed to open volume reconstruction file: " << fileName.toStdString()); 
	}
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::OpenInputImageClicked()
{
	LOG_TRACE("VolumeReconstructionToolbox::OpenInputImageClicked"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "MHA files ( *.mha );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open input sequence metafile image" ) ), "", filter);

	if (fileName.isNull()) {
		return;
	}

	ui.lineEdit_InputImage->setText(fileName);
	ui.lineEdit_InputImage->setToolTip(fileName);
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::ReconstructClicked()
{
	LOG_TRACE("VolumeReconstructionToolbox::ReconstructClicked"); 

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	VolumeReconstructionController::GetInstance()->ReconstructVolumeFromInputImage(ui.lineEdit_InputImage->text().toStdString());

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::SaveClicked()
{
	LOG_TRACE("VolumeReconstructionToolbox::ReconstructClicked"); 

	QString filter = QString( tr( "VTK files ( *.vtk );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save reconstructed volume"), "", filter);

	if (! fileName.isNull() ) {
		QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

		VolumeReconstructionController::GetInstance()->SaveVolumeToFile(fileName.toStdString());

		QApplication::restoreOverrideCursor();
	}	
}

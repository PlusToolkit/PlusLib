/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "VolumeReconstructionToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include <QFileDialog>

#include "vtkVolumeReconstructor.h"
#include "vtkImageExtractComponents.h"
#include "vtkDataSetWriter.h"
#include "vtkMarchingContourFilter.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkXMLUtilities.h"
#include "vtkTrackedFrameList.h"
#include "vtkImageData.h"

//-----------------------------------------------------------------------------

VolumeReconstructionToolbox::VolumeReconstructionToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParentMainWindow, aFlags)
	, m_VolumeReconstructor(NULL)
	, m_ReconstructedVolume(NULL)
	, m_VolumeReconstructionConfigFileLoaded(false)
	, m_ContouringThreshold(64.0)
{
	ui.setupUi(this);

  m_VolumeReconstructor = vtkVolumeReconstructor::New();
  m_ReconstructedVolume = vtkImageData::New();

  // Connect events
	connect( ui.pushButton_OpenVolumeReconstructionConfig, SIGNAL( clicked() ), this, SLOT( OpenVolumeReconstructionConfig() ) );
	connect( ui.pushButton_OpenInputImage, SIGNAL( clicked() ), this, SLOT( OpenInputImage() ) );
	connect( ui.pushButton_Reconstruct, SIGNAL( clicked() ), this, SLOT( Reconstruct() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );
}

//-----------------------------------------------------------------------------

VolumeReconstructionToolbox::~VolumeReconstructionToolbox()
{
	if (m_VolumeReconstructor != NULL) {
		m_VolumeReconstructor->Delete();
		m_VolumeReconstructor = NULL;
	}

  if (m_ReconstructedVolume != NULL) {
		m_ReconstructedVolume->Delete();
		m_ReconstructedVolume = NULL;
	}
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Initialize()
{
	LOG_TRACE("VolumeReconstructionToolbox::Initialize"); 

  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected()))
  {
    m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetTrackingOnly(false);
  }

  // Try to load volume reconstruction configuration from the device set configuration
  if ( (vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() != NULL)
    && (m_VolumeReconstructor->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS))
  {
    m_VolumeReconstructionConfigFileLoaded = true;
    ui.lineEdit_VolumeReconstructionConfig->setText(tr("Using session configuration"));
  }

  // Set initialized if it was uninitialized
  if (m_State == ToolboxState_Uninitialized) {
	  SetState(ToolboxState_Idle);
  }

  if (m_State != ToolboxState_Done) {
    m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->Initialize();
  }
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::RefreshContent()
{
	//LOG_TRACE("VolumeReconstructionToolbox::RefreshContent");

	// If in progress
	if (m_State == ToolboxState_InProgress)
  {
		// Needed for forced refreshing the UI (without this, no progress is shown)
		QApplication::processEvents();
	}
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("VolumeReconstructionToolbox::SetDisplayAccordingToState");

  m_ParentMainWindow->GetToolVisualizer()->HideAll();

	// If initialization failed
	if (m_State == ToolboxState_Uninitialized) {
		ui.label_Instructions->setText("N/A");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

	} else
	// If initialized
	if (m_State == ToolboxState_Idle) {
		ui.label_Instructions->setText(tr("N/A"));

		if (! m_VolumeReconstructionConfigFileLoaded) {
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
	if (m_State == ToolboxState_InProgress) {
		ui.label_Instructions->setText("");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

	} else
	// If done
	if (m_State == ToolboxState_Done) {
		ui.label_Instructions->setText("Reconstructed volume saved successfully");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(true);

		m_ParentMainWindow->SetStatusBarText(QString(" Reconstruction done"));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    m_ParentMainWindow->GetToolVisualizer()->GetVolumeActor()->VisibilityOn();
	  m_ParentMainWindow->GetToolVisualizer()->GetCanvasRenderer()->Modified();
	  m_ParentMainWindow->GetToolVisualizer()->GetCanvasRenderer()->ResetCamera();

	} else
	// If error occured
	if (m_State == ToolboxState_Error) {
		ui.label_Instructions->setText("Error occured!");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
	}
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::OpenVolumeReconstructionConfig()
{
	LOG_TRACE("VolumeReconstructionToolbox::OpenVolumeReconstructionConfig"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
  QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open volume reconstruction configuration XML" ) ), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

  // Parse XML file
	vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(fileName.toStdString().c_str());
	if (rootElement == NULL) {	
		LOG_ERROR("Unable to read the configuration file: " << fileName.toStdString()); 
		return;
	}

	// Load volume reconstruction configuration xml
  if (m_VolumeReconstructor->ReadConfiguration(rootElement) != PLUS_SUCCESS)
  {
    m_VolumeReconstructionConfigFileLoaded = false;

    ui.lineEdit_VolumeReconstructionConfig->setText(tr("Invalid file!"));
		ui.lineEdit_VolumeReconstructionConfig->setToolTip("");

		LOG_ERROR("Failed to open volume reconstruction file: " << fileName.toStdString());
		return;
	}

  m_VolumeReconstructionConfigFileLoaded = true;
	ui.lineEdit_VolumeReconstructionConfig->setText(fileName);
	ui.lineEdit_VolumeReconstructionConfig->setToolTip(fileName);

	SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::OpenInputImage()
{
	LOG_TRACE("VolumeReconstructionToolbox::OpenInputImage"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "MHA files ( *.mha );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open input sequence metafile image" ) ), "", filter);

	if (fileName.isNull()) {
		return;
	}

	ui.lineEdit_InputImage->setText(fileName);
	ui.lineEdit_InputImage->setToolTip(fileName);

	SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Reconstruct()
{
	LOG_TRACE("VolumeReconstructionToolbox::Reconstruct"); 

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	if (ReconstructVolumeFromInputImage(ui.lineEdit_InputImage->text().toStdString()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to reconstruct volume!");
  }

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Save()
{
	LOG_TRACE("VolumeReconstructionToolbox::Reconstruct"); 

	QString filter = QString( tr( "VTK files ( *.vtk );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save reconstructed volume"), "", filter);

	if (! fileName.isNull() ) {
		QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

		if (SaveVolumeToFile(fileName.toStdString()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to save volume to file!");
    }

		QApplication::restoreOverrideCursor();
	}	
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionToolbox::ReconstructVolumeFromInputImage(std::string aInputImage)
{
	LOG_TRACE("VolumeReconstructionToolbox::ReconstructVolumeFromInputImage(" << aInputImage << ")");

	SetState(ToolboxState_InProgress);

	// Read image
	m_ParentMainWindow->SetStatusBarText(QString(" Reading image sequence ..."));
	m_ParentMainWindow->SetStatusBarProgress(0);
	RefreshContent();

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if (trackedFrameList->ReadFromSequenceMetafile(aInputImage.c_str()) != PLUS_SUCCESS) {
    LOG_ERROR("Unable to load input image file!");
    return PLUS_FAIL;
  }
	
	m_ParentMainWindow->SetStatusBarText(QString(" Reconstructing volume ..."));
	m_ParentMainWindow->SetStatusBarProgress(0);
	RefreshContent();

  m_VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList);

	const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  std::string defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
	for ( int imgNumber = 0; imgNumber < numberOfFrames; ++imgNumber ) 
  {
		// Set progress
    m_ParentMainWindow->SetStatusBarProgress((int)((100.0 * imgNumber) / numberOfFrames + 0.49));
    RefreshContent();

		// Add this tracked frame to the reconstructor
    m_VolumeReconstructor->AddTrackedFrame(trackedFrameList->GetTrackedFrame(imgNumber), defaultFrameTransformName.c_str());
	}
	
	m_ParentMainWindow->SetStatusBarProgress(0);
	RefreshContent();

	trackedFrameList->Clear(); 
  	
	m_ParentMainWindow->SetStatusBarProgress(0);
	m_ParentMainWindow->SetStatusBarText(QString(" Filling holes in output volume..."));
	RefreshContent();

  m_VolumeReconstructor->GetReconstructedVolume(m_ReconstructedVolume);

	// Display result
	DisplayReconstructedVolume();

	m_ParentMainWindow->SetStatusBarProgress(100);

	SetState(ToolboxState_Done);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::DisplayReconstructedVolume()
{
	LOG_TRACE("VolumeReconstructionToolbox::DisplayReconstructedVolume"); 

	m_ParentMainWindow->SetStatusBarText(QString(" Generating contour for displaying..."));
	RefreshContent();

	vtkSmartPointer<vtkMarchingContourFilter> contourFilter = vtkSmartPointer<vtkMarchingContourFilter>::New();
	contourFilter->SetInput(m_ReconstructedVolume);
	contourFilter->SetValue(0, m_ContouringThreshold);

	vtkSmartPointer<vtkPolyDataMapper> contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	contourMapper->SetInputConnection(contourFilter->GetOutputPort());

  m_ParentMainWindow->GetToolVisualizer()->GetVolumeActor()->SetMapper(contourMapper);
	m_ParentMainWindow->GetToolVisualizer()->GetVolumeActor()->GetProperty()->SetColor(0.0, 0.0, 1.0);
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionToolbox::SaveVolumeToFile(std::string aOutput)
{
	LOG_TRACE("VolumeReconstructionToolbox::SaveVolumeToFile(" << aOutput << ")"); 

	// Write out to file
	vtkSmartPointer<vtkDataSetWriter> writer = vtkSmartPointer<vtkDataSetWriter>::New();
	writer->SetFileTypeToBinary();
	writer->SetInput(m_ReconstructedVolume);
	writer->SetFileName(aOutput.c_str());
	writer->Update();

	return PLUS_SUCCESS;
}

/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "VolumeReconstructionToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"
#include "CapturingToolbox.h"

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
#include "metaImage.h"

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
	connect( ui.comboBox_InputImage, SIGNAL( currentIndexChanged(int) ), this, SLOT( InputImageChanged(int) ) );
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
  }

  // Clear results poly data
  if (m_State != ToolboxState_Done)
  {
    m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->Initialize();

    // Check for new images
    PopulateImageComboBox();
  }

  // Set initialized if it was uninitialized
  if (m_State == ToolboxState_Uninitialized)
  {
	  SetState(ToolboxState_Idle);
  }
  else
  {
    SetDisplayAccordingToState();
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

  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(false);
  m_ParentMainWindow->GetToolVisualizer()->HideAll();

	if (m_State == ToolboxState_Uninitialized)
  {
		ui.label_Instructions->setText("N/A");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

	}
  else if (m_State == ToolboxState_Idle)
  {
		ui.label_Instructions->setText(tr("N/A"));

		if (! m_VolumeReconstructionConfigFileLoaded)
    {
			ui.label_Instructions->setText(tr("Volume reconstruction config XML has to be loaded"));
			ui.pushButton_Reconstruct->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
    }
    else if (ui.comboBox_InputImage->currentIndex() == -1)
    {
			ui.label_Instructions->setText(tr("Input image has to be selected"));
			ui.pushButton_Reconstruct->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
		}
    else
    {
			ui.label_Instructions->setText(tr("Press Reconstruct button start reconstruction"));
			ui.pushButton_Reconstruct->setEnabled(true);
			ui.pushButton_Save->setEnabled(false);
      ui.comboBox_InputImage->setToolTip(ui.comboBox_InputImage->currentText());
		}
	}
  else if (m_State == ToolboxState_InProgress)
  {
		ui.label_Instructions->setText("");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

	}
  else if (m_State == ToolboxState_Done)
  {
		ui.label_Instructions->setText("Reconstruction done");

		ui.pushButton_Reconstruct->setEnabled(false);
		ui.pushButton_Save->setEnabled(true);

		m_ParentMainWindow->SetStatusBarText(QString(" Reconstruction done"));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    m_ParentMainWindow->GetToolVisualizer()->GetVolumeActor()->VisibilityOn();
	  m_ParentMainWindow->GetToolVisualizer()->GetCanvasRenderer()->Modified();
	  m_ParentMainWindow->GetToolVisualizer()->GetCanvasRenderer()->ResetCamera();
	}
  else if (m_State == ToolboxState_Error)
  {
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
	if (fileName.isNull())
  {
		return;
	}

  // Parse XML file
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(fileName.toAscii().data()));
  if (rootElement == NULL)
  {
		LOG_ERROR("Unable to read the configuration file: " << fileName.toAscii().data()); 
		return;
	}

	// Load volume reconstruction configuration xml
  if (m_VolumeReconstructor->ReadConfiguration(rootElement) != PLUS_SUCCESS)
  {
    m_VolumeReconstructionConfigFileLoaded = false;

		LOG_ERROR("Failed to import volume reconstruction settings from " << fileName.toAscii().data());
		return;
	}

  m_VolumeReconstructionConfigFileLoaded = true;

	SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::OpenInputImage()
{
	LOG_TRACE("VolumeReconstructionToolbox::OpenInputImage"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "MHA files ( *.mha );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open input sequence metafile image" ) ), "", filter);

	if (fileName.isNull())
  {
		return;
	}

  m_ImageFileNames.append(fileName);

  PopulateImageComboBox();

  ui.comboBox_InputImage->setCurrentIndex( ui.comboBox_InputImage->count() - 1 );
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Reconstruct()
{
	LOG_TRACE("VolumeReconstructionToolbox::Reconstruct"); 

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	if (ReconstructVolumeFromInputImage() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to reconstruct volume!");
    SetState(ToolboxState_Error);
  }

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::Save()
{
	LOG_TRACE("VolumeReconstructionToolbox::Save"); 

	QString filter = QString( tr( "VTK files ( *.vtk );;Sequence metafiles ( *.mha );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save reconstructed volume"), QString(vtkPlusConfig::GetInstance()->GetImageDirectory()), filter);

	if (! fileName.isNull() )
  {
		QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

		if (SaveVolumeToFile(fileName) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to save volume to file!");
    }

		QApplication::restoreOverrideCursor();
	}	
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionToolbox::ReconstructVolumeFromInputImage()
{
	LOG_TRACE("VolumeReconstructionToolbox::ReconstructVolumeFromInputImage");

	SetState(ToolboxState_InProgress);

	// Read image
	m_ParentMainWindow->SetStatusBarText(QString(" Reading image sequence ..."));
	m_ParentMainWindow->SetStatusBarProgress(0);
	RefreshContent();

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = NULL;

  if (ui.comboBox_InputImage->currentText().left(1) == "<" && ui.comboBox_InputImage->currentText().right(1) == ">") // If unsaved image is selected
  {
    CapturingToolbox* capturingToolbox = dynamic_cast<CapturingToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_Capturing));
    if ((capturingToolbox == NULL) || ((trackedFrameList = capturingToolbox->GetRecordedFrames()) == NULL))
    {
      LOG_ERROR("Unable to get recorded frame list from Capturing toolbox!");
      return PLUS_FAIL;
    }
  }
  else
  {
    int imageFileNameIndex = -1;
    if (ui.comboBox_InputImage->itemText(0).left(1) == "<" && ui.comboBox_InputImage->itemText(0).right(1) == ">") // If unsaved image exists
    {
      imageFileNameIndex = ui.comboBox_InputImage->currentIndex() - 1;
    }
    else
    {
      imageFileNameIndex = ui.comboBox_InputImage->currentIndex();
    }

    trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
    if (trackedFrameList->ReadFromSequenceMetafile( m_ImageFileNames.at( imageFileNameIndex ) ) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to load input image file!");
      return PLUS_FAIL;
    }
  }
	
	m_ParentMainWindow->SetStatusBarText(QString(" Reconstructing volume ..."));
	m_ParentMainWindow->SetStatusBarProgress(0);
	RefreshContent();

  PlusTransformName imageToReferenceTransformName( m_ParentMainWindow->GetImageCoordinateFrame(), m_ParentMainWindow->GetReferenceCoordinateFrame() );
  m_VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList, m_ParentMainWindow->GetToolVisualizer()->GetTransformRepository(), imageToReferenceTransformName);

	const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
	for ( int imgNumber = 0; imgNumber < numberOfFrames; ++imgNumber ) 
  {
		// Set progress
    m_ParentMainWindow->SetStatusBarProgress((int)((100.0 * imgNumber) / numberOfFrames + 0.49));
    RefreshContent();

		// Add this tracked frame to the reconstructor
    if (m_VolumeReconstructor->AddTrackedFrame(trackedFrameList->GetTrackedFrame(imgNumber), m_ParentMainWindow->GetToolVisualizer()->GetTransformRepository(), imageToReferenceTransformName) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to add tracked frame to reconstructor! Maybe image to probe calibration is not in the configuration file!");
      return PLUS_FAIL;
    }
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

PlusStatus VolumeReconstructionToolbox::SaveVolumeToFile(QString aOutput)
{
	LOG_TRACE("VolumeReconstructionToolbox::SaveVolumeToFile(" << aOutput.toAscii().data() << ")"); 

	// Write out to file
  if (aOutput.right(3).toLower() == QString("vtk"))
  {
    vtkSmartPointer<vtkDataSetWriter> writer = vtkSmartPointer<vtkDataSetWriter>::New();
    writer->SetFileTypeToBinary();
    writer->SetInput(m_ReconstructedVolume);
    writer->SetFileName(aOutput.toAscii().data());
    writer->Update();
  }
  else if (aOutput.right(3).toLower() == QString("mha"))
  {
    MetaImage* metaImage = new MetaImage(m_ReconstructedVolume->GetDimensions()[0], m_ReconstructedVolume->GetDimensions()[1], m_ReconstructedVolume->GetDimensions()[2],
                                         m_ReconstructedVolume->GetSpacing()[0], m_ReconstructedVolume->GetSpacing()[1], m_ReconstructedVolume->GetSpacing()[2],
                                         MET_UCHAR, 1, m_ReconstructedVolume->GetScalarPointer());

    if (metaImage->Write(aOutput.toAscii().data(), aOutput.toAscii().data()) == false)
    {
      LOG_ERROR("Failed to save reconstructed volume in sequence metafile!");
      return PLUS_FAIL;
    }

    delete metaImage;
  }
  else
  {
    LOG_ERROR("Invalid file extension (.vtk or .mha expected)!");
    return PLUS_FAIL;
  }

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::AddImageFileName(QString aImageFileName)
{
	LOG_TRACE("VolumeReconstructionToolbox::AddImageFileName(" << aImageFileName.toAscii().data() << ")");

  m_ImageFileNames.append(aImageFileName);
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::PopulateImageComboBox()
{
	LOG_TRACE("VolumeReconstructionToolbox::PopulateImageComboBox");

  // Clear images combobox
  for (int i=ui.comboBox_InputImage->count()-1; i>=0; --i)
  {
    ui.comboBox_InputImage->removeItem(i);
  }

  // Get recorded tracked frame list from Capturing toolbox
  vtkTrackedFrameList* recordedFrames = NULL;
  CapturingToolbox* capturingToolbox = dynamic_cast<CapturingToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_Capturing));
  if ((capturingToolbox == NULL) || ((recordedFrames = capturingToolbox->GetRecordedFrames()) == NULL))
  {
    LOG_ERROR("Capturing toolbox not found!");
    return;
  }

  if (recordedFrames->GetNumberOfTrackedFrames() > 0)
  {
    ui.comboBox_InputImage->addItem("<unsaved image from Capturing>");
    ui.comboBox_InputImage->setCurrentIndex(0);
  }

  // Add images from the stored list
  QStringListIterator imagesIterator(m_ImageFileNames);
  while (imagesIterator.hasNext())
  {
    QString imageFileName(imagesIterator.next());
    int lastIndexOfSlash = imageFileName.lastIndexOf('/');
    int lastIndexOfBackslash = imageFileName.lastIndexOf('\\');
    imageFileName.right( imageFileName.length() - std::max(lastIndexOfSlash, lastIndexOfBackslash) );

    ui.comboBox_InputImage->addItem(imageFileName);
  }

  if (ui.comboBox_InputImage->count() > 0)
  {
    ui.comboBox_InputImage->setEnabled(true);
  }
  else // Disable the combobox and indicate that it is empty
  {
    ui.comboBox_InputImage->addItem(tr("Open or capture image first"));
    ui.comboBox_InputImage->setEnabled(false);
  }
}

//-----------------------------------------------------------------------------

void VolumeReconstructionToolbox::InputImageChanged(int aItemIndex)
{
	LOG_TRACE("VolumeReconstructionToolbox::InputImageChanged(" << aItemIndex << ")");

  SetState(ToolboxState_Idle);
}

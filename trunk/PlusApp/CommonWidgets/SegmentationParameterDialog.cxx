#include "SegmentationParameterDialog.h"

#include <QTimer>

#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkDataCollector.h"

#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkPolyDataMapper.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkImageActor.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkRenderWindow.h"

//-----------------------------------------------------------------------------

SegmentationParameterDialog::SegmentationParameterDialog(QWidget* aParent, vtkDataCollector* aDataCollector)
	: QDialog(aParent)
  , m_DataCollector(aDataCollector)
  , m_CanvasImageActor(NULL)
  , m_SegmentedPointsActor(NULL)
  , m_SegmentedPointsPolyData(NULL)
  , m_CandidatesActor(NULL)
  , m_CandidatesPolyData(NULL)
  , m_CanvasRenderer(NULL)
  , m_CanvasRefreshTimer(NULL)
{
	ui.setupUi(this);

  connect( ui.groupBox_ROI, SIGNAL( toggled(bool) ), this, SLOT( GroupBoxROIToggled(bool) ) );
  connect( ui.groupBox_Spacing, SIGNAL( toggled(bool) ), this, SLOT( GroupBoxSpacingToggled(bool) ) );
	connect( ui.pushButton_FreezeImage, SIGNAL( toggled(bool) ), this, SLOT( FreezeImage(bool) ) );
	connect( ui.pushButton_SaveAndExit, SIGNAL( clicked() ), this, SLOT( SaveAndExitClicked() ) );

	// Set up timer for refreshing UI
	m_CanvasRefreshTimer = new QTimer(this);
	connect(m_CanvasRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateCanvas()));

  if (InitializeVisualization() != PLUS_SUCCESS) {
    LOG_ERROR("Initialize visualization failed!");
    return;
  }

  if (ReadConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Fill form with configuration data failed!");
    return;
  }
}

//-----------------------------------------------------------------------------

SegmentationParameterDialog::~SegmentationParameterDialog()
{
  if (m_CanvasImageActor != NULL) {
    m_CanvasImageActor->Delete();
    m_CanvasImageActor = NULL;
  }

  if (m_SegmentedPointsActor != NULL) {
    m_SegmentedPointsActor->Delete();
    m_SegmentedPointsActor = NULL;
  }

  if (m_SegmentedPointsPolyData != NULL) {
    m_SegmentedPointsPolyData->Delete();
    m_SegmentedPointsPolyData = NULL;
  }

  if (m_CandidatesActor != NULL) {
    m_CandidatesActor->Delete();
    m_CandidatesActor = NULL;
  }

  if (m_CandidatesPolyData != NULL) {
    m_CandidatesPolyData->Delete();
    m_CandidatesPolyData = NULL;
  }

  if (m_CanvasRenderer != NULL) {
    m_CanvasRenderer->Delete();
    m_CanvasRenderer = NULL;
  }

	if (m_CanvasRefreshTimer != NULL) {
    m_CanvasRefreshTimer->stop();
		delete m_CanvasRefreshTimer;
		m_CanvasRefreshTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::InitializeVisualization()
{
  LOG_TRACE("SegmentationParameterDialog::InitializeVisualization");

  if (m_DataCollector == NULL || m_DataCollector->GetInitialized() == false) {
    LOG_ERROR("Data collector is not initialized!");
    return PLUS_FAIL;
  }

  // Create canvas image actor
	m_CanvasImageActor = vtkImageActor::New();

	if (m_DataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE) {
		m_CanvasImageActor->VisibilityOn();
		m_CanvasImageActor->SetInput(m_DataCollector->GetOutput());
	} else {
		LOG_WARNING("Data collector has no output port, canvas image actor initalization failed.");
	}

  // Create segmented points actor
	m_SegmentedPointsActor = vtkActor::New();

	m_SegmentedPointsPolyData = vtkPolyData::New();
	m_SegmentedPointsPolyData->Initialize();

	vtkSmartPointer<vtkPolyDataMapper> segmentedPointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkGlyph3D> segmentedPointGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	vtkSmartPointer<vtkSphereSource> segmentedPointSphereSource = vtkSmartPointer<vtkSphereSource>::New();
	segmentedPointSphereSource->SetRadius(4.0);

	segmentedPointGlyph->SetInputConnection(m_SegmentedPointsPolyData->GetProducerPort());
	segmentedPointGlyph->SetSourceConnection(segmentedPointSphereSource->GetOutputPort());
	segmentedPointMapper->SetInputConnection(segmentedPointGlyph->GetOutputPort());

	m_SegmentedPointsActor->SetMapper(segmentedPointMapper);
	m_SegmentedPointsActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
	m_SegmentedPointsActor->VisibilityOff();

  // Create fiducial candidates actor
	m_CandidatesActor = vtkActor::New();

	m_CandidatesPolyData = vtkPolyData::New();
	m_CandidatesPolyData->Initialize();

	vtkSmartPointer<vtkPolyDataMapper> candidatesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkGlyph3D> candidatesGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	vtkSmartPointer<vtkSphereSource> candidatesSphereSource = vtkSmartPointer<vtkSphereSource>::New();
	candidatesSphereSource->SetRadius(3.0);

	candidatesGlyph->SetInputConnection(m_CandidatesPolyData->GetProducerPort());
	candidatesGlyph->SetSourceConnection(candidatesSphereSource->GetOutputPort());
	candidatesMapper->SetInputConnection(candidatesGlyph->GetOutputPort());

	m_CandidatesActor->SetMapper(candidatesMapper);
	m_CandidatesActor->GetProperty()->SetColor(0.0, 1.0, 1.0);
  m_CandidatesActor->GetProperty()->SetOpacity(0.5);
	m_CandidatesActor->VisibilityOff();

  // Setup canvas
	m_CanvasRenderer = vtkRenderer::New(); 
	m_CanvasRenderer->SetBackground(0.1, 0.1, 0.1);
	ui.canvas->GetRenderWindow()->AddRenderer(m_CanvasRenderer);

  // Disable default mouse events
  m_CanvasRenderer->GetRenderWindow()->GetInteractor()->RemoveAllObservers();

  // Add actors to renderer
	m_CanvasRenderer->AddActor(m_CanvasImageActor);
	m_CanvasRenderer->AddActor(m_SegmentedPointsActor);
  m_CanvasRenderer->AddActor(m_CandidatesActor);

	// Compute image camera parameters and set it to display live image
	CalculateImageCameraParameters();

  // Start refresh timer
 	m_CanvasRefreshTimer->start(50);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::ReadConfiguration()
{
  LOG_TRACE("SegmentationParameterDialog::ReadConfiguration");

  //Find Device set element
  vtkSmartPointer<vtkXMLDataElement> usCalibration = m_DataCollector->GetConfigurationData()->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
		LOG_ERROR("No USCalibration element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController");
	if (calibrationController == NULL) {
		LOG_ERROR("No CalibrationController element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> segmentationParameters = calibrationController->FindNestedElementWithName("SegmentationParameters");
	if (segmentationParameters == NULL) {
		LOG_ERROR("No SegmentationParameters element is found in the XML tree!");
		return PLUS_FAIL;
	}

  // Feed parameters
	double approximateSpacingMmPerPixel(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ApproximateSpacingMmPerPixel", approximateSpacingMmPerPixel) )
	{
		ui.label_SpacingResult->setText(QString("%1 (original)").arg(approximateSpacingMmPerPixel));
	} else {
    LOG_WARNING("Could not read ApproximateSpacingMmPerPixel from configuration");
  }

	double morphologicalOpeningCircleRadiusMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MorphologicalOpeningCircleRadiusMm", morphologicalOpeningCircleRadiusMm) )
	{
		ui.doubleSpinBox_OpeningCircleRadius->setValue(morphologicalOpeningCircleRadiusMm);
	} else {
    LOG_WARNING("Could not read MorphologicalOpeningCircleRadiusMm from configuration");
  }

	double morphologicalOpeningBarSizeMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MorphologicalOpeningBarSizeMm", morphologicalOpeningBarSizeMm) )
	{
		ui.doubleSpinBox_OpeningBarSize->setValue(morphologicalOpeningBarSizeMm);
	} else {
    LOG_WARNING("Could not read MorphologicalOpeningBarSizeMm from configuration");
  }

	int regionOfInterest[4] = {0}; 
	if ( segmentationParameters->GetVectorAttribute("RegionOfInterest", 4, regionOfInterest) )
	{
    ui.spinBox_XMin->setValue(regionOfInterest[0]);
    ui.spinBox_YMin->setValue(regionOfInterest[1]);
    ui.spinBox_XMax->setValue(regionOfInterest[2]);
    ui.spinBox_YMax->setValue(regionOfInterest[3]);
	} else {
		LOG_WARNING("Cannot find RegionOfInterest attribute in the configuration");
	}

	double maxLineLengthErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLineLengthErrorPercent", maxLineLengthErrorPercent) )
	{
		ui.doubleSpinBox_LineLengthError->setValue(maxLineLengthErrorPercent);
	} else {
    LOG_WARNING("Could not read MaxLineLengthErrorPercent from configuration");
  }

	double maxLinePairDistanceErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLinePairDistanceErrorPercent", maxLinePairDistanceErrorPercent) )
	{
		ui.doubleSpinBox_LinePairDistanceError->setValue(maxLinePairDistanceErrorPercent);
	} else {
    LOG_WARNING("Could not read MaxLinePairDistanceErrorPercent from configuration");
  }

	double maxLineErrorMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLineErrorMm", maxLineErrorMm) )
	{
		ui.doubleSpinBox_LineError->setValue(maxLineErrorMm);
	} else {
    LOG_WARNING("Could not read MaxLineErrorMm from configuration");
  }

	double maxAngleDifferenceDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxAngleDifferenceDegrees", maxAngleDifferenceDegrees) )
	{
		ui.doubleSpinBox_AngleDifference->setValue(maxAngleDifferenceDegrees);
	} else {
    LOG_WARNING("Could not read MaxAngleDifferenceDegrees from configuration");
  }

	double minThetaDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MinThetaDegrees", minThetaDegrees) )
	{
		ui.doubleSpinBox_MinTheta->setValue(minThetaDegrees);
	} else {
    LOG_WARNING("Could not read MinThetaDegrees from configuration");
  }

	double maxThetaDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxThetaDegrees", maxThetaDegrees) )
	{
		ui.doubleSpinBox_MaxTheta->setValue(maxThetaDegrees);
	} else {
    LOG_WARNING("Could not read MaxThetaDegrees from configuration");
  }

	double thresholdImage(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ThresholdImage", thresholdImage) )
	{
		ui.doubleSpinBox_ImageThreshold->setValue(thresholdImage);
	} else {
    LOG_WARNING("Could not read ThresholdImage from configuration");
  }

	double findLines3PtDistanceMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("FindLines3PtDistanceMm", findLines3PtDistanceMm) )
	{
		ui.doubleSpinBox_Line3rdPointDist->setValue(findLines3PtDistanceMm);
	} else {
    LOG_WARNING("Could not read FindLines3PtDistanceMm from configuration");
  }

	double useOriginalImageIntensityForDotIntensityScore(-1); 
	if ( segmentationParameters->GetScalarAttribute("UseOriginalImageIntensityForDotIntensityScore", useOriginalImageIntensityForDotIntensityScore) )
	{
    if (useOriginalImageIntensityForDotIntensityScore == 0) {
      ui.checkBox_OriginalIntensityForDots->setChecked(false);
    } else if (useOriginalImageIntensityForDotIntensityScore == 1) {
      ui.checkBox_OriginalIntensityForDots->setChecked(true);
    } else {
      LOG_WARNING("The value of UseOriginalImageIntensityForDotIntensityScore segmentation parameter should be 0 or 1");
    }
	} else {
    LOG_WARNING("Could not read UseOriginalImageIntensityForDotIntensityScore from configuration");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::SaveAndExitClicked()
{
  LOG_TRACE("SegmentationParameterDialog::SaveAndExitClicked");

  if (WriteConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Write configuration failed!");
    return;
  }

  accept();
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::WriteConfiguration()
{
  LOG_TRACE("SegmentationParameterDialog::WriteConfiguration");

  /* TODO
  // Find Device set element
	vtkSmartPointer<vtkXMLDataElement> usDataCollection = m_ConfigurationData->FindNestedElementWithName("USDataCollection");
	if (usDataCollection == NULL) {
		LOG_ERROR("No USDataCollection element is found in the XML tree!");
		return;
	}

	vtkSmartPointer<vtkXMLDataElement> deviceSet = usDataCollection->FindNestedElementWithName("DeviceSet");
	if (deviceSet == NULL) {
		LOG_ERROR("No DeviceSet element is found in the XML tree!");
		return;
	}

  // Set name and description to XML
  deviceSet->SetAttribute("Name", ui.lineEdit_DeviceSetName->text());
  deviceSet->SetAttribute("Description", ui.textEdit_Description->text());
  */

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::GroupBoxROIToggled(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::GroupBoxROIToggled(" << (aOn?"true":"false") << ")");

  ui.groupBox_Spacing->blockSignals(true);
  ui.groupBox_Spacing->setChecked(!aOn);
  ui.groupBox_Spacing->blockSignals(false);

  if (aOn) {
    if (SwitchToROIMode() != PLUS_SUCCESS) {
      LOG_ERROR("Switch to ROI mode failed!");
      return;
    }
  } else {
    if (SwitchToSpacingMode() != PLUS_SUCCESS) {
      LOG_ERROR("Switch to ROI mode failed!");
      return;
    }
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::GroupBoxSpacingToggled(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::GroupBoxSpacingToggled(" << (aOn?"true":"false") << ")");

  ui.groupBox_ROI->blockSignals(true);
  ui.groupBox_ROI->setChecked(!aOn);
  ui.groupBox_ROI->blockSignals(false);

  if (aOn) {
    if (SwitchToSpacingMode() != PLUS_SUCCESS) {
      LOG_ERROR("Switch to ROI mode failed!");
      return;
    }
  } else {
    if (SwitchToROIMode() != PLUS_SUCCESS) {
      LOG_ERROR("Switch to ROI mode failed!");
      return;
    }
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::resizeEvent(QResizeEvent* aEvent)
{
  LOG_TRACE("SegmentationParameterDialog::resizeEvent");

  CalculateImageCameraParameters();
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::CalculateImageCameraParameters()
{
	LOG_TRACE("SegmentationParameterDialog::CalculateImageCameraParameters");

	// Calculate image center
	double imageCenterX = 0;
	double imageCenterY = 0;
	int dimensions[3];
	m_DataCollector->GetVideoSource()->GetFrameSize(dimensions);
	imageCenterX = dimensions[0] / 2.0;
	imageCenterY = dimensions[1] / 2.0;

	// Set up camera
	vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
	imageCamera->SetFocalPoint(imageCenterX, imageCenterY, 0);
	imageCamera->SetViewUp(0, -1, 0);
	imageCamera->SetClippingRange(0.1, 2000.0);
	imageCamera->ParallelProjectionOn();

	// Calculate distance of camera from the plane
	int *size = ui.canvas->GetRenderWindow()->GetSize();
	if ((double)size[0] / (double)size[1] > imageCenterX / imageCenterY) {
		// If canvas aspect ratio is more elongenated in the X position then compute the distance according to the Y axis
		imageCamera->SetParallelScale(imageCenterY);
	} else {
		imageCamera->SetParallelScale(imageCenterY * sqrt((double)size[0] / (double)size[1]));
	}

	imageCamera->SetPosition(imageCenterX, imageCenterY, -200.0);
  m_CanvasRenderer->SetActiveCamera(imageCamera);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::UpdateCanvas()
{
	//LOG_TRACE("SegmentationParameterDialog::UpdateCanvas");

  ui.canvas->update();
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::FreezeImage(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::FreezeImage(" << (aOn?"true":"false") << ")");

  if (aOn) {
    m_CanvasRefreshTimer->stop();
  } else {
   	m_CanvasRefreshTimer->start(50);
  }
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::SwitchToROIMode()
{
  //TODO

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::SwitchToSpacingMode()
{
  //TODO

  return PLUS_SUCCESS;
}

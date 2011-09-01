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
#include "vtkCallbackCommand.h"
#include "vtkLineSource.h"
#include "vtkCubeSource.h"
#include "vtkPropPicker.h"

//----------------------------------------------------------------------
class vtkROIModeHandler : public vtkCallbackCommand
{
public:

  //----------------------------------------------------------------------
  /*!
   * \brief Creator function
   */
	static vtkROIModeHandler *New()
	{
		vtkROIModeHandler *cb = new vtkROIModeHandler();
		return cb;
	}

  //----------------------------------------------------------------------
  /*!
   * \brief Execute function - called every time an observed event is fired
   */
	virtual void Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData))
	{
    if (! (vtkCommand::LeftButtonPressEvent == eventId || vtkCommand::MouseMoveEvent == eventId || vtkCommand::LeftButtonReleaseEvent == eventId)) {
      return;
    }

    vtkRenderWindowInteractor* interactor = dynamic_cast<vtkRenderWindowInteractor*>(caller);
    if (interactor && m_ParentDialog)
    {
      int x = 0;
      int y = 0;
      interactor->GetEventPosition(x, y);

      // Compute world coordinates
      int* canvasSize;
      canvasSize = m_ParentDialog->GetCanvasRenderer()->GetRenderWindow()->GetSize();
      int imageDimensions[3];
      m_ParentDialog->GetDataCollector()->GetVideoSource()->GetFrameSize(imageDimensions);

      double offsetX = 0.0;
      double offsetY = 0.0;
      double monitorPerImageScaling = 0.0;
    	if ((double)canvasSize[0] / (double)canvasSize[1] > (double)imageDimensions[0] / (double)imageDimensions[1]) {
        monitorPerImageScaling = (double)canvasSize[1] / (double)imageDimensions[1];
        offsetX = ((double)canvasSize[0] - ((double)imageDimensions[0] * monitorPerImageScaling)) / 2.0;
      } else {
        monitorPerImageScaling = (double)canvasSize[0] / (double)imageDimensions[0];
        offsetY = ((double)canvasSize[1] - ((double)imageDimensions[1] * (double)canvasSize[0] / (double)imageDimensions[0])) / 2.0;
      }

      double xWorld = ((double)x - offsetX) / monitorPerImageScaling;
      double yWorld = ((double)canvasSize[1] - (double)y - offsetY) / monitorPerImageScaling;

      // Handle events
      if (vtkCommand::LeftButtonPressEvent == eventId)
		  {
        LOG_DEBUG("Press - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        vtkRenderer* renderer = m_ParentDialog->GetCanvasRenderer();
        vtkPropPicker* picker = dynamic_cast<vtkPropPicker*>(renderer->GetRenderWindow()->GetInteractor()->GetPicker());

        if (picker && picker->Pick(x, y, 0.0, renderer) > 0) {
          if (picker->GetActor() == m_TopLeftHandleActor) {
            m_TopLeftHandlePicked = true;
          } else if (picker->GetActor() == m_BottomRightHandleActor) {
            m_BottomRightHandlePicked = true;
          }
        }
		  }
      else if ((vtkCommand::MouseMoveEvent == eventId) && (m_TopLeftHandlePicked || m_BottomRightHandlePicked))
      {
        LOG_DEBUG("Move - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        int newXMin = -1;
        int newYMin = -1;
        int newXMax = -1;
        int newYMax = -1;

        if (m_TopLeftHandlePicked) {
          double bottomRightPosition[3];
          m_BottomRightHandleCubeSource->GetCenter(bottomRightPosition);

          if ((xWorld < bottomRightPosition[0] - 10.0) && (yWorld < bottomRightPosition[1] - 10.0)) {
            m_TopLeftHandleCubeSource->SetCenter(xWorld, yWorld, -0.5);
          }

          newXMin = (int)(xWorld + 0.5);
          newYMin = (int)(yWorld + 0.5);

        } else if (m_BottomRightHandlePicked) {
          double topLeftPosition[3];
          m_TopLeftHandleCubeSource->GetCenter(topLeftPosition);

          if ((xWorld < topLeftPosition[0] - 10.0) && (yWorld < topLeftPosition[1] - 10.0)) {
            m_BottomRightHandleCubeSource->SetCenter(xWorld, yWorld, -0.5);
          }

          newXMax = (int)(xWorld + 0.5);
          newYMax = (int)(yWorld + 0.5);

        }

        m_ParentDialog->SetROI(newXMin, newYMin, newXMax, newYMax);
      }
      else if (vtkCommand::LeftButtonReleaseEvent == eventId)
      {
        LOG_DEBUG("Release - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        m_TopLeftHandlePicked = false;
        m_BottomRightHandlePicked = false;
      }
    }
	}

  //----------------------------------------------------------------------
  /*!
   * \brief Set parent segmentation parameter dialog
   * \param aParentDialog Pointer to the parent dialog
   */
  void SetParentDialog(SegmentationParameterDialog* aParentDialog)
  {
    m_ParentDialog = aParentDialog;

    if (InitializeVisualization() != PLUS_SUCCESS) {
      LOG_ERROR("Initializing visualization failed!");
      return;
    }
  }

  //----------------------------------------------------------------------
  /*!
   * \brief Enable/Disable handler
   * \param aOn True if enable, false if disable
   */
  void SetEnabled(bool aOn)
  {
    //TODO
  }

  //----------------------------------------------------------------------
  /*!
   * \brief Draw ROI - draw handles and lines on canvas
   */
  PlusStatus DrawROI()
  {
    // Get ROI
    int xMin = -1;
    int yMin = -1;
    int xMax = -1;
    int yMax = -1;

    m_ParentDialog->GetROI(xMin, yMin, xMax, yMax);

    // Set line positions
    m_LeftLineSource->SetPoint1(xMin, yMin, -0.5);
    m_LeftLineSource->SetPoint2(xMin, yMax, -0.5);
    m_TopLineSource->SetPoint1(xMin, yMin, -0.5);
    m_TopLineSource->SetPoint2(xMax, yMin, -0.5);
    m_RightLineSource->SetPoint1(xMax, yMin, -0.5);
    m_RightLineSource->SetPoint2(xMax, yMax, -0.5);
    m_BottomLineSource->SetPoint1(xMin, yMax, -0.5);
    m_BottomLineSource->SetPoint2(xMax, yMax, -0.5);

    // Set handle positions
    m_TopLeftHandleCubeSource->SetCenter(xMin, yMin, -0.5);
    m_BottomRightHandleCubeSource->SetCenter(xMax, yMax, -0.5);

    return PLUS_SUCCESS;
  }

private:
  //----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  vtkROIModeHandler::vtkROIModeHandler()
  {
    m_ParentDialog = NULL;
    m_ActorCollection = NULL;
    m_TopLeftHandleActor = NULL;
    m_BottomRightHandleActor = NULL;
    m_TopLeftHandleCubeSource = NULL;
    m_BottomRightHandleCubeSource = NULL;
    m_TopLeftHandlePicked = false;
    m_BottomRightHandlePicked = false;
    m_LeftLineSource = NULL;
    m_TopLineSource = NULL;
    m_RightLineSource = NULL;
    m_BottomLineSource = NULL;
  }

  //----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  vtkROIModeHandler::~vtkROIModeHandler()
  {
    if (m_ActorCollection != NULL) {
      m_ActorCollection->Delete();
      m_ActorCollection = NULL;
    }

    if (m_TopLeftHandleActor != NULL) {
      m_TopLeftHandleActor->Delete();
      m_TopLeftHandleActor = NULL;
    }

    if (m_BottomRightHandleActor != NULL) {
      m_BottomRightHandleActor->Delete();
      m_BottomRightHandleActor = NULL;
    }

    if (m_TopLeftHandleCubeSource != NULL) {
      m_TopLeftHandleCubeSource->Delete();
      m_TopLeftHandleCubeSource = NULL;
    }

    if (m_BottomRightHandleCubeSource != NULL) {
      m_BottomRightHandleCubeSource->Delete();
      m_BottomRightHandleCubeSource = NULL;
    }

    if (m_LeftLineSource != NULL) {
      m_LeftLineSource->Delete();
      m_LeftLineSource = NULL;
    }

    if (m_TopLineSource != NULL) {
      m_TopLineSource->Delete();
      m_TopLineSource = NULL;
    }

    if (m_RightLineSource != NULL) {
      m_RightLineSource->Delete();
      m_RightLineSource = NULL;
    }

    if (m_BottomLineSource != NULL) {
      m_BottomLineSource->Delete();
      m_BottomLineSource = NULL;
    }
  }

protected:

  //----------------------------------------------------------------------
  /*!
   * \brief Initialize visualization - create actors, draw input ROI
   */
  PlusStatus InitializeVisualization()
  {
    // Create actors
    m_ActorCollection = vtkActorCollection::New();

    vtkSmartPointer<vtkActor> leftLineActor = vtkSmartPointer<vtkActor>::New();
	  vtkSmartPointer<vtkPolyDataMapper> leftLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_LeftLineSource = vtkLineSource::New();
    leftLineMapper->SetInputConnection(m_LeftLineSource->GetOutputPort());
	  leftLineActor->SetMapper(leftLineMapper);
	  leftLineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_ActorCollection->AddItem(leftLineActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(leftLineActor);

    vtkSmartPointer<vtkActor> topLineActor = vtkSmartPointer<vtkActor>::New();
	  vtkSmartPointer<vtkPolyDataMapper> topLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_TopLineSource = vtkLineSource::New();
    topLineMapper->SetInputConnection(m_TopLineSource->GetOutputPort());
	  topLineActor->SetMapper(topLineMapper);
	  topLineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_ActorCollection->AddItem(topLineActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(topLineActor);

    vtkSmartPointer<vtkActor> rightLineActor = vtkSmartPointer<vtkActor>::New();
	  vtkSmartPointer<vtkPolyDataMapper> rightLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_RightLineSource = vtkLineSource::New();
    rightLineMapper->SetInputConnection(m_RightLineSource->GetOutputPort());
	  rightLineActor->SetMapper(rightLineMapper);
	  rightLineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_ActorCollection->AddItem(rightLineActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(rightLineActor);

    vtkSmartPointer<vtkActor> bottomLineActor = vtkSmartPointer<vtkActor>::New();
	  vtkSmartPointer<vtkPolyDataMapper> bottomLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_BottomLineSource = vtkLineSource::New();
    bottomLineMapper->SetInputConnection(m_BottomLineSource->GetOutputPort());
	  bottomLineActor->SetMapper(bottomLineMapper);
	  bottomLineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_ActorCollection->AddItem(bottomLineActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(bottomLineActor);

    m_TopLeftHandleActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> topLeftHandleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  m_TopLeftHandleCubeSource = vtkCubeSource::New();
	  m_TopLeftHandleCubeSource->SetXLength(6.0);
    m_TopLeftHandleCubeSource->SetYLength(6.0);
    m_TopLeftHandleCubeSource->SetZLength(6.0);
	  topLeftHandleMapper->SetInputConnection(m_TopLeftHandleCubeSource->GetOutputPort());
	  m_TopLeftHandleActor->SetMapper(topLeftHandleMapper);
	  m_TopLeftHandleActor->GetProperty()->SetColor(1.0, 0.0, 0.5);
    m_ActorCollection->AddItem(m_TopLeftHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_TopLeftHandleActor);

    m_BottomRightHandleActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> bottomRightHandleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  m_BottomRightHandleCubeSource = vtkCubeSource::New();
	  m_BottomRightHandleCubeSource->SetXLength(6.0);
    m_BottomRightHandleCubeSource->SetYLength(6.0);
    m_BottomRightHandleCubeSource->SetZLength(6.0);
	  bottomRightHandleMapper->SetInputConnection(m_BottomRightHandleCubeSource->GetOutputPort());
	  m_BottomRightHandleActor->SetMapper(bottomRightHandleMapper);
	  m_BottomRightHandleActor->GetProperty()->SetColor(1.0, 0.0, 0.5);
    m_ActorCollection->AddItem(m_BottomRightHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_BottomRightHandleActor);

    // Draw current (input) ROI
    if ( DrawROI() != PLUS_SUCCESS ) {
      LOG_ERROR("ROI drawing failed!");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

private:
  //! Parent segmentation parameter dialog
  SegmentationParameterDialog*  m_ParentDialog;

  //! Actor collection for ROI mode
  vtkActorCollection*           m_ActorCollection;

  //TODO
  vtkActor*                     m_TopLeftHandleActor;
  vtkActor*                     m_BottomRightHandleActor;
  vtkCubeSource*                m_TopLeftHandleCubeSource;
  vtkCubeSource*                m_BottomRightHandleCubeSource;
  vtkLineSource*                m_LeftLineSource;
  vtkLineSource*                m_TopLineSource;
  vtkLineSource*                m_RightLineSource;
  vtkLineSource*                m_BottomLineSource;

  bool                          m_TopLeftHandlePicked;
  bool                          m_BottomRightHandlePicked;
};

//----------------------------------------------------------------------
class vtkSpacingModeHandler : public vtkCallbackCommand
{
public:
  //! Creator function
	static vtkSpacingModeHandler *New()
	{
		vtkSpacingModeHandler *cb = new vtkSpacingModeHandler();
		return cb;
	}

  //! Execute function - called every time an observed event is fired
	virtual void Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData))
	{
	}

  void SetEnabled(bool aOn)
  {
  }

  //TODO
};

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
  , m_ROIModeHandler(NULL)
  , m_SpacingModeHandler(NULL)
{
	ui.setupUi(this);

  connect( ui.groupBox_ROI, SIGNAL( toggled(bool) ), this, SLOT( GroupBoxROIToggled(bool) ) );
  connect( ui.groupBox_Spacing, SIGNAL( toggled(bool) ), this, SLOT( GroupBoxSpacingToggled(bool) ) );
	connect( ui.pushButton_FreezeImage, SIGNAL( toggled(bool) ), this, SLOT( FreezeImage(bool) ) );
	connect( ui.pushButton_SaveAndExit, SIGNAL( clicked() ), this, SLOT( SaveAndExitClicked() ) );
  connect( ui.spinBox_XMin, SIGNAL( valueChanged(int) ), this, SLOT( ROIXMinChanged(int) ) );
	connect( ui.spinBox_YMin, SIGNAL( valueChanged(int) ), this, SLOT( ROIYMinChanged(int) ) );
	connect( ui.spinBox_XMax, SIGNAL( valueChanged(int) ), this, SLOT( ROIXMaxChanged(int) ) );
	connect( ui.spinBox_YMax, SIGNAL( valueChanged(int) ), this, SLOT( ROIYMaxChanged(int) ) );

	// Set up timer for refreshing UI
	m_CanvasRefreshTimer = new QTimer(this);
	connect(m_CanvasRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateCanvas()));

  if (ReadConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Fill form with configuration data failed!");
    return;
  }

  if (InitializeVisualization() != PLUS_SUCCESS) {
    LOG_ERROR("Initialize visualization failed!");
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

  if (m_ROIModeHandler != NULL) {
    m_ROIModeHandler->Delete(); 
    m_ROIModeHandler = NULL; 
  }

  if (m_SpacingModeHandler != NULL) {
    m_SpacingModeHandler->Delete(); 
    m_SpacingModeHandler = NULL; 
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

  // Create default picker
  m_CanvasRenderer->GetRenderWindow()->GetInteractor()->CreateDefaultPicker();

  // Add actors to renderer
	m_CanvasRenderer->AddActor(m_CanvasImageActor);
	m_CanvasRenderer->AddActor(m_SegmentedPointsActor);
  m_CanvasRenderer->AddActor(m_CandidatesActor);

	// Compute image camera parameters and set it to display live image
	CalculateImageCameraParameters();

  // Switch to ROI mode by default
  SwitchToROIMode();

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
	int *size = m_CanvasRenderer->GetRenderWindow()->GetSize();
	if ((double)size[0] / (double)size[1] > imageCenterX / imageCenterY) {
		// If canvas aspect ratio is more elongenated in the X position then compute the distance according to the Y axis
		imageCamera->SetParallelScale(imageCenterY);
	} else {
		imageCamera->SetParallelScale(imageCenterX * (double)size[1] / (double)size[0]);
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

  //!!!TODO!!! Segmentation
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
  LOG_TRACE("SegmentationParameterDialog::SwitchToROIMode");

  if (m_ROIModeHandler == NULL) {
    m_ROIModeHandler = vtkROIModeHandler::New(); 
    m_ROIModeHandler->SetParentDialog(this);
  }

  if (m_SpacingModeHandler != NULL) {
    m_SpacingModeHandler->SetEnabled(false);
  }

  m_ROIModeHandler->SetEnabled(true);

  ui.canvas->GetRenderWindow()->GetInteractor()->RemoveAllObservers();
  ui.canvas->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, m_ROIModeHandler);
  ui.canvas->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::LeftButtonReleaseEvent, m_ROIModeHandler);
  ui.canvas->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, m_ROIModeHandler);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::SwitchToSpacingMode()
{
  LOG_TRACE("SegmentationParameterDialog::SwitchToSpacingMode");

  //TODO

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::SetROI(int aXMin, int aYMin, int aXMax, int aYMax)
{
  LOG_TRACE("SegmentationParameterDialog::SetROI(" << aXMin << ", " << aYMin << ", " << aXMax << ", " << aYMax << ")");

  ui.spinBox_XMin->blockSignals(true);

  if (aXMin > 0) {
    ui.spinBox_XMin->setValue(aXMin);
  }
  if (aYMin > 0) {
    ui.spinBox_YMin->setValue(aYMin);
  }
  if (aXMax > 0) {
    ui.spinBox_XMax->setValue(aXMax);
  }
  if (aYMax > 0) {
    ui.spinBox_YMax->setValue(aYMax);
  }

  ui.spinBox_XMin->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::GetROI(int &aXMin, int &aYMin, int &aXMax, int &aYMax)
{
  LOG_TRACE("SegmentationParameterDialog::GetROI");

  aXMin = ui.spinBox_XMin->value();
  aYMin = ui.spinBox_YMin->value();
  aXMax = ui.spinBox_XMax->value();
  aYMax = ui.spinBox_YMax->value();
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIXMinChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIXMinChanged(" << aValue << ")");

  if (m_ROIModeHandler != NULL) {
    if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
      LOG_ERROR("Draw ROI failed!");
    }
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIYMinChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIYMinChanged(" << aValue << ")");

  if (m_ROIModeHandler != NULL) {
    if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
      LOG_ERROR("Draw ROI failed!");
    }
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIXMaxChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIXMaxChanged(" << aValue << ")");

  if (m_ROIModeHandler != NULL) {
    if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
      LOG_ERROR("Draw ROI failed!");
    }
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIYMaxChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIYMaxChanged(" << aValue << ")");

  if (m_ROIModeHandler != NULL) {
    if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
      LOG_ERROR("Draw ROI failed!");
    }
  }
}

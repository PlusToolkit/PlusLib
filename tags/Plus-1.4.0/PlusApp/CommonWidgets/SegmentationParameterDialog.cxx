/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "SegmentationParameterDialog.h"

#include "ConfigFileSaverDialog.h"

#include <QTimer>

#include "vtkDataCollector.h"
#include "FidPatternRecognitionCommon.h"
#include "FidPatternRecognition.h"
#include "PlusVideoFrame.h"

#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
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
#include "vtkMath.h"
#include "vtkConeSource.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "vtkImageData.h" 
#include "vtkPlusVideoSource.h"
#include "TrackedFrame.h"

//----------------------------------------------------------------------

/*! \class vtkSegmentationParameterDialogModeHandlerBase 
 *
 * \brief Base class for the segmentation parameter dialog mode handlers
 *
 * \ingroup PlusAppCommonWidgets
 *
 */
class vtkSegmentationParameterDialogModeHandlerBase : public vtkCallbackCommand
{
public:
  //----------------------------------------------------------------------
  /*!
   * \brief Set parent segmentation parameter dialog
   * \param aParentDialog Pointer to the parent dialog
   */
  void SetParentDialog(SegmentationParameterDialog* aParentDialog)
  {
    LOG_TRACE("vtkSegmentationParameterDialogModeHandlerBase::SetParentDialog");

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
    LOG_TRACE("vtkSegmentationParameterDialogModeHandlerBase::SetEnabled(" << (aOn?"true":"false") << ")");

    vtkSmartPointer<vtkProperty> prop = vtkSmartPointer<vtkProperty>::New();
    if (aOn) {
      prop->SetOpacity(1.0);
      prop->SetColor(1.0, 0.0, 0.0);
      
    } else {
      prop->SetOpacity(0.0);
    }
    
    m_ActorCollection->ApplyProperties(prop); 

    ColorActors();
  }

protected:
  //----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  vtkSegmentationParameterDialogModeHandlerBase::vtkSegmentationParameterDialogModeHandlerBase()
  {
    m_ParentDialog = NULL;
    m_ActorCollection = NULL;
  }

  //----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  virtual vtkSegmentationParameterDialogModeHandlerBase::~vtkSegmentationParameterDialogModeHandlerBase()
  {
    if (m_ActorCollection != NULL) {
      m_ActorCollection->Delete();
      m_ActorCollection = NULL;
    }
  }

  /*!
   * \brief Initialize visualization (pure virtual function)
   */
  virtual PlusStatus InitializeVisualization() = 0;

  /*!
   * \brief Color certain actors after re-enabling (the color are set to the same then) (pure virtual function)
   */
  virtual PlusStatus ColorActors() = 0;

protected:
  //! Parent segmentation parameter dialog
  SegmentationParameterDialog*  m_ParentDialog;

  //! Actor collection for the current mode
  vtkActorCollection*           m_ActorCollection;
};

//----------------------------------------------------------------------

/*! \class vtkROIModeHandler 
 *
 * \brief Class handling the events of the ROI mode in segmentation parameters dialog
 *
 * \ingroup PlusAppCommonWidgets
 *
 */
class vtkROIModeHandler : public vtkSegmentationParameterDialogModeHandlerBase
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
    //LOG_TRACE("vtkROIModeHandler::Execute");

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
      int imageDimensions[2];
      m_ParentDialog->GetFrameSize(imageDimensions);

      double offsetXMonitor = 0.0;
      double offsetYMonitor = 0.0;
      double monitorPerImageScaling = 0.0;
    	if ((double)canvasSize[0] / (double)canvasSize[1] > (double)imageDimensions[0] / (double)imageDimensions[1]) {
        monitorPerImageScaling = (double)canvasSize[1] / (double)imageDimensions[1];
        offsetXMonitor = ((double)canvasSize[0] - ((double)imageDimensions[0] * monitorPerImageScaling)) / 2.0;
      } else {
        monitorPerImageScaling = (double)canvasSize[0] / (double)imageDimensions[0];
        offsetYMonitor = ((double)canvasSize[1] - ((double)imageDimensions[1] * monitorPerImageScaling)) / 2.0;
      }

      double xWorld = ((double)x - offsetXMonitor) / monitorPerImageScaling;
      double yWorld = ((double)canvasSize[1] - (double)y - offsetYMonitor) / monitorPerImageScaling;

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

          newXMin = (int)(xWorld + 0.5);
          newYMin = (int)(yWorld + 0.5);

        } else if (m_BottomRightHandlePicked) {
          double topLeftPosition[3];
          m_TopLeftHandleCubeSource->GetCenter(topLeftPosition);

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
   * \brief Draw ROI - draw handles and lines on canvas
   */
  PlusStatus DrawROI()
  {
    LOG_TRACE("vtkROIModeHandler::DrawROI");

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
    : vtkSegmentationParameterDialogModeHandlerBase()
  {
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
   * \brief Initialize visualization - create actors, draw input ROI (overridden function)
   */
  PlusStatus InitializeVisualization()
  {
    LOG_TRACE("vtkROIModeHandler::InitializeVisualization");

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

  //----------------------------------------------------------------------
  /*!
   * \brief Color certain actors after re-enabling (the color are set to the same then) (overridden function)
   */
  PlusStatus ColorActors()
  {
    LOG_TRACE("vtkROIModeHandler::ColorActors");

	  m_TopLeftHandleActor->GetProperty()->SetColor(1.0, 0.0, 0.5);
	  m_BottomRightHandleActor->GetProperty()->SetColor(1.0, 0.0, 0.5);

    return PLUS_SUCCESS;
  }

private:
  //! Actor of top left corner handle
  vtkActor*                     m_TopLeftHandleActor;

  //! Actor of bottom right corner handle
  vtkActor*                     m_BottomRightHandleActor;

  //! Cube source of the top left corner handle (direct access needed to move it around)
  vtkCubeSource*                m_TopLeftHandleCubeSource;

  //! Cube source of the bottom right corner handle (direct access needed to move it around)
  vtkCubeSource*                m_BottomRightHandleCubeSource;

  //! Line source of left line (one side of the ROI rectangle)
  vtkLineSource*                m_LeftLineSource;

  //! Line source of top line (one side of the ROI rectangle)
  vtkLineSource*                m_TopLineSource;

  //! Line source of right line (one side of the ROI rectangle)
  vtkLineSource*                m_RightLineSource;

  //! Line source of bottom line (one side of the ROI rectangle)
  vtkLineSource*                m_BottomLineSource;

  //! Flag indicating if top left corner handle is picked
  bool                          m_TopLeftHandlePicked;

  //! Flag indicating if bottom right corner handle is picked
  bool                          m_BottomRightHandlePicked;
};

//----------------------------------------------------------------------

/*! \class vtkSpacingModeHandler 
 *
 * \brief Class handling the events of the spacing mode in segmentation parameters dialog
 *
 * \ingroup PlusAppCommonWidgets
 *
 */
class vtkSpacingModeHandler : public vtkSegmentationParameterDialogModeHandlerBase
{
public:
  //! Creator function
	static vtkSpacingModeHandler *New()
	{
		vtkSpacingModeHandler *cb = new vtkSpacingModeHandler();
		return cb;
	}

  //----------------------------------------------------------------------
  /*!
   * \brief Execute function - called every time an observed event is fired
   */
	virtual void Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData))
	{
    //LOG_TRACE("vtkSpacingModeHandler::Execute");

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
      m_ParentDialog->GetFrameSize(imageDimensions);

      double offsetXMonitor = 0.0;
      double offsetYMonitor = 0.0;
      double monitorPerImageScaling = 0.0;
    	if ((double)canvasSize[0] / (double)canvasSize[1] > (double)imageDimensions[0] / (double)imageDimensions[1]) {
        monitorPerImageScaling = (double)canvasSize[1] / (double)imageDimensions[1];
        offsetXMonitor = ((double)canvasSize[0] - ((double)imageDimensions[0] * monitorPerImageScaling)) / 2.0;
      } else {
        monitorPerImageScaling = (double)canvasSize[0] / (double)imageDimensions[0];
        offsetYMonitor = ((double)canvasSize[1] - ((double)imageDimensions[1] * monitorPerImageScaling)) / 2.0;
      }

      double xWorld = ((double)x - offsetXMonitor) / monitorPerImageScaling;
      double yWorld = ((double)canvasSize[1] - (double)y - offsetYMonitor) / monitorPerImageScaling;

      // Handle events
      if (vtkCommand::LeftButtonPressEvent == eventId)
		  {
        LOG_DEBUG("Press - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        vtkRenderer* renderer = m_ParentDialog->GetCanvasRenderer();
        vtkPropPicker* picker = dynamic_cast<vtkPropPicker*>(renderer->GetRenderWindow()->GetInteractor()->GetPicker());

        if (picker && picker->Pick(x, y, 0.0, renderer) > 0) {
          if (picker->GetActor() == m_HorizontalLeftHandleActor) {
            m_HorizontalLeftHandlePicked = true;
          } else if (picker->GetActor() == m_HorizontalRightHandleActor) {
            m_HorizontalRightHandlePicked = true;
          } else if (picker->GetActor() == m_VerticalTopHandleActor) {
            m_VerticalTopHandlePicked = true;
          } else if (picker->GetActor() == m_VerticalBottomHandleActor) {
            m_VerticalBottomHandlePicked = true;
          }
        }
		  }
      else if ((vtkCommand::MouseMoveEvent == eventId) && (m_HorizontalLeftHandlePicked || m_HorizontalRightHandlePicked || m_VerticalTopHandlePicked || m_VerticalBottomHandlePicked))
      {
        LOG_DEBUG("Move - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        // Get the positions of all handles
        double horizontalLeftPosition[3];
        m_HorizontalLeftHandleCubeSource->GetCenter(horizontalLeftPosition);
        double horizontalRightPosition[3];
        m_HorizontalRightHandleCubeSource->GetCenter(horizontalRightPosition);
        double verticalTopPosition[3];
        m_VerticalTopHandleCubeSource->GetCenter(verticalTopPosition);
        double verticalBottomPosition[3];
        m_VerticalBottomHandleCubeSource->GetCenter(verticalBottomPosition);

        // Change position of the picked handle
        if (m_HorizontalLeftHandlePicked) {
          if (xWorld < horizontalRightPosition[0] - 10.0) {
            m_HorizontalLeftHandleCubeSource->SetCenter(xWorld, yWorld, -0.5);
            m_HorizontalLineSource->SetPoint1(xWorld, yWorld, -0.5);
          }
          m_HorizontalLeftHandleCubeSource->GetCenter(horizontalLeftPosition);

        } else if (m_HorizontalRightHandlePicked) {
          if (xWorld > horizontalLeftPosition[0] + 10.0) {
            m_HorizontalRightHandleCubeSource->SetCenter(xWorld, yWorld, -0.5);
            m_HorizontalLineSource->SetPoint2(xWorld, yWorld, -0.5);
          }
          m_HorizontalRightHandleCubeSource->GetCenter(horizontalRightPosition);

        } else if (m_VerticalTopHandlePicked) {
          if (yWorld < verticalBottomPosition[1] - 10.0) {
            m_VerticalTopHandleCubeSource->SetCenter(xWorld, yWorld, -0.5);
            m_VerticalLineSource->SetPoint1(xWorld, yWorld, -0.5);
          }
          m_VerticalTopHandleCubeSource->GetCenter(verticalTopPosition);

        } else if (m_VerticalBottomHandlePicked) {
          if (yWorld > verticalTopPosition[1] + 10.0) {
            m_VerticalBottomHandleCubeSource->SetCenter(xWorld, yWorld, -0.5);
            m_VerticalLineSource->SetPoint2(xWorld, yWorld, -0.5);
          }
          m_VerticalBottomHandleCubeSource->GetCenter(verticalBottomPosition);

        }

        // Compute and set spacing
        double horizontalLength = sqrt( vtkMath::Distance2BetweenPoints(horizontalLeftPosition, horizontalRightPosition) );
        double verticalLength = sqrt( vtkMath::Distance2BetweenPoints(verticalTopPosition, verticalBottomPosition) );

        m_LineLengthSumImagePixel = horizontalLength + verticalLength;

        if (horizontalLength > 0 && verticalLength > 0) {
          m_ParentDialog->ComputeSpacingFromMeasuredLengthSum();
        }

      }
      else if (vtkCommand::LeftButtonReleaseEvent == eventId)
      {
        LOG_DEBUG("Release - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        m_HorizontalLeftHandlePicked = false;
        m_HorizontalRightHandlePicked = false;
        m_VerticalTopHandlePicked = false;
        m_VerticalBottomHandlePicked = false;
      }
    }
	}

  //----------------------------------------------------------------------
  /*!
   * \brief Get summed line length
   */
  double GetLineLengthSumImagePixel()
  {
    LOG_TRACE("vtkSpacingModeHandler::GetLineLengthSumImagePixel");

    return m_LineLengthSumImagePixel;
  }

private:
  //----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  vtkSpacingModeHandler::vtkSpacingModeHandler()
    : vtkSegmentationParameterDialogModeHandlerBase()
  {
    m_HorizontalLeftHandleActor = NULL;
    m_HorizontalRightHandleActor = NULL;
    m_VerticalTopHandleActor = NULL;
    m_VerticalBottomHandleActor = NULL;
    m_HorizontalLineActor = NULL;
    m_VerticalLineActor = NULL;
    m_HorizontalLeftHandleCubeSource = NULL;
    m_HorizontalRightHandleCubeSource = NULL;
    m_VerticalTopHandleCubeSource = NULL;
    m_VerticalBottomHandleCubeSource = NULL;
    m_HorizontalLeftHandlePicked = false;
    m_HorizontalRightHandlePicked = false;
    m_VerticalTopHandlePicked = false;
    m_VerticalBottomHandlePicked = false;
    m_HorizontalLineSource = NULL;
    m_VerticalLineSource = NULL;
    m_LineLengthSumImagePixel = 0.0;
  }

  //----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  vtkSpacingModeHandler::~vtkSpacingModeHandler()
  {
    if (m_HorizontalLeftHandleActor != NULL) {
      m_HorizontalLeftHandleActor->Delete();
      m_HorizontalLeftHandleActor = NULL;
    }

    if (m_HorizontalRightHandleActor != NULL) {
      m_HorizontalRightHandleActor->Delete();
      m_HorizontalRightHandleActor = NULL;
    }

    if (m_VerticalTopHandleActor != NULL) {
      m_VerticalTopHandleActor->Delete();
      m_VerticalTopHandleActor = NULL;
    }

    if (m_VerticalBottomHandleActor != NULL) {
      m_VerticalBottomHandleActor->Delete();
      m_VerticalBottomHandleActor = NULL;
    }

    if (m_HorizontalLineActor != NULL) {
      m_HorizontalLineActor->Delete();
      m_HorizontalLineActor = NULL;
    }

    if (m_VerticalLineActor != NULL) {
      m_VerticalLineActor->Delete();
      m_VerticalLineActor = NULL;
    }

    if (m_HorizontalLeftHandleCubeSource != NULL) {
      m_HorizontalLeftHandleCubeSource->Delete();
      m_HorizontalLeftHandleCubeSource = NULL;
    }

    if (m_HorizontalRightHandleCubeSource != NULL) {
      m_HorizontalRightHandleCubeSource->Delete();
      m_HorizontalRightHandleCubeSource = NULL;
    }

    if (m_VerticalTopHandleCubeSource != NULL) {
      m_VerticalTopHandleCubeSource->Delete();
      m_VerticalTopHandleCubeSource = NULL;
    }

    if (m_VerticalBottomHandleCubeSource != NULL) {
      m_VerticalBottomHandleCubeSource->Delete();
      m_VerticalBottomHandleCubeSource = NULL;
    }

    if (m_VerticalLineSource != NULL) {
      m_VerticalLineSource->Delete();
      m_VerticalLineSource = NULL;
    }

    if (m_VerticalBottomHandleCubeSource != NULL) {
      m_VerticalBottomHandleCubeSource->Delete();
      m_VerticalBottomHandleCubeSource = NULL;
    }
  }

protected:

  //----------------------------------------------------------------------
  /*!
   * \brief Initialize visualization - create actors, draw input ROI
   */
  PlusStatus InitializeVisualization()
  {
    LOG_TRACE("vtkSpacingModeHandler::InitializeVisualization");

    // Create actors
    m_ActorCollection = vtkActorCollection::New();

    m_HorizontalLineActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> horizontalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_HorizontalLineSource = vtkLineSource::New();
    horizontalLineMapper->SetInputConnection(m_HorizontalLineSource->GetOutputPort());
	  m_HorizontalLineActor->SetMapper(horizontalLineMapper);
	  m_HorizontalLineActor->GetProperty()->SetColor(0.0, 0.7, 0.0);
    m_ActorCollection->AddItem(m_HorizontalLineActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_HorizontalLineActor);

    m_VerticalLineActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> verticalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_VerticalLineSource = vtkLineSource::New();
    verticalLineMapper->SetInputConnection(m_VerticalLineSource->GetOutputPort());
	  m_VerticalLineActor->SetMapper(verticalLineMapper);
	  m_VerticalLineActor->GetProperty()->SetColor(0.0, 0.0, 0.8);
    m_ActorCollection->AddItem(m_VerticalLineActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_VerticalLineActor);

    m_HorizontalLeftHandleActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> horizontalLeftHandleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  m_HorizontalLeftHandleCubeSource = vtkCubeSource::New();
	  m_HorizontalLeftHandleCubeSource->SetXLength(6.0);
    m_HorizontalLeftHandleCubeSource->SetYLength(6.0);
    m_HorizontalLeftHandleCubeSource->SetZLength(6.0);
	  horizontalLeftHandleMapper->SetInputConnection(m_HorizontalLeftHandleCubeSource->GetOutputPort());
	  m_HorizontalLeftHandleActor->SetMapper(horizontalLeftHandleMapper);
	  m_HorizontalLeftHandleActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
    m_ActorCollection->AddItem(m_HorizontalLeftHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_HorizontalLeftHandleActor);

    m_HorizontalRightHandleActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> horizontalRightHandleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  m_HorizontalRightHandleCubeSource = vtkCubeSource::New();
	  m_HorizontalRightHandleCubeSource->SetXLength(6.0);
    m_HorizontalRightHandleCubeSource->SetYLength(6.0);
    m_HorizontalRightHandleCubeSource->SetZLength(6.0);
	  horizontalRightHandleMapper->SetInputConnection(m_HorizontalRightHandleCubeSource->GetOutputPort());
	  m_HorizontalRightHandleActor->SetMapper(horizontalRightHandleMapper);
	  m_HorizontalRightHandleActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
    m_ActorCollection->AddItem(m_HorizontalRightHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_HorizontalRightHandleActor);

    m_VerticalTopHandleActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> verticalTopHandleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  m_VerticalTopHandleCubeSource = vtkCubeSource::New();
	  m_VerticalTopHandleCubeSource->SetXLength(6.0);
    m_VerticalTopHandleCubeSource->SetYLength(6.0);
    m_VerticalTopHandleCubeSource->SetZLength(6.0);
	  verticalTopHandleMapper->SetInputConnection(m_VerticalTopHandleCubeSource->GetOutputPort());
	  m_VerticalTopHandleActor->SetMapper(verticalTopHandleMapper);
	  m_VerticalTopHandleActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_ActorCollection->AddItem(m_VerticalTopHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_VerticalTopHandleActor);

    m_VerticalBottomHandleActor = vtkActor::New();
	  vtkSmartPointer<vtkPolyDataMapper> verticalBottomHandleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  m_VerticalBottomHandleCubeSource = vtkCubeSource::New();
	  m_VerticalBottomHandleCubeSource->SetXLength(6.0);
    m_VerticalBottomHandleCubeSource->SetYLength(6.0);
    m_VerticalBottomHandleCubeSource->SetZLength(6.0);
	  verticalBottomHandleMapper->SetInputConnection(m_VerticalBottomHandleCubeSource->GetOutputPort());
	  m_VerticalBottomHandleActor->SetMapper(verticalBottomHandleMapper);
	  m_VerticalBottomHandleActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_ActorCollection->AddItem(m_VerticalBottomHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_VerticalBottomHandleActor);

    // Get offsets (distance between the canvas edge and the image) and reference lengths
    int* canvasSize;
    canvasSize = m_ParentDialog->GetCanvasRenderer()->GetRenderWindow()->GetSize();
    int imageDimensions[2];
    m_ParentDialog->GetFrameSize(imageDimensions);

    double offsetXImage = 0.0;
    double offsetYImage = 0.0;
    double monitorPerImageScaling = 0.0;
  	if ((double)canvasSize[0] / (double)canvasSize[1] > (double)imageDimensions[0] / (double)imageDimensions[1]) {
      monitorPerImageScaling = (double)canvasSize[1] / (double)imageDimensions[1];
      offsetXImage = (((double)canvasSize[0] / monitorPerImageScaling) - (double)imageDimensions[0]) / 2.0;
    } else {
      monitorPerImageScaling = (double)canvasSize[0] / (double)imageDimensions[0];
      offsetYImage = (((double)canvasSize[1] / monitorPerImageScaling) - (double)imageDimensions[1]) / 2.0;
    }

    double referenceWidth = m_ParentDialog->GetSpacingReferenceWidth() / m_ParentDialog->GetApproximateSpacingMmPerPixel() / monitorPerImageScaling;
    double referenceHeight = m_ParentDialog->GetSpacingReferenceHeight() / m_ParentDialog->GetApproximateSpacingMmPerPixel() / monitorPerImageScaling;

    // Determine and set positions
    double horizontalLeftX = imageDimensions[0] / 2.0 - offsetXImage - referenceWidth / 2.0;
    double horizontalLeftY = imageDimensions[1] / 2.0 - offsetYImage - referenceHeight / 2.0 - 10.0;
    double horizontalRightX = imageDimensions[0] / 2.0 - offsetXImage + referenceWidth / 2.0;
    double horizontalRightY = imageDimensions[1] / 2.0 - offsetYImage - referenceHeight / 2.0 - 10.0;

    double verticalTopX = imageDimensions[0] / 2.0 - offsetXImage + referenceWidth / 2.0 + 10.0;
    double verticalTopY = imageDimensions[1] / 2.0 - offsetYImage - referenceHeight / 2.0;
    double verticalBottomX = imageDimensions[0] / 2.0 - offsetXImage + referenceWidth / 2.0 + 10.0;
    double verticalBottomY = imageDimensions[1] / 2.0 - offsetYImage + referenceHeight / 2.0;

    m_HorizontalLeftHandleCubeSource->SetCenter(horizontalLeftX, horizontalLeftY, -0.5);
    m_HorizontalRightHandleCubeSource->SetCenter(horizontalRightX, horizontalRightY, -0.5);
    m_VerticalTopHandleCubeSource->SetCenter(verticalTopX, verticalTopY, -0.5);
    m_VerticalBottomHandleCubeSource->SetCenter(verticalBottomX, verticalBottomY, -0.5);
    m_HorizontalLineSource->SetPoint1(horizontalLeftX, horizontalLeftY, -0.5);
    m_HorizontalLineSource->SetPoint2(horizontalRightX, horizontalRightY, -0.5);
    m_VerticalLineSource->SetPoint1(verticalTopX, verticalTopY, -0.5);
    m_VerticalLineSource->SetPoint2(verticalBottomX, verticalBottomY, -0.5);

    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------
  /*!
   * \brief Color certain actors after re-enabling (the color are set to the same then) (overridden function)
   */
  PlusStatus ColorActors()
  {
    LOG_TRACE("vtkSpacingModeHandler::ColorActors");

	  m_HorizontalLeftHandleActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
	  m_HorizontalRightHandleActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
	  m_VerticalTopHandleActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
	  m_VerticalBottomHandleActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_HorizontalLineActor->GetProperty()->SetColor(0.0, 0.7, 0.0);
    m_VerticalLineActor->GetProperty()->SetColor(0.0, 0.0, 0.8);

    return PLUS_SUCCESS;
  }

private:
  //! Actor of left handle of the horizontal line
  vtkActor*                     m_HorizontalLeftHandleActor;

  //! Actor of right handle of the horizontal line
  vtkActor*                     m_HorizontalRightHandleActor;

  //! Actor of left handle of the vertical line
  vtkActor*                     m_VerticalTopHandleActor;

  //! Actor of the horizontal line
  vtkActor*                     m_HorizontalLineActor;

  //! Actor of the vertical line
  vtkActor*                     m_VerticalLineActor;

  //! Actor of right handle of the vertical line
  vtkActor*                     m_VerticalBottomHandleActor;

  //! Cube source of the left handle of the horizontal line (direct access needed to move it around)
  vtkCubeSource*                m_HorizontalLeftHandleCubeSource;

  //! Cube source of the right handle of the horizontal line (direct access needed to move it around)
  vtkCubeSource*                m_HorizontalRightHandleCubeSource;

  //! Cube source of the left handle of the vertical line (direct access needed to move it around)
  vtkCubeSource*                m_VerticalTopHandleCubeSource;

  //! Cube source of the right handle of the vertical line (direct access needed to move it around)
  vtkCubeSource*                m_VerticalBottomHandleCubeSource;

  //! Line source of horizontal line
  vtkLineSource*                m_HorizontalLineSource;

  //! Line source of vertical line
  vtkLineSource*                m_VerticalLineSource;

  //! Flag indicating if horizontal left handle is picked
  bool                          m_HorizontalLeftHandlePicked;

  //! Flag indicating if horizontal right handle is picked
  bool                          m_HorizontalRightHandlePicked;

  //! Flag indicating if vertical left handle is picked
  bool                          m_VerticalTopHandlePicked;

  //! Flag indicating if vertical right handle is picked
  bool                          m_VerticalBottomHandlePicked;

  //! Summed line length in pixel value
  double                        m_LineLengthSumImagePixel;
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
  , m_ApproximateSpacingMmPerPixel(0.0)
  , m_ImageFrozen(false)
{
	ui.setupUi(this);

  connect( ui.groupBox_ROI, SIGNAL( toggled(bool) ), this, SLOT( GroupBoxROIToggled(bool) ) );
  connect( ui.groupBox_Spacing, SIGNAL( toggled(bool) ), this, SLOT( GroupBoxSpacingToggled(bool) ) );
	connect( ui.pushButton_FreezeImage, SIGNAL( toggled(bool) ), this, SLOT( FreezeImage(bool) ) );
	connect( ui.pushButton_ApplyAndClose, SIGNAL( clicked() ), this, SLOT( ApplyAndCloseClicked() ) );
	connect( ui.pushButton_SaveAndClose, SIGNAL( clicked() ), this, SLOT( SaveAndCloseClicked() ) );
  connect( ui.spinBox_XMin, SIGNAL( valueChanged(int) ), this, SLOT( ROIXMinChanged(int) ) );
	connect( ui.spinBox_YMin, SIGNAL( valueChanged(int) ), this, SLOT( ROIYMinChanged(int) ) );
	connect( ui.spinBox_XMax, SIGNAL( valueChanged(int) ), this, SLOT( ROIXMaxChanged(int) ) );
	connect( ui.spinBox_YMax, SIGNAL( valueChanged(int) ), this, SLOT( ROIYMaxChanged(int) ) );
	connect( ui.spinBox_ReferenceWidth, SIGNAL( valueChanged(int) ), this, SLOT( ReferenceWidthChanged(int) ) );
	connect( ui.spinBox_ReferenceHeight, SIGNAL( valueChanged(int) ), this, SLOT( ReferenceHeightChanged(int) ) );
	connect( ui.doubleSpinBox_OpeningCircleRadius, SIGNAL( valueChanged(double) ), this, SLOT( OpeningCircleRadiusChanged(double) ) );
	connect( ui.doubleSpinBox_OpeningBarSize, SIGNAL( valueChanged(double) ), this, SLOT( OpeningBarSizeChanged(double) ) );
  connect( ui.doubleSpinBox_LinePairDistanceError, SIGNAL( valueChanged(double) ), this, SLOT( LinePairDistanceErrorChanged(double) ) );
	connect( ui.doubleSpinBox_AngleDifference, SIGNAL( valueChanged(double) ), this, SLOT( AngleDifferenceChanged(double) ) );
	connect( ui.doubleSpinBox_MinTheta, SIGNAL( valueChanged(double) ), this, SLOT( MinThetaChanged(double) ) );
	connect( ui.doubleSpinBox_MaxTheta, SIGNAL( valueChanged(double) ), this, SLOT( MaxThetaChanged(double) ) );
	connect( ui.doubleSpinBox_AngleTolerance, SIGNAL( valueChanged(double) ), this, SLOT( AngleToleranceChanged(double) ) );
	connect( ui.doubleSpinBox_CollinearPointsMaxDistanceFromLine, SIGNAL( valueChanged(double) ), this, SLOT( CollinearPointsMaxDistanceFromLineChanged(double) ) );
	connect( ui.doubleSpinBox_ImageThreshold, SIGNAL( valueChanged(double) ), this, SLOT( ImageThresholdChanged(double) ) );
	connect( ui.checkBox_OriginalIntensityForDots, SIGNAL( toggled(bool) ), this, SLOT( OriginalIntensityForDotsToggled(bool) ) );

	// Set up timer for refreshing UI
	m_CanvasRefreshTimer = new QTimer(this);
	connect(m_CanvasRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateCanvas()));

  // Initialize calibration controller (does the segmentation)
  m_PatternRecognition = new FidPatternRecognition();
  m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  // Fill form with configuration data
  if (ReadConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Fill form with configuration data failed!");
    return;
  }

  // Initialize visualization
  if (InitializeVisualization() != PLUS_SUCCESS) {
    LOG_ERROR("Initialize visualization failed!");
    return;
  }
}

//-----------------------------------------------------------------------------

SegmentationParameterDialog::~SegmentationParameterDialog()
{
  if (m_PatternRecognition != NULL) {
    delete m_PatternRecognition; 
    m_PatternRecognition = NULL; 
  }

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

  if (m_DataCollector == NULL || m_DataCollector->GetConnected() == false) {
    LOG_ERROR("Data collector is not initialized!");
    return PLUS_FAIL;
  }

  // Create canvas image actor
	m_CanvasImageActor = vtkImageActor::New();

	if (m_DataCollector->GetVideoEnabled()) {
		m_CanvasImageActor->VisibilityOn();
		//m_CanvasImageActor->SetInput(m_DataCollector->GetOutput()); // Commented because instead of the live image, snapshots are displayed with the overlay (segmented points)
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
  m_SegmentedPointsActor->VisibilityOn();

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
	m_CandidatesActor->GetProperty()->SetColor(0.8, 0.0, 0.0);
  m_CandidatesActor->GetProperty()->SetOpacity(0.7);
  m_CandidatesActor->VisibilityOn();

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

  // Indicate US orientation
  DrawUSOrientationIndicators();

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

  //Find segmentation parameters element
  vtkXMLDataElement* segmentationParameters = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("Segmentation");
	if (segmentationParameters == NULL) {
		LOG_ERROR("No Segmentation element is found in the XML tree!");
		return PLUS_FAIL;
	}

  // Feed parameters
	double approximateSpacingMmPerPixel(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ApproximateSpacingMmPerPixel", approximateSpacingMmPerPixel) )
	{
    m_ApproximateSpacingMmPerPixel = approximateSpacingMmPerPixel;
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

	double maxLinePairDistanceErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLinePairDistanceErrorPercent", maxLinePairDistanceErrorPercent) )
	{
		ui.doubleSpinBox_LinePairDistanceError->setValue(maxLinePairDistanceErrorPercent);
	} else {
    LOG_WARNING("Could not read MaxLinePairDistanceErrorPercent from configuration");
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

	double angleToleranceDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("AngleToleranceDegrees", angleToleranceDegrees) )
	{
		ui.doubleSpinBox_AngleTolerance->setValue(angleToleranceDegrees);
	} else {
    LOG_WARNING("Could not read AngleToleranceDegrees from configuration");
  }

	double thresholdImagePercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ThresholdImagePercent", thresholdImagePercent) )
	{
		ui.doubleSpinBox_ImageThreshold->setValue(thresholdImagePercent);
	} else {
    LOG_WARNING("Could not read ThresholdImagePercent from configuration");
  }

	double collinearPointsMaxDistanceFromLineMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("CollinearPointsMaxDistanceFromLineMm", collinearPointsMaxDistanceFromLineMm) )
	{
		ui.doubleSpinBox_CollinearPointsMaxDistanceFromLine->setValue(collinearPointsMaxDistanceFromLineMm);
	} else {
    LOG_WARNING("Could not read CollinearPointsMaxDistanceFromLineMm from configuration");
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

void SegmentationParameterDialog::ApplyAndCloseClicked()
{
  LOG_TRACE("SegmentationParameterDialog::ApplyAndCloseClicked");

  if (WriteConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Write configuration failed!");
    return;
  }

  accept();
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::SaveAndCloseClicked()
{
  LOG_TRACE("SegmentationParameterDialog::SaveAndCloseClicked");

  if (WriteConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Write configuration failed!");
    return;
  }

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;

  accept();
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::WriteConfiguration()
{
  LOG_TRACE("SegmentationParameterDialog::WriteConfiguration");

  //Find segmentation parameters element
  vtkXMLDataElement* segmentationParameters = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("Segmentation");
	if (segmentationParameters == NULL) {
		LOG_ERROR("No Segmentation element is found in the XML tree!");
		return PLUS_FAIL;
	}

  // Save parameters
  bool ok = true;
  if (ui.label_SpacingResult->text().indexOf("original") == -1) // If has been changed
  {
    segmentationParameters->SetDoubleAttribute("ApproximateSpacingMmPerPixel", ui.label_SpacingResult->text().toDouble(&ok));
    if (!ok)
    {
      LOG_ERROR("ApproximateSpacingMmPerPixel parameter cannot be saved!");
      return PLUS_FAIL;
    }
  }

  segmentationParameters->SetDoubleAttribute("MorphologicalOpeningCircleRadiusMm", ui.doubleSpinBox_OpeningCircleRadius->value());

  segmentationParameters->SetDoubleAttribute("MorphologicalOpeningBarSizeMm", ui.doubleSpinBox_OpeningBarSize->value());

  segmentationParameters->SetDoubleAttribute("MorphologicalOpeningBarSizeMm", ui.doubleSpinBox_OpeningBarSize->value());

	char ROIChars[256];
  sprintf_s(ROIChars, 64, "%d %d %d %d", ui.spinBox_XMin->value(), ui.spinBox_YMin->value(), ui.spinBox_XMax->value(), ui.spinBox_YMax->value());
	segmentationParameters->SetAttribute("RegionOfInterest", ROIChars);

  segmentationParameters->SetDoubleAttribute("MaxLinePairDistanceErrorPercent", ui.doubleSpinBox_LinePairDistanceError->value());

  segmentationParameters->SetDoubleAttribute("MaxAngleDifferenceDegrees", ui.doubleSpinBox_AngleDifference->value());

  segmentationParameters->SetDoubleAttribute("MinThetaDegrees", ui.doubleSpinBox_MinTheta->value());

  segmentationParameters->SetDoubleAttribute("MaxThetaDegrees", ui.doubleSpinBox_MaxTheta->value());

  segmentationParameters->SetDoubleAttribute("AngleToleranceDegrees", ui.doubleSpinBox_AngleTolerance->value());

  segmentationParameters->SetDoubleAttribute("ThresholdImagePercent", ui.doubleSpinBox_ImageThreshold->value());

  segmentationParameters->SetDoubleAttribute("CollinearPointsMaxDistanceFromLineMm", ui.doubleSpinBox_CollinearPointsMaxDistanceFromLine->value());

  segmentationParameters->SetIntAttribute("UseOriginalImageIntensityForDotIntensityScore", (ui.checkBox_OriginalIntensityForDots->isChecked() ? 1 : 0) );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::GroupBoxROIToggled(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::GroupBoxROIToggled(" << (aOn?"true":"false") << ")");

  ui.groupBox_Spacing->blockSignals(true);
  ui.groupBox_Spacing->setChecked(!aOn);
  ui.groupBox_Spacing->blockSignals(false);

  if (aOn)
  {
    if (SwitchToROIMode() != PLUS_SUCCESS)
    {
      LOG_ERROR("Switch to ROI mode failed!");
      return;
    }
  }
  else
  {
    if (SwitchToSpacingMode() != PLUS_SUCCESS)
    {
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

  if (aOn)
  {
    if (SwitchToSpacingMode() != PLUS_SUCCESS)
    {
      LOG_ERROR("Switch to ROI mode failed!");
      return;
    }
  }
  else
  {
    if (SwitchToROIMode() != PLUS_SUCCESS)
    {
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

  if (m_CanvasRenderer == NULL)
  {
    return PLUS_FAIL;
  }

	// Calculate image center
	double imageCenterX = 0;
	double imageCenterY = 0;
	int dimensions[2];
	m_DataCollector->GetFrameSize(dimensions);
	imageCenterX = dimensions[0] / 2.0;
	imageCenterY = dimensions[1] / 2.0;

	// Set up camera
	vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
	imageCamera->SetFocalPoint(imageCenterX, imageCenterY, 0);
	imageCamera->SetViewUp(0, -1, 0);
	imageCamera->SetClippingRange(0.1, 2000.0);
	imageCamera->ParallelProjectionOn();

	// Calculate distance of camera from the plane
  QSize size = ui.canvas->size();
  if ((double)size.width() / (double)size.height() > imageCenterX / imageCenterY) 
  {
		// If canvas aspect ratio is more elongenated in the X position then compute the distance according to the Y axis
		imageCamera->SetParallelScale(imageCenterY);
	}
  else
  {
		imageCamera->SetParallelScale(imageCenterX * (double)size.height() / (double)size.width());
	}

	imageCamera->SetPosition(imageCenterX, imageCenterY, -200.0);
  m_CanvasRenderer->SetActiveCamera(imageCamera);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::UpdateCanvas()
{
	//LOG_TRACE("SegmentationParameterDialog::UpdateCanvas");

  SegmentCurrentImage();

  ui.canvas->update();
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::SegmentCurrentImage()
{
  LOG_TRACE("SegmentationParameterDialog::SegmentCurrentImage");

  // If image is not frozen, then have DataCollector get the latest frame (else it uses the frozen one for segmentation)
  if (!m_ImageFrozen)
  {
    m_DataCollector->Modified();
    m_DataCollector->Update();
  }

  // Get and convert currently displayed image // TODO Get TrackedFrame directly from vtkDataCollector
  vtkSmartPointer<vtkImageData> currentImage = vtkSmartPointer<vtkImageData>::New();
  currentImage->DeepCopy(m_DataCollector->GetOutput());

  PlusVideoFrame videoFrame;
  videoFrame.DeepCopyFrom(currentImage);

  TrackedFrame* trackedFrame = new TrackedFrame();
  trackedFrame->SetImageData(videoFrame);

  // Set image for canvas
  m_CanvasImageActor->SetInput(currentImage);

  // Segment image
  PatternRecognitionResult segResults;
  m_PatternRecognition->RecognizePattern(trackedFrame, segResults);

  LOG_DEBUG("Candidate count: " << segResults.GetCandidateFidValues().size());
  if (segResults.GetFoundDotsCoordinateValue().size() > 0)
  {
    LOG_DEBUG("Segmented point count: " << segResults.GetFoundDotsCoordinateValue().size());
  }
  else
  {
    LOG_DEBUG("Segmentation failed");
  }

  // Display candidate points
	vtkSmartPointer<vtkPoints> candidatePoints = vtkSmartPointer<vtkPoints>::New();
  candidatePoints->SetNumberOfPoints(segResults.GetCandidateFidValues().size());

  std::vector<Dot> candidateDots = segResults.GetCandidateFidValues();
	for (int i=0; i<candidateDots.size(); ++i)
  {
    candidatePoints->InsertPoint(i, candidateDots[i].GetX(), candidateDots[i].GetY(), -0.3);
	}
  candidatePoints->Modified();

	m_CandidatesPolyData->Initialize();
	m_CandidatesPolyData->SetPoints(candidatePoints);

  // Display segmented points (result in tracked frame is not usable in themselves because we need to transform the points)
	vtkSmartPointer<vtkPoints> segmentedPoints = vtkSmartPointer<vtkPoints>::New();
  segmentedPoints->SetNumberOfPoints(segResults.GetFoundDotsCoordinateValue().size());

	std::vector<std::vector<double>> segmentedDots = segResults.GetFoundDotsCoordinateValue();
	for (int i=0; i<segmentedDots.size(); ++i)
  {
		segmentedPoints->InsertPoint(i, segmentedDots[i][0], segmentedDots[i][1], -0.3);
	}
	segmentedPoints->Modified();

	m_SegmentedPointsPolyData->Initialize();
	m_SegmentedPointsPolyData->SetPoints(segmentedPoints);

  delete trackedFrame;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::FreezeImage(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::FreezeImage(" << (aOn?"true":"false") << ")");

  m_ImageFrozen = aOn;
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

  if (m_SpacingModeHandler == NULL) {
    m_SpacingModeHandler = vtkSpacingModeHandler::New(); 
    m_SpacingModeHandler->SetParentDialog(this);
  }

  if (m_ROIModeHandler != NULL) {
    m_ROIModeHandler->SetEnabled(false);
  }

  m_SpacingModeHandler->SetEnabled(true);

  ui.canvas->GetRenderWindow()->GetInteractor()->RemoveAllObservers();
  ui.canvas->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, m_SpacingModeHandler);
  ui.canvas->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::LeftButtonReleaseEvent, m_SpacingModeHandler);
  ui.canvas->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, m_SpacingModeHandler);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::ComputeSpacingFromMeasuredLengthSum()
{
  LOG_TRACE("SegmentationParameterDialog::ComputeSpacingFromMeasuredLengthSum");

  double spacing = (ui.spinBox_ReferenceWidth->text().toDouble() + ui.spinBox_ReferenceHeight->text().toDouble()) / m_SpacingModeHandler->GetLineLengthSumImagePixel();
  ui.label_SpacingResult->setText(QString("%1").arg(spacing));

  m_PatternRecognition->GetFidSegmentation()->SetApproximateSpacingMmPerPixel(spacing);
  m_PatternRecognition->GetFidLineFinder()->SetApproximateSpacingMmPerPixel(spacing);
  m_PatternRecognition->GetFidLabeling()->SetApproximateSpacingMmPerPixel(spacing);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

double SegmentationParameterDialog::GetSpacingReferenceWidth()
{
  LOG_TRACE("SegmentationParameterDialog::GetSpacingReferenceWidth");

  return ui.spinBox_ReferenceWidth->text().toDouble();
}

//-----------------------------------------------------------------------------

double SegmentationParameterDialog::GetSpacingReferenceHeight()
{
  LOG_TRACE("SegmentationParameterDialog::GetSpacingReferenceHeight");

  return ui.spinBox_ReferenceHeight->text().toDouble();
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::DrawUSOrientationIndicators()
{
  LOG_TRACE("SegmentationParameterDialog::DrawUSOrientationIndicators");

  // Since the internal orientation is always MF, display the indicators for MF in all cases
	vtkSmartPointer<vtkTextActor3D> horizontalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
	horizontalOrientationTextActor->GetTextProperty()->SetColor(0.0, 1.0, 0.0);
	horizontalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  horizontalOrientationTextActor->GetTextProperty()->SetFontSize(16);
	horizontalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
	horizontalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
	horizontalOrientationTextActor->GetTextProperty()->BoldOn(); 
  horizontalOrientationTextActor->SetInput("M");
  horizontalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0); 
  horizontalOrientationTextActor->SetPosition(35.0, 22.0, -0.5);
  m_CanvasRenderer->AddActor(horizontalOrientationTextActor);

	vtkSmartPointer<vtkTextActor3D> verticalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
	verticalOrientationTextActor->GetTextProperty()->SetColor(0.0, 1.0, 0.0);
	verticalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  verticalOrientationTextActor->GetTextProperty()->SetFontSize(16);
	verticalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
	verticalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
	verticalOrientationTextActor->GetTextProperty()->BoldOn(); 
  verticalOrientationTextActor->SetInput("F");
  verticalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0); 
  verticalOrientationTextActor->SetPosition(9.0, 45.0, -0.5);
  m_CanvasRenderer->AddActor(verticalOrientationTextActor);

  vtkSmartPointer<vtkActor> horizontalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkLineSource> horizontalLineSource = vtkSmartPointer<vtkLineSource>::New();
  horizontalLineSource->SetPoint1(5.0, 5.0, -0.5);
  horizontalLineSource->SetPoint2(50.0, 5.0, -0.5);
  horizontalLineMapper->SetInputConnection(horizontalLineSource->GetOutputPort());
  horizontalLineActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  horizontalLineActor->SetMapper(horizontalLineMapper);
  m_CanvasRenderer->AddActor(horizontalLineActor);

  vtkSmartPointer<vtkActor> verticalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkLineSource> verticalLineSource = vtkSmartPointer<vtkLineSource>::New();
  verticalLineSource->SetPoint1(5.0, 5.0, -0.5);
  verticalLineSource->SetPoint2(5.0, 50.0, -0.5);
  verticalLineMapper->SetInputConnection(verticalLineSource->GetOutputPort());
  verticalLineActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  verticalLineActor->SetMapper(verticalLineMapper);
  m_CanvasRenderer->AddActor(verticalLineActor);

  vtkSmartPointer<vtkActor> horizontalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkConeSource> horizontalConeSource = vtkSmartPointer<vtkConeSource>::New();
  horizontalConeSource->SetHeight(15.0);
  horizontalConeSource->SetRadius(5.0);
  horizontalConeMapper->SetInputConnection(horizontalConeSource->GetOutputPort());
  horizontalConeActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  horizontalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  horizontalConeActor->SetPosition(56.0, 5.0, -0.5);
  horizontalConeActor->SetMapper(horizontalConeMapper);
  m_CanvasRenderer->AddActor(horizontalConeActor);

  vtkSmartPointer<vtkActor> verticalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkConeSource> verticalConeSource = vtkSmartPointer<vtkConeSource>::New();
  verticalConeSource->SetHeight(15.0);
  verticalConeSource->SetRadius(5.0);
  verticalConeMapper->SetInputConnection(verticalConeSource->GetOutputPort());
  verticalConeActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  verticalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  verticalConeActor->RotateWXYZ(90.0, 0.0, 0.0, 1.0); 
  verticalConeActor->SetPosition(5.0, 56.0, -0.5);
  verticalConeActor->SetMapper(verticalConeMapper);
  m_CanvasRenderer->AddActor(verticalConeActor);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::GetFrameSize(int aImageDimensions[2])
{
  LOG_TRACE("SegmentationParameterDialog::GetFrameSize");

  m_DataCollector->GetFrameSize(aImageDimensions);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::SetROI(int aXMin, int aYMin, int aXMax, int aYMax)
{
  LOG_TRACE("SegmentationParameterDialog::SetROI(" << aXMin << ", " << aYMin << ", " << aXMax << ", " << aYMax << ")");

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

  m_PatternRecognition->GetFidSegmentation()->SetRegionOfInterest(aXMin, aYMin, aXMax, aYMax);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialog::GetROI(int &aXMin, int &aYMin, int &aXMax, int &aYMax)
{
  LOG_TRACE("SegmentationParameterDialog::GetROI");

  aXMin = ui.spinBox_XMin->value();
  aYMin = ui.spinBox_YMin->value();
  aXMax = ui.spinBox_XMax->value();
  aYMax = ui.spinBox_YMax->value();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIXMinChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIXMinChanged(" << aValue << ")");

  if (aValue > m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx()) {
    if (m_ROIModeHandler != NULL) {
      if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
        LOG_ERROR("Draw ROI failed!");
      }
    }

    if (m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[0] > 0) {
      m_PatternRecognition->GetFidSegmentation()->SetRegionOfInterest(aValue, -1, -1, -1);
    }
  } else {
    ui.spinBox_XMin->blockSignals(true);
    ui.spinBox_XMin->setValue(m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx());
    ui.spinBox_XMin->blockSignals(false);
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIYMinChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIYMinChanged(" << aValue << ")");

  if (aValue > m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx()) {
    if (m_ROIModeHandler != NULL) {
      if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
        LOG_ERROR("Draw ROI failed!");
      }
    }

    if (m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[1] > 0) {
      m_PatternRecognition->GetFidSegmentation()->SetRegionOfInterest(-1, aValue, -1, -1);
    }
  } else {
    ui.spinBox_YMin->blockSignals(true);
    ui.spinBox_YMin->setValue(m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx());
    ui.spinBox_YMin->blockSignals(false);
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIXMaxChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIXMaxChanged(" << aValue << ")");

  if (m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[0] <= 0) {
    return;
  }

  if (aValue < m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[0] - m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx()) {
    if (m_ROIModeHandler != NULL) {
      if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
        LOG_ERROR("Draw ROI failed!");
      }
    }

    m_PatternRecognition->GetFidSegmentation()->SetRegionOfInterest(-1, -1, aValue, -1);
  } else {
    ui.spinBox_XMax->blockSignals(true);
    ui.spinBox_XMax->setValue(m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[0] - m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx() - 1);
    ui.spinBox_XMax->blockSignals(false);
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ROIYMaxChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ROIYMaxChanged(" << aValue << ")");

  if (m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[1] <= 0) {
    return;
  }

  if (aValue < m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[1] - m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx()) {
    if (m_ROIModeHandler != NULL) {
      if (m_ROIModeHandler->DrawROI() != PLUS_SUCCESS) {
        LOG_ERROR("Draw ROI failed!");
      }
    }

    m_PatternRecognition->GetFidSegmentation()->SetRegionOfInterest(-1, -1, -1, aValue);
  } else {
    ui.spinBox_YMax->blockSignals(true);
    ui.spinBox_YMax->setValue(m_PatternRecognition->GetFidSegmentation()->GetFrameSize()[1] - m_PatternRecognition->GetFidSegmentation()->GetMorphologicalOpeningBarSizePx() - 1);
    ui.spinBox_YMax->blockSignals(false);
  }
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ReferenceWidthChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ReferenceWidthChanged(" << aValue << ")");

  ComputeSpacingFromMeasuredLengthSum();
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ReferenceHeightChanged(int aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ReferenceHeightChanged(" << aValue << ")");

  ComputeSpacingFromMeasuredLengthSum();
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::OpeningCircleRadiusChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::OpeningCircleRadiusChanged(" << aValue << ")");

  m_PatternRecognition->GetFidSegmentation()->SetMorphologicalOpeningCircleRadiusMm(aValue);
  m_PatternRecognition->GetFidSegmentation()->UpdateParameters();
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::OpeningBarSizeChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::OpeningBarSizeChanged(" << aValue << ")");

  m_PatternRecognition->GetFidSegmentation()->SetMorphologicalOpeningBarSizeMm(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::LinePairDistanceErrorChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::LinePairDistanceErrorChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLabeling()->SetMaxLinePairDistanceErrorPercent(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::AngleDifferenceChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::AngleDifferenceChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLabeling()->SetMaxAngleDifferenceDegrees(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::MinThetaChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::MinThetaChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLineFinder()->SetMinThetaDegrees(aValue);
  m_PatternRecognition->GetFidLabeling()->SetMinThetaDegrees(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::MaxThetaChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::MaxThetaChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLineFinder()->SetMaxThetaDegrees(aValue);
  m_PatternRecognition->GetFidLabeling()->SetMaxThetaDegrees(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::AngleToleranceChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::AngleToleranceChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLabeling()->SetAngleToleranceDegrees(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::CollinearPointsMaxDistanceFromLineChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::CollinearPointsMaxDistanceFromLineChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLineFinder()->SetCollinearPointsMaxDistanceFromLineMm(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ImageThresholdChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::ImageThresholdChanged(" << aValue << ")");

  m_PatternRecognition->GetFidSegmentation()->SetThresholdImagePercent(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::OriginalIntensityForDotsToggled(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::OriginalIntensityForDotsToggled(" << (aOn?"true":"false") << ")");

  m_PatternRecognition->GetFidSegmentation()->SetUseOriginalImageIntensityForDotIntensityScore(aOn);
}

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "ConfigFileSaverDialog.h"
#include "FidPatternRecognition.h"
#include "FidPatternRecognitionCommon.h"
#include "PlusVideoFrame.h"
#include "SegmentationParameterDialog.h"
#include "TrackedFrame.h"
#include "vtkActor.h"
#include "vtkActor.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkConeSource.h"
#include "vtkDataCollector.h"
#include "vtkGlyph3D.h"
#include "vtkImageActor.h"
#include "vtkImageData.h" 
#include "vtkImageVisualizer.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPlusVideoSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPropPicker.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkSphereSource.h"
#include "vtkSphereSource.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"
#include <QMessageBox>
#include <QTimer>

static const int HANDLE_SIZE = 8;

void CreateNewHandleActor(vtkActor* &actor, vtkSphereSource* &source, double r, double g, double b)
{
  actor = vtkActor::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  source = vtkSphereSource::New();
  source->SetRadius(HANDLE_SIZE);
  mapper->SetInputConnection(source->GetOutputPort());
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(r, g, b);
}

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
      prop->SetOpacity(0.5);
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
  vtkSegmentationParameterDialogModeHandlerBase()
  {
    m_ParentDialog = NULL;
    m_ActorCollection = NULL;
  }

  //----------------------------------------------------------------------
  /*!
  * \brief Destructor
  */
  virtual ~vtkSegmentationParameterDialogModeHandlerBase()
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
            m_ClickOffsetFromCenterOfSource.first = m_TopLeftHandleSource->GetCenter()[0] - xWorld;
            m_ClickOffsetFromCenterOfSource.second = m_TopLeftHandleSource->GetCenter()[1] - yWorld;
          } else if (picker->GetActor() == m_BottomRightHandleActor) {
            m_BottomRightHandlePicked = true;
            m_ClickOffsetFromCenterOfSource.first = m_BottomRightHandleActor->GetCenter()[0] - xWorld;
            m_ClickOffsetFromCenterOfSource.second = m_BottomRightHandleActor->GetCenter()[1] - yWorld;
          }
          else
          {
            m_ClickOffsetFromCenterOfSource.first = 0.0;
            m_ClickOffsetFromCenterOfSource.second = 0.0;
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
          newXMin = (int)(xWorld + 0.5 + m_ClickOffsetFromCenterOfSource.first);
          newYMin = (int)(yWorld + 0.5 + m_ClickOffsetFromCenterOfSource.second);
        } else if (m_BottomRightHandlePicked) {
          newXMax = (int)(xWorld + 0.5 + m_ClickOffsetFromCenterOfSource.first);
          newYMax = (int)(yWorld + 0.5 + m_ClickOffsetFromCenterOfSource.second);
        }

        m_ParentDialog->SetROI(newXMin, newYMin, newXMax, newYMax);
      }
      else if (vtkCommand::LeftButtonReleaseEvent == eventId)
      {
        LOG_DEBUG("Release - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        m_TopLeftHandlePicked = false;
        m_BottomRightHandlePicked = false;

        m_ClickOffsetFromCenterOfSource.first = 0.0;
        m_ClickOffsetFromCenterOfSource.second = 0.0;
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

    // Set handle positions
    m_TopLeftHandleSource->SetCenter(xMin, yMin, -0.5);
    m_BottomRightHandleSource->SetCenter(xMax, yMax, -0.5);

    return PLUS_SUCCESS;
  }

private:
  //----------------------------------------------------------------------
  /*!
  * \brief Constructor
  */
  vtkROIModeHandler()
    : vtkSegmentationParameterDialogModeHandlerBase()
  {
    m_TopLeftHandleActor = NULL;
    m_BottomRightHandleActor = NULL;
    m_TopLeftHandleSource = NULL;
    m_BottomRightHandleSource = NULL;
    m_TopLeftHandlePicked = false;
    m_BottomRightHandlePicked = false;
  }

  //----------------------------------------------------------------------
  /*!
  * \brief Destructor
  */
  virtual ~vtkROIModeHandler()
  {
    if (m_TopLeftHandleActor != NULL) {
      m_TopLeftHandleActor->Delete();
      m_TopLeftHandleActor = NULL;
    }

    if (m_BottomRightHandleActor != NULL) {
      m_BottomRightHandleActor->Delete();
      m_BottomRightHandleActor = NULL;
    }

    if (m_TopLeftHandleSource != NULL) {
      m_TopLeftHandleSource->Delete();
      m_TopLeftHandleSource = NULL;
    }

    if (m_BottomRightHandleSource != NULL) {
      m_BottomRightHandleSource->Delete();
      m_BottomRightHandleSource = NULL;
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

    CreateNewHandleActor(m_TopLeftHandleActor, m_TopLeftHandleSource, vtkImageVisualizer::ROI_COLOUR[0],  vtkImageVisualizer::ROI_COLOUR[1], vtkImageVisualizer:: ROI_COLOUR[2]);
    m_ActorCollection->AddItem(m_TopLeftHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_TopLeftHandleActor);

    CreateNewHandleActor(m_BottomRightHandleActor, m_BottomRightHandleSource, vtkImageVisualizer::ROI_COLOUR[0],  vtkImageVisualizer::ROI_COLOUR[1],  vtkImageVisualizer::ROI_COLOUR[2]);
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

  //! Sphere source of the top left corner handle (direct access needed to move it around)
  vtkSphereSource*              m_TopLeftHandleSource;

  //! Sphere source of the bottom right corner handle (direct access needed to move it around)
  vtkSphereSource*              m_BottomRightHandleSource;

  //! Flag indicating if top left corner handle is picked
  bool                          m_TopLeftHandlePicked;

  //! Flag indicating if bottom right corner handle is picked
  bool                          m_BottomRightHandlePicked;

  /*! Pair to store the offset of the click from the center of the desired source */
  std::pair<double, double>     m_ClickOffsetFromCenterOfSource;
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
            m_ClickOffsetFromCenterOfSource.first = m_HorizontalLeftHandleSource->GetCenter()[0] - xWorld;
            m_ClickOffsetFromCenterOfSource.second = m_HorizontalLeftHandleSource->GetCenter()[1] - yWorld;
          } 
          else if (picker->GetActor() == m_HorizontalRightHandleActor) {
            m_HorizontalRightHandlePicked = true;
            m_ClickOffsetFromCenterOfSource.first = m_HorizontalRightHandleSource->GetCenter()[0] - xWorld;
            m_ClickOffsetFromCenterOfSource.second = m_HorizontalRightHandleSource->GetCenter()[1] - yWorld;
          } 
          else if (picker->GetActor() == m_VerticalTopHandleActor) {
            m_VerticalTopHandlePicked = true;
            m_ClickOffsetFromCenterOfSource.first = m_VerticalTopHandleSource->GetCenter()[0] - xWorld;
            m_ClickOffsetFromCenterOfSource.second = m_VerticalTopHandleSource->GetCenter()[1] - yWorld;
          } 
          else if (picker->GetActor() == m_VerticalBottomHandleActor) {
            m_VerticalBottomHandlePicked = true;
            m_ClickOffsetFromCenterOfSource.first = m_VerticalBottomHandleSource->GetCenter()[0] - xWorld;
            m_ClickOffsetFromCenterOfSource.second = m_VerticalBottomHandleSource->GetCenter()[1] - yWorld;
          }
          else
          {
            m_ClickOffsetFromCenterOfSource.first = 0.0;
            m_ClickOffsetFromCenterOfSource.second = 0.0;
          }
        }
      }
      else if ((vtkCommand::MouseMoveEvent == eventId) && (m_HorizontalLeftHandlePicked || m_HorizontalRightHandlePicked || m_VerticalTopHandlePicked || m_VerticalBottomHandlePicked))
      {
        LOG_DEBUG("Move - pixel: (" << x << ", " << y << "), world: (" << xWorld << ", " << yWorld << ")");

        // Get the positions of all handles
        double horizontalLeftPosition[3];
        m_HorizontalLeftHandleSource->GetCenter(horizontalLeftPosition);
        double horizontalRightPosition[3];
        m_HorizontalRightHandleSource->GetCenter(horizontalRightPosition);
        double verticalTopPosition[3];
        m_VerticalTopHandleSource->GetCenter(verticalTopPosition);
        double verticalBottomPosition[3];
        m_VerticalBottomHandleSource->GetCenter(verticalBottomPosition);

        double xWorldWithOffset = xWorld + m_ClickOffsetFromCenterOfSource.first;
        double yWorldWithOffset = yWorld + m_ClickOffsetFromCenterOfSource.second;

        // Change position of the picked handle
        if (m_HorizontalLeftHandlePicked) {
          if (xWorld < horizontalRightPosition[0] - 10.0) {
            m_HorizontalLeftHandleSource->SetCenter(xWorldWithOffset, yWorldWithOffset, -0.5);
            m_HorizontalLineSource->SetPoint1(xWorldWithOffset, yWorldWithOffset, -0.5);
          }
          m_HorizontalLeftHandleSource->GetCenter(horizontalLeftPosition);
        } 
        else if (m_HorizontalRightHandlePicked) {
          if (xWorld > horizontalLeftPosition[0] + 10.0) {
            m_HorizontalRightHandleSource->SetCenter(xWorldWithOffset, yWorldWithOffset, -0.5);
            m_HorizontalLineSource->SetPoint2(xWorldWithOffset, yWorldWithOffset, -0.5);
          }
          m_HorizontalRightHandleSource->GetCenter(horizontalRightPosition);
        } 
        else if (m_VerticalTopHandlePicked) {
          if (yWorld < verticalBottomPosition[1] - 10.0) {
            m_VerticalTopHandleSource->SetCenter(xWorldWithOffset, yWorldWithOffset, -0.5);
            m_VerticalLineSource->SetPoint1(xWorldWithOffset, yWorldWithOffset, -0.5);
          }
          m_VerticalTopHandleSource->GetCenter(verticalTopPosition);
        } 
        else if (m_VerticalBottomHandlePicked) {
          if (yWorld > verticalTopPosition[1] + 10.0) {
            m_VerticalBottomHandleSource->SetCenter(xWorldWithOffset, yWorldWithOffset, -0.5);
            m_VerticalLineSource->SetPoint2(xWorldWithOffset, yWorldWithOffset, -0.5);
          }
          m_VerticalBottomHandleSource->GetCenter(verticalBottomPosition);
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

        m_ClickOffsetFromCenterOfSource.first = 0.0;
        m_ClickOffsetFromCenterOfSource.second = 0.0;
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
  vtkSpacingModeHandler()
    : vtkSegmentationParameterDialogModeHandlerBase()
  {
    m_HorizontalLeftHandleActor = NULL;
    m_HorizontalRightHandleActor = NULL;
    m_VerticalTopHandleActor = NULL;
    m_VerticalBottomHandleActor = NULL;
    m_HorizontalLineActor = NULL;
    m_VerticalLineActor = NULL;
    m_HorizontalLeftHandleSource = NULL;
    m_HorizontalRightHandleSource = NULL;
    m_VerticalTopHandleSource = NULL;
    m_VerticalBottomHandleSource = NULL;
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
  virtual ~vtkSpacingModeHandler()
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

    if (m_HorizontalLeftHandleSource != NULL) {
      m_HorizontalLeftHandleSource->Delete();
      m_HorizontalLeftHandleSource = NULL;
    }

    if (m_HorizontalRightHandleSource != NULL) {
      m_HorizontalRightHandleSource->Delete();
      m_HorizontalRightHandleSource = NULL;
    }

    if (m_VerticalTopHandleSource != NULL) {
      m_VerticalTopHandleSource->Delete();
      m_VerticalTopHandleSource = NULL;
    }

    if (m_VerticalBottomHandleSource != NULL) {
      m_VerticalBottomHandleSource->Delete();
      m_VerticalBottomHandleSource = NULL;
    }

    if (m_VerticalLineSource != NULL) {
      m_VerticalLineSource->Delete();
      m_VerticalLineSource = NULL;
    }

    if (m_VerticalBottomHandleSource != NULL) {
      m_VerticalBottomHandleSource->Delete();
      m_VerticalBottomHandleSource = NULL;
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

    CreateNewHandleActor(m_HorizontalLeftHandleActor, m_HorizontalLeftHandleSource, 0.0, 0.8, 0.0);
    m_ActorCollection->AddItem(m_HorizontalLeftHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_HorizontalLeftHandleActor);

    CreateNewHandleActor(m_HorizontalRightHandleActor, m_HorizontalRightHandleSource, 0.0, 0.8, 0.0);
    m_ActorCollection->AddItem(m_HorizontalRightHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_HorizontalRightHandleActor);

    CreateNewHandleActor(m_VerticalTopHandleActor, m_VerticalTopHandleSource, 0.0, 0.0, 1.0);
    m_ActorCollection->AddItem(m_VerticalTopHandleActor);
    m_ParentDialog->GetCanvasRenderer()->AddActor(m_VerticalTopHandleActor);

    CreateNewHandleActor(m_VerticalBottomHandleActor, m_VerticalBottomHandleSource, 0.0, 0.0, 1.0);
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

    m_HorizontalLeftHandleSource->SetCenter(horizontalLeftX, horizontalLeftY, -0.5);
    m_HorizontalRightHandleSource->SetCenter(horizontalRightX, horizontalRightY, -0.5);
    m_VerticalTopHandleSource->SetCenter(verticalTopX, verticalTopY, -0.5);
    m_VerticalBottomHandleSource->SetCenter(verticalBottomX, verticalBottomY, -0.5);
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
  vtkSphereSource*                m_HorizontalLeftHandleSource;

  //! Cube source of the right handle of the horizontal line (direct access needed to move it around)
  vtkSphereSource*                m_HorizontalRightHandleSource;

  //! Cube source of the left handle of the vertical line (direct access needed to move it around)
  vtkSphereSource*                m_VerticalTopHandleSource;

  //! Cube source of the right handle of the vertical line (direct access needed to move it around)
  vtkSphereSource*                m_VerticalBottomHandleSource;

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

  /*! Pair to store the offset of the click from the center of the desired source */
  std::pair<double, double>     m_ClickOffsetFromCenterOfSource;
};

//-----------------------------------------------------------------------------

SegmentationParameterDialog::SegmentationParameterDialog(QWidget* aParent, vtkDataCollector* aDataCollector)
: QDialog(aParent)
, m_DataCollector(aDataCollector)
, m_SegmentedPointsActor(NULL)
, m_SegmentedPointsPolyData(NULL)
, m_CandidatesPolyData(NULL)
, m_ImageVisualizer(NULL)
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
  connect( ui.pushButton_Export, SIGNAL( clicked() ), this, SLOT( ExportImage() ) );
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
  connect( ui.doubleSpinBox_MaxLineShiftMm, SIGNAL( valueChanged(double) ), this, SLOT( MaxLineShiftMmChanged(double) ) );
  connect( ui.checkBox_OriginalIntensityForDots, SIGNAL( toggled(bool) ), this, SLOT( OriginalIntensityForDotsToggled(bool) ) );
  connect( ui.doubleSpinBox_MaxCandidates, SIGNAL( valueChanged(double) ), this, SLOT( MaxCandidatesChanged(double) ) );

  // Set up timer for refreshing UI
  m_CanvasRefreshTimer = new QTimer(this);
  connect(m_CanvasRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateCanvas()));

  // Initialize calibration controller (does the segmentation)
  m_PatternRecognition = new FidPatternRecognition();
  m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  ui.doubleSpinBox_MaxLineShiftMm->setValue(m_PatternRecognition->GetFidLabeling()->GetMaxLineShiftMm());

  // Initialize visualization
  if (InitializeVisualization() != PLUS_SUCCESS) {
    LOG_ERROR("Initialize visualization failed!");
    return;
  }

  // Fill form with configuration data
  if (ReadConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Fill form with configuration data failed!");
    return;
  }

  // Force a single redraw of the ROI to correct the position of the ROI handles
  if( this->m_ROIModeHandler != NULL )
  {
    this->m_ROIModeHandler->DrawROI();
  }
}

//-----------------------------------------------------------------------------

SegmentationParameterDialog::~SegmentationParameterDialog()
{
  if (m_PatternRecognition != NULL) {
    delete m_PatternRecognition; 
    m_PatternRecognition = NULL; 
  }

  if (m_SegmentedPointsActor != NULL) {
    m_SegmentedPointsActor->Delete();
    m_SegmentedPointsActor = NULL;
  }

  if (m_SegmentedPointsPolyData != NULL) {
    m_SegmentedPointsPolyData->Delete();
    m_SegmentedPointsPolyData = NULL;
  }

  if (m_CandidatesPolyData != NULL) {
    m_CandidatesPolyData->Delete();
    m_CandidatesPolyData = NULL;
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

  if( m_ImageVisualizer != NULL ) {
    m_ImageVisualizer->Delete();
    m_ImageVisualizer = NULL;
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

  if (!m_DataCollector->GetVideoEnabled()) 
  {
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

  // Re-use the results actor in ImageVisualizer, no need to duplicate!
  m_CandidatesPolyData = vtkPolyData::New();
  m_CandidatesPolyData->Initialize();

  // Setup canvas
  m_ImageVisualizer = vtkImageVisualizer::New();
  if( m_ImageVisualizer->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to initialize the image visualizer.");
  }
  m_ImageVisualizer->AssignResultPolyData(m_CandidatesPolyData);
  m_ImageVisualizer->AssignDataCollector(m_DataCollector);
  m_CanvasRenderer = m_ImageVisualizer->GetCanvasRenderer();
  m_ImageVisualizer->GetCanvasRenderer()->SetBackground(0.1, 0.1, 0.1);
  m_ImageVisualizer->SetResultColor(0.8, 0.0, 0.0);
  m_ImageVisualizer->SetResultOpacity(0.8);
  ui.canvas->GetRenderWindow()->AddRenderer(m_ImageVisualizer->GetCanvasRenderer());

  // Create default picker
  m_ImageVisualizer->GetCanvasRenderer()->GetRenderWindow()->GetInteractor()->CreateDefaultPicker();

  // Add segmented points to renderer
  m_ImageVisualizer->GetCanvasRenderer()->AddActor(m_SegmentedPointsActor);

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

  double maxCandidates(20.0); 
  if ( segmentationParameters->GetScalarAttribute("MaxCandidates", maxCandidates) )
  {
    ui.doubleSpinBox_MaxCandidates->setValue(maxCandidates);
  } else {
    LOG_WARNING("Could not read MaxCandidates from configuration");
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

  char ROIChars[64];
  SNPRINTF(ROIChars, 64, "%d %d %d %d", ui.spinBox_XMin->value(), ui.spinBox_YMin->value(), ui.spinBox_XMax->value(), ui.spinBox_YMax->value());
  segmentationParameters->SetAttribute("RegionOfInterest", ROIChars);

  segmentationParameters->SetDoubleAttribute("MaxLinePairDistanceErrorPercent", ui.doubleSpinBox_LinePairDistanceError->value());

  segmentationParameters->SetDoubleAttribute("MaxAngleDifferenceDegrees", ui.doubleSpinBox_AngleDifference->value());

  segmentationParameters->SetDoubleAttribute("MinThetaDegrees", ui.doubleSpinBox_MinTheta->value());

  segmentationParameters->SetDoubleAttribute("MaxThetaDegrees", ui.doubleSpinBox_MaxTheta->value());

  segmentationParameters->SetDoubleAttribute("MaxLineShiftMm", ui.doubleSpinBox_MaxLineShiftMm->value());

  segmentationParameters->SetDoubleAttribute("AngleToleranceDegrees", ui.doubleSpinBox_AngleTolerance->value());

  segmentationParameters->SetDoubleAttribute("ThresholdImagePercent", ui.doubleSpinBox_ImageThreshold->value());

  segmentationParameters->SetDoubleAttribute("CollinearPointsMaxDistanceFromLineMm", ui.doubleSpinBox_CollinearPointsMaxDistanceFromLine->value());

  segmentationParameters->SetIntAttribute("UseOriginalImageIntensityForDotIntensityScore", (ui.checkBox_OriginalIntensityForDots->isChecked() ? 1 : 0) );

  segmentationParameters->SetIntAttribute("MaxCandidates", ui.doubleSpinBox_MaxCandidates->value());

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

  if( m_ImageVisualizer != NULL )
  {
    m_ImageVisualizer->UpdateCameraPose();
  }
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
    m_DataCollector->GetTrackedFrame(&m_Frame);
  }

  // Set image for canvas
  m_ImageVisualizer->SetInput(m_Frame.GetImageData()->GetVtkImage());

  // Segment image
  PatternRecognitionResult segResults;
  FidPatternRecognition::PatternRecognitionError error;
  if( m_PatternRecognition->RecognizePattern(&m_Frame, segResults, error) != PLUS_SUCCESS )
  {
    if( error == FidPatternRecognition::PATTERN_RECOGNITION_ERROR_TOO_MANY_CANDIDATES )
    {
      ui.label_Feedback->setText("Too many candidates. Reduce ROI region.");
      ui.label_Feedback->setStyleSheet("QLabel { color : red; }");
    }
    return PLUS_FAIL;
  }

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

  std::vector<std::vector<double> > segmentedDots = segResults.GetFoundDotsCoordinateValue();
  for (int i=0; i<segmentedDots.size(); ++i)
  {
    segmentedPoints->InsertPoint(i, segmentedDots[i][0], segmentedDots[i][1], -0.3);
  }
  segmentedPoints->Modified();

  m_ImageVisualizer->SetWireLabelPositions(segmentedPoints);

  m_SegmentedPointsPolyData->Initialize();
  m_SegmentedPointsPolyData->SetPoints(segmentedPoints);

  ui.label_Feedback->setText("");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::FreezeImage(bool aOn)
{
  LOG_INFO("FreezeImage turned " << (aOn?"on":"off"));

  m_ImageFrozen = aOn;

  ui.pushButton_Export->setEnabled(m_ImageFrozen);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::ExportImage()
{
  LOG_TRACE("SegmentationParameterDialog::ExportImage()");

  std::string metafileName = "SegmentationParameterDialog_ExportedImage_";
  metafileName.append(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S"));

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  trackedFrameList->AddTrackedFrame(&m_Frame);

  if (trackedFrameList->SaveToSequenceMetafile(vtkPlusConfig::GetInstance()->GetImageDirectory(), metafileName.c_str(), vtkTrackedFrameList::SEQ_METAFILE_MHA, false) == PLUS_SUCCESS)
  {
    QMessageBox::information(this, tr("Image exported"),
      QString("Image exported as metafile as %1\\%2.mha").arg(vtkPlusConfig::GetInstance()->GetImageDirectory()).arg(metafileName.c_str()));

    // Write the current state into the device set configuration XML
    if (m_DataCollector->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to save configuration of data collector"); 
    }
    else
    {
      // Save config file next to the tracked frame list
      std::string configFileName = (std::string)vtkPlusConfig::GetInstance()->GetImageDirectory() + "\\" + metafileName + "_Config.xml";
      PlusCommon::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
    }
  }
  else
  {
    QMessageBox::critical(this, tr("Image export failed"),
      QString("Image export failed to metafile %1\\%2.mha").arg(vtkPlusConfig::GetInstance()->GetImageDirectory()).arg(metafileName.c_str()));
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
  m_ImageVisualizer->EnableROI(true);

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

  m_ImageVisualizer->EnableROI(false);
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

PlusStatus SegmentationParameterDialog::GetFrameSize(int aImageDimensions[2])
{
  LOG_TRACE("SegmentationParameterDialog::GetFrameSize");

  m_DataCollector->GetBrightnessFrameSize(aImageDimensions);

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
  m_ImageVisualizer->SetROIBounds(aXMin, aXMax, aYMin, aYMax);

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
      m_ImageVisualizer->SetROIBounds(aValue, -1, -1, -1);
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
      m_ImageVisualizer->SetROIBounds(-1, -1, aValue, -1);
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
    m_ImageVisualizer->SetROIBounds(-1, aValue, -1, -1);
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
    m_ImageVisualizer->SetROIBounds(-1, -1, -1, aValue);
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

  m_PatternRecognition->GetFidLineFinder()->SetMinThetaDeg(aValue);
  m_PatternRecognition->GetFidLabeling()->SetMinThetaDeg(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::MaxThetaChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::MaxThetaChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLineFinder()->SetMaxThetaDeg(aValue);
  m_PatternRecognition->GetFidLabeling()->SetMaxThetaDeg(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::AngleToleranceChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::AngleToleranceChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLabeling()->SetAngleToleranceDeg(aValue);
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

void SegmentationParameterDialog::MaxLineShiftMmChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::MaxLineShiftMmChanged(" << aValue << ")");

  m_PatternRecognition->GetFidLabeling()->SetMaxLineShiftMm(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::MaxCandidatesChanged(double aValue)
{
  LOG_TRACE("SegmentationParameterDialog::MaxCandidatesChanged(" << aValue << ")");

  m_PatternRecognition->SetMaxNumberOfCandidates(aValue);
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialog::OriginalIntensityForDotsToggled(bool aOn)
{
  LOG_TRACE("SegmentationParameterDialog::OriginalIntensityForDotsToggled(" << (aOn?"true":"false") << ")");

  m_PatternRecognition->GetFidSegmentation()->SetUseOriginalImageIntensityForDotIntensityScore(aOn);
}


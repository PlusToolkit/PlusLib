/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkImageVisualizer_h
#define __vtkImageVisualizer_h

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkCamera.h"
#include "vtkDataCollector.h"
#include "vtkGlyph3D.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTextActor3D.h"

class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*! \class ImageVisualizer 
* \brief Class that manages the displaying of a 2D image in a QT canvas element
* \ingroup PlusAppCommonWidgets
*/
class vtkImageVisualizer : public vtkObject
{
public:
  /*!
  * New
  */
  static vtkImageVisualizer *New();

  /*!
  * Read rendering configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  * Calculate and set camera parameters so that image fits canvas in image mode
  */
  PlusStatus UpdateCameraPose();

  /*!
  * Show or hide the MF orientation markers
  * \param aEnable Show/Hide
  */
  PlusStatus ShowOrientationMarkers( bool aShow );

  /*!
  * Show or hide input points
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowInput(bool aOn);

  /*!
  * Set the input source
  * \param aImage pointer to the image data to show
  */
  void SetInput( vtkSmartPointer<vtkImageData> aImage );

  /*!
  * Show or hide result points
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowResult(bool aOn);

  /*!
  * Hide all tools, other models and the image from main canvas
  */
  PlusStatus HideAll();

  /*!
  * Set MF orientation in 2D mode
  * \param aNewOrientation The new MF orientation
  */
  PlusStatus SetScreenRightDownAxesOrientation(US_IMAGE_ORIENTATION aOrientation = US_IMG_ORIENT_MF);

  /*!
  * Add an actor to the list of screen aligned actors
  * \param aProp vtkProp3D to be managed
  */
  PlusStatus AddScreenAlignedProp(vtkSmartPointer<vtkProp3D> aProp);

  // Set/Get macros for member variables
  vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(HorizontalOrientationTextActor, vtkTextActor3D);
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  vtkGetObjectMacro(ImageCamera, vtkCamera);
  vtkGetObjectMacro(OrientationMarkerAssembly, vtkAssembly);
  vtkGetObjectMacro(ResultPolyData, vtkPolyData);
  vtkGetObjectMacro(VerticalOrientationTextActor, vtkTextActor3D);
  vtkSetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(DataCollector, vtkDataCollector);

  // These will conflict with vtk macros, figure out new naming convention instead of "Set"
  PlusStatus InitializeDataCollector(vtkSmartPointer<vtkDataCollector> aCollector);
  PlusStatus InitializeResultPolyData(vtkSmartPointer<vtkPolyData> aResultPolyData);

  // Utility functions
  PlusStatus SetResultColor(double r, double g, double b);
  PlusStatus SetResultOpacity(double aOpacity);

protected:
  /*!
  * Constructor
  */
  vtkImageVisualizer();

  /*!
  * Destructor
  */
  ~vtkImageVisualizer();

  vtkSetObjectMacro(DataCollector, vtkDataCollector);
  vtkSetObjectMacro(HorizontalOrientationTextActor, vtkTextActor3D);
  vtkSetObjectMacro(ImageActor, vtkImageActor);
  vtkSetObjectMacro(ImageCamera, vtkCamera);
  vtkSetObjectMacro(OrientationMarkerAssembly, vtkAssembly);
  vtkSetObjectMacro(ResultActor, vtkActor);
  vtkSetObjectMacro(ResultPolyData, vtkPolyData);
  vtkSetObjectMacro(ResultGlyph, vtkGlyph3D);
  vtkSetObjectMacro(VerticalOrientationTextActor, vtkTextActor3D);
  vtkGetObjectMacro(ResultGlyph, vtkGlyph3D);

  /*!
  * Initialize Orientation 3D Actors
  */
  PlusStatus InitializeOrientationMarkers();

  /*!
  * Calculate the correct orientation and position of the markers
  */
  PlusStatus UpdateOrientationMarkerLabelling();

  /*!
  * Clear list of screen-aligned actors
  * Also does memory cleanup
  */
  PlusStatus ClearScreenAlignedActorList();

  /*!
  * Update the position and orientation of actors to become screen aligned
  */
  PlusStatus UpdateScreenAlignedActors();

  /*! Data Collector link */
  vtkDataCollector* DataCollector;

  /*! Renderer for the canvas */
  vtkRenderer* CanvasRenderer; 

  /*! Canvas image actor */
  vtkImageActor* ImageActor;

  /*! Polydata holding the result points (eg. stylus tip, segmented points) */
  vtkPolyData* ResultPolyData;

  /*! Actor for displaying the result points (eg. stylus tip, segmented points) */
  vtkActor* ResultActor;

  /*! Glyph producer for result */
  vtkGlyph3D* ResultGlyph;

  /*! Camera of the scene */
  vtkCamera* ImageCamera;

  /*! Assembly of actors for displaying the MF orientation */
  vtkAssembly* OrientationMarkerAssembly;

  /*! Specific reference to the horizontal text actor */
  vtkTextActor3D* HorizontalOrientationTextActor;

  /*! Specific reference to the vertical text actor */
  vtkTextActor3D* VerticalOrientationTextActor;

  /*! The current horizontal orientation of the orientation markers */
  double ScreenAlignedCurrentXRotation;

  /*! The current vertical orientation of the orientation markers */
  double ScreenAlignedCurrentYRotation;

  /*! Record the current state of the marker orientation */
  US_IMAGE_ORIENTATION CurrentMarkerOrientation;

  /*! List of objects maintained by the visualizer to be screen aligned */
  std::vector<vtkSmartPointer<vtkProp3D>> ScreenAlignedProps;

  /*! List of original positions of screen-aligned objects */
  std::vector<std::vector<int>> ScreenAlignedPropOriginalPosition;
};

#endif
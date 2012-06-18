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
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkObject.h"
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
  /*! Constant for initial image camera position */
  static const double IMAGE_CAMERA_Z_POSITION;
  /*! Constant for the offset from the root of the assembly for the horizontal text actor */
  static double HORIZONTAL_TEXT_ORIENTATION_MARKER_OFFSET[3];
  /*! Constant for the offset from the root of the assembly for the vertical text actor */
  static double VERTICAL_TEXT_ORIENTATION_MARKER_OFFSET[3];
  /*! Constant for the color of the marker */
  static double ORIENTATION_MARKER_COLOUR[3];
  /*! Constant for the end point of the horizontal line */
  static double HORIZONTAL_LINE_ORIENTATION_MARKER_END_POINT[3];
  /*! Constant for the end point of the vertical line */
  static double VERTICAL_LINE_ORIENTATION_MARKER_END_POINT[3];
  /*! Constant for the initial position of the assembly */
  static double ORIENTATION_MARKER_ASSEMBLY_POSITION[3];
  /*! Constant for the orientation marker cone radius */
  static const double ORIENTATION_MARKER_CONE_RADIUS;
  /*! Constant for the orientation marker cone height */
  static const double ORIENTATION_MARKER_CONE_HEIGHT;
  /*! Constant for the offset from the root of the assembly for the horizontal cone actor */
  static double HORIZONTAL_CONE_ORIENTATION_MARKER_OFFSET[3];
  /*! Constant for the offset from the root of the assembly for the vertical cone actor */
  static double VERTICAL_CONE_ORIENTATION_MARKER_OFFSET[3];

  /*!
  * New
  */
  static vtkImageVisualizer *New();

public:
  /*!
  * Initialize object
  */
  PlusStatus Initialize(vtkSmartPointer<vtkDataCollector> aCollector, vtkSmartPointer<vtkPolyData> aResultPolyData);

  /*!
  * Read rendering configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  * Calculate and set camera parameters so that image fits canvas in image mode
  */
  PlusStatus CalculateCameraParameters();

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

  // Set/Get macros for member variables
  vtkBooleanMacro(Initialized, bool); 
  vtkGetMacro(Initialized, bool); 
  vtkSetMacro(Initialized, bool);

  vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(HorizontalOrientationTextActor, vtkTextActor3D);
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  vtkGetObjectMacro(ImageCamera, vtkCamera);
  vtkGetObjectMacro(OrientationMarkerAssembly, vtkAssembly);
  vtkGetObjectMacro(ResultPolyData, vtkPolyData);
  PlusStatus SetResultColor(double r, double g, double b);
  PlusStatus SetResultOpacity(double aOpacity);
  vtkGetObjectMacro(VerticalOrientationTextActor, vtkTextActor3D);

  vtkSetObjectMacro(CanvasRenderer, vtkRenderer);

  vtkSetObjectMacro(DataCollector, vtkDataCollector);
  vtkGetObjectMacro(DataCollector, vtkDataCollector);

protected:

  /*!
  * Constructor
  */
  vtkImageVisualizer();

  /*!
  * Destructor
  */
  ~vtkImageVisualizer();

  vtkSetObjectMacro(HorizontalOrientationTextActor, vtkTextActor3D);
  vtkSetObjectMacro(ImageActor, vtkImageActor);
  vtkSetObjectMacro(ImageCamera, vtkCamera);
  vtkSetObjectMacro(OrientationMarkerAssembly, vtkAssembly);
  vtkSetObjectMacro(ResultActor, vtkActor);
  vtkSetObjectMacro(ResultPolyData, vtkPolyData);
  vtkSetObjectMacro(VerticalOrientationTextActor, vtkTextActor3D);

  /*!
  * Initialize Orientation 3D Actors
  */
  PlusStatus InitializeOrientationMarkers();

  /*!
  * Calculate the correct orientation and position of the markers
  */
  PlusStatus UpdateOrientationMarkerTransformPosition(US_IMAGE_ORIENTATION aOrientation = US_IMG_ORIENT_MF);

  /*! Initialization flag */
  bool Initialized;

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

  /*! Camera of the scene */
  vtkCamera* ImageCamera;

  /*! Assembly of actors for displaying the MF orientation */
  vtkAssembly* OrientationMarkerAssembly;

  /*! Specific reference to the horizontal text actor */
  vtkTextActor3D* HorizontalOrientationTextActor;

  /*! Specific reference to the vertical text actor */
  vtkTextActor3D* VerticalOrientationTextActor;

  /*! The current horizontal orientation of the orientation markers */
  double OrientationMarkerCurrentXRotation;

  /*! The current vertical orientation of the orientation markers */
  double OrientationMarkerCurrentYRotation;
};

#endif
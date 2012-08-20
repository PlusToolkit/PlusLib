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
#include "vtkProp3DCollection.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTextActor3D.h"

class vtkXMLDataElement;
class vtkLineSource;

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

  /*! Publicly available colour for ROI */
  static double ROI_COLOUR[3];

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
  void SetInput( vtkImageData* aImage );

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
  PlusStatus AddScreenAlignedProp(vtkProp3D* aProp);

  /*!
  * Set the ROI region
  * \param xMin min x bounds of ROI
  * \param xMax max x bounds of ROI
  * \param yMin min y bounds of ROI
  * \param yMax max y bounds of ROI
  */
  PlusStatus SetROIBounds(int xMin, int xMax, int yMin, int yMax);

  /*!
  * Enable/disable ROI visualization
  * \param aEnable enable/disable flag
  */
  PlusStatus EnableROI(bool aEnable);

  /*!
  * Accept fiducial point information for displaying wire labels
  * \param aPointList x/y location for labels
  */
  PlusStatus SetWireLabelPositions(vtkPoints* aPointList);

  /*!
  * Enable/disable wire label visualization
  * \param aEnable enable/disable flag
  */
  PlusStatus EnableWireLabels(bool aEnable);

  // Set/Get macros for member variables
  vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(HorizontalOrientationTextActor, vtkTextActor3D);
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  vtkGetObjectMacro(ImageCamera, vtkCamera);
  vtkGetObjectMacro(OrientationMarkerAssembly, vtkAssembly);
  vtkGetObjectMacro(ResultPolyData, vtkPolyData);
  vtkGetObjectMacro(ScreenAlignedProps, vtkProp3DCollection);
  vtkGetObjectMacro(VerticalOrientationTextActor, vtkTextActor3D);
  vtkSetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(DataCollector, vtkDataCollector);

  // These will conflict with vtk macros, figure out new naming convention instead of "Set"
  PlusStatus AssignDataCollector(vtkDataCollector* aCollector);
  PlusStatus AssignResultPolyData(vtkPolyData* aResultPolyData);

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
  vtkSetObjectMacro(ROIActorAssembly, vtkAssembly);
  vtkSetObjectMacro(ResultActor, vtkActor);
  vtkSetObjectMacro(ScreenAlignedProps, vtkProp3DCollection);
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

  /*!
  * Initialize the ROI sources/actors
  */
  PlusStatus InitializeROIVisualization();

  /*!
  * Initialize the wire actors
  */
  PlusStatus InitializeWireLabelVisualization(vtkXMLDataElement* aConfig);

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
  vtkProp3DCollection* ScreenAlignedProps;

  /*! List of original positions of screen-aligned objects */
  std::vector<std::vector<double>> ScreenAlignedPropOriginalPosition;

  /*! Flag to hold value of show/hide ROI */
  bool ShowROI;

  /*! Assembly to hold all of the actors of the ROI for easy hide/show */
  vtkAssembly*                  ROIActorAssembly;

  /*! Line source of left line (one side of the ROI rectangle) */
  vtkLineSource*                LeftLineSource;

  /*! Line source of top line (one side of the ROI rectangle) */
  vtkLineSource*                TopLineSource;

  /*! Line source of right line (one side of the ROI rectangle) */
  vtkLineSource*                RightLineSource;

  /*! Line source of bottom line (one side of the ROI rectangle) */
  vtkLineSource*                BottomLineSource;

  /*! Array holding the bounds of the ROI */
  double                        RegionOfInterest[4];

  /*! Vector to hold the actors for each wire */
  std::vector<vtkTextActor3D*>  WireActors;
};

#endif
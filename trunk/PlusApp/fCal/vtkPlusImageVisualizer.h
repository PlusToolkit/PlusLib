/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkImageVisualizer_h
#define __vtkImageVisualizer_h

// PlusLib includes
#include <PlusConfigure.h>
#include <PlusVideoFrame.h>
#include <vtkPlusChannel.h>

// VTK includes
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkObject.h>
#include <vtkPolyData.h>
#include <vtkProp3DCollection.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor3D.h>

class vtkXMLDataElement;
class vtkLineSource;
class vtkImageSliceMapper;

//-----------------------------------------------------------------------------

/*! \class ImageVisualizer
* \brief Class that manages the displaying of a 2D image in a QT canvas element
* \ingroup PlusAppCommonWidgets
*/
class vtkPlusImageVisualizer : public vtkObject
{

public:
  static vtkPlusImageVisualizer* New();
  vtkTypeMacro(vtkPlusImageVisualizer, vtkObject);

  /*! Publicly available color for ROI */
  static double ROI_COLOR[3];

  /*! Read clipping rectangle parameters from XML */
  PlusStatus ReadRoiConfiguration(vtkXMLDataElement* aXMLElement);

  /* Read rendering configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /// Calculate and set camera parameters so that image fits canvas in image mode

  PlusStatus UpdateCameraPose();

  /*! Show or hide the MF orientation markers
  * \param aEnable Show/Hide
  */
  PlusStatus ShowOrientationMarkers(bool aShow);

  /*! Set the input source
  * \param aImage pointer to the image data to show
  */
  void SetInputData(vtkImageData* aImage);

  /* Set the slice number of the data */
  PlusStatus SetSliceNumber(int number);

  void SetResultPolyData(vtkPolyData* aResultPolyData);

  /*! Show or hide result points
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowResult(bool aOn);

  /// Hide all tools, other models and the image from main canvas

  PlusStatus HideAll();

  /*! Set MF orientation in 2D mode
  * \param aNewOrientation The new MF orientation
  */
  PlusStatus SetScreenRightDownAxesOrientation(US_IMAGE_ORIENTATION aOrientation = US_IMG_ORIENT_MF);

  /*! Add an actor to the list of screen aligned actors
  * \param aProp vtkProp3D to be managed
  */
  PlusStatus AddScreenAlignedProp(vtkProp3D* aProp);

  /*! Remove an actor from the list of screen aligned actors
  * \param aProp vtkProp3D to be managed
  */
  PlusStatus RemoveScreenAlignedProp(vtkProp3D* aProp);

  /*! Set the ROI region
  * \param xMin min x bounds of ROI
  * \param xMax max x bounds of ROI
  * \param yMin min y bounds of ROI
  * \param yMax max y bounds of ROI
  */
  PlusStatus SetROIBounds(int xMin, int xMax, int yMin, int yMax);

  /*! Enable/disable ROI visualization
  * \param aEnable enable/disable flag
  */
  PlusStatus EnableROI(bool aEnable);

  /*! Accept fiducial point information for displaying wire labels
  * \param aPointList x/y location for labels
  */
  PlusStatus SetWireLabelPositions(vtkPoints* aPointList);

  /*! Enable/disable wire label visualization
  * \param aEnable enable/disable flag
  */
  PlusStatus EnableWireLabels(bool aEnable);

  /// Control the image actor visibility
  vtkImageActor* GetImageActor();

  /// Accessors
  vtkRenderer* GetCanvasRenderer();

  /// Reset the visualization
  PlusStatus Reset();

  void SetChannel(vtkPlusChannel* channel);

  // Utility functions
  PlusStatus SetResultColor(double r, double g, double b);
  PlusStatus SetResultOpacity(double aOpacity);

  // Line segmentation actor functions
  void SetLineSegmentationVisible(bool _arg);
  void SetLineSegmentationPoints(double startPoint_Image[2], double endPoint_Image[2]);

protected:
  vtkPlusImageVisualizer();
  ~vtkPlusImageVisualizer();

  vtkSetObjectMacro(SelectedChannel, vtkPlusChannel);

  /// Initialize Orientation 3D Actors
  void InitializeOrientationMarkers();

  // Initialize line segmentation 3D actors
  void InitializeLineSegmentationMarkers();

  /// Initialize the ROI sources/actors
  void InitializeROIVisualization();

  /// Calculate the correct orientation and position of the markers
  PlusStatus UpdateOrientationMarkerLabelling();

  /// Clear list of screen-aligned actors, Also does memory cleanup
  PlusStatus ClearScreenAlignedActorList();

  /// Update the position and orientation of actors to become screen aligned

  PlusStatus UpdateScreenAlignedActors();

  /// Initialize the wire actors
  PlusStatus InitializeWireLabelVisualization(vtkXMLDataElement* aConfig);

  /// Clear the wire actors
  PlusStatus ClearWireLabelVisualization();

protected:
  ///  Renderer for the canvas
  vtkSmartPointer<vtkRenderer>                          CanvasRenderer;
  ///  Canvas image actor
  vtkSmartPointer<vtkImageActor>                        ImageActor;
  ///  Image slice mapper of image actor
  vtkSmartPointer<vtkImageSliceMapper>                  ImageMapper;
  ///  Polydata holding the result points (eg. stylus tip, segmented points)
  vtkSmartPointer<vtkPolyData>                         ResultPolyData;
  ///  Actor for displaying the result points (eg. stylus tip, segmented points)
  vtkSmartPointer<vtkActor>                             ResultActor;
  ///  Glyph producer for result
  vtkSmartPointer<vtkGlyph3D>                           ResultGlyph;
  ///  Camera of the scene
  vtkSmartPointer<vtkCamera>                            ImageCamera;
  ///  Assembly of actors for displaying the MF orientation
  vtkSmartPointer<vtkAssembly>                          OrientationMarkerAssembly;
  ///  Specific reference to the horizontal text actor
  vtkSmartPointer<vtkTextActor3D>                       HorizontalOrientationTextActor;
  ///  Specific reference to the vertical text actor
  vtkSmartPointer<vtkTextActor3D>                       VerticalOrientationTextActor;
  ///  The current horizontal orientation of the orientation markers
  double                                                ScreenAlignedCurrentXRotation;
  ///  The current vertical orientation of the orientation markers
  double                                                ScreenAlignedCurrentYRotation;
  ///  Record the current state of the marker orientation
  US_IMAGE_ORIENTATION                                  CurrentMarkerOrientation;
  ///  List of objects maintained by the visualizer to be screen aligned
  vtkSmartPointer<vtkProp3DCollection>                 ScreenAlignedProps;
  ///  List of original positions of screen-aligned objects
  std::vector<std::vector<double> >                     ScreenAlignedPropOriginalPosition;
  ///  Flag to hold value of show/hide ROI
  bool                                                  ShowROI;
  ///  Assembly to hold all of the actors of the ROI for easy hide/show
  vtkSmartPointer<vtkAssembly>                          ROIActorAssembly;
  ///  Line source of left line (one side of the ROI rectangle)
  vtkSmartPointer<vtkLineSource>                        LeftLineSource;
  ///  Line source of top line (one side of the ROI rectangle)
  vtkSmartPointer<vtkLineSource>                        TopLineSource;
  ///  Line source of right line (one side of the ROI rectangle)
  vtkSmartPointer<vtkLineSource>                        RightLineSource;
  ///  Line source of bottom line (one side of the ROI rectangle)
  vtkSmartPointer<vtkLineSource>                        BottomLineSource;
  ///  Array holding the bounds of the ROI
  double                                                RegionOfInterest[4];
  ///  The channel to visualize
  vtkPlusChannel*                                       SelectedChannel;
  ///  Vector to hold the actors for each wire
  std::vector<vtkSmartPointer<vtkTextActor3D>>          WireActors;
  /// Line visualization members
  vtkSmartPointer<vtkLineSource>                        LineSegmentationLineSource;
  vtkSmartPointer<vtkActor>                             LineSegmentationActor;
};

#endif

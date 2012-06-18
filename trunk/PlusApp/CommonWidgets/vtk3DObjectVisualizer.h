/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtk3DObjectVisualizer_h
#define __vtk3DObjectVisualizer_h

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkDataCollector.h"
#include "vtkDisplayableObject.h"
#include "vtkImageActor.h"
#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTransformRepository.h"

//-----------------------------------------------------------------------------

/*! \class vtk3DObjectVisualizer 
 * \brief Class that manages the displaying of a 3D object visualization in a QT canvas element
 * \ingroup PlusAppCommonWidgets
 */
class vtk3DObjectVisualizer : public vtkObject
{
public:
  /*!
  * New
  */
  static vtk3DObjectVisualizer *New();

  /*!
  * Initializes device visualization - loads models, transforms, assembles visualization pipeline
  */
  PlusStatus Initialize(vtkSmartPointer<vtkDataCollector> aCollector, vtkSmartPointer<vtkPolyData> aInputPolyData, vtkSmartPointer<vtkPolyData> aResultPolyData, vtkSmartPointer<vtkTransformRepository> aTransformRepository);

  /*!
  * Get displayable tool object
  * \param aObjectCoordinateFrame Object coordinate frame name
  * \param aDisplayableTool Displayable object out parameter
  */
  PlusStatus GetDisplayableObject(const char* aObjectCoordinateFrame, vtkDisplayableObject* &aDisplayableTool);

  /*! Clear displayable object vector */
  PlusStatus ClearDisplayableObjects();

  /*!
  * Show or hide all displayable objects
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowAllObjects(bool aOn);

  /*!
  * Show or hide a displayable object
  * \param aObjectCoordinateFrame Object coordinate frame name
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowObject(const char* aObjectCoordinateFrame, bool aOn);

  /*!
  * Show or hide input points
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowInput(bool aOn);

  /*!
  * Show or hide result points
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowResult(bool aOn);

  /*!
  * Hide all tools, other models and the image from main canvas
  */
  PlusStatus HideAll();

public:
  /*!
  * Update the displayable objects
  */
  PlusStatus Update();

  // Set/Get macros for member variables
  vtkSetMacro(Initialized, bool); 
  vtkGetMacro(Initialized, bool); 
  vtkBooleanMacro(Initialized, bool); 

  vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(DataCollector, vtkDataCollector); 
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  vtkGetObjectMacro(VolumeActor, vtkActor);
  vtkGetObjectMacro(TransformRepository, vtkTransformRepository);

  vtkSetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkSetObjectMacro(DataCollector, vtkDataCollector); 

  vtkGetStringMacro(WorldCoordinateFrame);
  vtkSetStringMacro(WorldCoordinateFrame);

protected:
  vtkSetObjectMacro(Camera, vtkCamera);
  vtkSetObjectMacro(ImageActor, vtkImageActor);
  vtkSetObjectMacro(InputActor, vtkActor);
  vtkSetObjectMacro(ResultActor, vtkActor);
  vtkSetObjectMacro(InputPolyData, vtkPolyData);
  vtkSetObjectMacro(ResultPolyData, vtkPolyData);
  vtkSetObjectMacro(VolumeActor, vtkActor);
  vtkSetObjectMacro(TransformRepository, vtkTransformRepository);

protected:
  /*!
  * Constructor
  */
  vtk3DObjectVisualizer();

  /*!
  * Destructor
  */
  virtual ~vtk3DObjectVisualizer();	

protected:
  /*! Data collector object */
  vtkDataCollector*	DataCollector;

  /*! List of displayable objects */
  std::map<std::string, vtkDisplayableObject*> DisplayableObjects;

  /*! Initialization flag */
  bool Initialized;

  /*! Renderer for the canvas */
  vtkRenderer* CanvasRenderer; 

  /*! Canvas image actor */
  vtkImageActor* ImageActor;

  /*! Polydata holding the input points */
  vtkPolyData* InputPolyData;

  /*! Actor for displaying the input points in 3D */
  vtkActor* InputActor;

  /*! Polydata holding the result points (eg. stylus tip, segmented points) */
  vtkPolyData* ResultPolyData;

  /*! Actor for displaying the result points (eg. stylus tip, segmented points) */
  vtkActor* ResultActor;

  /*! Actor for displaying a volume */
  vtkActor* VolumeActor;

  /*! Camera of the scene */
  vtkCamera* Camera;

  /*! Name of the rendering world coordinate frame */
  char* WorldCoordinateFrame;

  /*! Reference to Transform repository that stores and handles all transforms */
  vtkTransformRepository* TransformRepository;
};

#endif  //__vtk3DObjectVisualizer_h
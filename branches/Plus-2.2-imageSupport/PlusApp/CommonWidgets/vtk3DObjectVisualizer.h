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
#include "vtkGlyph3D.h"
#include "vtkImageActor.h"
#include "vtkObject.h"
#include "vtkPlusChannel.h"
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
  * Return a displayable object
  * \param aModelId Model ID of the object to return
  */
  vtkDisplayableObject* GetObjectById( const char* aModelId );

  /*! Clear displayable object vector */
  PlusStatus ClearDisplayableObjects();

  /*!
  * Show or hide all displayable objects
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowAllObjects(bool aOn);

  /* Return the actor of the volume actor */
  vtkActor* GetVolumeActor();

  /*!
  * Set the volume actor mapper
  * \param aContourMapper new mapper to use
  */
  PlusStatus SetVolumeMapper( vtkPolyDataMapper* aContourMapper );

  /*!
  * Set the volume actor colour
  * \param r red value
  * \param g green value
  * \param b blue value
  */
  PlusStatus SetVolumeColor( double r, double g, double b );

  /*!
  * Show or hide a displayable object
  * \param aModelId Model ID of the object to work on
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowObjectById( const char* aModelId, bool aOn );

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

  /*!
  * Read the active configuration file to create displayable objects
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aXMLElement);

  /*!
  * Update the displayable objects
  */
  PlusStatus Update();

  // Set/Get macros for member variables
  vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(DataCollector, vtkDataCollector); 
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  virtual vtkSmartPointer<vtkTransformRepository> GetTransformRepository();
  vtkSetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetStringMacro(WorldCoordinateFrame);
  vtkSetStringMacro(WorldCoordinateFrame);
  vtkGetStringMacro(VolumeID);

  vtkSetObjectMacro(SelectedChannel, vtkPlusChannel);

  // These will conflict with vtk macros, figure out new naming convention instead of "Set"
  PlusStatus AssignDataCollector(vtkDataCollector* aCollector);
  PlusStatus AssignInputPolyData(vtkPolyData* aInputPolyData);
  PlusStatus AssignResultPolyData(vtkPolyData* aResultPolyData);
  PlusStatus AssignTransformRepository(vtkSmartPointer<vtkTransformRepository> aTransformRepository);

protected:
  vtkSetObjectMacro(ImageActor, vtkImageActor);
  vtkSetObjectMacro(InputActor, vtkActor);
  vtkSetObjectMacro(ResultActor, vtkActor);
  vtkSetObjectMacro(InputPolyData, vtkPolyData);
  vtkSetObjectMacro(ResultPolyData, vtkPolyData);
  virtual void SetTransformRepository(vtkSmartPointer<vtkTransformRepository> aRepository);
  vtkSetObjectMacro(DataCollector, vtkDataCollector);
  vtkSetObjectMacro(ResultGlyph, vtkGlyph3D);
  vtkSetObjectMacro(InputGlyph, vtkGlyph3D);
  vtkGetObjectMacro(ResultGlyph, vtkGlyph3D);
  vtkGetObjectMacro(InputGlyph, vtkGlyph3D);
  vtkSetStringMacro(VolumeID);

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
  std::vector<vtkDisplayableObject*> DisplayableObjects;

  /*! Renderer for the canvas */
  vtkRenderer* CanvasRenderer; 

  /*! Canvas image actor */
  vtkImageActor* ImageActor;

  /*! Polydata holding the input points */
  vtkPolyData* InputPolyData;

  /*! Actor for displaying the input points in 3D */
  vtkActor* InputActor;

  /*! Glyph producer for result */
  vtkGlyph3D* InputGlyph;

  /*! Polydata holding the result points (eg. stylus tip, segmented points) */
  vtkPolyData* ResultPolyData;

  /*! Actor for displaying the result points (eg. stylus tip, segmented points) */
  vtkActor* ResultActor;

  /*! Glyph producer for result */
  vtkGlyph3D* ResultGlyph;

  /*! Name of the rendering world coordinate frame */
  char* WorldCoordinateFrame;

  /*! Name of the volume object ID */
  char* VolumeID;

  /*! Reference to Transform repository that stores and handles all transforms */
  vtkSmartPointer<vtkTransformRepository> TransformRepository;

  /*! Channel to visualize */
  vtkPlusChannel* SelectedChannel;
};

#endif  //__vtk3DObjectVisualizer_h
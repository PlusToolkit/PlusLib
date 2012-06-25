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
  * Get displayable tool object
  * \param aObjectCoordinateFrame Object coordinate frame name
  * \param aDisplayableTool Displayable object out parameter
  */
  template <class T>
  std::vector<T*> GetDisplayableObjects(const char* aObjectCoordinateFrame);

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

  /*!
  * Read the active configuration file to create displayable objects
  */
  PlusStatus ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> aXMLElement);

public:
  /*!
  * Update the displayable objects
  */
  PlusStatus Update();

  // Set/Get macros for member variables
  vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetObjectMacro(DataCollector, vtkDataCollector); 
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  vtkGetObjectMacro(VolumeActor, vtkActor);
  vtkGetObjectMacro(TransformRepository, vtkTransformRepository);
  vtkSetObjectMacro(CanvasRenderer, vtkRenderer);
  vtkGetStringMacro(WorldCoordinateFrame);
  vtkSetStringMacro(WorldCoordinateFrame);

  // These will conflict with vtk macros, figure out new naming convention instead of "Set"
  PlusStatus InitializeDataCollector(vtkSmartPointer<vtkDataCollector> aCollector);
  PlusStatus InitializeInputPolyData(vtkSmartPointer<vtkPolyData> aInputPolyData);
  PlusStatus InitializeResultPolyData(vtkSmartPointer<vtkPolyData> aResultPolyData);
  PlusStatus InitializeTransformRepository(vtkSmartPointer<vtkTransformRepository> aTransformRepository);

protected:
  vtkSetObjectMacro(Camera, vtkCamera);
  vtkSetObjectMacro(ImageActor, vtkImageActor);
  vtkSetObjectMacro(InputActor, vtkActor);
  vtkSetObjectMacro(ResultActor, vtkActor);
  vtkSetObjectMacro(InputPolyData, vtkPolyData);
  vtkSetObjectMacro(ResultPolyData, vtkPolyData);
  vtkSetObjectMacro(VolumeActor, vtkActor);
  vtkSetObjectMacro(TransformRepository, vtkTransformRepository);
  vtkSetObjectMacro(DataCollector, vtkDataCollector);
  vtkSetObjectMacro(ResultGlyph, vtkGlyph3D);
  vtkSetObjectMacro(InputGlyph, vtkGlyph3D);
  vtkGetObjectMacro(ResultGlyph, vtkGlyph3D);
  vtkGetObjectMacro(InputGlyph, vtkGlyph3D);

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

  /*! Actor for displaying a volume */
  vtkActor* VolumeActor;

  /*! Camera of the scene */
  vtkCamera* Camera;

  /*! Name of the rendering world coordinate frame */
  char* WorldCoordinateFrame;

  /*! Reference to Transform repository that stores and handles all transforms */
  vtkTransformRepository* TransformRepository;
};

//-----------------------------------------------------------------------------

template <class T>
std::vector<T*> vtk3DObjectVisualizer::GetDisplayableObjects(const char* aObjectCoordinateFrame)
{
  LOG_TRACE("vtkPerspectiveVisualizer::GetDisplayableObjects");

  std::vector<T*> result;

  if (aObjectCoordinateFrame == NULL)
  {
    LOG_ERROR("Invalid object coordinate frame name!");
    return result;
  }

  for( std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* pObj = *it;
    T* pCastedObj = dynamic_cast<T*>(pObj);
    if(STRCASECMP(pObj->GetObjectCoordinateFrame(), aObjectCoordinateFrame) == 0 && pCastedObj != NULL)
    {
      result.push_back(pCastedObj);
    }
  }

  if( result.size() == 0 )
  {
    LOG_ERROR("Requested displayable objects for identifier '" << aObjectCoordinateFrame << "' is/are missing!");
  }

  return result;
}

#endif  //__vtk3DObjectVisualizer_h
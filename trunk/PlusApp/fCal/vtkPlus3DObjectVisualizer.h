/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtk3DObjectVisualizer_h
#define __vtk3DObjectVisualizer_h

// Local includes
#include "vtkPlusDisplayableObject.h"

// PlusLib includes
#include <PlusConfigure.h>
#include <vtkPlusTransformRepository.h>
#include <vtkPlusChannel.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkImageActor.h>
#include <vtkObject.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

class vtkImageSliceMapper;

/*! \class vtkPlus3DObjectVisualizer
 * \brief Class that manages the displaying of a 3D object visualization in a QT canvas element
 * \ingroup PlusAppCommonWidgets
 */
class vtkPlus3DObjectVisualizer : public vtkObject
{
public:
  vtkTypeMacro(vtkPlus3DObjectVisualizer, vtkObject);
  static vtkPlus3DObjectVisualizer* New();

  /*!
  * Return a displayable object
  * \param aModelId Model ID of the object to return
  */
  vtkPlusDisplayableObject* GetObjectById(const std::string& aModelId);

  /*!
  * Return a displayable object
  * \param aModelId Model ID of the object to return
  */
  PlusStatus AddObject(vtkPlusDisplayableObject* displayableObject);

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
  PlusStatus SetVolumeMapper(vtkPolyDataMapper* aContourMapper);

  /*! Set the slice number
  * \param sliceNumber the slice number to display
  */
  PlusStatus SetSliceNumber(int number);

  /*!
  * Set the volume actor color
  * \param r red value
  * \param g green value
  * \param b blue value
  */
  PlusStatus SetVolumeColor(double r, double g, double b);

  /*!
  * Show or hide a displayable object
  * \param aModelId Model ID of the object to work on
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowObjectById(const std::string& aModelId, bool aOn);

  /*!
  * Show or hide input points
  * \param aOn Show if true, else hide
  */
  PlusStatus ShowInput(bool aOn);

  /*!
  * Set the actor color of the input polydata
  * \param r g b
  */
  PlusStatus SetInputColor(double r, double g, double b);

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

  // Set/Get for member variables
  vtkRenderer* GetCanvasRenderer() const;
  vtkImageActor* GetImageActor() const;
  vtkSetObjectMacro(TransformRepository, vtkPlusTransformRepository);

  vtkSetMacro(WorldCoordinateFrame, std::string);
  vtkGetMacro(WorldCoordinateFrame, std::string);

  vtkGetMacro(VolumeID, std::string);

  void SetInputPolyData(vtkPolyData* aPolyData);
  void SetResultPolyData(vtkPolyData* aPolyData);

  PlusStatus SetChannel(vtkPlusChannel* channel);

protected:
  void SetCanvasRenderer(vtkSmartPointer<vtkRenderer> renderer);
  void SetImageActor(vtkSmartPointer<vtkImageActor> imageActor);
  void SetInputActor(vtkSmartPointer<vtkActor> inputActor);
  void SetResultActor(vtkSmartPointer<vtkActor> resultActor);

  void SetResultGlyph(vtkSmartPointer<vtkGlyph3D> glyph);
  vtkSmartPointer<vtkGlyph3D> GetResultGlyph() const;

  void SetInputGlyph(vtkSmartPointer<vtkGlyph3D> glyph);
  vtkSmartPointer<vtkGlyph3D> GetInputGlyph() const;

  vtkSetMacro(VolumeID, std::string);
  vtkSetObjectMacro(SelectedChannel, vtkPlusChannel);

protected:
  /*! List of displayable objects */
  std::vector<vtkPlusDisplayableObject*> DisplayableObjects;

  /*! Renderer for the canvas */
  vtkSmartPointer<vtkRenderer> CanvasRenderer;

  /*! Canvas image actor */
  vtkSmartPointer<vtkImageActor> ImageActor;

  /*! Actor for displaying the input points in 3D */
  vtkSmartPointer<vtkActor> InputActor;

  /*! Slice mapper to enable slice selection */
  vtkSmartPointer<vtkImageSliceMapper> ImageMapper;

  /*! Glyph producer for result */
  vtkSmartPointer<vtkGlyph3D> InputGlyph;

  /*! Actor for displaying the result points (eg. stylus tip, segmented points) */
  vtkSmartPointer<vtkActor> ResultActor;

  /*! Glyph producer for result */
  vtkSmartPointer<vtkGlyph3D> ResultGlyph;

  /*! Name of the rendering world coordinate frame */
  std::string WorldCoordinateFrame;

  /*! Name of the volume object ID */
  std::string VolumeID;

  /*! Reference to Transform repository that stores and handles all transforms */
  vtkPlusTransformRepository* TransformRepository;

  /*! Channel to visualize */
  vtkPlusChannel* SelectedChannel;

protected:
  vtkPlus3DObjectVisualizer();
  virtual ~vtkPlus3DObjectVisualizer();
};

#endif  //__vtk3DObjectVisualizer_h
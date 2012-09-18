/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorAlgo_h
#define __vtkUsSimulatorAlgo_h

#include "vtkImageAlgorithm.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

/*!
  \class vtkUsSimulatorAlgo 
  \brief Class that simulates ultrasound images using an STL model and
         transformations of the intersecting plane as input and
  \ingroup PlusLibUsSimulatorAlgo
*/
class VTK_EXPORT vtkUsSimulatorAlgo : public vtkImageAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkUsSimulatorAlgo,vtkImageAlgorithm);	
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkUsSimulatorAlgo *New();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Load stl or vtp model  */ 
  PlusStatus LoadModel(std::string absoluteImagePath);

  /*! Create stencil background image according to the already read configuration settings */
  PlusStatus CreateStencilBackgroundImage();

public:
  // Set model to image transformation matrix
  vtkSetObjectMacro(ModelToImageMatrix, vtkMatrix4x4); 
  // Get model to image transformation matrix
  vtkGetObjectMacro(ModelToImageMatrix, vtkMatrix4x4); 

  /*! Set model file name */
  vtkSetStringMacro(ModelFileName); 
  /*! Get model file name */
  vtkGetStringMacro(ModelFileName);

  /*! Set image frame name */
  vtkSetStringMacro(ImageCoordinateFrame); 
  /*! Get image frame name */
  vtkGetStringMacro(ImageCoordinateFrame);

  /*! Set reference frame name */
  vtkSetStringMacro(ReferenceCoordinateFrame); 
  /*! Get reference frame name */
  vtkGetStringMacro(ReferenceCoordinateFrame);

  /*! Set background value */
  vtkSetMacro(BackgroundValue, int); 
  /*! Get background value */
  vtkGetMacro(BackgroundValue, int);

  /*! Get frame size */
  vtkGetVector2Macro(FrameSize, int);

  /*! Get pixel spacing */
  vtkGetVector2Macro(SpacingMmPerPixel, double);

//protected:
  /*! Set stencil background image */
  vtkSetObjectMacro(StencilBackgroundImage, vtkImageData); 

protected:
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);
  virtual int RequestData(vtkInformation *request,
                        vtkInformationVector** inputVector,
                        vtkInformationVector* outputVector);

protected:
  vtkUsSimulatorAlgo();
  ~vtkUsSimulatorAlgo(); 

private:
  /*! Stencil background image */
  vtkImageData* StencilBackgroundImage;

  /*! Model to image transformation matrix */
  vtkMatrix4x4* ModelToImageMatrix;

  /*! Grayscale value of the background */
  int BackgroundValue;

  /*! Image frame size in pixel */
  int FrameSize[2];

  /*! Pixel spacing */
  double SpacingMmPerPixel[2];

  /*! Path and name of the STL model file */
  char* ModelFileName;

  /*! Name of the image coordinate frame */
  char* ImageCoordinateFrame;

  /*! Name of the reference coordinate frame */
  char* ReferenceCoordinateFrame;
};

#endif // __vtkUsSimulatorAlgo_h

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

public:
  // Set model to image transformation matrix
  vtkSetObjectMacro(ModelToImageMatrix, vtkMatrix4x4); 
  // Get model to image transformation matrix
  vtkGetObjectMacro(ModelToImageMatrix, vtkMatrix4x4); 

  // Set stencil background image
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
  // Stencil background image
  vtkImageData* StencilBackgroundImage; 

  // Model to image transformation matrix
  vtkMatrix4x4* ModelToImageMatrix;  

  // Output image
  vtkImageData* OutputImage;

  static const unsigned char OUTVALSTENCILFOREGROUND;
  static const int INPUTOBJECTNUM;
  static const int OUTPUTOBJECTNUM;
};

#endif // __vtkUsSimulatorAlgo_h

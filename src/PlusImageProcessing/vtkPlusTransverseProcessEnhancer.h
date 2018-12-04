/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusTransverseProcessEnhancer_h
#define __vtkPlusTransverseProcessEnhancer_h

// Local includes
#include "vtkPlusImageProcessingExport.h"
#include "vtkPlusBoneEnhancer.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkSetGet.h>

class vtkImageData;

/*!
\class vtkPlusTransverseProcessEnhancer
\brief Localize transverse process bone surfaces in ultrasound images
\ingroup PlusLibImageProcessingAlgo
*/
class vtkPlusImageProcessingExport vtkPlusTransverseProcessEnhancer : public vtkPlusBoneEnhancer
{
public:
  static vtkPlusTransverseProcessEnhancer* New();
  vtkTypeMacro(vtkPlusTransverseProcessEnhancer, vtkPlusBoneEnhancer);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char* GetProcessorTypeName() { return "vtkPlusTransverseProcessEnhancer"; };

  /*! Process input frame to localize transverse process bone surfaces */
  PlusStatus ProcessFrame(igsioTrackedFrame* inputFrame, igsioTrackedFrame* outputFrame);

  void RemoveOffCameraBones(vtkSmartPointer<vtkImageData> inputImage);

  void CompareShadowAreas(vtkSmartPointer<vtkImageData> originalImage, vtkSmartPointer<vtkImageData> inputImage);

protected:
  vtkPlusTransverseProcessEnhancer();
  virtual ~vtkPlusTransverseProcessEnhancer();

private:
  vtkPlusTransverseProcessEnhancer(const vtkPlusTransverseProcessEnhancer&);  // Not implemented.
  void operator=(const vtkPlusTransverseProcessEnhancer&);  // Not implemented.
};

#endif

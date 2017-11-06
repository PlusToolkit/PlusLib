/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/



#ifndef __vtkPlusTransverseProcessEnhancer_h
#define __vtkPlusTransverseProcessEnhancer_h

// Local includes
#include "vtkPlusImageProcessingExport.h"
#include "vtkPlusTrackedFrameProcessor.h"
#include "vtkPlusBoneEnhancer.h"
#include "vtkImageAlgorithm.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkSetGet.h>

class vtkImageData;
class vtkImageThreshold;
class vtkImageGaussianSmooth;
class vtkImageSobel2D;
class vtkImageIslandRemoval2D;
class vtkImageDilateErode3D;
//class vtkImageCast;
//class vtkImageShiftScale;
class vtkPlusUsScanConvert;

/*!
\class vtkPlusTransverseProcessEnhancer
\brief Improves bone surface visibility in ultrasound images
\ingroup PlusLibImageProcessingAlgo
*/
class vtkPlusImageProcessingExport vtkPlusTransverseProcessEnhancer : public vtkPlusBoneEnhancer
{
public:

	static vtkPlusTransverseProcessEnhancer* New();

	PlusStatus ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame);
	void RemoveOffCameraBones(vtkSmartPointer<vtkImageData> inputImage);
	void CompareShadowAreas(vtkSmartPointer<vtkImageData> originalImage, vtkSmartPointer<vtkImageData> inputImage);
};


#endif
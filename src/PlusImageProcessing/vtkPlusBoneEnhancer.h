/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusBoneEnhancer_h
#define __vtkPlusBoneEnhancer_h

// Local includes
#include "vtkPlusImageProcessingExport.h"
#include "vtkPlusTrackedFrameProcessor.h"

// VTK includes
#include <vtkSmartPointer.h>

class vtkImageCast;
class vtkImageData;
#ifdef PLUS_USE_INTEL_MKL
class vtkPlusForoughiBoneSurfaceProbability;
#else
class vtkImageThreshold;
#endif
class vtkPlusUsScanConvert;

/*!
  \class vtkPlusBoneEnhancer
  \brief Improves bone surface visibility in ultrasound images
  \ingroup PlusLibImageProcessingAlgo
*/
class vtkPlusImageProcessingExport vtkPlusBoneEnhancer : public vtkPlusTrackedFrameProcessor
{
public:
  static vtkPlusBoneEnhancer* New();
  vtkTypeMacro(vtkPlusBoneEnhancer , vtkPlusTrackedFrameProcessor);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Update output frame from input frame */
  virtual PlusStatus ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame);

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* processingElement);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* processingElement);

  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() { return "BoneEnhancer"; };

  /*! Get/Set thresholding parameter */
  virtual void SetThreshold(double threshold);
  virtual double GetThreshold();

protected:
  vtkPlusBoneEnhancer();
  virtual ~vtkPlusBoneEnhancer();

  vtkSmartPointer<vtkPlusUsScanConvert> ScanConverter;
#ifdef PLUS_USE_INTEL_MKL
  vtkSmartPointer<vtkPlusForoughiBoneSurfaceProbability> BoneSurfaceFilter;
  vtkSmartPointer<vtkImageCast> CastToDouble;
  vtkSmartPointer<vtkImageCast> CastToUnsignedChar;
#else
  vtkSmartPointer<vtkImageThreshold> Thresholder;
#endif

};

#endif

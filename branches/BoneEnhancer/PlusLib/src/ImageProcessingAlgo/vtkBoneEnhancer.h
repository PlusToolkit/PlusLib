/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkBoneEnhancer_h
#define __vtkBoneEnhancer_h

#include "vtkImageProcessingAlgoExport.h"
#include "vtkSmartPointer.h"
#include "vtkTrackedFrameProcessor.h"

class vtkImageData;
class vtkImageThreshold;
class vtkTrackedFrameList;
class vtkUsScanConvert;

/*!
  \class vtkBoneEnhancer 
  \brief Improves bone surface visibility in ultrasound images
  \ingroup PlusLibImageProcessingAlgo
*/ 
class vtkImageProcessingAlgoExport vtkBoneEnhancer : public vtkTrackedFrameProcessor
{
public:
  static vtkBoneEnhancer *New();
  vtkTypeMacro(vtkBoneEnhancer , vtkTrackedFrameProcessor);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

   /*! Update output frame from input frame */
  virtual PlusStatus ProcessFrame(TrackedFrame* inputFrame, TrackedFrame* outputFrame);
 
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
  vtkBoneEnhancer();
  virtual ~vtkBoneEnhancer();

  /*! Helper function for drawing a line on a VTK image */
  void DrawLine(vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints);

  /*! Draws scanlines on the image - for testing and debugging */
  void DrawScanLines(vtkUsScanConvert* scanConverter, vtkImageData* imageData);
  
  vtkSmartPointer<vtkImageThreshold> Thresholder;
  vtkSmartPointer<vtkUsScanConvert> ScanConverter;
}; 

#endif

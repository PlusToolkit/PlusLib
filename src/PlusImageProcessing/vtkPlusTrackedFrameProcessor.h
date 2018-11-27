/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusTrackedFrameProcessor_h
#define __vtkPlusTrackedFrameProcessor_h

#include "vtkPlusImageProcessingExport.h"

//class igsioTrackedFrame; 
//class vtkIGSIOTrackedFrameList;
//class vtkIGSIOTransformRepository;

/*!
  \class vtkPlusTrackedFrameProcessor 
  \brief Simple interface class to allow running various algorithms that process tracked frame lists
  \ingroup PlusLibImageProcessingAlgo
*/ 
class vtkPlusImageProcessingExport vtkPlusTrackedFrameProcessor : public vtkObject
{
public:
  vtkTypeMacro(vtkPlusTrackedFrameProcessor , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Set the input tracked frame data
    \param frame frame containing image (and optional tracking or other metadata)
  */
  virtual void SetInputFrames(vtkIGSIOTrackedFrameList* inputFrames);
  vtkGetObjectMacro(InputFrames, vtkIGSIOTrackedFrameList);
  
  /*! Set the transform repository (optional, may contain various persistent calibration transforms) */
  virtual void SetTransformRepository(vtkIGSIOTransformRepository* transformRepository);
  vtkGetObjectMacro(TransformRepository, vtkIGSIOTransformRepository);

   /*!
     Perform processing. Results are saved to OutputFrames. It calls ProcessFrame for each input frame. The method can be overriden, for example if frames
     are not processed one by one.
   */
  virtual PlusStatus Update();
 
  /*! Get the processed output data. Perform processing if needed. */
  vtkGetObjectMacro(OutputFrames, vtkIGSIOTrackedFrameList);

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* processingElement); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* processingElement); 
  
  /*! Get the tag name of the configuration element */
  static const char* GetTagName() { return "Processor"; };
  
  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() = 0;

protected:
  vtkPlusTrackedFrameProcessor();
  virtual ~vtkPlusTrackedFrameProcessor();

  /*!
    Default Update() method calls this processing function for each frame. Typically this method should be overridden in derived classes.
    Both input and output frames are already allocated by the caller method.
  */
  virtual PlusStatus ProcessFrame(igsioTrackedFrame* inputFrame, igsioTrackedFrame* outputFrame) = 0;

  vtkIGSIOTrackedFrameList* InputFrames;
  vtkIGSIOTransformRepository *TransformRepository;
  vtkIGSIOTrackedFrameList* OutputFrames;
}; 

#endif

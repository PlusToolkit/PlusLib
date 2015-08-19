/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkTrackedFrameProcessor_h
#define __vtkTrackedFrameProcessor_h

#include "vtkImageProcessingAlgoExport.h"

class TrackedFrame;
class vtkTrackedFrameList;
class vtkTransformRepository;

/*!
  \class vtkTrackedFrameProcessor 
  \brief Simple interface class to allow running various algorithms that process tracked frame lists
  \ingroup PlusLibImageProcessingAlgo
*/ 
class vtkImageProcessingAlgoExport vtkTrackedFrameProcessor : public vtkObject
{
public:
  vtkTypeMacro(vtkTrackedFrameProcessor , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Set the input tracked frame data
    \param frame frame containing image (and optional tracking or other metadata)
  */
  virtual void SetInputFrames(vtkTrackedFrameList* inputFrames);
  vtkGetObjectMacro(InputFrames, vtkTrackedFrameList);
  
  /*! Set the transform repository (optional, may contain various persistent calibration transforms) */
  virtual void SetTransformRepository(vtkTransformRepository* transformRepository);
  vtkGetObjectMacro(TransformRepository, vtkTransformRepository);

   /*!
     Perform processing. Results are saved to OutputFrames. It calls ProcessFrame for each input frame. The method can be overriden, for example if frames
     are not processed one by one.
   */
  virtual PlusStatus Update();
 
  /*! Get the processed output data. Perform processing if needed. */
  vtkGetObjectMacro(OutputFrames, vtkTrackedFrameList);

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* processingElement); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* processingElement); 
  
  /*! Get the tag name of the configuration element */
  static const char* GetTagName() { return "Processor"; };
  
  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() = 0;

protected:
  vtkTrackedFrameProcessor();
  virtual ~vtkTrackedFrameProcessor();

  /*!
    Default Update() method calls this processing function for each frame. Typically this method should be overridden in derived classes.
    Both input and output frames are already allocated by the caller method.
  */
  virtual PlusStatus ProcessFrame(TrackedFrame* inputFrame, TrackedFrame* outputFrame);

  vtkTrackedFrameList* InputFrames;
  vtkTransformRepository *TransformRepository;
  vtkTrackedFrameList* OutputFrames;
}; 

#endif

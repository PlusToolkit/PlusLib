/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusImageProcessorVideoSource_h
#define __vtkPlusImageProcessorVideoSource_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include <string>

//class vtkIGSIOTransformRepository;
class vtkPlusTrackedFrameProcessor;

/*!
\class vtkPlusImageProcessorVideoSource 
\brief Virtual device that performs real-time image processing on the input channel

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusImageProcessorVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusImageProcessorVideoSource *New();
  vtkTypeMacro(vtkPlusImageProcessorVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus InternalUpdate();

  virtual PlusStatus NotifyConfigured();
  
  /*! Enables processing frames. It can be used for pausing the processing. */
  vtkGetMacro(EnableProcessing, bool);
  void SetEnableProcessing(bool aValue);

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

protected:
  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  vtkPlusImageProcessorVideoSource();
  virtual ~vtkPlusImageProcessorVideoSource();

  double LastProcessedInputDataTimestamp;

  bool EnableProcessing;

  /*!
    This repository stores all fixed (persistent) transforms, such as calibration matrices.
    It is initialized once, ReadConfig and not updated if calibration transforms are changed in the application's global transform repository.
  */
  vtkIGSIOTransformRepository* TransformRepository;

  /*! Mutex instance simultaneous access of the processing algorithm (writer may be accessed from command processing thread and also the internal update thread) */ 
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection> ProcessingAlgorithmAccessMutex;

  vtkPlusLogger::LogLevelType GracePeriodLogLevel;

  vtkPlusTrackedFrameProcessor* ProcessorAlgorithm;

private:
  vtkPlusImageProcessorVideoSource(const vtkPlusImageProcessorVideoSource&);  // Not implemented.
  void operator=(const vtkPlusImageProcessorVideoSource&);  // Not implemented. 
};

#endif

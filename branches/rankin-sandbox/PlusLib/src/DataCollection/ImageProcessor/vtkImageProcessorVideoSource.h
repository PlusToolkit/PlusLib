/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkImageProcessorVideoSource_h
#define __vtkImageProcessorVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include <string>

class vtkTransformRepository;
class vtkTrackedFrameProcessor;

/*!
\class vtkImageProcessorVideoSource 
\brief Virtual device that performs real-time image processing on the input channel

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkImageProcessorVideoSource : public vtkPlusDevice
{
public:
  static vtkImageProcessorVideoSource *New();
  vtkTypeMacro(vtkImageProcessorVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

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

  vtkImageProcessorVideoSource();
  virtual ~vtkImageProcessorVideoSource();

  double LastProcessedInputDataTimestamp;

  bool EnableProcessing;

  /*!
    This repository stores all fixed (persistent) transforms, such as calibration matrices.
    It is initialized once, ReadConfig and not updated if calibration transforms are changed in the application's global transform repository.
  */
  vtkTransformRepository* TransformRepository;

  /*! Mutex instance simultaneous access of the processing algorithm (writer may be accessed from command processing thread and also the internal update thread) */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> ProcessingAlgorithmAccessMutex;

  vtkPlusLogger::LogLevelType GracePeriodLogLevel;

  vtkTrackedFrameProcessor* ProcessorAlgorithm;

private:
  vtkImageProcessorVideoSource(const vtkImageProcessorVideoSource&);  // Not implemented.
  void operator=(const vtkImageProcessorVideoSource&);  // Not implemented. 
};

#endif

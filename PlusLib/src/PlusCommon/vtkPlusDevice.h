/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDevice_h
#define __vtkPlusDevice_h

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h"

#include <string>

class vtkMultiThreader;
class vtkXMLDataElement;

/*!
\class vtkPlusDevice 
\brief Abstract interface for tracker and video devices

vtkPlusDevice is an abstract VTK interface to real-time tracking and imaging
systems.  Derived classes should override the Connect(), Disconnect(), 
GetSdkVersion(), ReadConfiguration(), WriteConfiguration() methods.

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusDevice : public vtkImageAlgorithm
{
public:
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Hardware device SDK version. This method should be overridden in subclasses. */
  virtual std::string GetSdkVersion();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*) = 0;

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*) = 0;

  /*! Connect to device. Connection is needed for recording or single frame grabbing */
  virtual PlusStatus Connect() = 0;

  /*!
    Disconnect from device.
    This method must be called before application exit, or else the
    application might hang during exit.
  */
  virtual PlusStatus Disconnect() = 0;

public:
  /*! Get the acquisition rate */
  vtkGetMacro(AcquisitionRate, double);

  /*! Get whether recording is underway */
  vtkGetMacro(Recording,int);

protected:
  vtkPlusDevice();
  virtual ~vtkPlusDevice();

protected:
  /*! Thread used for acquisition */
  vtkMultiThreader* Threader;

  /*! Recording thread id */
  int ThreadId;

  /*! Acquisition rate */
  double AcquisitionRate;

  /* Flag whether the device is recording */
  int Recording;

private:
  vtkPlusDevice(const vtkPlusDevice&);  // Not implemented.
  void operator=(const vtkPlusDevice&);  // Not implemented. 
};

#endif

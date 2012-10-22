/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDevice_h
#define __vtkPlusDevice_h

#include "PlusConfigure.h"
#include "vtkAlgorithm.h"

#include "vtkTrackedFrameList.h"

#include <string>

class vtkMultiThreader;
class vtkXMLDataElement;

/*! Flags for tool statuses */
enum ToolStatus 
{
  TOOL_OK,			      /*!< Tool OK */
  TOOL_MISSING,       /*!< Tool or tool port is not available */
  TOOL_OUT_OF_VIEW,   /*!< Cannot obtain transform for tool */
  TOOL_OUT_OF_VOLUME, /*!< Tool is not within the sweet spot of system */
  TOOL_SWITCH1_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_SWITCH2_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_SWITCH3_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_REQ_TIMEOUT,   /*!< Request timeout status */
  TOOL_INVALID        /*!< Invalid tool status */
};

/*!
\class vtkPlusDevice 
\brief Abstract interface for tracker and video devices

vtkPlusDevice is an abstract VTK interface to real-time tracking and imaging
systems.  Derived classes should override the Connect(), Disconnect(), 
GetSdkVersion(), ReadConfiguration(), WriteConfiguration() methods.

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusDevice : public vtkAlgorithm
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

  /*!
    Get tracked frame containing the transform(s) or the
    image(s) acquired from the device at a specific timestamp
  */
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame *trackedFrame) = 0;

  /*!
    Return whether or not the device can be reset
  */
  virtual bool IsResettable();

  /*!
    Reset the device
  */
  virtual PlusStatus Reset();

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

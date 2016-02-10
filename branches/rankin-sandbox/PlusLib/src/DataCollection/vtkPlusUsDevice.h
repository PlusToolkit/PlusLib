/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsDevice_h
#define __vtkPlusUsDevice_h

#include "PlusCommon.h"
#include "PlusConfigure.h"
#include "vtkDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkUSImagingParameters.h"

/*!
\class vtkPlusUsDevice 
\brief Abstract interface for ultrasound video devices

vtkPlusUsDevice is an abstract VTK interface to ultrasound imaging
systems.  Derived classes should override the SetNewImagingParametersDevice() method.

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkPlusUsDevice : public vtkPlusDevice
{
   
public:
  static vtkPlusUsDevice* New();
  vtkTypeMacro(vtkPlusUsDevice, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*!
  Copy the new imaging parameters into the current parameter set
  It is up to subclasses to take the new imaging parameter set and apply it to their respective devices
  /param newImagingParameters class containing the new ultrasound imaging parameters
  */
  virtual PlusStatus SetNewImagingParameters(const vtkUsImagingParameters& newImagingParameters);

  /*!
  This function can be called to add a video item to all video data sources
  */
  PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const PlusVideoFrame& frame, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, const TrackedFrame::FieldMapType* customFields = NULL);

  /*!
  This function can be called to add a video item to the specified video data sources
  */
  PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const int frameSizeInPx[3], 
    PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, const TrackedFrame::FieldMapType* customFields= NULL);

protected:
  vtkPlusUsDevice();
  virtual ~vtkPlusUsDevice();

  /// Store the requested imaging parameters
  vtkUsImagingParameters* RequestedImagingParameters;
  /// Store the current imaging parameters
  vtkUsImagingParameters* CurrentImagingParameters;

private:
  vtkPlusUsDevice(const vtkPlusUsDevice&);  // Not implemented.
  void operator=(const vtkPlusUsDevice&);  // Not implemented. 
};

#endif

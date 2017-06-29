/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsDevice_h
#define __vtkPlusUsDevice_h

#include "PlusCommon.h"
#include "PlusConfigure.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

class vtkPlusUsImagingParameters;
class vtkPlusChannel;

/*!
\class vtkPlusUsDevice 
\brief Abstract interface for ultrasound video devices

vtkPlusUsDevice is an abstract VTK interface to ultrasound imaging
systems.  Derived classes should override the SetNewImagingParametersDevice() method.

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusUsDevice : public vtkPlusDevice
{
   
public:
  static vtkPlusUsDevice* New();
  vtkTypeMacro(vtkPlusUsDevice, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus InternalUpdate();

  /*!
  Perform any completion tasks once configured
  */
  virtual PlusStatus NotifyConfigured();

  /*!
  Copy the new imaging parameters into the current parameter set
  It is up to subclasses to take the new imaging parameter set and apply it to their respective devices
  /param newImagingParameters class containing the new ultrasound imaging parameters
  */
  virtual PlusStatus SetNewImagingParameters(const vtkPlusUsImagingParameters& newImagingParameters);

  /*!
  This function can be called to add a video item to all video data sources
  */
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const PlusVideoFrame& frame, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, const PlusTrackedFrame::FieldMapType* customFields = NULL);

  /*!
  This function can be called to add a video item to the specified video data sources
  */
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const int frameSizeInPx[3], 
    PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, const PlusTrackedFrame::FieldMapType* customFields= NULL);
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const unsigned int frameSizeInPx[3],
    PlusCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp = UNDEFINED_TIMESTAMP,
    double filteredTimestamp = UNDEFINED_TIMESTAMP, const PlusTrackedFrame::FieldMapType* customFields = NULL);

  /*!
    If non-NULL then ImageToTransducer transform is added as a custom field to the image data with the specified name.
    The Transducer coordinate system origin is in the center of the transducer crystal array,
    x axis direction is towards marked side, y axis direction is towards sound propagation direction,
    and z direction is cross product of x and y, unit is mm.
  */
  vtkGetStringMacro(TextRecognizerInputChannelName);
  vtkSetStringMacro(TextRecognizerInputChannelName);

  /*!
    Get current imaging parameters
  */
  vtkGetObjectMacro(CurrentImagingParameters, vtkPlusUsImagingParameters);

protected:
  /*!
  Set changed imaging parameter to device
  */
  virtual PlusStatus RequestImagingParameterChange() { return PLUS_FAIL; };

  void CalculateImageToTransducer(PlusTrackedFrame::FieldMapType& customFields);

  vtkPlusUsDevice();
  virtual ~vtkPlusUsDevice();

protected:
  /// Store the requested imaging parameters
  vtkPlusUsImagingParameters* RequestedImagingParameters;
  /// Store the current imaging parameters
  vtkPlusUsImagingParameters* CurrentImagingParameters;

  /// Values used in calculation of image to transducer matrix
  double CurrentPixelSpacingMm[3];
  /// Values used in calculation of image to transducer matrix
  int CurrentTransducerOriginPixels[3];

  /// Channel to retrieve parameters from
  vtkPlusChannel* InputChannel;

  /// Container to hold values retrieved from the input
  PlusTrackedFrame::FieldMapType FrameFields;

  PlusTransformName ImageToTransducerTransform;
  char* TextRecognizerInputChannelName;

private:
  vtkPlusUsDevice(const vtkPlusUsDevice&);  // Not implemented.
  void operator=(const vtkPlusUsDevice&);  // Not implemented. 
};

#endif

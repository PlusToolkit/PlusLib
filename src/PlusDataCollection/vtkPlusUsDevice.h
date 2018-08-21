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
#include <igtlioUsSectorDefinitions.h>

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

  /*! Perform any completion tasks once configured */
  virtual PlusStatus NotifyConfigured() override;

  /*!
  Copy the new imaging parameters into the current parameter set
  It is up to subclasses to take the new imaging parameter set and apply it to their respective devices
  /param newImagingParameters class containing the new ultrasound imaging parameters
  */
  virtual PlusStatus SetNewImagingParameters(const vtkPlusUsImagingParameters& newImagingParameters);

  /*!
  This function can be called to add a video item to all video data sources
  */
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const PlusVideoFrame& frame, long frameNumber, double unfilteredTimestamp = UNDEFINED_TIMESTAMP,
      double filteredTimestamp = UNDEFINED_TIMESTAMP, const PlusTrackedFrame::FieldMapType* customFields = NULL) override;

  /*!
  This function can be called to add a video item to the specified video data sources
  */
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const FrameSizeType& frameSizeInPx,
      PlusCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp = UNDEFINED_TIMESTAMP,
      double filteredTimestamp = UNDEFINED_TIMESTAMP, const PlusTrackedFrame::FieldMapType* customFields = NULL) override;

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
  vtkGetObjectMacro(ImagingParameters, vtkPlusUsImagingParameters);

  // Virtual functions for creating the OpenIGTLinkIO ultrasound parameters.
  // Implement these in all US devices that should support ultrasound sector information

  /*! Get probe type. */
  virtual IGTLIO_PROBE_TYPE GetProbeType() { return UNKNOWN; }

  /*! Sector origin relative to upper left corner of image in pixels */
  virtual std::vector<double> CalculateOrigin() { return std::vector<double>(); }

  /*! Probe sector angles relative to down, in radians.
   *  2 angles for 2D, and 4 for 3D probes.
   * For regular imaging with linear probes these will be 0 */
  virtual std::vector<double> CalculateAngles() { return std::vector<double>(); }

  /*! Boundaries to cut away areas outside the US sector, in pixels.
   * 4 for 2D, and 6 for 3D. */
  virtual std::vector<double> CalculateBoundingBox() { return std::vector<double>(); }

  /*! Start, stop depth for the imaging, in mm. */
  virtual std::vector<double> CalculateDepths() { return std::vector<double>(); }

  /*! Width of linear probe. */
  virtual  double CalculateLinearWidth() { return 0; }

protected:
  /*! Set changed imaging parameter to device */
  virtual PlusStatus RequestImagingParameterChange() { return PLUS_FAIL; };

  void CalculateImageToTransducer(PlusTrackedFrame::FieldMapType& customFields);

  vtkPlusUsDevice();
  virtual ~vtkPlusUsDevice();

protected:
  /// Store the current imaging parameters
  vtkPlusUsImagingParameters* ImagingParameters;

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

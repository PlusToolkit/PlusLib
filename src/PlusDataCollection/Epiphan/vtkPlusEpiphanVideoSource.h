/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusEpiphanVideoSource_h
#define __vtkPlusEpiphanVideoSource_h

// Local includes
#include "igsioCommon.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// Epiphan includes
#include <v2u_defs.h>

/*!
  \class vtkPlusEpiphanVideoSource
  \brief Class for providing video input interfaces between VTK and Epiphan frame grabber device
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusEpiphanVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusEpiphanVideoSource* New();
  vtkTypeMacro(vtkPlusEpiphanVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const;

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*!
    Set the Epiphan device location. If no location parameter is specified then the device is attempted to be detected automatically.
    Format of the location string:
    local:[SERIAL] Specifies a local frame grabber. Optionally the serial number can be specified.
    net:[ADDRESS[:PORT]] Specifies a network frame grabber at the specified address/port. If no address is specified, then a random network frame grabber on the local subnet may be found.
    sn:SERIAL Specifies a local or network frame grabber with the specified serial number. Checks the local framegrabbers first.
    id:INDEX Specifies a local frame grabber with the specified index.
  */
  vtkSetStdStringMacro(GrabberLocation);
  /*! Get the Epiphan device location */
  vtkGetStdStringMacro(GrabberLocation);

  /*!
    Set the clip rectangle size to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the values are ignored and the whole frame is captured.
    Width of the ClipRectangle typically have to be a multiple of 4.
  */
  void SetClipRectangleSize(const std::array<int, 3>& size);
  /*!
    Get the clip rectangle size to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the values are ignored and the whole frame is captured.
  */
  std::array<int, 3> GetClipRectangleSize() const;

  /*!
    Set the clip rectangle origin to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the whole frame is captured.
  */
  void SetClipRectangleOrigin(const std::array<int, 3>& origin);
  /*!
    Get the clip rectangle origin to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the whole frame is captured.
  */
  std::array<int, 3> GetClipRectangleOrigin() const;

  /*!
    Perform any completion tasks once configured
  */
  virtual PlusStatus NotifyConfigured();

protected:
  /*! Constructor */
  vtkPlusEpiphanVideoSource();
  /*! Destructor */
  ~vtkPlusEpiphanVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*! The internal function which actually does the grab.  */
  PlusStatus InternalUpdate();

  vtkSetStdStringMacro(ScaleMode);
  vtkSetStdStringMacro(RotationMode);

protected:
  std::array<int, 3> ClipRectangleOrigin;   // Crop rectangle origin for the grabber (in pixels, done in hardware)
  std::array<int, 3> ClipRectangleSize;     // Crop rectangle size for the grabber (in pixels, done in hardware). If it is (0,0) then the whole frame will be captured.
  std::string GrabberLocation;              // String to specify the framegrabber to connect to (auto-detection is attempted if unspecified)
  void* FrameGrabber;                       // Epiphan Pointer to the grabber
  FrameSizeType FrameSize;                  // Frame size of the captured image, third dimension is set to 1
  US_IMAGE_TYPE CaptureImageType;           // The type of image to capture from the hardware
  V2URect* CropRectangle;                   // Dimensions to request from framegrabber

  std::string RotationMode;
  V2URotationMode Rotation;                 // Rotation of the acquired image
  std::string ScaleMode;
  V2UScaleMode Scale;                       // Scaling of the acquired image

private:
  vtkPlusEpiphanVideoSource(const vtkPlusEpiphanVideoSource&);  // Not implemented.
  void operator=(const vtkPlusEpiphanVideoSource&);  // Not implemented.
};

#endif
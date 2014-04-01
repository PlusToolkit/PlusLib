/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkEpiphanVideoSource_h
#define __vtkEpiphanVideoSource_h

#include "vtkPlusDevice.h"

/*!
  \class vtkEpiphanVideoSource 
  \brief Class for providing video input interfaces between VTK and Epiphan frame grabber device
  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkEpiphanVideoSource : public vtkPlusDevice
{
public:
  static vtkEpiphanVideoSource *New();
  vtkTypeRevisionMacro(vtkEpiphanVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  enum VideoFormatType
  {
    VIDEO_FORMAT_UNKNOWN,
    VIDEO_FORMAT_RGB8,
    VIDEO_FORMAT_Y8,
    VIDEO_FORMAT_RGB24,
  };

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);
  /*! Set the Epiphan device video format (e.g. "VIDEO_FORMAT_Y8" ) */
  vtkSetMacro(VideoFormat,VideoFormatType); 
  /*! Get the Epiphan device video format (e.g. "VIDEO_FORMAT_Y8" ) */
  vtkGetMacro(VideoFormat,VideoFormatType);  

  /*!
    Set the Epiphan device location. If no location parameter is specified then the device is attempted to be detected automatically.
    Format of the location string:
    local:[SERIAL] Specifies a local frame grabber. Optionally the serial number can be specified.
    net:[ADDRESS[:PORT]] Specifies a network frame grabber at the specified address/port. If no address is specified, then a random network frame grabber on the local subnet may be found.
    sn:SERIAL Specifies a local or network frame grabber with the specified serial number. Checks the local framegrabbers first.
    id:INDEX Specifies a local frame grabber with the specified index.
  */
  vtkSetStringMacro(GrabberLocation); 
  /*! Get the Epiphan device location */
  vtkGetStringMacro(GrabberLocation); 

  /*!
    Set the clip rectangle size to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the values are ignored and the whole frame is captured.
    Width of the ClipRectangle typically have to be a multiple of 4.
  */
  vtkSetVector2Macro(ClipRectangleSize,int);
  /*!
    Get the clip rectangle size to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the values are ignored and the whole frame is captured.
  */
  vtkGetVector2Macro(ClipRectangleSize,int);

  /*!
    Set the clip rectangle origin to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the whole frame is captured.
  */
  vtkSetVector2Macro(ClipRectangleOrigin,int);
  /*!
    Get the clip rectangle origin to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the whole frame is captured.
  */
  vtkGetVector2Macro(ClipRectangleOrigin,int);

protected:
  /*! Constructor */
  vtkEpiphanVideoSource();
  /*! Destructor */
  ~vtkEpiphanVideoSource();

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

  /*! Video format (e.g. Y8) */
  VideoFormatType VideoFormat;

  /*! Crop rectangle origin for the grabber (in pixels) */
  int ClipRectangleOrigin[2];

  /*! Crop rectangle size for the grabber (in pixels). If it is (0,0) then the whole frame will be captured. */
  int ClipRectangleSize[2];

  /*! String to specify the framegrabber to connect to (auto-detection is attempted if unspecified) */
  char* GrabberLocation;

  /*! Epiphan Pointer to the grabber */
  void* FrameGrabber;

  /*! Frame size of the captured image */
  int FrameSize[2];

private:
  vtkEpiphanVideoSource(const vtkEpiphanVideoSource&);  // Not implemented.
  void operator=(const vtkEpiphanVideoSource&);  // Not implemented.
};

#endif






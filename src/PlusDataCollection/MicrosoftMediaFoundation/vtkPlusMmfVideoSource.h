/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/

#ifndef __vtkPlusMmfVideoSource_h
#define __vtkPlusMmfVideoSource_h

// Local includes
//#include "igsioVideoFrame.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkIGSIORecursiveCriticalSection.h"

// VTK includes
#include <vtkSmartPointer.h>

class MmfVideoSourceReader;

/*!
  \class vtkPlusMmfVideoSource
  \brief Microsoft media foundation video digitizer

  Media foundation require Microsoft Windows SDK 7.1 or later. Download <a href="http://www.microsoft.com/en-us/download/details.aspx?id=8279">here</a>

  \sa vtkPlusDevice
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusMmfVideoSource : public vtkPlusDevice
{
  friend class MmfVideoSourceReader;

  struct VideoFormat
  {
    unsigned int DeviceId;
    unsigned int StreamIndex;
    FrameSizeType FrameSize;
    std::wstring PixelFormatName; // e.g., YUY2
  };

public:
  static vtkPlusMmfVideoSource* New();
  vtkTypeMacro(vtkPlusMmfVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual void SetRequestedDeviceId(unsigned int deviceId);
  virtual void SetRequestedVideoFormat(const std::wstring& pixelFormatName);
  virtual void SetRequestedStreamIndex(unsigned int streamIndex);
  virtual void SetRequestedFrameSize(const FrameSizeType& frameSize);

  std::wstring GetRequestedDeviceName();
  std::wstring GetActiveDeviceName();
  void GetListOfCaptureVideoFormats(std::vector<std::wstring>& videoModes, unsigned int deviceId);
  void LogListOfCaptureVideoFormats(unsigned int deviceId);
  void GetListOfCaptureDevices(std::vector<std::wstring>& deviceNames);
  void LogListOfCaptureDevices();

  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* xmlElement);
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* xmlElement);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

protected:
  /*! Constructor */
  vtkPlusMmfVideoSource();
  /*! Destructor */
  ~vtkPlusMmfVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  PlusStatus UpdateFrameSize();

  PlusStatus AddFrame(unsigned char* bufferData, DWORD bufferSize);

  std::wstring GetCaptureDeviceName(unsigned int deviceId);

  int FrameIndex;

  vtkSmartPointer<vtkIGSIORecursiveCriticalSection> Mutex;
  igsioVideoFrame UncompressedVideoFrame;
  VideoFormat RequestedVideoFormat;
  VideoFormat ActiveVideoFormat;

  MmfVideoSourceReader* MmfSourceReader;
private:
  vtkPlusMmfVideoSource(const vtkPlusMmfVideoSource&);  // Not implemented.
  void operator=(const vtkPlusMmfVideoSource&);  // Not implemented.
};

#endif

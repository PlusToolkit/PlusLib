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

#ifndef __vtkMmfVideoSource_h
#define __vtkMmfVideoSource_h

#include "PlusVideoFrame.h"
#include "vtkPlusDevice.h"

// Media foundation includes - require Microsoft Windows SDK 7.1 or later.
// Download from: http://www.microsoft.com/en-us/download/details.aspx?id=8279
#include <Mfidl.h>
#include <Mfreadwrite.h>

/*!
  \class vtkMmfVideoSource 
  \brief Microsoft media foundation video digitizer

  \sa vtkPlusDevice
  \ingroup PlusLibDataCollection
*/ 
class VTK_EXPORT vtkMmfVideoSource : public vtkPlusDevice, public IMFSourceReaderCallback
{
  class VideoFormat
  {
  public:
    GUID SourcePixelFormat;
    int width;
    int height;
  };

public:
  static vtkMmfVideoSource *New();
  vtkTypeRevisionMacro(vtkMmfVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Bring up a modal dialog box for video format selection. */
  PlusStatus VideoFormatDialog();

  /*! Bring up a modal dialog box for video input selection. */
  PlusStatus VideoSourceDialog();

  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* xmlElement);
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* xmlElement);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

protected:
  /*! Constructor */
  vtkMmfVideoSource();
  /*! Destructor */
  ~vtkMmfVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  PlusStatus ConfigureDecoder();

  PlusStatus UpdateFrameSize();

  int FrameIndex;

  IMFActivate** CaptureDevices;
  IMFAttributes* CaptureAttributes;
  IMFMediaSource* CaptureSource;
  IMFSourceReader* CaptureSourceReader;
  UINT32 DeviceCount;
  UINT32 ImageStride;
  vtkSmartPointer<vtkRecursiveCriticalSection> Mutex;
  PlusVideoFrame UncompressedVideoFrame;
  VideoFormat RequestedVideoFormat;
  VideoFormat ActiveVideoFormat;

private:
  vtkMmfVideoSource(const vtkMmfVideoSource&);  // Not implemented.
  void operator=(const vtkMmfVideoSource&);  // Not implemented.

 //------- IMFSourceReaderCallback functions ----------------------
public:
  // IUnknown methods
  STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();
  STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *);
  STDMETHODIMP OnFlush(DWORD);
  STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample);
protected:
  long RefCount;
};

#endif

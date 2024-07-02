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
Authors include: Danielle Pace, Francisco Marcano (University of La Laguna)
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/

#ifndef __vtkPlusDAQMMFCombinedVideo_h
#define __vtkPlusDAQMMFCombinedVideo_h

// Local includes
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkIGSIORecursiveCriticalSection.h"

// VTK includes
#include <vtkSmartPointer.h>

class DAQMmfVideoSourceReader;

/*!
  \class vtkPlusDAQMMFCombinedVideo
  \sa vtkPlusDevice
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusDAQMMFCombinedVideo : public vtkPlusDevice
{
  friend class DAQMmfVideoSourceReader;

  struct VideoFormat
  {
    unsigned int DeviceId;
    unsigned int StreamIndex;
    FrameSizeType FrameSize;
    std::wstring PixelFormatName; // e.g., YUY2
  };

public:
  static vtkPlusDAQMMFCombinedVideo* New();
  vtkTypeMacro(vtkPlusDAQMMFCombinedVideo, vtkPlusDevice);
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

  vtkGetStdStringMacro(MMFDataSourceName);
  vtkGetStdStringMacro(DAQDataSourceName);

protected:
  vtkSetStdStringMacro(MMFDataSourceName);
  vtkSetStdStringMacro(DAQDataSourceName);


  /*! Constructor */
  vtkPlusDAQMMFCombinedVideo();
  /*! Destructor */
  ~vtkPlusDAQMMFCombinedVideo();

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

  DAQMmfVideoSourceReader* MmfSourceReader;
private:
  vtkPlusDAQMMFCombinedVideo(const vtkPlusDAQMMFCombinedVideo&);  // Not implemented.
  void operator=(const vtkPlusDAQMMFCombinedVideo&);  // Not implemented.

  typedef unsigned short CAMERADATATYPE_t;
  enum COLORDEPTH_E { COLORDEPTH_8 = 8, COLORDEPTH_16 = 16, COLORDEPTH_32 = 32, COLORDEPTH_64 = 64 } colorDepth;
  enum DATAMODE_E { DATAMODE_8 = 0, DATAMODE_16 = 1, DATAMODE_32 = 2, DATAMODE_64 = 3 } m_dataMode;
  enum CAMERAMODE_E { CAMERAMODE_SCAN, CAMERAMODE_LINE } cameraMode;
  bool m_deviceRunning;
  CAMERADATATYPE_t* pImgBuf;
  unsigned char* pImgBufAux;
  DWORD m_width;
  DWORD m_height;
  DWORD m_nwidth;
  DWORD m_nheight;
  DWORD m_maxBuffSize;
  DWORD m_dwCharCount;
  int m_nbytesMode;
  double m_currentTime = UNDEFINED_TIMESTAMP;
  
  std::string MMFDataSourceName;
  std::string DAQDataSourceName;

  vtkPlusDataSource* MMFDataSource;
  vtkPlusDataSource* DAQDataSource;

  PlusStatus DAQ_GetFrame();
  PlusStatus DAQ_InternalConnect();
  PlusStatus DAQ_InternalDisconnect();
  PlusStatus DAQ_ReadConfiguration(vtkXMLDataElement* rootConfigElement);
  PlusStatus DAQ_ProcessFrame();
};

#endif

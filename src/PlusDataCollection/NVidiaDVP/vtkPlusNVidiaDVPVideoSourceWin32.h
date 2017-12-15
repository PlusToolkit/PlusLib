/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusNvidiaDVPVideoSource_h
#define __vtkPlusNvidiaDVPVideoSource_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "nvCommon.h"
#include "nvSDIin.h"
#include "nvConfigure.h"

class CNvGpu;

/*!
\class vtkPlusNvidiaDVPVideoSource
\brief Class for providing VTK video input interface from an NVidia digital video platform interface
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusNvidiaDVPVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusNvidiaDVPVideoSource* New();
  vtkTypeMacro(vtkPlusNvidiaDVPVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /// Read configuration from XML data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /// Write configuration to XML data
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /// Respond to the query if this is a tracker or not
  virtual bool IsTracker() const;

  /// Perform any completion tasks once configured
  virtual PlusStatus NotifyConfigured();

  /// Get the field that determines if the frames are copied from the GPU to the CPU to be used downstream
  vtkGetMacro(EnableGPUCPUCopy, bool);

protected:
  /// Set the field that determines if the frames are copied from the GPU to the CPU to be used downstream
  vtkSetMacro(EnableGPUCPUCopy, bool);

  vtkPlusNvidiaDVPVideoSource();
  virtual ~vtkPlusNvidiaDVPVideoSource();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus InternalUpdate();

  HRESULT SetupSDIDevices();
  GLboolean SetupGL();
  GLboolean CleanupGL();
  HRESULT CleanupSDIDevices();
  HRESULT StartSDIPipeline();
  HRESULT StopSDIPipeline();
  GLenum CaptureVideo();
  HRESULT CopyGPUToCPU();
  void Shutdown();

protected:
  HRESULT SetupSDIinGL();
  HRESULT SetupSDIinDevices();
  HRESULT CleanupSDIinGL();

protected:
  CNvGpu* NvGPU;

  /// Windows device context
  HDC HandleDC;

  /// Windows GL context
  HGLRC HandleGLRC;

  /// Options to configure capture
  nvOptions NvOptions;

  /// Capture interface class
  CNvSDIin NvSDIin;

  FrameSizeType VideoSize;

  /// CPU memory storage of image
  unsigned char* CPUFrame;

  GLuint VideoBufferObject[MAX_VIDEO_STREAMS];
  GLuint VideoBufferFormat;
  int NumFrames;

  /// The current frame number
  uint64_t FrameNumber;

  /// Enable copying of frame data from GPU to CPU for broadcasting
  bool EnableGPUCPUCopy;

  /// Data source for CPU output (if enabled)
  vtkPlusDataSource* OutputDataSource;

private:
  vtkPlusNvidiaDVPVideoSource(const vtkPlusNvidiaDVPVideoSource&);   // Not implemented.
  void operator=(const vtkPlusNvidiaDVPVideoSource&);   // Not implemented.
};

#endif


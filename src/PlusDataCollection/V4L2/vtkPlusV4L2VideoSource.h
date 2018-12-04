/*=Plus=header=begin======================================================
 Program: Plus
 Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
 See License.txt for details.
 =========================================================Plus=header=end*/

#ifndef __vtkPlusV4L2VideoSource_h
#define __vtkPlusV4L2VideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// V4L2 includes
#include <linux/videodev2.h>

class vtkPlusDataSource;

/*!
 \class vtkPlusV4L2VideoSource
 \brief Class for interfacing an V4L2 device and recording frames into a Plus buffer

 Requires the PLUS_USE_V4L2 option in CMake.

 \ingroup PlusLibDataCollection
 */

class vtkPlusDataCollectionExport vtkPlusV4L2VideoSource : public vtkPlusDevice
{
  enum V4L2_IO_METHOD
  {
    IO_METHOD_UNKNOWN,
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR
  };

  struct FrameBuffer
  {
    void* start;
    size_t length;
  };

public:
  static vtkPlusV4L2VideoSource* New();
  vtkTypeMacro(vtkPlusV4L2VideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Is this device a tracker */
  bool IsTracker() const
  { return false;}

  /*! Poll the device for new frames */
  PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  vtkSetStdStringMacro(DeviceName);
  vtkGetStdStringMacro(DeviceName);

protected:
  vtkPlusV4L2VideoSource();
  ~vtkPlusV4L2VideoSource();

  PlusStatus ReadFrame(unsigned int& currentBufferIndex, unsigned int& bytesUsed);

  PlusStatus ReadFrameFileDescriptor(unsigned int& currentBufferIndex, unsigned int& bytesUsed);
  PlusStatus ReadFrameMemoryMap(unsigned int& currentBufferIndex, unsigned int& bytesUsed);
  PlusStatus ReadFrameUserPtr(unsigned int& currentBufferIndex, unsigned int& bytesUsed);

  PlusStatus InitRead(unsigned int bufferSize);
  PlusStatus InitMmap();
  PlusStatus InitUserp(unsigned int bufferSize);

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

  virtual PlusStatus InternalStopRecording() VTK_OVERRIDE;
  virtual PlusStatus InternalStartRecording() VTK_OVERRIDE;

  // Conversion methods
  static std::string IOMethodToString(V4L2_IO_METHOD ioMethod);
  static V4L2_IO_METHOD StringToIOMethod(const std::string& method);

  static std::string PixelFormatToString(unsigned int format);
  static unsigned int StringToPixelFormat(const std::string& format);

  static std::string FieldOrderToString(v4l2_field field);
  static v4l2_field StringToFieldOrder(const std::string& field);

protected:
  // Configuration variables
  std::string                         DeviceName;
  V4L2_IO_METHOD                      IOMethod;
  // If not nullptr, override these settings in InternalConnect
  std::shared_ptr<unsigned int>       FormatWidth;
  std::shared_ptr<unsigned int>       FormatHeight;
  std::shared_ptr<unsigned int>       PixelFormat;
  std::shared_ptr<v4l2_field>         FieldOrder;

  // State variables
  int                                 FileDescriptor;
  FrameBuffer*                        FrameBuffers;
  unsigned int                        BufferCount;
  vtkPlusDataSource*                  DataSource;
  igsioTrackedFrame::FieldMapType      FrameFields;
  std::shared_ptr<struct v4l2_format> DeviceFormat;

  // Cached state variable (duplicate of DeviceFormat members, for passing to Plus functions)
  FrameSizeType                       ImageSize;
  uint32_t                            NumberOfScalarComponents; // Calculated from device format in InternalConnect
};

#endif

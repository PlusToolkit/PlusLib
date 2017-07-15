/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusRTSPVideoSource_h
#define __vtkPlusRTSPVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
  \class vtkPlusRTSPVideoSource
  \brief Class for interfacing an RTSP stream (rtsp://127.0.0.1/...) and capturing frames into a Plus buffer

  Requires the PLUS_USE_RTSP_VIDEO option in CMake.
  Requires FFMPEG

  \ingroup PlusLibDataCollection
*/
struct SwsContext;
struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVCodec;
struct AVStream;
struct AVFrame;

class vtkPlusDataCollectionExport vtkPlusRTSPVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusRTSPVideoSource* New();
  vtkTypeMacro(vtkPlusRTSPVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  bool IsTracker() const {return false;}

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  vtkGetStdStringMacro(StreamURL);
  vtkSetStdStringMacro(StreamURL);

protected:
  /*! Constructor */
  vtkPlusRTSPVideoSource();
  /*! Destructor */
  ~vtkPlusRTSPVideoSource();

  /*! Device-specific connect */
  PlusStatus InternalConnect();
  /*! Device-specific disconnect */
  PlusStatus InternalDisconnect();

protected:
  SwsContext*       ImageConvertContext;
  AVFormatContext*  FormatContext;
  AVCodecContext*   CodecContext;
  int               VideoStreamIndex;
  std::string       StreamURL;
  AVPacket*         Packet;
  AVCodec*          Codec;
  AVStream*         Stream;
  AVFormatContext*  OutputContext;
  AVFrame*          FrameYUV;
  uint8_t*          FrameBufferYUV;
  AVFrame*          FrameRGB;
  uint8_t*          FrameBufferRGB;
};

#endif // __vtkPlusRTSPVideoSource_h
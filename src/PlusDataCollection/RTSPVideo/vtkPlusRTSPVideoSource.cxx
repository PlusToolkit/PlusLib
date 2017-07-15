/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusRTSPVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// FFMPEG includes
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusRTSPVideoSource);

//----------------------------------------------------------------------------
vtkPlusRTSPVideoSource::vtkPlusRTSPVideoSource()
  : StreamURL("")
  , ImageConvertContext(nullptr)
  , FormatContext(avformat_alloc_context())
  , CodecContext(nullptr)
  , VideoStreamIndex(-1)
  , Packet(nullptr)
  , Codec(nullptr)
  , FrameYUV(nullptr)
  , FrameBufferYUV(nullptr)
  , FrameRGB(nullptr)
  , FrameBufferRGB(nullptr)
  , Stream(nullptr)
{
  // TODO : move to static instance so this is only ever called once per executable
  av_register_all();

  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusRTSPVideoSource::~vtkPlusRTSPVideoSource()
{
}

//----------------------------------------------------------------------------
void vtkPlusRTSPVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "StreamURL: " << this->StreamURL << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusRTSPVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(StreamURL, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("StreamURL", this->StreamURL.c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::FreezeDevice(bool freeze)
{

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::InternalConnect()
{
  avformat_network_init();

  // Open RTSP
  if (avformat_open_input(&this->FormatContext, this->StreamURL.c_str(), NULL, NULL) != 0)
  {
    LOG_ERROR("Unable to connect to stream at " << this->StreamURL);
    avformat_network_deinit();
    return PLUS_FAIL;
  }

  if (avformat_find_stream_info(this->FormatContext, NULL) < 0)
  {
    LOG_ERROR("Unable to retrieve stream info.");
    avformat_network_deinit();
    return PLUS_FAIL;
  }

  // Search for video stream
  for (unsigned int i = 0; i < this->FormatContext->nb_streams; i++)
  {
    if (this->FormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      this->VideoStreamIndex = i;
      break;
    }
  }

  this->Packet = av_packet_alloc();
  av_init_packet(this->Packet);

  // Start reading packets from stream and write them to file
  av_read_play(this->FormatContext);    // Play RTSP

  // Get the codec
  this->Codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!this->Codec)
  {
    LOG_ERROR("Unable to locate codec for h264.");
    av_packet_free(&this->Packet);
    avformat_network_deinit();
    return PLUS_FAIL;
  }

  this->OutputContext = avformat_alloc_context();
  this->CodecContext = avcodec_alloc_context3(this->Codec);

  avcodec_get_context_defaults3(this->CodecContext, this->Codec);
  avcodec_copy_context(this->CodecContext, this->FormatContext->streams[this->VideoStreamIndex]->codec);

  if (avcodec_open2(this->CodecContext, this->Codec, NULL) < 0)
  {
    LOG_ERROR("Unable to open the codec context.");
    avcodec_free_context(&this->CodecContext);
    av_packet_free(&this->Packet);
    avformat_network_deinit();
    return PLUS_FAIL;
  }

  this->ImageConvertContext = sws_getContext(this->CodecContext->width,
                              this->CodecContext->height,
                              this->CodecContext->pix_fmt,
                              this->CodecContext->width,
                              this->CodecContext->height,
                              AV_PIX_FMT_RGB24,
                              SWS_BICUBIC,
                              NULL,
                              NULL,
                              NULL);

  this->FrameYUV = av_frame_alloc();
  this->FrameRGB = av_frame_alloc();

  this->FrameBufferYUV = (uint8_t*)(av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,
                                    this->CodecContext->width,
                                    this->CodecContext->height)));
  this->FrameBufferRGB = (uint8_t*)(av_malloc(avpicture_get_size(AV_PIX_FMT_RGB24,
                                    this->CodecContext->width,
                                    this->CodecContext->height)));

  avpicture_fill((AVPicture*)this->FrameYUV,
                 this->FrameBufferYUV,
                 AV_PIX_FMT_YUV420P,
                 this->CodecContext->width,
                 this->CodecContext->height);
  avpicture_fill((AVPicture*)this->FrameRGB,
                 this->FrameBufferRGB,
                 AV_PIX_FMT_RGB24,
                 this->CodecContext->width,
                 this->CodecContext->height);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::InternalDisconnect()
{
  av_read_pause(this->FormatContext);


  av_free(this->FrameBufferYUV);
  av_free(this->FrameBufferRGB);
  av_frame_free(&this->FrameYUV);
  av_frame_free(&this->FrameRGB);
  avcodec_free_context(&this->CodecContext);

  av_packet_free(&this->Packet);
  avformat_network_deinit();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusRTSPVideoSource::InternalUpdate");

  // Capture one frame from the RTSP device
  av_read_frame(this->FormatContext, this->Packet);

  if (this->Packet->stream_index != this->VideoStreamIndex)
  {
    return PLUS_SUCCESS;
  }

  if (this->Stream == NULL)
  {
    this->Stream = avformat_new_stream(this->OutputContext, this->FormatContext->streams[this->VideoStreamIndex]->codec->codec);
    avcodec_copy_context(this->Stream->codec, this->FormatContext->streams[this->VideoStreamIndex]->codec);
    this->Stream->sample_aspect_ratio = this->FormatContext->streams[this->VideoStreamIndex]->codec->sample_aspect_ratio;
  }

  this->Packet->stream_index = this->Stream->id;
  int check;
  int result = avcodec_decode_video2(this->CodecContext, this->FrameYUV, &check, this->Packet);

  av_free_packet(this->Packet);
  av_init_packet(this->Packet);

  if (check > 0)
  {
    sws_scale(this->ImageConvertContext,
              this->FrameYUV->data,
              this->FrameYUV->linesize,
              0,
              this->CodecContext->height,
              this->FrameRGB->data,
              this->FrameRGB->linesize);

    // Retrieve the video source in RTSP device
    vtkPlusDataSource* aSource = NULL;
    if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to retrieve the video source in the RTSP device.");
      return PLUS_FAIL;
    }

    // If the buffer is empty, set the pixel type and frame size to the first received properties
    if (aSource->GetNumberOfItems() == 0)
    {
      LOG_DEBUG("Set up image buffer for RTSP");
      aSource->SetPixelType(VTK_UNSIGNED_CHAR);
      aSource->SetImageType(US_IMG_RGB_COLOR);
      aSource->SetNumberOfScalarComponents(3);
      aSource->SetInputFrameSize(this->CodecContext->width, this->CodecContext->height, 1);
    }


    // Add the frame to the stream buffer
    int frameSize[3] = { this->CodecContext->width, this->CodecContext->height, 1 };
    if (aSource->AddItem(this->FrameRGB->data, US_IMG_ORIENT_MF, frameSize, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }

    this->FrameNumber++;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusRTSPVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusRTSPVideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
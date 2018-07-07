/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusIntelRealSense.h"

// Local includes
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// IntelRealSense includes
#include <rs.hpp>

// stl includes
#include <vector>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusIntelRealSense);
#define REALSENSE_DEFAULT_FRAME_WIDTH 640
#define REALSENSE_DEFAULT_FRAME_HEIGHT 480
#define REALSENSE_DEFAULT_FRAME_RATE 30

//----------------------------------------------------------------------------
class vtkPlusIntelRealSense::vtkInternal
{
public:
  vtkPlusIntelRealSense * External;

  vtkInternal(vtkPlusIntelRealSense* external)
    : External(external)
    , Align(nullptr)
  {
  }

  virtual ~vtkInternal()
  {
  }

  struct RSFrameConfig
  {
    RSFrameConfig(rs2_stream st, std::string sn, int width, int height, int fr)
    {
      this->StreamType = st;
      this->SourceName = sn;
      this->Height = height;
      this->Width = width;
      this->FrameRate = fr;
    }
    rs2_stream StreamType;
    std::string SourceName;
    vtkPlusDataSource* Source;
    unsigned int Height;
    unsigned int Width;
    unsigned int FrameRate;
  };

  // Configuration parameters
  bool UseRealSenseColorizer = false;
  bool AlignDepthStream = false;

  // Frame setup for RGB & depth
  std::vector<RSFrameConfig> VideoSources;

  PlusStatus SetDepthScaleToMm(rs2::device dev);
  float DepthScaleToMm;
  
  rs2::config Config;
  rs2::pipeline Pipe;
  rs2::pipeline_profile Profile;

  // Alignment
  PlusStatus SetStreamToAlign(const std::vector<rs2::stream_profile>& streams);
  rs2_stream AlignTo;
  rs2::align* Align; // rs2::align doesn't have a default constructor, so we must use a pointer
};

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::vtkInternal::SetDepthScaleToMm(rs2::device dev)
{
  // Go over the device's sensors
  for (rs2::sensor& sensor : dev.query_sensors())
  {
    // Check if the sensor if a depth sensor
    if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
    {
      this->DepthScaleToMm = dpt.get_depth_scale();
      return PLUS_SUCCESS;
    }
  }
  LOG_ERROR("Failed to get depth scale from Intel RealSense.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::vtkInternal::SetStreamToAlign(const std::vector<rs2::stream_profile>& streams)
{
  //Given a vector of streams, we try to find a depth stream and another stream to align depth with.
  //We prioritize color streams to make the view look better.
  //If color is not available, we take another stream that (other than depth)
  rs2_stream align_to = RS2_STREAM_ANY;
  bool depth_stream_found = false;
  bool color_stream_found = false;
  for (rs2::stream_profile sp : streams)
  {
    rs2_stream profile_stream = sp.stream_type();
    if (profile_stream != RS2_STREAM_DEPTH)
    {
      if (!color_stream_found)         //Prefer color
        align_to = profile_stream;

      if (profile_stream == RS2_STREAM_COLOR)
      {
        color_stream_found = true;
      }
    }
    else
    {
      depth_stream_found = true;
    }
  }

  if (!depth_stream_found)
  {
    LOG_ERROR("Failed to align RealSense streams. No Intel RealSense Depth stream available. Please proivde a depth stream, or set AlignDepthStream to FALSE.");
    return PLUS_FAIL;
  }

  if (align_to == RS2_STREAM_ANY)
  {
    LOG_ERROR("Failed to align RealSense streams. No Intel RealSense stream found to align with Depth. Please proivde an RGB stream, or set AlignDepthStream to FALSE");
    return PLUS_FAIL;
  }
  this->AlignTo = align_to;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusIntelRealSense::vtkPlusIntelRealSense()
  : Internal(new vtkInternal(this))
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = REALSENSE_DEFAULT_FRAME_RATE;
  this->AcquisitionRate = REALSENSE_DEFAULT_FRAME_RATE;
}

//----------------------------------------------------------------------------
vtkPlusIntelRealSense::~vtkPlusIntelRealSense()
{
  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusIntelRealSense::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // TODO: Re-implement this without hard-coding
  //os << indent << "Intel RealSense 3d Camera: D415" << std::endl;
  //os << indent << "RgbDataSourceName: " << RgbDataSourceName << std::endl;
  //os << indent << "DepthDataSourceName: " << DepthDataSourceName << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(UseRealSenseColorizer, this->Internal->UseRealSenseColorizer, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(AlignDepthStream, this->Internal->AlignDepthStream, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* dataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(dataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }

    if (dataElement->GetAttribute("Type") != NULL && STRCASECMP(dataElement->GetAttribute("Type"), "Video") == 0)
    {
      // this is a video element
      // get tool ID
      const char* toolId = dataElement->GetAttribute("Id");
      if (toolId == NULL)
      {
        // tool doesn't have ID needed to generate transform
        LOG_ERROR("Failed to initialize IntelRealSense DataSource: Id is missing");
        continue;
      }

      // get Intel RealSense video parameters for this source
      rs2_stream sourceType;
      XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_REQUIRED(FrameType, sourceType, dataElement, "RGB", RS2_STREAM_COLOR, "DEPTH", RS2_STREAM_DEPTH);
      int frameSize[2] = { REALSENSE_DEFAULT_FRAME_WIDTH, REALSENSE_DEFAULT_FRAME_HEIGHT };
      XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 2, FrameSize, frameSize, dataElement);
      int frameRate = REALSENSE_DEFAULT_FRAME_RATE;
      XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, FrameRate, frameRate, dataElement);
      vtkInternal::RSFrameConfig source(sourceType, toolId, frameSize[0], frameSize[1], frameRate);
      this->Internal->VideoSources.push_back(source);
    }
    else
    {
      LOG_ERROR("DataSource with unknown Type.");
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  // write bool  UseRealSenseColorizer
  if (this->Internal->UseRealSenseColorizer)
  {
    deviceConfig->SetAttribute("UseRealSenseColorizer", "TRUE");
  }
  else
  {
    deviceConfig->SetAttribute("UseRealSenseColorizer", "FALSE");
  }

  // write bool AlignDepthStream
  if (this->Internal->AlignDepthStream)
  {
    deviceConfig->SetAttribute("AlignDepthStream", "TRUE");
  }
  else
  {
    deviceConfig->SetAttribute("AlignDepthStream", "FALSE");
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::InternalConnect()
{
  // configure IntelRealSense streams
  this->Internal->Config.disable_all_streams();
  std::vector<vtkInternal::RSFrameConfig>::iterator it;
  for (it = begin(this->Internal->VideoSources); it != end(this->Internal->VideoSources); it++)
  {
    rs2_format format;
    switch (it->StreamType)
    {
    case RS2_STREAM_COLOR:
      format = RS2_FORMAT_RGB8;
      break;
    case RS2_STREAM_DEPTH:
      format = RS2_FORMAT_Z16;
      break;
    }

    // get Plus data source for this stream
    GetVideoSource(it->SourceName.c_str(), it->Source);

    // enable stream on RealSense
    this->Internal->Config.enable_stream(
      it->StreamType,
      it->Width,
      it->Height,
      format,
      it->FrameRate
    );
  }

  // try starting pipeline to check if all stream parameters are OK
  PlusStatus pipeStartResult = PLUS_SUCCESS;
  try
  {
    this->Internal->Profile = this->Internal->Pipe.start(this->Internal->Config);
  }
  catch (rs2::error e)
  {
    LOG_ERROR("Failed to start IntelRealSense streams. Check your RealSense camera supports the stream types, resolutions & frame rates you are trying to use. RealSense API gave the error: '"
      << e.what() << "'");
    pipeStartResult = PLUS_FAIL;
  }

  if (pipeStartResult == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  if (this->Internal->SetDepthScaleToMm(this->Internal->Profile.get_device()) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->Internal->Pipe.stop();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::InternalDisconnect()
{
  delete this->Internal->Align;
  this->Internal->Align = nullptr;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::InternalStartRecording()
{
  this->FrameNumber = 0;

  PlusStatus pipeStartResult = PLUS_SUCCESS;
  try
  {
    this->Internal->Profile = this->Internal->Pipe.start(this->Internal->Config);
  }
  catch (rs2::error e)
  {
    LOG_ERROR("Failed to start IntelRealSense streams. Check your RealSense camera supports the stream types, resolutions & frame rates you are trying to use. RealSense API gave the error: '"
      << e.what() << "'");
    pipeStartResult = PLUS_FAIL;
  }

  if (pipeStartResult == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  if (this->Internal->AlignDepthStream)
  {
    // setup to align depth frame to color
    if (this->Internal->SetStreamToAlign(this->Internal->Profile.get_streams()) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }
    this->Internal->Align = new rs2::align(this->Internal->AlignTo);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::InternalStopRecording()
{
  this->Internal->Pipe.stop();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::NotifyConfigured()
{
  // TODO: Implement some configuration checks here
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSense::InternalUpdate()
{
  // wait for frame
  rs2::frameset frames = this->Internal->Pipe.wait_for_frames();

  // if requested, align depth to color
  if (this->Internal->AlignDepthStream)
  {
    frames = this->Internal->Align->process(frames);
  }
  
  // forward video data to PlusDataSource
  std::vector<vtkInternal::RSFrameConfig>::iterator it;
  for (it = begin(this->Internal->VideoSources); it != end(this->Internal->VideoSources); it++)
  {
    // if source is null, return an error
    if (it->Source == nullptr)
    {
      LOG_WARNING("vtkPlusIntelRealSense::InternalUpdate Unable to grab video source '" << it->SourceName << "'. Skipping frame.");
      return PLUS_FAIL;
    }

    // if this is the first frame, initialize the buffer
    if (it->Source->GetNumberOfItems() == 0 && it->StreamType == RS2_STREAM_COLOR)
    {
      LOG_INFO("setting up color frame");
      it->Source->SetImageType(US_IMG_RGB_COLOR);
      it->Source->SetPixelType(VTK_UNSIGNED_CHAR);
      it->Source->SetNumberOfScalarComponents(3);
      it->Source->SetInputFrameSize(it->Width, it->Height, 1);
    }
    else if (it->Source->GetNumberOfItems() == 0 && it->StreamType == RS2_STREAM_DEPTH)
    {
      if (this->Internal->UseRealSenseColorizer)
      {
        // depth output is RGB from rs2::colorizer
        it->Source->SetImageType(US_IMG_RGB_COLOR);
        it->Source->SetPixelType(VTK_UNSIGNED_CHAR);
        it->Source->SetNumberOfScalarComponents(3);
        it->Source->SetInputFrameSize(it->Width, it->Height, 1);
      }
      else
      {
        // depth output is raw depth data
        LOG_INFO("setting up depth frame");
        it->Source->SetImageType(US_IMG_BRIGHTNESS);
        it->Source->SetPixelType(VTK_TYPE_UINT16);
        it->Source->SetNumberOfScalarComponents(1);
        it->Source->SetInputFrameSize(it->Width, it->Height, 1);
      }
    }

    // get frame data
    rs2::frame frame = frames.first(it->StreamType);
    if (!frame)
    {
      LOG_ERROR("Failed to get IntelRealSense frame");
      return PLUS_FAIL;
    }

    // add frame to PLUS buffer
    if (it->StreamType == RS2_STREAM_COLOR)
    {
      FrameSizeType frameSizeColor = { it->Width, it->Height, 1 };
      if (it->Source->AddItem((void *)frame.get_data(), it->Source->GetInputImageOrientation(), frameSizeColor, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
      {
        LOG_ERROR("vtkPlusIntelRealSense::InternalUpdate Unable to send RGB image. Skipping frame.");
        return PLUS_FAIL;
      }
    }
    else if (it->StreamType == RS2_STREAM_DEPTH)
    {
      FrameSizeType frameSizeDepth = { it->Width, it->Height, 1 };
      if (this->Internal->UseRealSenseColorizer)
      {
        rs2::colorizer color;
        color.set_option(RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED, 1);
        color.set_option(RS2_OPTION_MIN_DISTANCE, 0.6);
        color.set_option(RS2_OPTION_MAX_DISTANCE, 1.0);
        rs2::video_frame vfr = color.colorize(frame);
        if (it->Source->AddItem((void *)vfr.get_data(), it->Source->GetInputImageOrientation(), frameSizeDepth, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
        {
          LOG_ERROR("vtkPlusIntelRealSense::InternalUpdate Unable to send RGB image. Skipping frame.");
          return PLUS_FAIL;
        }
      }
      else
      {
        if (it->Source->AddItem((void *)frame.get_data(), it->Source->GetInputImageOrientation(), frameSizeDepth, VTK_TYPE_UINT16, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
        {
          LOG_ERROR("vtkPlusIntelRealSense::InternalUpdate Unable to send DEPTH image. Skipping frame.");
          return PLUS_FAIL;
        }
      }
    }
  }

  this->FrameNumber++; 
  return PLUS_SUCCESS;
}
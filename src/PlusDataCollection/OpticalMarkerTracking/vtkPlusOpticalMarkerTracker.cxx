/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#define MM_PER_M 1000

// Local includes
#include "PixelCodec.h"
#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpticalMarkerTracker.h"

// VTK includes
#include <vtkExtractVOI.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

// OS includes
#include <fstream>
#include <iostream>
#include <set>

// aruco includes
#include <markerdetector.h>
#include <cameraparameters.h>
#include <dictionary.h>
#include <posetracker.h>

// OpenCV includes
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusOpticalMarkerTracker);

namespace
{
  class TrackedTool
  {
  public:
    enum TOOL_MARKER_TYPE
    {
      SINGLE_MARKER,
      MARKER_MAP
    };

    TrackedTool(int markerId, float markerSizeMm, const std::string& toolSourceId)
      : ToolMarkerType(SINGLE_MARKER)
      , MarkerId(markerId)
      , MarkerSizeMm(markerSizeMm)
      , ToolSourceId(toolSourceId)
    {
    }
    TrackedTool(const std::string& markerMapFile, const std::string& toolSourceId)
      : ToolMarkerType(MARKER_MAP)
      , MarkerMapFile(markerMapFile)
      , ToolSourceId(toolSourceId)
    {
    }

    int MarkerId;
    TOOL_MARKER_TYPE ToolMarkerType;
    float MarkerSizeMm;

    std::string MarkerMapFile;
    std::string ToolSourceId;
    std::string ToolName;
    aruco::MarkerPoseTracker MarkerPoseTracker;
    vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  };
}
//----------------------------------------------------------------------------
class vtkPlusOpticalMarkerTracker::vtkInternal
{
public:
  vtkPlusOpticalMarkerTracker* External;

  vtkInternal(vtkPlusOpticalMarkerTracker* external)
    : External(external)
    , MarkerDetector(std::make_shared<aruco::MarkerDetector>())
    , CameraParameters(std::make_shared<aruco::CameraParameters>())
  {
  }

  virtual ~vtkInternal()
  {
    MarkerDetector = nullptr;
    CameraParameters = nullptr;
  }

  PlusStatus BuildTransformMatrix(vtkSmartPointer<vtkMatrix4x4> transformMatrix, const cv::Mat& Rvec, const cv::Mat& Tvec);

  std::string               CameraCalibrationFile;
  TRACKING_METHOD           TrackingMethod;
  std::string               MarkerDictionary;
  std::vector<TrackedTool>  Tools;

  /*! Pointer to main aruco objects */
  std::shared_ptr<aruco::MarkerDetector>    MarkerDetector;
  std::shared_ptr<aruco::CameraParameters>  CameraParameters;
  std::vector<aruco::Marker>                Markers;
};

//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::vtkPlusOpticalMarkerTracker()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{
  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::~vtkPlusOpticalMarkerTracker()
{
  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusOpticalMarkerTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  // TODO: Improve error checking
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(CameraCalibrationFile, this->Internal->CameraCalibrationFile, deviceConfig);
  XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_OPTIONAL(TrackingMethod, this->Internal->TrackingMethod, deviceConfig, "OPTICAL", TRACKING_OPTICAL, "OPTICAL_AND_DEPTH", TRACKING_OPTICAL_AND_DEPTH);
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(MarkerDictionary, this->Internal->MarkerDictionary, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(toolDataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }
    if (toolDataElement->GetAttribute("Type") != NULL && STRCASECMP(toolDataElement->GetAttribute("Type"), "Tool") != 0)
    {
      // if this is not a Tool element, skip it
      continue;
    }

    const char* toolId = toolDataElement->GetAttribute("Id");
    if (toolId == NULL)
    {
      // tool doesn't have ID needed to generate transform
      LOG_ERROR("Failed to initialize OpticalMarkerTracking tool: DataSource Id is missing");
      continue;
    }

    igsioTransformName toolTransformName(toolId, this->GetToolReferenceFrameName());
    std::string toolSourceId = toolTransformName.GetTransformName();

    if (toolDataElement->GetAttribute("MarkerId") != NULL && toolDataElement->GetAttribute("MarkerSizeMm") != NULL)
    {
      // this tool is tracked by a single marker
      int MarkerId;
      toolDataElement->GetScalarAttribute("MarkerId", MarkerId);
      float MarkerSizeMm;
      toolDataElement->GetScalarAttribute("MarkerSizeMm", MarkerSizeMm);
      TrackedTool newTool(MarkerId, MarkerSizeMm, toolSourceId);
      this->Internal->Tools.push_back(newTool);
    }
    else if (toolDataElement->GetAttribute("MarkerMapFile") != NULL)
    {
      // this tool is tracked by a marker map
      // TODO: Implement marker map tracking.
    }
    else
    {
      LOG_ERROR("Incorrectly formatted tool data source.");
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  if (!this->Internal->CameraCalibrationFile.empty())
  {
    deviceConfig->SetAttribute("CameraCalibrationFile", this->Internal->CameraCalibrationFile.c_str());
  }
  if (!this->Internal->MarkerDictionary.empty())
  {
    deviceConfig->SetAttribute("MarkerDictionary", this->Internal->MarkerDictionary.c_str());
  }
  switch (this->Internal->TrackingMethod)
  {
    case TRACKING_OPTICAL:
      deviceConfig->SetAttribute("TrackingMethod", "OPTICAL");
      break;
    case TRACKING_OPTICAL_AND_DEPTH:
      deviceConfig->SetAttribute("TrackingMethod", "OPTICAL_AND_DEPTH");
      break;
    default:
      LOG_ERROR("Unknown tracking method passed to vtkPlusOpticalMarkerTracker::WriteConfiguration");
      return PLUS_FAIL;
  }

  //TODO: Write data for custom attributes

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::Probe()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalConnect()
{
  // get calibration file path && check file exists
  std::string calibFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->Internal->CameraCalibrationFile);
  LOG_INFO("Use aruco camera calibration file located at: " << calibFilePath);
  if (!vtksys::SystemTools::FileExists(calibFilePath.c_str(), true))
  {
    LOG_ERROR("Unable to find aruco camera calibration file at: " << calibFilePath);
    return PLUS_FAIL;
  }

  // TODO: Need error handling for this?
  this->Internal->CameraParameters->readFromXMLFile(calibFilePath);
  this->Internal->MarkerDetector->setDictionary(this->Internal->MarkerDictionary);
  // threshold tuning numbers from aruco_test
  aruco::MarkerDetector::Params params;
  params._thresParam1 = 7;
  params._thresParam2 = 7;
  params._thresParam1_range = 2;
  this->Internal->MarkerDetector->setParams(params);

  bool lowestRateKnown = false;
  double lowestRate = 30; // just a usual value (FPS)
  for (ChannelContainerConstIterator it = begin(this->InputChannels); it != end(this->InputChannels); ++it)
  {
    vtkPlusChannel* anInputStream = (*it);
    if (anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
    {
      lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown = true;
    }
  }
  if (lowestRateKnown)
  {
    this->AcquisitionRate = lowestRate;
  }
  else
  {
    LOG_WARNING("vtkPlusOpticalMarkerTracker acquisition rate is not known");
  }

  this->LastProcessedInputDataTimestamp = 0;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalDisconnect()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalStartRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::vtkInternal::BuildTransformMatrix(vtkSmartPointer<vtkMatrix4x4> transformMatrix, const cv::Mat& Rvec, const cv::Mat& Tvec)
{
  transformMatrix->Identity();
  cv::Mat Rmat(3, 3, CV_32FC1);
  try
  {
    cv::Rodrigues(Rvec, Rmat);
  }
  catch (...)
  {
    return PLUS_FAIL;
  }

  for (int x = 0; x <= 2; x++)
  {
    transformMatrix->SetElement(x, 3, MM_PER_M * Tvec.at<float>(x, 0));
    for (int y = 0; y <= 2; y++)
    {
      transformMatrix->SetElement(x, y, Rmat.at<float>(x, y));
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalUpdate()
{
  if (this->InputChannels.size() != 1)
  {
    LOG_ERROR("ImageProcessor device requires exactly 1 input stream (that contains video data). Check configuration.");
    return PLUS_FAIL;
  }

  // Get image to tracker transform from the tracker (only request 1 frame, the latest)
  if (!this->InputChannels[0]->GetVideoDataAvailable())
  {
    LOG_TRACE("Processed data is not generated, as no video data is available yet. Device ID: " << this->GetDeviceId());
    return PLUS_SUCCESS;
  }

  double oldestTrackingTimestamp(0);
  if (this->InputChannels[0]->GetOldestTimestamp(oldestTrackingTimestamp) == PLUS_SUCCESS)
  {
    if (this->LastProcessedInputDataTimestamp > oldestTrackingTimestamp)
    {
      LOG_INFO("Processed image generation started. No tracking data was available between " << this->LastProcessedInputDataTimestamp << "-" << oldestTrackingTimestamp <<
               "sec, therefore no processed images were generated during this time period.");
      this->LastProcessedInputDataTimestamp = oldestTrackingTimestamp;
    }
  }

  igsioTrackedFrame trackedFrame;
  if (this->InputChannels[0]->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while getting latest tracked frame. Last recorded timestamp: " << std::fixed << this->LastProcessedInputDataTimestamp << ". Device ID: " << this->GetDeviceId());
    this->LastProcessedInputDataTimestamp = vtkIGSIOAccurateTimer::GetSystemTime(); // forget about the past, try to add frames that are acquired from now on
    return PLUS_FAIL;
  }

  LOG_TRACE("Image to be processed: timestamp=" << trackedFrame.GetTimestamp());

  // get dimensions & data
  FrameSizeType dim = trackedFrame.GetFrameSize();
  igsioVideoFrame* frame = trackedFrame.GetImageData();

  // converting trackedFrame (vtkImageData) to cv::Mat
  cv::Mat image(dim[1], dim[0], CV_8UC3, cv::Scalar(0, 0, 255));

  // Plus image uses RGB and OpenCV uses BGR, swapping is only necessary for colored markers
  //PixelCodec::RgbBgrSwap(dim[0], dim[1], (unsigned char*)frame->GetScalarPointer(), image.data);
  image.data = (unsigned char*)frame->GetScalarPointer();

  // detect markers in frame
  this->Internal->MarkerDetector->detect(image, this->Internal->Markers);

  // iterate through tools updating tracking
  for (std::vector<TrackedTool>::iterator toolIt = begin(this->Internal->Tools); toolIt != end(this->Internal->Tools); ++toolIt)
  {
    bool toolInFrame = false;
    const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
    for (std::vector<aruco::Marker>::iterator markerIt = begin(this->Internal->Markers); markerIt != end(this->Internal->Markers); ++markerIt)
    {
      if (toolIt->MarkerId == markerIt->id)
      {
        //marker is in frame
        toolInFrame = true;

        if (toolIt->MarkerPoseTracker.estimatePose(*markerIt, *this->Internal->CameraParameters, toolIt->MarkerSizeMm / MM_PER_M, 4))
        {
          // pose successfully estimated, update transform
          cv::Mat Rvec = toolIt->MarkerPoseTracker.getRvec();
          cv::Mat Tvec = toolIt->MarkerPoseTracker.getTvec();
          this->Internal->BuildTransformMatrix(toolIt->transformMatrix, Rvec, Tvec);
          ToolTimeStampedUpdate(toolIt->ToolSourceId, toolIt->transformMatrix, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
        }
        else
        {
          // pose estimation failed
          // TODO: add frame num, marker id, etc. Make this error more helpful.  Is there a way to handle it?
          LOG_ERROR("Pose estimation failed. Tool " << toolIt->ToolSourceId << " with marker " << toolIt->MarkerId << ".");
        }
        break;
      }
    }
    if (!toolInFrame)
    {
      // tool not in frame
      ToolTimeStampedUpdate(toolIt->ToolSourceId, toolIt->transformMatrix, TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
    }
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}
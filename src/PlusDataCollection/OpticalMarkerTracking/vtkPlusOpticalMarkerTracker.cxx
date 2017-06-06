/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#define MM_PER_M 1000

#include "PlusConfigure.h"
#include "vtkPlusOpticalMarkerTracker.h"
#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkExtractVOI.h"
#include <fstream>
#include <iostream>
#include <set>
#include "vtkPlusDataSource.h"
#include "vtkMatrix4x4.h"

// aruco
#include "dictionary.h"

// OpenCV
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

vtkStandardNewMacro(vtkPlusOpticalMarkerTracker);
//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::TrackedTool::TrackedTool(int MarkerId, float MarkerSizeMm, std::string ToolSourceId)
{
  ToolMarkerType = SINGLE_MARKER;
  this->MarkerId = MarkerId;
  this->MarkerSizeMm = MarkerSizeMm;
  this->ToolSourceId = ToolSourceId;
}

vtkPlusOpticalMarkerTracker::TrackedTool::TrackedTool(std::string MarkerMapFile, std::string ToolSourceId)
{
  ToolMarkerType = MARKER_MAP;
  this->MarkerMapFile = MarkerMapFile;
  this->ToolSourceId = ToolSourceId;
}

//----------------------------------------------------------------------------

class vtkPlusOpticalMarkerTracker::vtkInternal
{
public:
  vtkPlusOpticalMarkerTracker *External;


  vtkInternal(vtkPlusOpticalMarkerTracker* external)
    : External(external)
  {
  }

  virtual ~vtkInternal()
  {
  }
};

//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::vtkPlusOpticalMarkerTracker()
: vtkPlusDevice()
{
  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::~vtkPlusOpticalMarkerTracker() 
{

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

  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(CameraCalibrationFile, deviceConfig);
  XML_READ_ENUM2_ATTRIBUTE_OPTIONAL(TrackingMethod, deviceConfig, "OPTICAL_ONLY", OPTICAL_DEPTH, "OPTICAL_DEPTH", OPTICAL_DEPTH);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(MarkerDictionary, deviceConfig);

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

    PlusTransformName toolTransformName(toolId, this->GetToolReferenceFrameName());
    std::string toolSourceId = toolTransformName.GetTransformName();

    if (toolDataElement->GetAttribute("MarkerId") != NULL && toolDataElement->GetAttribute("MarkerSizeMm") != NULL)
    {
      // this tool is tracked by a single marker
      int MarkerId;
      toolDataElement->GetScalarAttribute("MarkerId", MarkerId);
      float MarkerSizeMm;
      toolDataElement->GetScalarAttribute("MarkerSizeMm", MarkerSizeMm);
      TrackedTool newTool(MarkerId, MarkerSizeMm, toolSourceId);
      Tools.push_back(newTool);
    }
    else if (toolDataElement->GetAttribute("MarkerMapFile") != NULL)
    {
      // this tool is tracked by a marker map
      // TODO: Implement marker map tracking.
    }
    else {
      LOG_ERROR("Incorrectly formatted tool data source.");
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE(CameraCalibrationFile, deviceConfig);
  // no write enum method
  XML_WRITE_STRING_ATTRIBUTE(MarkerDictionary, deviceConfig);

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
  std::string calibFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->CameraCalibrationFile);
  LOG_INFO("Use aruco camera calibration file located at: " << calibFilePath);
  if (!vtksys::SystemTools::FileExists(calibFilePath.c_str(), true))
  {
    LOG_ERROR("Unable to find aruco camera calibration file at: " << calibFilePath);
    return PLUS_FAIL;
  }

  // TODO: Need error handling for this?
  CP.readFromXMLFile(calibFilePath);
  MDetector.setDictionary(MarkerDictionary);

  bool lowestRateKnown = false;
  double lowestRate = 30; // just a usual value (FPS)
  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
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
void vtkPlusOpticalMarkerTracker::BuildTransformMatrix(vtkSmartPointer<vtkMatrix4x4> transformMatrix, cv::Mat Rvec, cv::Mat Tvec)
{
  transformMatrix->Identity();
  cv::Mat Rmat(3, 3, CV_32FC1);
  cv::Rodrigues(Rvec, Rmat);

  for (int x = 0; x <= 2; x++)
  {
    transformMatrix->SetElement(x, 3, MM_PER_M * Tvec.at<float>(x, 0));
    for (int y = 0; y <= 2; y++)
      transformMatrix->SetElement(x, y, Rmat.at<float>(x, y));
  }
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

  PlusTrackedFrame trackedFrame;
  if (this->InputChannels[0]->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while getting latest tracked frame. Last recorded timestamp: " << std::fixed << this->LastProcessedInputDataTimestamp << ". Device ID: " << this->GetDeviceId());
    this->LastProcessedInputDataTimestamp = vtkPlusAccurateTimer::GetSystemTime(); // forget about the past, try to add frames that are acquired from now on
    return PLUS_FAIL;
  }

  LOG_TRACE("Image to be processed: timestamp=" << trackedFrame.GetTimestamp());

  // get dimensions & data
  unsigned int *dim = trackedFrame.GetFrameSize();
  PlusVideoFrame *frame = trackedFrame.GetImageData();

  // converting trackedFrame (vtkImageData) to cv::Mat
  cv::Mat image(dim[1], dim[0], CV_8UC3, cv::Scalar(0, 0, 255));
  // TODO: error checking - image.isContinuous());
  vtkImageData *vtkImage = frame->GetImage();
  for (int i = 0; i < dim[0]; i++)
  {
    for (int j = 0; j < dim[1]; j++)
    {
      uchar *pixel = static_cast<uchar*>(vtkImage->GetScalarPointer(i, j, 0));
      uchar temp = pixel[0];
      // swap B and R channels since vtkImage is RGB and OCV is BGR
      pixel[0] = pixel[2];
      pixel[2] = temp;
      image.at<cv::Vec3b>(j, i) = static_cast<cv::Vec3b>(pixel);
    }
  }

  // detect markers in frame
  MDetector.detect(image, markers);

  // iterate through tools updating tracking
  for (vector<TrackedTool>::iterator toolIt = Tools.begin(); toolIt != Tools.end(); ++toolIt)
  {
    bool toolInFrame = false;
    const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();
    for (vector<aruco::Marker>::iterator markerIt = markers.begin(); markerIt != markers.end(); ++markerIt)
    {
      if (toolIt->MarkerId == markerIt->id) {
        //marker is in frame
        toolInFrame = true;

        if (MarkerPoseTracker.estimatePose(*markerIt, CP, toolIt->MarkerSizeMm / MM_PER_M, 4))
        {
          // pose successfully estimated, update transform
          cv::Mat Rvec = MarkerPoseTracker.getRvec();
          cv::Mat Tvec = MarkerPoseTracker.getTvec();
          BuildTransformMatrix(toolIt->transformMatrix, Rvec, Tvec);
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
    if (!toolInFrame) {
      // tool not in frame
      ToolTimeStampedUpdate(toolIt->ToolSourceId, toolIt->transformMatrix, TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
    }
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}
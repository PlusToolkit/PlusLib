/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// TODO: need to evaluate if USE_MicronTracker_TIMESTAMPS without filtering
// is better then simply using accurate timestamp with filtering.
// E.g., it could be checked by performing temporal calibration.
// #define USE_MicronTracker_TIMESTAMPS

// Local includes
#include "PlusConfigure.h"
//#include "igsioVideoFrame.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusMicronTracker.h"

// Micron interface includes
#include <MicronTrackerInterface.h>
#include <MicronTrackerLogger.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STL includes
#include <fstream>
#include <iostream>
#include <set>

// Note that "MTC.h" is not included directly, as it causes compilation warnings
// and unnecessary coupling to lower-level MTC functions.
// All MTC internal functions shall be accessed through MicronTrackerInterface.

/****************************************************************************/

vtkStandardNewMacro(vtkPlusMicronTracker);

//----------------------------------------------------------------------------
vtkPlusMicronTracker::vtkPlusMicronTracker()
  : IsMicronTrackingInitialized(false)
  , MicronTracker(new MicronTrackerInterface())
#ifdef USE_MicronTracker_TIMESTAMPS
  , TrackerTimeToSystemTimeSec(0)
  , TrackerTimeToSystemTimeComputed(false)
#endif
  , IniFile("MicronTracker.ini")
  , FrameLeft(vtkSmartPointer<vtkImageData>::New())
  , FrameRight(vtkSmartPointer<vtkImageData>::New())
{
  MicronTrackerLogger::Instance()->SetLogMessageCallback(LogMessageCallback, this);

  this->FrameNumber = 0;
  this->RequirePortNameInDeviceSetConfiguration = true;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 20;

  this->FrameLeft = vtkSmartPointer<vtkImageData>::New();
  this->FrameRight = vtkSmartPointer<vtkImageData>::New();
  this->FrameSize[0] = 0;
  this->FrameSize[1] = 0;
  this->FrameSize[2] = 1;
}

//----------------------------------------------------------------------------
vtkPlusMicronTracker::~vtkPlusMicronTracker()
{
  if (this->IsMicronTrackingInitialized)
  {
    this->MicronTracker->mtEnd();
    this->IsMicronTrackingInitialized = false;
  }

  delete this->MicronTracker;
  this->MicronTracker = NULL;
}

//----------------------------------------------------------------------------
std::string vtkPlusMicronTracker::GetSdkVersion()
{
  return this->MicronTracker->GetSdkVersion();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::Probe()
{
  if (this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("vtkPlusMicronTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  std::string iniFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->IniFile);
  LOG_DEBUG("Use MicronTracker ini file: " << iniFilePath);
  if (!vtksys::SystemTools::FileExists(iniFilePath.c_str(), true))
  {
    LOG_WARNING("Unable to find MicronTracker IniFile file at: " << iniFilePath);
  }
  std::string templateFullPath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->TemplateDirectory.c_str());
  LOG_DEBUG("Loading the marker templates from " << templateFullPath);
  if (!vtksys::SystemTools::FileExists(templateFullPath.c_str(), false))
  {
    LOG_WARNING("Unable to find MicronTracker TemplateDirectory at: " << templateFullPath);
  }

  if (this->MicronTracker->mtInit(iniFilePath) != 1)
  {
    LOG_ERROR("Error in initializing Micron Tracker");
    return PLUS_FAIL;
  }

  // Try to attach the cameras till find the cameras
  if (this->MicronTracker->mtSetupCameras() != 1)
  {
    LOG_ERROR("Error in initializing Micron Tracker: setup cameras failed. Check the camera connections.");
    return PLUS_FAIL;
  }

  int numOfCameras = this->MicronTracker->mtGetNumOfCameras();
  if (numOfCameras == 0)
  {
    LOG_ERROR("Error in initializing Micron Tracker: no cameras attached. Check the camera connections.");
    return PLUS_FAIL;
  }

  LOG_DEBUG("Number of attached cameras: " << numOfCameras);

  this->MicronTracker->mtEnd();
  this->IsMicronTrackingInitialized = false;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::InternalStartRecording()
{
  if (!this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("InternalStartRecording failed: MicronTracker has not been initialized");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::InternalUpdate()
{
  if (!this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("InternalUpdate failed: MicronTracker has not been initialized");
    return PLUS_FAIL;
  }

  // Setting the timestamp
  const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  if (this->MicronTracker->mtGrabFrame() != 0) // mtOK
  {
    // If grabbing a frame was not successful then just skip this attempt and retry on the next callback
    LOG_WARNING("Failed to grab a new frame (" << this->MicronTracker->GetLastErrorString() << "). Maybe the requested frame rate is too high.");
    return PLUS_FAIL;
  }

#ifdef USE_MicronTracker_TIMESTAMPS
  if (!this->TrackerTimeToSystemTimeComputed)
  {
    const double timeSystemSec = unfilteredTimestamp;
    const double timeTrackerSec = this->MicronTracker->mtGetLatestFrameTime();
    this->TrackerTimeToSystemTimeSec = timeSystemSec - timeTrackerSec;
    this->TrackerTimeToSystemTimeComputed = true;
  }
  const double timeTrackerSec = this->MicronTracker->mtGetLatestFrameTime();
  const double timeSystemSec = timeTrackerSec + this->TrackerTimeToSystemTimeSec;
#endif

  if (this->MicronTracker->mtProcessFrame() != 0) // mtOK
  {
    LOG_ERROR("Error in processing a frame! (" << this->MicronTracker->GetLastErrorString() << ")");
    return PLUS_FAIL;
  }

  this->MicronTracker->mtFindIdentifiedMarkers();

  // Generate a frame number, as the tool does not provide a frame number.
  // FrameNumber will be used in ToolTimeStampedUpdate for timestamp filtering
  ++this->FrameNumber;

  int numOfIdentifiedMarkers = this->MicronTracker->mtGetIdentifiedMarkersCount();
  LOG_TRACE("Number of identified markers: " << numOfIdentifiedMarkers);

  // Set status and transform for tools with detected markers
  std::set<std::string> identifiedToolSourceIds;
  vtkNew<vtkMatrix4x4> toolToTracker;
  for (int identifedMarkerIndex = 0; identifedMarkerIndex < numOfIdentifiedMarkers; identifedMarkerIndex++)
  {
    char* identifiedTemplateName = this->MicronTracker->mtGetIdentifiedTemplateName(identifedMarkerIndex);
    vtkPlusDataSource* tool = NULL;
    if (this->GetToolByPortName(identifiedTemplateName, tool) != PLUS_SUCCESS)
    {
      LOG_DEBUG("Marker " << identifiedTemplateName << " has no associated tool");
      continue;
    }

    this->GetTransformMatrix(identifedMarkerIndex, toolToTracker.GetPointer());
#ifdef USE_MicronTracker_TIMESTAMPS
    this->ToolTimeStampedUpdateWithoutFiltering(tool->GetSourceId(), toolToTracker, TOOL_OK, timeSystemSec, timeSystemSec);
#else
    this->ToolTimeStampedUpdate(tool->GetSourceId(), toolToTracker.GetPointer(), TOOL_OK, this->FrameNumber, unfilteredTimestamp);
#endif

    identifiedToolSourceIds.insert(tool->GetSourceId());
  }

  // Set status for tools with non-detected markers
  vtkNew<vtkMatrix4x4> transformMatrix;
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if (identifiedToolSourceIds.find(it->second->GetSourceId()) != identifiedToolSourceIds.end())
    {
      // this tool has been found and update has been already called with the correct transform
      LOG_TRACE("Tool " << it->second->GetSourceId() << ": found");
      continue;
    }
    LOG_TRACE("Tool " << it->second->GetSourceId() << ": not found");
#ifdef USE_MicronTracker_TIMESTAMPS
    ToolTimeStampedUpdateWithoutFiltering(it->second->GetSourceId(), transformMatrix, TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
#else
    ToolTimeStampedUpdate(it->second->GetSourceId(), transformMatrix.GetPointer(), TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
#endif
  }

  if (this->GetNumberOfVideoSources() > 0)
  {
    this->GetImage(this->FrameLeft, this->FrameRight);
    vtkPlusDataSource* aSource(NULL);
    for (int i = 0; i < this->GetNumberOfVideoSources(); ++i)
    {
      if (this->GetVideoSourceByIndex(i, aSource) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to retrieve MicronTracker video source");
        return PLUS_FAIL;
      }
      aSource->SetInputImageOrientation(US_IMG_ORIENT_MN);
      aSource->SetInputFrameSize(this->FrameSize);
      if (aSource->AddItem((i == 0) ? this->FrameLeft : this->FrameRight, US_IMG_ORIENT_MN, US_IMG_BRIGHTNESS, this->FrameNumber, unfilteredTimestamp) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to add item " << i << " to MicronTracker video source");
        return PLUS_FAIL;
      }
      this->Modified();
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::RefreshMarkerTemplates()
{
  std::vector<std::string> vTemplatesName;
  std::vector<std::string> vTemplatesError;

  std::string templateFullPath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->TemplateDirectory.c_str());
  LOG_DEBUG("Loading the marker templates from " << templateFullPath);
  if (!vtksys::SystemTools::FileExists(templateFullPath.c_str(), false))
  {
    LOG_WARNING("Unable to find MicronTracker TemplateDirectory at: " << templateFullPath);
  }
  int callResult = this->MicronTracker->mtRefreshTemplates(vTemplatesName, vTemplatesError, templateFullPath);
  for (unsigned int i = 0; i < vTemplatesName.size(); i++)
  {
    LOG_DEBUG("Loaded " << vTemplatesName[i]);
  }
  if (callResult != 0)
  {
    LOG_ERROR("Failed to load marker templates from " << templateFullPath);
    for (unsigned int i = 0; i < vTemplatesError.size(); i++)
    {
      LOG_ERROR("Error loading template: " << vTemplatesError[i]);
    }
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusMicronTracker::GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix)
{
  std::vector<double> vRotMat;
  this->MicronTracker->mtGetRotations(vRotMat, markerIndex);
  std::vector<double> vPos;
  this->MicronTracker->mtGetTranslations(vPos, markerIndex);

  transformMatrix->Identity();
  int rotIndex = 0;
  for (int col = 0; col < 3; col++)
  {
    for (int row = 0; row < 3; row++)
    {
      transformMatrix->SetElement(row, col, vRotMat[rotIndex++]);
    }
  }
  // Add the offset to the last column of the transformation matrix
  transformMatrix->SetElement(0, 3, vPos[0]);
  transformMatrix->SetElement(1, 3, vPos[1]);
  transformMatrix->SetElement(2, 3, vPos[2]);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::GetImage(vtkImageData* leftImage, vtkImageData* rightImage)
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  unsigned char** leftImageArray = 0;
  unsigned char** rightImageArray = 0;
  if (this->MicronTracker->mtGetLeftRightImageArray(leftImageArray, rightImageArray) != 0) // mtOK
  {
    LOG_ERROR("Error getting images from MicronTracker");
    return PLUS_FAIL;
  }

  if (leftImage != NULL)
  {
    vtkSmartPointer<vtkImageImport> imageImport = vtkSmartPointer<vtkImageImport>::New();
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetImportVoidPointer((unsigned char*)leftImageArray);
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetDataExtent(0, this->FrameSize[0] - 1, 0, this->FrameSize[1] - 1, 0, 0);
    imageImport->SetWholeExtent(0, this->FrameSize[0] - 1, 0, this->FrameSize[1] - 1, 0, 0);
    imageImport->Update();
    leftImage->DeepCopy(imageImport->GetOutput());
  }

  if (rightImage != NULL)
  {
    vtkSmartPointer<vtkImageImport> imageImport = vtkSmartPointer<vtkImageImport>::New();
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetImportVoidPointer((unsigned char*)rightImageArray);
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetDataExtent(0, this->FrameSize[0] - 1, 0, this->FrameSize[1] - 1, 0, 0);
    imageImport->SetWholeExtent(0, this->FrameSize[0] - 1, 0, this->FrameSize[1] - 1, 0, 0);
    imageImport->Update();
    rightImage->DeepCopy(imageImport->GetOutput());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(TemplateDirectory, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(IniFile, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(TemplateDirectory, trackerConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(IniFile, trackerConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::InternalConnect()
{
  if (this->IsMicronTrackingInitialized)
  {
    LOG_DEBUG("Already connected to MicronTracker");
    return PLUS_SUCCESS;
  }

  std::string iniFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->IniFile);
  LOG_DEBUG("Use MicronTracker ini file: " << iniFilePath);
  if (!vtksys::SystemTools::FileExists(iniFilePath.c_str(), true))
  {
    LOG_WARNING("Unable to find MicronTracker IniFile file at: " << iniFilePath);
  }
  std::string templateFullPath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->TemplateDirectory.c_str());
  LOG_DEBUG("Loading the marker templates from " << templateFullPath);
  if (!vtksys::SystemTools::FileExists(templateFullPath.c_str(), false))
  {
    LOG_ERROR("Unable to find MicronTracker TemplateDirectory at: " << templateFullPath);
  }

  if (this->MicronTracker->mtInit(iniFilePath) != 1)
  {
    LOG_ERROR("Error in initializing Micron Tracker");
    return PLUS_FAIL;
  }

  // Try to attach the cameras till find the cameras
  if (this->MicronTracker->mtSetupCameras() != 1)
  {
    LOG_ERROR("Error in initializing Micron Tracker: setup cameras failed. Check the camera connections and INI and Markers file locations.");
    this->MicronTracker->mtEnd();
    return PLUS_FAIL;
  }

  int numOfCameras = this->MicronTracker->mtGetNumOfCameras();
  if (numOfCameras == 0)
  {
    LOG_ERROR("Error in initializing Micron Tracker: no cameras attached. Check the camera connections and INI and Markers file locations.");
    this->MicronTracker->mtEnd();
    return PLUS_FAIL;
  }
  LOG_DEBUG("Number of attached cameras: " << numOfCameras);
  for (int i = 0; i < numOfCameras; i++)
  {
    LOG_DEBUG("Camera " << i << ": "
              << this->MicronTracker->mtGetXResolution(i) << "x" << this->MicronTracker->mtGetYResolution(i) << ", "
              << this->MicronTracker->mtGetNumOfSensors(i) << " sensors "
              << "(serial number: " << this->MicronTracker->mtGetSerialNum(i) << ")");
  }

  if (RefreshMarkerTemplates() != PLUS_SUCCESS)
  {
    LOG_ERROR("Error in initializing Micron Tracker: Failed to load marker templates. Check if the marker directory is set correctly.");
    this->MicronTracker->mtEnd();
    return PLUS_FAIL;
  }

#ifdef USE_MicronTracker_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif

  int imageWidth = this->MicronTracker->mtGetXResolution(-1);
  int imageHeight = this->MicronTracker->mtGetYResolution(-1);
  if (imageWidth < 0 || imageHeight < 0)
  {
    LOG_ERROR("Invalid resolution returned from Micron device.");
    return PLUS_FAIL;
  }
  this->FrameSize[0] = static_cast<unsigned int>(imageWidth);
  this->FrameSize[1] = static_cast<unsigned int>(imageHeight);

  this->IsMicronTrackingInitialized = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::InternalDisconnect()
{
  if (this->IsMicronTrackingInitialized)
  {
    this->MicronTracker->mtEnd();
    this->IsMicronTrackingInitialized = false;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMicronTracker::NotifyConfigured()
{
  if (this->GetNumberOfVideoSources() > 0 && this->GetNumberOfVideoSources() != 2)
  {
    LOG_ERROR("Micron stereo camera capture requires exactly 2 video sources. Check configuration.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusMicronTracker::LogMessageCallback(int level, const char* message, void* /*userdata*/)
{
  switch (level)
  {
    case MicronTrackerLogger::WARNING_LEVEL:
      LOG_WARNING("MicronTracker: " << (message ? message : ""));
      break;
    case MicronTrackerLogger::DEBUG_LEVEL:
      if (message)
      {
        LOG_DEBUG("MicronTracker: " << message);
      }
      break;
    case MicronTrackerLogger::ERROR_LEVEL:
    default:
      LOG_ERROR("MicronTracker: " << (message ? message : ""));
      break;
  }
}

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// TODO: need to evaluate if USE_INTELREALSENSE_TIMESTAMPS without filtering
// is better then simply using accurate timestamp with filtering.
// E.g., it could be checked by performing temporal calibration.
// #define USE_INTELREALSENSE_TIMESTAMPS

#include "PlusConfigure.h"
#include "vtkPlusIntelRealSenseTracker.h"

#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include <fstream>
#include <iostream>
#include <set>

#include "pxcsensemanager.h"
#include "pxctracker.h"
#include "pxcsession.h"
#include "pxcprojection.h"

// From FF_ObjectTracking sample
#define ID_DEVICEX   21000

/****************************************************************************/

vtkStandardNewMacro(vtkPlusIntelRealSenseTracker);

void StringToWString(std::wstring &ws, const std::string &s)
{
  std::wstring wsTmp(s.begin(), s.end());
  ws = wsTmp;
}

class Model
{
public:
  Model()
  {
    model_filename[0] = '\0';
  }

  Model(pxcCHAR *filename)
  {
    wcsncpy_s<1024>(model_filename, filename, 1024);
  }

  struct TrackingState
  {
    pxcUID	cosID;
    pxcCHAR friendlyName[256];
    bool	isTracking;
  };

  void addCosID(pxcUID cosID, pxcCHAR *friendlyName)
  {
    TrackingState state;
    state.cosID = cosID;
    wcscpy_s<256>(state.friendlyName, friendlyName);
    state.isTracking = false;

    cosIDs.push_back(state);
  }

  pxcCHAR  model_filename[1024];
  std::vector<TrackingState> cosIDs;
};

typedef std::vector<Model>::iterator				TargetIterator;
typedef std::vector<Model::TrackingState>::iterator TrackingIterator;

class vtkPlusIntelRealSenseTracker::vtkInternal
{
public:
  vtkPlusIntelRealSenseTracker *External;

  PXCSession* Session;

  PXCSenseManager* SenseMgr;
  PXCCaptureManager* CaptureMgr;
  PXCProjection* Projection;

  std::vector<Model> Targets;
  pxcCHAR File[1024];
  pxcCHAR CalibrationFile[1024];
  PXCRectI32 Roi;
  PXCTracker *Tracker;

  bool GetDeviceInfo(int deviceIndex, PXCCapture::DeviceInfo& dinfo)
  {
    PXCSession::ImplDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.group = PXCSession::IMPL_GROUP_SENSOR;
    desc.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
    for (int i = 0, k = ID_DEVICEX;; i++)
    {
      PXCSession::ImplDesc desc1;
      if (this->Session->QueryImpl(&desc, i, &desc1) < PXC_STATUS_NO_ERROR)
      {
        break;
      }
      PXCCapture *capture;
      if (this->Session->CreateImpl<PXCCapture>(&desc1, &capture) < PXC_STATUS_NO_ERROR)
      {
        continue;
      }
      for (int j = 0;; j++)
      {
        if (capture->QueryDeviceInfo(j, &dinfo) < PXC_STATUS_NO_ERROR) break;
        if (dinfo.orientation == PXCCapture::DEVICE_ORIENTATION_REAR_FACING) break;
        if (j == deviceIndex)
        {
          return true;
        }
      }
    }
    return false;
  }


  vtkInternal(vtkPlusIntelRealSenseTracker* external)
    : External(external)
    , Session(NULL)
    , Tracker(NULL)
    , SenseMgr(NULL)
    , CaptureMgr(NULL)
    , Projection(NULL)
  {
  }

  virtual ~vtkInternal()
  {
  }
};

//----------------------------------------------------------------------------
vtkPlusIntelRealSenseTracker::vtkPlusIntelRealSenseTracker()
  : Internal(new vtkInternal(this))
{
#ifdef USE_INTELREALSENSE_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif

  this->IsTrackingInitialized = 0;
//  this->MT = new IntelRealSenseTrackerInterface();


  // for accurate timing
  this->FrameNumber = 0;

  // PortName for data source is not required because MapFile identifies each tool, therefore we don't need to enable this->RequirePortNameInDeviceSetConfiguration
  
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 20;
  
  this->CameraCalibrationFile="IntelRealSenseToolDefinitions/CameraCalibration-CreativeSR300.xml";
  this->DeviceName = "Intel(R) RealSense(TM) 3D Camera SR300";
}

//----------------------------------------------------------------------------
vtkPlusIntelRealSenseTracker::~vtkPlusIntelRealSenseTracker() 
{
  if (this->IsTrackingInitialized)
  {
    //this->MT->mtEnd();
    this->IsTrackingInitialized=false;
  }
  /*
  if ( this->MT != NULL )
  {    
    delete this->MT;
    this->MT = NULL;
  }
  */
}

//----------------------------------------------------------------------------
std::string vtkPlusIntelRealSenseTracker::GetSdkVersion()
{
  //return this->MT->GetSdkVersion(); 
	return "";
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::Probe()
{  
  if (this->IsTrackingInitialized)
  {
    LOG_ERROR("vtkPlusIntelRealSenseTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  std::string cameraCalibrationFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->CameraCalibrationFile);
  LOG_DEBUG("Use camera calibration file: " << cameraCalibrationFilePath);
  if (!vtksys::SystemTools::FileExists(cameraCalibrationFilePath.c_str(), true))
  {
    LOG_DEBUG("Unable to find IntelRealSenseTracker camera calibration file at: " << cameraCalibrationFilePath);
  }

  /*
  if (this->MT->mtInit(iniFilePath)!=1)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera");
    return PLUS_FAIL;
  }
  */

  /*
  // Try to attach the cameras till find the cameras
  if (this->MT->mtSetupCameras()!=1)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera: setup cameras failed. Check the camera connections.");
    return PLUS_FAIL;
  }
  */

  /*
  int numOfCameras = this->MT->mtGetNumOfCameras();
  if (numOfCameras==0)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera: no cameras attached. Check the camera connections.");
    return PLUS_FAIL;
  }
  LOG_DEBUG("Number of attached cameras: " << numOfCameras );
  */

  //this->MT->mtEnd();
  this->IsTrackingInitialized=false;

  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::InternalStartRecording()
{
  if (!this->IsTrackingInitialized)
  {
    LOG_ERROR("InternalStartRecording failed: IntelRealSenseTracker has not been initialized");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::InternalStopRecording()
{
  // No need to do anything here, as the IntelRealSenseTracker only performs grabbing on request
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::InternalUpdate()
{
  if (!this->IsTrackingInitialized)
  {
    LOG_ERROR("InternalUpdate failed: IntelRealSenseTracker has not been initialized");
    return PLUS_FAIL;
  }

  // Generate a frame number, as the tool does not provide a frame number.
  // FrameNumber will be used in ToolTimeStampedUpdate for timestamp filtering
  ++this->FrameNumber;

  // Setting the timestamp
  const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();

#ifdef USE_INTELREALSENSE_TIMESTAMPS
  if (!this->TrackerTimeToSystemTimeComputed)
  {
    const double timeSystemSec = unfilteredTimestamp;
    const double timeTrackerSec = this->MT->mtGetLatestFrameTime();
    this->TrackerTimeToSystemTimeSec = timeSystemSec-timeTrackerSec;
    this->TrackerTimeToSystemTimeComputed = true;
  }
  const double timeTrackerSec = this->MT->mtGetLatestFrameTime();
  const double timeSystemSec = timeTrackerSec + this->TrackerTimeToSystemTimeSec;        
#endif

  // Set status and transform for tools with detected markers
  vtkSmartPointer<vtkMatrix4x4> transformMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  std::set<std::string> identifiedToolSourceIds;
  vtkSmartPointer< vtkMatrix4x4 > mToolToTracker = vtkSmartPointer< vtkMatrix4x4 >::New();
  mToolToTracker->Identity();
 
  /*
  for (int identifedMarkerIndex=0; identifedMarkerIndex<this->MT->mtGetIdentifiedMarkersCount(); identifedMarkerIndex++)
  {
  char* identifiedTemplateName=this->MT->mtGetIdentifiedTemplateName(identifedMarkerIndex);
  vtkPlusDataSource* tool = NULL;
  if ( this->GetToolByPortName(identifiedTemplateName, tool) != PLUS_SUCCESS )
  {
  LOG_DEBUG("Marker " << identifiedTemplateName << " has no associated tool");
  continue;
  }

  GetTransformMatrix(identifedMarkerIndex, mToolToTracker);
  #ifdef USE_INTELREALSENSE_TIMESTAMPS
  this->ToolTimeStampedUpdateWithoutFiltering( tool->GetSourceId(), mToolToTracker, TOOL_OK, timeSystemSec, timeSystemSec);
  #else
  this->ToolTimeStampedUpdate( tool->GetSourceId(), mToolToTracker, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
  #endif

  identifiedToolSourceIds.insert(tool->GetSourceId());
  }

  // Set status for tools with non-detected markers
  transformMatrix->Identity();
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
  if (identifiedToolSourceIds.find(it->second->GetSourceId())!=identifiedToolSourceIds.end())
  {
  // this tool has been found and update has been already called with the correct transform
  LOG_TRACE("Tool "<<it->second->GetSourceId()<<": found");
  continue;
  }
  LOG_TRACE("Tool "<<it->second->GetSourceId()<<": not found");

  }
  */

  if (this->Internal->SenseMgr->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
  {
    LOG_ERROR("AcquireFrame failed");
    return PLUS_FAIL;
  }


  /* Display Results */
  PXCTracker::TrackingValues  trackData;
  int updatedTrackingCount = 0;
  const PXCCapture::Sample *sample = this->Internal->SenseMgr->QueryTrackerSample();
  if (sample)
  {
    LOG_DEBUG("Sample found!")
  }

  // Loop over all of the registered targets (COS IDs) and see if they are tracked
  for (TargetIterator targetIter = this->Internal->Targets.begin(); targetIter != this->Internal->Targets.end(); targetIter++)
  {
    for (TrackingIterator iter = targetIter->cosIDs.begin(); iter != targetIter->cosIDs.end(); iter++)
    {
      this->Internal->Tracker->QueryTrackingValues(iter->cosID, trackData);

      if (PXCTracker::IsTracking(trackData.state))
      {
        updatedTrackingCount += (!iter->isTracking) ? 1 : 0;
        iter->isTracking = true;
        LOG_INFO("Position: " << trackData.translation.x << ", " << trackData.translation.y << ", " << trackData.translation.z)
        //TODO: compute transformMatrix
      }
      else
      {
        updatedTrackingCount += iter->isTracking ? 1 : 0;
        iter->isTracking = false;
      }

      /*
#ifdef USE_INTELREALSENSE_TIMESTAMPS
      ToolTimeStampedUpdateWithoutFiltering(iter->GetSourceId(), transformMatrix, iter->isTracking?TOOL_OK:TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
#else
      ToolTimeStampedUpdate(iter->GetSourceId(), transformMatrix, iter->isTracking ? TOOL_OK : TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
#endif
      */

    }
  }

  this->Internal->SenseMgr->ReleaseFrame();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::RefreshMarkerTemplates()
{
  /*
  std::vector<std::string> vTemplatesName;
  std::vector<std::string> vTemplatesError;

  std::string templateFullPath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->TemplateDirectory.c_str());
  LOG_DEBUG("Loading the marker templates from "<<templateFullPath);
  if ( !vtksys::SystemTools::FileExists( templateFullPath.c_str(), false) )
  {
    LOG_WARNING("Unable to find IntelRealSenseTracker TemplateDirectory at: " << templateFullPath);
  }
  int callResult = this->MT->mtRefreshTemplates(vTemplatesName, vTemplatesError, templateFullPath);
  for (int i=0; i<vTemplatesName.size(); i++)
  {
    LOG_DEBUG("Loaded " << vTemplatesName[i]);
  }
  if (callResult != 0)
  {
    LOG_ERROR("Failed to load marker templates from "<<templateFullPath);
    for (int i=0; i<vTemplatesError.size(); i++)
    {
      LOG_ERROR("Error loading template: " << vTemplatesError[i]);
    }
    return PLUS_FAIL;
  }
  */
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusIntelRealSenseTracker::GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix)
{  
/*
  std::vector<double> vRotMat;
  this->MT->mtGetRotations( vRotMat, markerIndex );
  std::vector<double> vPos;
  this->MT->mtGetTranslations(vPos, markerIndex);

  transformMatrix->Identity();
  int rotIndex =0;
  for(int col=0; col < 3; col++)
  {
    for (int row=0; row < 3; row++)
    {
      transformMatrix->SetElement(row, col, vRotMat[rotIndex++]);
    }
  }
  // Add the offset to the last column of the transformation matrix
  transformMatrix->SetElement(0,3,vPos[0]);
  transformMatrix->SetElement(1,3,vPos[1]);
  transformMatrix->SetElement(2,3,vPos[2]);
*/
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::GetImage(vtkImageData* leftImage, vtkImageData* rightImage)
{
  PlusLockGuard<vtkPlusRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
  /*
  unsigned char** leftImageArray=0;
  unsigned char** rightImageArray=0;
  if (this->MT->mtGetLeftRightImageArray(leftImageArray, rightImageArray) == -1)
  {
    LOG_ERROR("Error getting images from IntelRealSenseTracker");
    return PLUS_FAIL;
  }

  int imageWidth=this->MT->mtGetXResolution(-1);
  int imageHeight=this->MT->mtGetYResolution(-1);

  if (leftImage != NULL)
  {
    vtkSmartPointer<vtkImageImport> imageImport=vtkSmartPointer<vtkImageImport>::New();
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetImportVoidPointer((unsigned char*)leftImageArray);
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetDataExtent(0,imageWidth-1, 0,imageHeight-1, 0,0);
    imageImport->SetWholeExtent(0,imageWidth-1, 0,imageHeight-1, 0,0);
    imageImport->Update();
    leftImage->DeepCopy(imageImport->GetOutput());
  }

  if (rightImage != NULL)
  {
    vtkSmartPointer<vtkImageImport> imageImport=vtkSmartPointer<vtkImageImport>::New();
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetImportVoidPointer((unsigned char*)rightImageArray);
    imageImport->SetDataScalarTypeToUnsignedChar();
    imageImport->SetDataExtent(0,imageWidth-1, 0,imageHeight-1, 0,0);
    imageImport->SetWholeExtent(0,imageWidth-1, 0,imageHeight-1, 0,0);
    imageImport->Update();
    rightImage->DeepCopy(imageImport->GetOutput());
  }
  */
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(CameraCalibrationFile, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceName, deviceConfig);



  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkPlusDataSource *tool = it->second;
    /*
    ->GetSourceId()) != identifiedToolSourceIds.end())
    {
    // this tool has been found and update has been already called with the correct transform
    LOG_TRACE("Tool " << it->second->GetSourceId() << ": found");
    continue;
    }
    */
  }

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
      LOG_ERROR("Failed to initialize NDI tool: DataSource Id is missing");
      continue;
    }
    if (toolDataElement->GetAttribute("MapFile") != NULL)
    {
      std::string mapFile = toolDataElement->GetAttribute("MapFile");
      std::string mapFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(mapFile);
      std::wstring mapFilePathW;
      StringToWString(mapFilePathW, mapFilePath);
      this->Internal->Targets.push_back(Model((pxcCHAR*)mapFilePathW.c_str()));
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  trackerConfig->SetAttribute("CameraCalibrationFile", this->CameraCalibrationFile.c_str()); 
  trackerConfig->SetAttribute("DeviceName", this->DeviceName.c_str());  
  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::InternalConnect()
{ 
  if (this->IsTrackingInitialized)
  {
    LOG_DEBUG("Already connected to IntelRealSenseTracker");
    return PLUS_SUCCESS;
  }

  this->Internal->Session = PXCSession::CreateInstance();
  if (!this->Internal->Session)
  {
    LOG_ERROR("Failed to create an SDK session");
    return PLUS_FAIL;
  }

  this->Internal->SenseMgr = this->Internal->Session->CreateSenseManager();
  if (!this->Internal->SenseMgr)
  {
    LOG_ERROR("Failed to create an SDK SenseManager");
    return PLUS_FAIL;
  }

  /* Set Mode & Source */
  pxcStatus sts = PXC_STATUS_NO_ERROR;
  this->Internal->CaptureMgr = this->Internal->SenseMgr->QueryCaptureManager(); //no need to Release it is released with senseMgr

  // Live streaming
  //pxcCHAR* device = this->DeviceName.c_str();
  PXCCapture::DeviceInfo dinfo;
  this->Internal->GetDeviceInfo(0, dinfo);
  pxcCHAR* device = dinfo.name;
  this->Internal->CaptureMgr->FilterByDeviceInfo(device, 0, 0);

  bool stsFlag = true;

  /* Set Module */
  sts = this->Internal->SenseMgr->EnableTracker();
  if (sts < PXC_STATUS_NO_ERROR)
  {
    LOG_ERROR("Failed to enable tracking module");
    return PLUS_FAIL;
  }

  // Init
  LOG_DEBUG("Init Started");

  this->Internal->Tracker = this->Internal->SenseMgr->QueryTracker();
  if (this->Internal->Tracker == NULL)
  {
    LOG_ERROR("Failed to Query tracking module");
    return PLUS_FAIL;
  }

  if (!this->CameraCalibrationFile.empty())
  {
    std::string cameraCalibrationFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->CameraCalibrationFile);
    LOG_DEBUG("Use IntelRealSenseTracker ini file: " << cameraCalibrationFilePath);
    if (!vtksys::SystemTools::FileExists(cameraCalibrationFilePath.c_str(), true))
    {
      LOG_WARNING("Unable to find IntelRealSenseTracker camera calibration file at: " << cameraCalibrationFilePath);
    }
    std::wstring cameraCalibrationFileW;
    StringToWString(cameraCalibrationFileW, cameraCalibrationFilePath);
    if (this->Internal->Tracker->SetCameraParameters(cameraCalibrationFileW.c_str()) != PXC_STATUS_NO_ERROR)
    {
      LOG_WARNING("Warning: failed to load camera calibration");
    }
  }

  if (this->Internal->SenseMgr->Init() < PXC_STATUS_NO_ERROR)
  {
    LOG_ERROR("senseMgr->Init failed");
    return PLUS_FAIL;
  }

  this->Internal->Projection = this->Internal->SenseMgr->QueryCaptureManager()->QueryDevice()->CreateProjection();

  for (size_t i = 0; i < this->Internal->Targets.size(); i++)
  {
    this->Internal->Targets[i].cosIDs.clear();

    pxcUID firstID, lastID;
    sts = this->Internal->Tracker->Set3DTrack(this->Internal->Targets[i].model_filename, firstID, lastID);
    while (firstID <= lastID)
    {
      PXCTracker::TrackingValues vals;
      this->Internal->Tracker->QueryTrackingValues(firstID, vals);
      this->Internal->Targets[i].addCosID(firstID, vals.targetName);
      firstID++;
    }

    if (sts < PXC_STATUS_NO_ERROR)
    {
      LOG_ERROR("Failed to set tracking configuration");
      return PLUS_FAIL;
    }
  }



  /*
  if (this->MT->mtInit(iniFilePath)!=1)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera");
    return PLUS_FAIL;
  }

  // Try to attach the cameras till find the cameras
  if (this->MT->mtSetupCameras()!=1)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera: setup cameras failed. Check the camera connections and INI and Markers file locations.");
    this->MT->mtEnd();
    return PLUS_FAIL;
  }

  int numOfCameras = this->MT->mtGetNumOfCameras();
  if (numOfCameras==0)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera: no cameras attached. Check the camera connections and INI and Markers file locations.");
    this->MT->mtEnd();
    return PLUS_FAIL;
  }
  LOG_DEBUG("Number of attached cameras: " << numOfCameras );
  for (int i=0; i<numOfCameras; i++)
  {
    LOG_DEBUG("Camera "<<i<<": "
      <<this->MT->mtGetXResolution(i)<<"x"<<this->MT->mtGetYResolution(i)<<", "
      <<this->MT->mtGetNumOfSensors(i)<<" sensors "
      <<"(serial number: "<<this->MT->mtGetSerialNum(i)<<")");
  }
  
  if (RefreshMarkerTemplates()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Error in initializing Intel RealSense Camera: Failed to load marker templates. Check if the marker directory is set correctly.");
    this->MT->mtEnd();
    return PLUS_FAIL;
  }

#ifdef USE_INTELREALSENSE_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif
  */
  this->IsTrackingInitialized=1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseTracker::InternalDisconnect()
{ 
  if (this->IsTrackingInitialized)
  {
    if (this->Internal->Projection)
    {
      this->Internal->Projection->Release();
      this->Internal->Projection = NULL;
    }
    this->Internal->SenseMgr->Close();
    this->Internal->SenseMgr->Release();
    this->IsTrackingInitialized=false;
  }  
  return PLUS_SUCCESS;
}

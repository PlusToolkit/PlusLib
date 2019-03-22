/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusOptiTrack.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkXMLDataElement.h>

// Motive API includes
#include <NatNetClient.h>
#include <NatNetTypes.h>
#include <NPTrackingTools.h>

// std includes
#include <set>

vtkStandardNewMacro(vtkPlusOptiTrack);

//----------------------------------------------------------------------------
class vtkPlusOptiTrack::vtkInternal
{
public:
  vtkPlusOptiTrack* External;

  vtkInternal(vtkPlusOptiTrack* external)
    : External(external)
  {
    this->NNClient = NULL;
    this->UnitsToMm = 1.0;
    this->MotiveDataDescriptionsUpdateTimeSec = 1.0;
    this->LastMotiveDataDescriptionsUpdateTimestamp = -1;
  }

  virtual ~vtkInternal()
  {
  }

  // NatNet client, parameters, and callback function
  NatNetClient* NNClient;
  float UnitsToMm;

  // Motive Files
#if MOTIVE_VERSION_MAJOR >= 2
  std::string Profile;
#else
  std::string ProjectFile;
#endif

  std::string CalibrationFile;
  std::vector<std::string> AdditionalRigidBodyFiles;

  // Maps rigid body names to transform names
  std::map<int, igsioTransformName> MapRBNameToTransform;

  // Flag to run Motive in background if user doesn't need GUI
  bool AttachToRunningMotive;

  // Time of last tool update
  double LastMotiveDataDescriptionsUpdateTimestamp;
  double MotiveDataDescriptionsUpdateTimeSec;

  /*!
  Print user friendly Motive API message to console
  */
  std::string GetMotiveErrorMessage(NPRESULT result);

  /*!
  Receive updated tracking information from the server and push the new transforms to the tools
  */
  static void InternalCallback(sFrameOfMocapData* data, void* pUserData);

  void UpdateMotiveDataDescriptions();
};

//-----------------------------------------------------------------------
std::string vtkPlusOptiTrack::vtkInternal::GetMotiveErrorMessage(NPRESULT result)
{
  return std::string(TT_GetResultString(result));
}

//-----------------------------------------------------------------------
void vtkPlusOptiTrack::vtkInternal::UpdateMotiveDataDescriptions()
{
  LOG_TRACE("vtkPlusOptiTrack::vtkInternal::MatchTrackedTools");

  std::string referenceFrame = this->External->GetToolReferenceFrameName();
  this->MapRBNameToTransform.clear();
  sDataDescriptions* dataDescriptions;
  this->NNClient->GetDataDescriptions(&dataDescriptions);
  for (int i = 0; i < dataDescriptions->nDataDescriptions; ++i)
  {
    sDataDescription currentDescription = dataDescriptions->arrDataDescriptions[i];
    if (currentDescription.type == Descriptor_RigidBody)
    {
      // Map the numerical ID of the tracked tool from motive to the name of the tool
      igsioTransformName toolToTracker = igsioTransformName(currentDescription.Data.RigidBodyDescription->szName, referenceFrame);
      this->MapRBNameToTransform[currentDescription.Data.RigidBodyDescription->ID] = toolToTracker;
    }
  }

  this->LastMotiveDataDescriptionsUpdateTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
}

//-----------------------------------------------------------------------
vtkPlusOptiTrack::vtkPlusOptiTrack()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{
  this->FrameNumber = 0;
  // always uses NatNet's callback to update
  this->InternalUpdateRate = 120;
  this->StartThreadForInternalUpdates = false;
}

//----------------------------------------------------------------------------
vtkPlusOptiTrack::~vtkPlusOptiTrack()
{
  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusOptiTrack::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusOptiTrack::ReadConfiguration")
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

#if MOTIVE_VERSION_MAJOR >= 2
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(Profile, this->Internal->Profile, deviceConfig);
#else
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(ProjectFile, this->Internal->ProjectFile, deviceConfig);
#endif
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_REQUIRED(AttachToRunningMotive, this->Internal->AttachToRunningMotive, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(double, MotiveDataDescriptionsUpdateTimeSec, this->Internal->MotiveDataDescriptionsUpdateTimeSec, deviceConfig);

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

    std::string toolId(toolDataElement->GetAttribute("Id"));
    if (toolId.empty())
    {
      // tool doesn't have ID needed to generate transform
      LOG_ERROR("Failed to initialize OptiTrack tool: DataSource Id is missing. This should be the name of the Motive Rigid Body that tracks the tool.");
      continue;
    }

    if (toolDataElement->GetAttribute("RigidBodyFile") != NULL)
    {
      // this tool has an associated rigid body definition
      const char* rigidBodyFile = toolDataElement->GetAttribute("RigidBodyFile");
      this->Internal->AdditionalRigidBodyFiles.push_back(rigidBodyFile);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusOptiTrack::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::Probe()
{
  LOG_TRACE("vtkPlusOptiTrack::Probe");
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalConnect()
{
  LOG_TRACE("vtkPlusOptiTrack::InternalConnect");
  if (!this->Internal->AttachToRunningMotive)
  {
    // RUN MOTIVE IN BACKGROUND
    // initialize the API
    if (TT_Initialize() != NPRESULT_SUCCESS)
    {
      LOG_ERROR("Failed to start Motive.");
      return PLUS_FAIL;
    }

    // pick up recently-arrived cameras
    TT_Update();

    // open project file
#if MOTIVE_VERSION_MAJOR >= 2
    std::string profilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->Internal->Profile);
    NPRESULT ttpLoad = TT_LoadProfile(profilePath.c_str());
#else
    std::string projectFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->Internal->ProjectFile);
    NPRESULT ttpLoad = TT_LoadProject(projectFilePath.c_str());
#endif
    if (ttpLoad != NPRESULT_SUCCESS)
    {
      LOG_ERROR("Failed to load Motive project file. Motive error: " << TT_GetResultString(ttpLoad));
      return PLUS_FAIL;
    }

    // add any additional rigid body files to project
    std::string rbFilePath;
    for (auto it = this->Internal->AdditionalRigidBodyFiles.begin(); it != this->Internal->AdditionalRigidBodyFiles.end(); it++)
    {
      rbFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(*it);
      NPRESULT addRBResult = TT_AddRigidBodies(rbFilePath.c_str());
      if (addRBResult != NPRESULT_SUCCESS)
      {
        LOG_ERROR("Failed to load rigid body file located at: " << rbFilePath << ". Motive error message: " << this->Internal->GetMotiveErrorMessage(addRBResult));
        return PLUS_FAIL;
      }
    }

    LOG_INFO("\n---------------------------------MOTIVE SETTINGS--------------------------------")
      // list connected cameras
      LOG_INFO("Connected cameras:")
      for (int i = 0; i < TT_CameraCount(); i++)
      {
        LOG_INFO(i << ": " << TT_CameraName(i));
      }
    // list project file
#if MOTIVE_VERSION_MAJOR >= 2
    LOG_INFO("\nUsing Motive profile located at:\n" << profilePath);
#else
    LOG_INFO("\nUsing Motive project file located at:\n" << projectFilePath);
#endif
    // list rigid bodies
    LOG_INFO("\nTracked rigid bodies:");
    for (int i = 0; i < TT_RigidBodyCount(); ++i)
    {
      LOG_INFO(TT_RigidBodyName(i));
    }
    LOG_INFO("--------------------------------------------------------------------------------\n");

    this->StartThreadForInternalUpdates = true;
  }

  // CONFIGURE NATNET CLIENT
  this->Internal->NNClient = new NatNetClient(ConnectionType_Multicast);
  this->Internal->NNClient->SetVerbosityLevel(Verbosity_None);
  this->Internal->NNClient->SetVerbosityLevel(Verbosity_Warning);
  this->Internal->NNClient->SetDataCallback(vtkPlusOptiTrack::vtkInternal::InternalCallback, this);

  int retCode = this->Internal->NNClient->Initialize("127.0.0.1", "127.0.0.1");

  void* response;
  int nBytes;
  if (this->Internal->NNClient->SendMessageAndWait("UnitsToMillimeters", &response, &nBytes) == ErrorCode_OK)
  {
    this->Internal->UnitsToMm = (*(float*)response);
  }
  else
  {
    // Fail if motive is not running
    LOG_ERROR("Failed to connect to Motive. Please either set AttachToRunningMotive=FALSE or ensure that Motive is running and streaming is enabled.");
    return PLUS_FAIL;
  }

  // verify all rigid bodies in Motive have unique names
  std::set<std::string> rigidBodies;
  sDataDescriptions* dataDescriptions;
  this->Internal->NNClient->GetDataDescriptions(&dataDescriptions);
  for (int i = 0; i < dataDescriptions->nDataDescriptions; ++i)
  {
    sDataDescription currentDescription = dataDescriptions->arrDataDescriptions[i];
    if (currentDescription.type == Descriptor_RigidBody)
    {
      // Map the numerical ID of the tracked tool from motive to the name of the tool
      if (!rigidBodies.insert(currentDescription.Data.RigidBodyDescription->szName).second)
      {
        LOG_ERROR("Duplicate rigid bodies with name: " << currentDescription.Data.RigidBodyDescription->szName);
        return PLUS_FAIL;
      }
    }
  }

  // cause update of tools from Motive
  this->Internal->LastMotiveDataDescriptionsUpdateTimestamp = -1;

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalDisconnect()
{
  LOG_TRACE("vtkPlusOptiTrack::InternalDisconnect")
  if (!this->Internal->AttachToRunningMotive)
  {
    TT_Shutdown();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalStartRecording()
{
  LOG_TRACE("vtkPlusOptiTrack::InternalStartRecording")
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalUpdate()
{
  LOG_TRACE("vtkPlusOptiTrack::InternalUpdate");
  // internal update is only called if usign Motive API
  TT_Update();
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
void vtkPlusOptiTrack::vtkInternal::InternalCallback(sFrameOfMocapData* data, void* pUserData)
{
  vtkPlusOptiTrack* self = (vtkPlusOptiTrack*)pUserData;

  LOG_TRACE("vtkPlusOptiTrack::InternalCallback");
  const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  if (self->Internal->LastMotiveDataDescriptionsUpdateTimestamp < 0)
  {
    // do an initial match of tracked tools
    self->Internal->UpdateMotiveDataDescriptions();
  }

  if (self->Internal->AttachToRunningMotive && self->Internal->MotiveDataDescriptionsUpdateTimeSec >= 0)
  {
    double timeSinceMotiveDataDescriptionsUpdate = unfilteredTimestamp - self->Internal->LastMotiveDataDescriptionsUpdateTimestamp;
    if (timeSinceMotiveDataDescriptionsUpdate > self->Internal->MotiveDataDescriptionsUpdateTimeSec)
    {
      self->Internal->UpdateMotiveDataDescriptions();
    }
  }

  int numberOfRigidBodies = data->nRigidBodies;
  sRigidBodyData* rigidBodies = data->RigidBodies;

  // identity transform for tools out of view
  vtkSmartPointer<vtkMatrix4x4> rigidBodyToTrackerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  for (int rigidBodyId = 0; rigidBodyId < numberOfRigidBodies; ++rigidBodyId)
  {
    // TOOL IN VIEW
    rigidBodyToTrackerMatrix->Identity();
    sRigidBodyData currentRigidBody = rigidBodies[rigidBodyId];

    if (currentRigidBody.MeanError != 0)
    {
      // convert translation to mm
      double translation[3] = { currentRigidBody.x * self->Internal->UnitsToMm, currentRigidBody.y * self->Internal->UnitsToMm, currentRigidBody.z * self->Internal->UnitsToMm };

      // convert rotation from quaternion to 3x3 matrix
      double quaternion[4] = { currentRigidBody.qw, currentRigidBody.qx, currentRigidBody.qy, currentRigidBody.qz };
      double rotation[3][3] = { 0,0,0, 0,0,0, 0,0,0 };
      vtkMath::QuaternionToMatrix3x3(quaternion, rotation);

      // construct the transformation matrix from the rotation and translation components
      for (int i = 0; i < 3; ++i)
      {
        for (int j = 0; j < 3; ++j)
        {
          rigidBodyToTrackerMatrix->SetElement(i, j, rotation[i][j]);
        }
        rigidBodyToTrackerMatrix->SetElement(i, 3, translation[i]);
      }

      // make sure the tool was specified in the Config file
      igsioTransformName toolToTracker = self->Internal->MapRBNameToTransform[currentRigidBody.ID];
      self->ToolTimeStampedUpdate(toolToTracker.GetTransformName(), rigidBodyToTrackerMatrix, TOOL_OK, self->FrameNumber, unfilteredTimestamp);
    }
    else
    {
      // TOOL OUT OF VIEW
      igsioTransformName toolToTracker = self->Internal->MapRBNameToTransform[currentRigidBody.ID];
      self->ToolTimeStampedUpdate(toolToTracker.GetTransformName(), rigidBodyToTrackerMatrix, TOOL_OUT_OF_VIEW, self->FrameNumber, unfilteredTimestamp);
    }

  }

  self->FrameNumber++;
}

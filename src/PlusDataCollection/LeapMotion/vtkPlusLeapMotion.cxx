/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusLeapMotion.h"
#include "PlusMath.h"

// VTK includes
#include <vtkImageImport.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPlusDataSource.h>
#include <vtkQuaternion.h>
#include <vtkTransform.h>
#include <vtkXMLDataElement.h>
#include <vtksys/SystemTools.hxx>

// OS includes
#include <math.h>

// STL includes
#include <sstream>

//-------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusLeapMotion);

//-------------------------------------------------------------------------

namespace
{
  static void* allocate(uint32_t size, eLeapAllocatorType typeHint, void* state)
  {
    void* ptr = malloc(size);
    return ptr;
  }

  static void deallocate(void* ptr, void* state)
  {
    if (!ptr)
    {
      return;
    }
    free(ptr);
  }
}

//-------------------------------------------------------------------------
vtkPlusLeapMotion::vtkPlusLeapMotion()
  : PollTimeoutMs(1000)
  , Mutex(vtkIGSIORecursiveCriticalSection::New())
  , LeapHMDPolicy(false)
  , RefusePauseResumePolicy(false)
  , Initialized(false)
  , ImageInitialized(false)
  , LeftCameraSource(nullptr)
  , RightCameraSource(nullptr)
  , InvertImage(false)
{
  this->RequirePortNameInDeviceSetConfiguration = false;
  this->StartThreadForInternalUpdates = true; // polling based device
  this->AcquisitionRate = 120;
}

//-------------------------------------------------------------------------
vtkPlusLeapMotion::~vtkPlusLeapMotion()
{
  this->Mutex->Delete();
  if (this->Recording)
  {
    this->StopRecording();
  }
}

//-------------------------------------------------------------------------
void vtkPlusLeapMotion::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Poll timeout (ms)" << this->PollTimeoutMs << std::endl;
  os << "Leap HMD policy" << (this->LeapHMDPolicy ? "TRUE" : "FALSE") << std::endl;
  os << "Override pause/resume policy" << (this->RefusePauseResumePolicy ? "TRUE" : "FALSE") << std::endl;
  os << "Invert image:" << (this->InvertImage ? "TRUE" : "FALSE") << std::endl;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(LeapHMDPolicy, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(RefusePauseResumePolicy, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(InvertImage, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  XML_WRITE_BOOL_ATTRIBUTE(LeapHMDPolicy, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(RefusePauseResumePolicy, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(InvertImage, deviceConfig);

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalConnect()
{
  LOG_TRACE("vtkPlusLeapMotion::Connect");

  if (this->Connected)
  {
    return PLUS_SUCCESS;
  }
  eLeapRS result;
  if ((result = LeapCreateConnection(NULL, &this->Connection)) != eLeapRS_Success)
  {
    LOG_ERROR("Unable to create the connection to the LeapMotion device:" << this->ResultToString(result));
    return PLUS_FAIL;
  }
  if ((result = LeapOpenConnection(this->Connection)) != eLeapRS_Success)
  {
    LOG_ERROR("Unable to open the connection to the LeapMotion device:" << this->ResultToString(result));
    return PLUS_FAIL;
  }

  LEAP_ALLOCATOR allocator = { allocate, deallocate, NULL };
  if ((result = LeapSetAllocator(this->Connection, &allocator)) != eLeapRS_Success)
  {
    LOG_WARNING("Unable to set allocator. Some functionality may be missing.");
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalDisconnect()
{
  LOG_TRACE("vtkPlusLeapMotion::Disconnect");

  LeapCloseConnection(this->Connection);
  LeapDestroyConnection(this->Connection);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalUpdate()
{
  LOG_TRACE("vtkPlusLeapMotion::InternalUpdate");
  eLeapRS result = LeapPollConnection(this->Connection, this->PollTimeoutMs, &this->LastMessage);

  if (result == eLeapRS_Timeout)
  {
    this->PollTimeoutMs += 2;
    return PLUS_SUCCESS;
  }

  if (result != eLeapRS_Success)
  {
    LOG_ERROR("LeapC PollConnection call error: " << this->ResultToString(result));
    return PLUS_FAIL;
  }

  if (!this->Initialized)
  {
    // Set policy flags once
    uint64_t set = eLeapPolicyFlag_Images;
    uint64_t clear = 0;
    if (this->RefusePauseResumePolicy)
    {
      clear |= eLeapPolicyFlag_AllowPauseResume;
    }
    else
    {
      set |= eLeapPolicyFlag_AllowPauseResume;
    }

    if (this->LeapHMDPolicy)
    {
      set |= eLeapPolicyFlag_OptimizeHMD;
    }
    else
    {
      clear |= eLeapPolicyFlag_OptimizeHMD;
    }

    if ((result = LeapSetPolicyFlags(this->Connection, set, clear)) != eLeapRS_Success)
    {
      LOG_WARNING("Unable to set HMD policy flag, tracking will be greatly degraded if attached to an HMD: " << this->ResultToString(result));
    }
    this->Initialized = true;
  }

  switch (this->LastMessage.type)
  {
    case eLeapEventType_Connection:
      return this->OnConnectionEvent(this->LastMessage.connection_event);
    case eLeapEventType_ConnectionLost:
      return this->OnConnectionLostEvent(this->LastMessage.connection_lost_event);
    case eLeapEventType_Device:
      return this->OnDeviceEvent(this->LastMessage.device_event);
    case eLeapEventType_DeviceLost:
      return this->OnDeviceLostEvent(this->LastMessage.device_event);
    case eLeapEventType_DeviceFailure:
      return this->OnDeviceFailureEvent(this->LastMessage.device_failure_event);
    case eLeapEventType_Tracking:
      return this->OnTrackingEvent(this->LastMessage.tracking_event);
    case eLeapEventType_LogEvent:
      return this->OnLogEvent(this->LastMessage.log_event);
    case eLeapEventType_Policy:
      return this->OnPolicyEvent(this->LastMessage.policy_event);
    case eLeapEventType_ConfigChange:
      return this->OnConfigChangeEvent(this->LastMessage.config_change_event);
    case eLeapEventType_ConfigResponse:
      return this->OnConfigResponseEvent(this->LastMessage.config_response_event);
    case eLeapEventType_Image:
      return this->OnImageEvent(this->LastMessage.image_event);
    case eLeapEventType_PointMappingChange:
      return this->OnPointMappingChangeEvent(this->LastMessage.point_mapping_change_event);
    case eLeapEventType_LogEvents:
      return this->OnLogEvents(this->LastMessage.log_events);
    case eLeapEventType_HeadPose:
      return this->OnHeadPoseEvent(this->LastMessage.head_pose_event);
    case eLeapEventType_ImageComplete:
    case eLeapEventType_ImageRequestError:
      return PLUS_SUCCESS;
    default:
      LOG_WARNING("Unhandled event type " << EventToString(this->LastMessage.type));
  }

  return PLUS_FAIL;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::Probe()
{
  LOG_TRACE("vtkPlusLeapMotion::Probe");

  if (this->InternalConnect() == PLUS_SUCCESS)
  {
    return this->InternalDisconnect();
  }
  return PLUS_FAIL;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalStartRecording()
{
  LOG_TRACE("vtkPlusLeapMotion::InternalStartRecording");

  eLeapRS result = LeapOpenConnection(this->Connection);
  if (result == eLeapRS_Success)
  {
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalStopRecording()
{
  LOG_TRACE("vtkPlusLeapMotion::InternalStopRecording");

  LeapCloseConnection(this->Connection);

  return PLUS_SUCCESS;
}

#define CHECK_DATA_SOURCE(name) if(this->GetDataSource(std::string(#name) + "To" + this->ToolReferenceFrameName, aSource) != PLUS_SUCCESS){ LOG_WARNING("Data source with ID \"" << #name << "\" doesn't exist. Joint will not be tracked.");}
//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::NotifyConfigured()
{
  vtkPlusDataSource* aSource(nullptr);

  // Check for 19 data sources per hand
  CHECK_DATA_SOURCE(LeftThumbProximal);
  CHECK_DATA_SOURCE(LeftThumbIntermediate);
  CHECK_DATA_SOURCE(LeftThumbDistal);

  CHECK_DATA_SOURCE(LeftIndexMetacarpal);
  CHECK_DATA_SOURCE(LeftIndexProximal);
  CHECK_DATA_SOURCE(LeftIndexIntermediate);
  CHECK_DATA_SOURCE(LeftIndexDistal);

  CHECK_DATA_SOURCE(LeftMiddleMetacarpal);
  CHECK_DATA_SOURCE(LeftMiddleProximal);
  CHECK_DATA_SOURCE(LeftMiddleIntermediate);
  CHECK_DATA_SOURCE(LeftMiddleDistal);

  CHECK_DATA_SOURCE(LeftRingMetacarpal);
  CHECK_DATA_SOURCE(LeftRingProximal);
  CHECK_DATA_SOURCE(LeftRingIntermediate);
  CHECK_DATA_SOURCE(LeftRingDistal);

  CHECK_DATA_SOURCE(LeftPinkyMetacarpal);
  CHECK_DATA_SOURCE(LeftPinkyProximal);
  CHECK_DATA_SOURCE(LeftPinkyIntermediate);
  CHECK_DATA_SOURCE(LeftPinkyDistal);

  CHECK_DATA_SOURCE(RightThumbProximal);
  CHECK_DATA_SOURCE(RightThumbIntermediate);
  CHECK_DATA_SOURCE(RightThumbDistal);

  CHECK_DATA_SOURCE(RightIndexMetacarpal);
  CHECK_DATA_SOURCE(RightIndexProximal);
  CHECK_DATA_SOURCE(RightIndexIntermediate);
  CHECK_DATA_SOURCE(RightIndexDistal);

  CHECK_DATA_SOURCE(RightMiddleMetacarpal);
  CHECK_DATA_SOURCE(RightMiddleProximal);
  CHECK_DATA_SOURCE(RightMiddleIntermediate);
  CHECK_DATA_SOURCE(RightMiddleDistal);

  CHECK_DATA_SOURCE(RightRingMetacarpal);
  CHECK_DATA_SOURCE(RightRingProximal);
  CHECK_DATA_SOURCE(RightRingIntermediate);
  CHECK_DATA_SOURCE(RightRingDistal);

  CHECK_DATA_SOURCE(RightPinkyMetacarpal);
  CHECK_DATA_SOURCE(RightPinkyProximal);
  CHECK_DATA_SOURCE(RightPinkyIntermediate);
  CHECK_DATA_SOURCE(RightPinkyDistal);

  CHECK_DATA_SOURCE(LeftPalm);
  CHECK_DATA_SOURCE(RightPalm);

  if (this->GetNumberOfTools() < 1)
  {
    LOG_ERROR("Must record at least one joint/palm. Please add a data source.");
    return PLUS_FAIL;
  }

  if (this->OutputChannelCount() < 1)
  {
    LOG_ERROR("Device must have at least one output channel.");
    return PLUS_FAIL;
  }

  if (this->GetNumberOfVideoSources() >= 2)
  {
    if (this->GetVideoSourceByIndex(0, this->LeftCameraSource) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    if (this->GetVideoSourceByIndex(1, this->RightCameraSource) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  // Add dummy transform into tool so that plus server doesn't complain about inability to retrieve a timestamp
  vtkNew<vtkMatrix4x4> mat;
  this->ToolTimeStampedUpdate(this->Tools.begin()->second->GetSourceId(), mat, TOOL_INVALID, 0, UNDEFINED_TIMESTAMP);

  this->PollTimeoutMs = 1000.0 / this->AcquisitionRate;
  return PLUS_SUCCESS;
}
#undef CHECK_DATA_SOURCE

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::ToolTimeStampedUpdateBone(std::string boneName, eLeapHandType handIndex, Finger fingerIndex, Bone boneIndex)
{
  vtkPlusDataSource* aSource(nullptr);
  if (this->GetDataSource(boneName + "To" + this->ToolReferenceFrameName, aSource) == PLUS_SUCCESS)
  {
    vtkNew<vtkTransform> pose;
    bool found(false);
    for (uint32_t i = 0; i < this->LastTrackingEvent.nHands; ++i)
    {
      LEAP_HAND& hand = this->LastTrackingEvent.pHands[i];
      if (hand.type != handIndex)
      {
        continue;
      }
      LEAP_BONE& bone = hand.digits[fingerIndex].bones[boneIndex];

      vtkQuaternion<float> orientation;
      orientation.Set(bone.rotation.w, bone.rotation.x, bone.rotation.y, bone.rotation.z);
      float axis[3];
      float angle = orientation.GetRotationAngleAndAxis(axis);
      pose->Translate(bone.next_joint.x, bone.next_joint.y, bone.next_joint.z);
      pose->RotateWXYZ(vtkMath::DegreesFromRadians(angle), axis);
      float x = bone.next_joint.x - bone.prev_joint.x;
      float y = bone.next_joint.y - bone.prev_joint.y;
      float z = bone.next_joint.z - bone.prev_joint.z;
      float mag = std::sqrtf(x * x + y * y + z * z);
      igsioFieldMapType fields;
      {
        std::stringstream ss;
        ss << mag;
        fields[boneName + "To" + this->ToolReferenceFrameName + "lengthMm"].first = FRAMEFIELD_FORCE_SERVER_SEND;
        fields[boneName + "To" + this->ToolReferenceFrameName + "lengthMm"].second = ss.str();
      }

      {
        std::stringstream ss;
        ss << bone.width;
        fields[boneName + "To" + this->ToolReferenceFrameName + "radiusMm"].first = FRAMEFIELD_FORCE_SERVER_SEND;
        fields[boneName + "To" + this->ToolReferenceFrameName + "radiusMm"].second = ss.str();
      }

      if (this->ToolTimeStampedUpdate(boneName + "To" + this->ToolReferenceFrameName, pose->GetMatrix(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP, &fields) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to record " << boneName << " transform.");
        return PLUS_FAIL;
      }

      return PLUS_SUCCESS;
    }

    if (!found)
    {
      vtkNew<vtkMatrix4x4> mat;
      this->ToolTimeStampedUpdate(boneName + "To" + this->ToolReferenceFrameName, mat, TOOL_OUT_OF_VIEW, this->FrameNumber, UNDEFINED_TIMESTAMP);
    }

    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::ToolTimeStampedUpdatePalm(const std::string& name, eLeapHandType handType)
{
  vtkPlusDataSource* aSource(nullptr);
  if (this->GetDataSource(name + "To" + this->ToolReferenceFrameName, aSource) == PLUS_SUCCESS)
  {
    vtkNew<vtkTransform> pose;
    bool found(false);
    for (uint32_t i = 0; i < this->LastTrackingEvent.nHands; ++i)
    {
      LEAP_HAND& hand = this->LastTrackingEvent.pHands[i];
      if (hand.type != handType)
      {
        continue;
      }
      LEAP_PALM& palm = hand.palm;
      vtkQuaternion<float> orientation;
      orientation.Set(palm.orientation.w, palm.orientation.x, palm.orientation.y, palm.orientation.z);
      pose->Translate(palm.position.x, palm.position.y, palm.position.z);
      float axis[3];
      float angle = orientation.GetRotationAngleAndAxis(axis);
      pose->RotateWXYZ(angle, axis);
      if (this->ToolTimeStampedUpdate(name + "To" + this->ToolReferenceFrameName, pose->GetMatrix(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to record " << name << " transform.");
        return PLUS_FAIL;
      }

      return PLUS_SUCCESS;
    }
    if (!found)
    {
      vtkNew<vtkMatrix4x4> mat;
      this->ToolTimeStampedUpdate(name + "To" + this->ToolReferenceFrameName, mat, TOOL_OUT_OF_VIEW, this->FrameNumber, UNDEFINED_TIMESTAMP);
    }

    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusLeapMotion::SetFrame(const LEAP_TRACKING_EVENT* trackingEvent)
{
  this->Mutex->Lock();
  this->LastTrackingEvent = *trackingEvent;
  this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
LEAP_TRACKING_EVENT* vtkPlusLeapMotion::GetFrame()
{
  return &this->LastTrackingEvent;
}

//----------------------------------------------------------------------------
void vtkPlusLeapMotion::SetHeadPose(const LEAP_HEAD_POSE_EVENT* headPose)
{
  this->Mutex->Lock();
  this->LastHeadPoseEvent = *headPose;
  this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
LEAP_HEAD_POSE_EVENT* vtkPlusLeapMotion::GetHeadPose()
{
  return &this->LastHeadPoseEvent;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnConnectionEvent(const LEAP_CONNECTION_EVENT* connectionEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnConnectionLostEvent(const LEAP_CONNECTION_LOST_EVENT* connectionLostEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnDeviceEvent(const LEAP_DEVICE_EVENT* deviceEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnDeviceLostEvent(const LEAP_DEVICE_EVENT* deviceEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnDeviceFailureEvent(const LEAP_DEVICE_FAILURE_EVENT* deviceFailureEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnPolicyEvent(const LEAP_POLICY_EVENT* policyEvent)
{
  if ((policyEvent->current_policy & eLeapPolicyFlag_OptimizeHMD) == (this->LeapHMDPolicy ? eLeapPolicyFlag_OptimizeHMD : 0) &&
      (policyEvent->current_policy & eLeapPolicyFlag_AllowPauseResume) == (this->RefusePauseResumePolicy ? 0 : eLeapPolicyFlag_AllowPauseResume) &&
      (policyEvent->current_policy & eLeapPolicyFlag_Images) == eLeapPolicyFlag_Images)
  {
    LOG_INFO("Successfully changed policy.");
    return PLUS_SUCCESS;
  }
  else
  {
    LOG_ERROR("Unable to change policy.");
    return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnTrackingEvent(const LEAP_TRACKING_EVENT* trackingEvent)
{
  this->SetFrame(trackingEvent);

  // Left
  ToolTimeStampedUpdateBone("LeftThumbProximal", eLeapHandType_Left, Finger_Thumb, Bone_Proximal);
  ToolTimeStampedUpdateBone("LeftThumbIntermediate", eLeapHandType_Left, Finger_Thumb, Bone_Intermediate);
  ToolTimeStampedUpdateBone("LeftThumbDistal", eLeapHandType_Left, Finger_Thumb, Bone_Distal);

  ToolTimeStampedUpdateBone("LeftIndexMetacarpal", eLeapHandType_Left, Finger_Index, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("LeftIndexProximal", eLeapHandType_Left, Finger_Index, Bone_Proximal);
  ToolTimeStampedUpdateBone("LeftIndexIntermediate", eLeapHandType_Left, Finger_Index, Bone_Intermediate);
  ToolTimeStampedUpdateBone("LeftIndexDistal", eLeapHandType_Left, Finger_Index, Bone_Distal);

  ToolTimeStampedUpdateBone("LeftMiddleMetacarpal", eLeapHandType_Left, Finger_Middle, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("LeftMiddleProximal", eLeapHandType_Left, Finger_Middle, Bone_Proximal);
  ToolTimeStampedUpdateBone("LeftMiddleIntermediate", eLeapHandType_Left, Finger_Middle, Bone_Intermediate);
  ToolTimeStampedUpdateBone("LeftMiddleDistal", eLeapHandType_Left, Finger_Middle, Bone_Distal);

  ToolTimeStampedUpdateBone("LeftRingMetacarpal", eLeapHandType_Left, Finger_Ring, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("LeftRingProximal", eLeapHandType_Left, Finger_Ring, Bone_Proximal);
  ToolTimeStampedUpdateBone("LeftRingIntermediate", eLeapHandType_Left, Finger_Ring, Bone_Intermediate);
  ToolTimeStampedUpdateBone("LeftRingDistal", eLeapHandType_Left, Finger_Ring, Bone_Distal);

  ToolTimeStampedUpdateBone("LeftPinkyMetacarpal", eLeapHandType_Left, Finger_Pinky, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("LeftPinkyProximal", eLeapHandType_Left, Finger_Pinky, Bone_Proximal);
  ToolTimeStampedUpdateBone("LeftPinkyIntermediate", eLeapHandType_Left, Finger_Pinky, Bone_Intermediate);
  ToolTimeStampedUpdateBone("LeftPinkyDistal", eLeapHandType_Left, Finger_Pinky, Bone_Distal);

  // Right
  ToolTimeStampedUpdateBone("RightThumbProximal", eLeapHandType_Right, Finger_Thumb, Bone_Proximal);
  ToolTimeStampedUpdateBone("RightThumbIntermediate", eLeapHandType_Right, Finger_Thumb, Bone_Intermediate);
  ToolTimeStampedUpdateBone("RightThumbDistal", eLeapHandType_Right, Finger_Thumb, Bone_Distal);

  ToolTimeStampedUpdateBone("RightIndexMetacarpal", eLeapHandType_Right, Finger_Index, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("RightIndexProximal", eLeapHandType_Right, Finger_Index, Bone_Proximal);
  ToolTimeStampedUpdateBone("RightIndexIntermediate", eLeapHandType_Right, Finger_Index, Bone_Intermediate);
  ToolTimeStampedUpdateBone("RightIndexDistal", eLeapHandType_Right, Finger_Index, Bone_Distal);

  ToolTimeStampedUpdateBone("RightMiddleMetacarpal", eLeapHandType_Right, Finger_Middle, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("RightMiddleProximal", eLeapHandType_Right, Finger_Middle, Bone_Proximal);
  ToolTimeStampedUpdateBone("RightMiddleIntermediate", eLeapHandType_Right, Finger_Middle, Bone_Intermediate);
  ToolTimeStampedUpdateBone("RightMiddleDistal", eLeapHandType_Right, Finger_Middle, Bone_Distal);

  ToolTimeStampedUpdateBone("RightRingMetacarpal", eLeapHandType_Right, Finger_Ring, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("RightRingProximal", eLeapHandType_Right, Finger_Ring, Bone_Proximal);
  ToolTimeStampedUpdateBone("RightRingIntermediate", eLeapHandType_Right, Finger_Ring, Bone_Intermediate);
  ToolTimeStampedUpdateBone("RightRingDistal", eLeapHandType_Right, Finger_Ring, Bone_Distal);

  ToolTimeStampedUpdateBone("RightPinkyMetacarpal", eLeapHandType_Right, Finger_Pinky, Bone_Metacarpal);
  ToolTimeStampedUpdateBone("RightPinkyProximal", eLeapHandType_Right, Finger_Pinky, Bone_Proximal);
  ToolTimeStampedUpdateBone("RightPinkyIntermediate", eLeapHandType_Right, Finger_Pinky, Bone_Intermediate);
  ToolTimeStampedUpdateBone("RightPinkyDistal", eLeapHandType_Right, Finger_Pinky, Bone_Distal);

  ToolTimeStampedUpdatePalm("LeftPalm", eLeapHandType_Left);
  ToolTimeStampedUpdatePalm("RightPalm", eLeapHandType_Right);

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnLogEvent(const LEAP_LOG_EVENT* logEvent)
{
  switch (logEvent->severity)
  {
    case eLeapLogSeverity_Unknown:
    case eLeapLogSeverity_Information:
      LOG_INFO(logEvent->message);
      return PLUS_SUCCESS;
    case eLeapLogSeverity_Warning:
      LOG_WARNING(logEvent->message);
      return PLUS_SUCCESS;
    case eLeapLogSeverity_Critical:
      LOG_ERROR(logEvent->message);
      return PLUS_SUCCESS;
    default:
      return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnLogEvents(const LEAP_LOG_EVENTS* logEvents)
{
  for (uint32_t i = 0; i < logEvents->nEvents; ++i)
  {
    LEAP_LOG_EVENT& event = logEvents->events[i];
    switch (event.severity)
    {
      case eLeapLogSeverity_Unknown:
      case eLeapLogSeverity_Information:
        LOG_INFO(event.message);
        break;
      case eLeapLogSeverity_Warning:
        LOG_WARNING(event.message);
        break;
      case eLeapLogSeverity_Critical:
        LOG_ERROR(event.message);
        break;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnConfigChangeEvent(const LEAP_CONFIG_CHANGE_EVENT* configChangeEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnConfigResponseEvent(const LEAP_CONFIG_RESPONSE_EVENT* configResponseEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnImageEvent(const LEAP_IMAGE_EVENT* imageEvent)
{
  if (this->GetNumberOfVideoSources() >= 2)
  {
    FrameSizeType leftFrameSize{ imageEvent->image[0].properties.width, imageEvent->image[0].properties.height, 1 };
    FrameSizeType rightFrameSize{ imageEvent->image[1].properties.width, imageEvent->image[1].properties.height, 1 };
    if (!this->ImageInitialized)
    {
      this->LeftCameraSource->SetInputImageOrientation(US_IMG_ORIENT_MN);
      this->LeftCameraSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);
      this->RightCameraSource->SetInputImageOrientation(US_IMG_ORIENT_MN);
      this->RightCameraSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);

      // First image received, set up the data sources
      this->LeftCameraSource->SetInputFrameSize(leftFrameSize);
      this->RightCameraSource->SetInputFrameSize(rightFrameSize);

      this->LeftCameraSource->SetPixelType(VTK_UNSIGNED_CHAR);
      this->RightCameraSource->SetPixelType(VTK_UNSIGNED_CHAR);

      if (imageEvent->image[0].properties.format == eLeapImageFormat_RGBIr_Bayer)
      {
        this->LeftCameraSource->SetImageType(US_IMG_RGB_COLOR);
      }
      else
      {
        this->LeftCameraSource->SetImageType(US_IMG_BRIGHTNESS);
      }

      if (imageEvent->image[1].properties.format == eLeapImageFormat_RGBIr_Bayer)
      {
        this->RightCameraSource->SetImageType(US_IMG_RGB_COLOR);
      }
      else
      {
        this->RightCameraSource->SetImageType(US_IMG_BRIGHTNESS);
      }

      this->LeftCameraSource->SetNumberOfScalarComponents(imageEvent->image[0].properties.bpp);
      this->RightCameraSource->SetNumberOfScalarComponents(imageEvent->image[1].properties.bpp);

      this->ImageInitialized = true;
    }

    if (this->InvertImage)
    {
      // Invert left image
      uint32_t byteCount = ((imageEvent->image[0].properties.format == eLeapImageFormat_RGBIr_Bayer)) ? 3 : 1 * leftFrameSize[0] * leftFrameSize[1] * leftFrameSize[2];
      for (uint32_t i = 0; i < byteCount;)
      {
        ((unsigned char*)(imageEvent->image[0].data))[i] = 255 - ((unsigned char*)(imageEvent->image[0].data))[i];
        if (imageEvent->image[1].properties.format == eLeapImageFormat_RGBIr_Bayer)
        {
          ((unsigned char*)(imageEvent->image[0].data))[i + 1] = 255 - ((unsigned char*)(imageEvent->image[0].data))[i + 1];
          ((unsigned char*)(imageEvent->image[0].data))[i + 2] = 255 - ((unsigned char*)(imageEvent->image[0].data))[i + 2];
          i = i + 3;
        }
        else
        {
          i++;
        }
      }

      // Invert right image
      byteCount = ((imageEvent->image[1].properties.format == eLeapImageFormat_RGBIr_Bayer)) ? 3 : 1 * rightFrameSize[0] * rightFrameSize[1] * rightFrameSize[2];
      for (uint32_t i = 0; i < rightFrameSize[0];)
      {
        ((unsigned char*)(imageEvent->image[1].data))[i] = 255 - ((unsigned char*)(imageEvent->image[1].data))[i];
        if (imageEvent->image[1].properties.format == eLeapImageFormat_RGBIr_Bayer)
        {
          ((unsigned char*)(imageEvent->image[1].data))[i + 1] = 255 - ((unsigned char*)(imageEvent->image[1].data))[i + 1];
          ((unsigned char*)(imageEvent->image[1].data))[i + 2] = 255 - ((unsigned char*)(imageEvent->image[1].data))[i + 2];
          i = i + 3;
        }
        else
        {
          i++;
        }
      }
    }

    this->LeftCameraSource->AddItem(imageEvent->image[0].data,
                                    US_IMG_ORIENT_MN,
                                    leftFrameSize,
                                    VTK_UNSIGNED_CHAR,
                                    imageEvent->image[0].properties.bpp,
                                    (imageEvent->image[0].properties.format == eLeapImageFormat_RGBIr_Bayer) ? US_IMG_RGB_COLOR : US_IMG_BRIGHTNESS,
                                    0,
                                    this->FrameNumber); // For now, use plus timestamps, until figure out how to offset leap timestamp by plus timestamp
    this->RightCameraSource->AddItem(imageEvent->image[0].data,
                                     US_IMG_ORIENT_MN,
                                     rightFrameSize,
                                     VTK_UNSIGNED_CHAR,
                                     imageEvent->image[1].properties.bpp,
                                     (imageEvent->image[1].properties.format == eLeapImageFormat_RGBIr_Bayer) ? US_IMG_RGB_COLOR : US_IMG_BRIGHTNESS,
                                     0,
                                     this->FrameNumber); // For now, use plus timestamps, until figure out how to offset leap timestamp by plus timestamp

    this->FrameNumber++;
  }
  else
  {
    LOG_DEBUG("Image event received but no video data sources are available.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnPointMappingChangeEvent(const LEAP_POINT_MAPPING_CHANGE_EVENT* pointMappingChangeEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnHeadPoseEvent(const LEAP_HEAD_POSE_EVENT* headPoseEvent)
{
  SetHeadPose(headPoseEvent);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusLeapMotion::ResultToString(eLeapRS r)
{
  switch (r)
  {
    case eLeapRS_Success:
      return "eLeapRS_Success";
    case eLeapRS_UnknownError:
      return "eLeapRS_UnknownError";
    case eLeapRS_InvalidArgument:
      return "eLeapRS_InvalidArgument";
    case eLeapRS_InsufficientResources:
      return "eLeapRS_InsufficientResources";
    case eLeapRS_InsufficientBuffer:
      return "eLeapRS_InsufficientBuffer";
    case eLeapRS_Timeout:
      return "eLeapRS_Timeout";
    case eLeapRS_NotConnected:
      return "eLeapRS_NotConnected";
    case eLeapRS_HandshakeIncomplete:
      return "eLeapRS_HandshakeIncomplete";
    case eLeapRS_BufferSizeOverflow:
      return "eLeapRS_BufferSizeOverflow";
    case eLeapRS_ProtocolError:
      return "eLeapRS_ProtocolError";
    case eLeapRS_InvalidClientID:
      return "eLeapRS_InvalidClientID";
    case eLeapRS_UnexpectedClosed:
      return "eLeapRS_UnexpectedClosed";
    case eLeapRS_UnknownImageFrameRequest:
      return "eLeapRS_UnknownImageFrameRequest";
    case eLeapRS_UnknownTrackingFrameID:
      return "eLeapRS_UnknownTrackingFrameID";
    case eLeapRS_RoutineIsNotSeer:
      return "eLeapRS_RoutineIsNotSeer";
    case eLeapRS_TimestampTooEarly:
      return "eLeapRS_TimestampTooEarly";
    case eLeapRS_ConcurrentPoll:
      return "eLeapRS_ConcurrentPoll";
    case eLeapRS_NotAvailable:
      return "eLeapRS_NotAvailable";
    case eLeapRS_NotStreaming:
      return "eLeapRS_NotStreaming";
    case eLeapRS_CannotOpenDevice:
      return "eLeapRS_CannotOpenDevice";
    default:
      return "Unknown result type.";
  }
}

//----------------------------------------------------------------------------
std::string vtkPlusLeapMotion::EventToString(_eLeapEventType t)
{
  switch (t)
  {
    case eLeapEventType_None:
      return "eLeapEventType_None";
    case eLeapEventType_Connection:
      return "eLeapEventType_Connection";
    case eLeapEventType_ConnectionLost:
      return "eLeapEventType_ConnectionLost";
    case eLeapEventType_Device:
      return "eLeapEventType_Device";
    case eLeapEventType_DeviceFailure:
      return "eLeapEventType_DeviceFailure";
    case eLeapEventType_Policy:
      return "eLeapEventType_Policy";
    case eLeapEventType_Tracking:
      return "eLeapEventType_Tracking";
    case eLeapEventType_ImageRequestError:
      return "eLeapEventType_ImageRequestError";
    case eLeapEventType_ImageComplete:
      return "eLeapEventType_ImageComplete";
    case eLeapEventType_LogEvent:
      return "eLeapEventType_LogEvent";
    case eLeapEventType_DeviceLost:
      return "eLeapEventType_DeviceLost";
    case eLeapEventType_ConfigResponse:
      return "eLeapEventType_ConfigResponse";
    case eLeapEventType_ConfigChange:
      return "eLeapEventType_ConfigChange";
    case eLeapEventType_DeviceStatusChange:
      return "eLeapEventType_DeviceStatusChange";
    case eLeapEventType_DroppedFrame:
      return "eLeapEventType_DroppedFrame";
    case eLeapEventType_Image:
      return "eLeapEventType_Image";
    case eLeapEventType_PointMappingChange:
      return "eLeapEventType_PointMappingChange";
    case eLeapEventType_LogEvents:
      return "eLeapEventType_LogEvents";
    case eLeapEventType_HeadPose:
      return "eLeapEventType_HeadPose";
    default:
      return "Unknown event type.";
  }
}

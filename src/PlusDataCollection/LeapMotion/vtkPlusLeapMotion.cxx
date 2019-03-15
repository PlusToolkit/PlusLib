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
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPlusDataSource.h>
#include <vtkXMLDataElement.h>
#include <vtksys/SystemTools.hxx>

// OS includes
#include <math.h>

// STL includes
#include <sstream>

//-------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusLeapMotion);

//-------------------------------------------------------------------------
vtkPlusLeapMotion::vtkPlusLeapMotion()
  : PollTimeoutMs(1000)
{
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->StartThreadForInternalUpdates = true; // Polling based device, message pump
  this->AcquisitionRate = 60; // set to the maximum speed by default
}

//-------------------------------------------------------------------------
vtkPlusLeapMotion::~vtkPlusLeapMotion()
{
  if (this->Recording)
  {
    this->StopRecording();
  }
}

//-------------------------------------------------------------------------
void vtkPlusLeapMotion::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Connection: " << "4" << std::endl;
  os << "Poll timeout (ms)" << this->PollTimeoutMs << std::endl;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);


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
  if ((result = LeapCreateConnection(NULL, &this->Connection)) == eLeapRS_Success)
  {
    result = LeapOpenConnection(this->Connection);
    if (result == eLeapRS_Success)
    {
      return PLUS_SUCCESS;
    }
    LOG_ERROR("Unable to open the connection to the LeapMotion device:" << this->ResultToString(result));
  }
  else
  {
    LOG_ERROR("Unable to create the connection to the LeapMotion device:" << this->ResultToString(result));
  }

  return PLUS_FAIL;
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

  if (result != eLeapRS_Success)
  {
    LOG_ERROR("LeapC PollConnection call error: " << this->ResultToString(result));
    return PLUS_FAIL;
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
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnTrackingEvent(const LEAP_TRACKING_EVENT* trackingEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnLogEvent(const LEAP_LOG_EVENT* logEvent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::OnLogEvents(const LEAP_LOG_EVENTS* logEvents)
{
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

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusLeapMotion_h
#define __vtkPlusLeapMotion_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// LeapMotion includes
#include <LeapC.h>

/*!
\class vtkPlusLeapMotion
\brief Interface for the LeapMotion hand tracker

\ingroup PlusLibDataCollection
*/

class vtkPlusDataSource;
class vtkIGSIORecursiveCriticalSection;

class vtkPlusDataCollectionExport vtkPlusLeapMotion : public vtkPlusDevice
{
public:
  static vtkPlusLeapMotion* New();
  vtkTypeMacro(vtkPlusLeapMotion, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool IsTracker() const { return true; }

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  vtkGetMacro(LeapHMDPolicy, bool);
  vtkSetMacro(LeapHMDPolicy, bool);
  vtkBooleanMacro(LeapHMDPolicy, bool);

  vtkGetMacro(RefusePauseResumePolicy, bool);
  vtkSetMacro(RefusePauseResumePolicy, bool);
  vtkBooleanMacro(RefusePauseResumePolicy, bool);

  vtkGetMacro(InvertImage, bool);
  vtkSetMacro(InvertImage, bool);
  vtkBooleanMacro(InvertImage, bool);

protected:
  vtkPlusLeapMotion();
  ~vtkPlusLeapMotion();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus InternalUpdate();
  virtual PlusStatus InternalStartRecording();
  virtual PlusStatus InternalStopRecording();
  virtual PlusStatus NotifyConfigured();

  enum Finger
  {
    Finger_Thumb, // Yeah yeah... not a finger... hush.
    Finger_Index,
    Finger_Middle,
    Finger_Ring,
    Finger_Pinky
  };
  enum Bone
  {
    Bone_Metacarpal,    // Closest to heart
    Bone_Proximal,
    Bone_Intermediate,
    Bone_Distal         // Furthest from heart (aka, finger tip)
  };
  PlusStatus ToolTimeStampedUpdateBone(std::string boneName, eLeapHandType handIndex, Finger fingerIndex, Bone boneIndex);
  PlusStatus ToolTimeStampedUpdatePalm(const std::string& name, eLeapHandType hand);

  void SetFrame(const LEAP_TRACKING_EVENT* trackingEvent);
  LEAP_TRACKING_EVENT* GetFrame();
  void SetHeadPose(const LEAP_HEAD_POSE_EVENT* headPose);
  LEAP_HEAD_POSE_EVENT* GetHeadPose();

protected:
  // Leap events
  PlusStatus OnConnectionEvent(const LEAP_CONNECTION_EVENT* connectionEvent);
  PlusStatus OnConnectionLostEvent(const LEAP_CONNECTION_LOST_EVENT* connectionLostEvent);
  PlusStatus OnDeviceEvent(const LEAP_DEVICE_EVENT* deviceEvent);
  PlusStatus OnDeviceLostEvent(const LEAP_DEVICE_EVENT* deviceEvent);
  PlusStatus OnDeviceFailureEvent(const LEAP_DEVICE_FAILURE_EVENT* deviceFailureEvent);
  PlusStatus OnPolicyEvent(const LEAP_POLICY_EVENT* policyEvent);
  PlusStatus OnTrackingEvent(const LEAP_TRACKING_EVENT* trackingEvent);
  PlusStatus OnLogEvent(const LEAP_LOG_EVENT* logEvent);
  PlusStatus OnLogEvents(const LEAP_LOG_EVENTS* logEvents);
  PlusStatus OnConfigChangeEvent(const LEAP_CONFIG_CHANGE_EVENT* configChangeEvent);
  PlusStatus OnConfigResponseEvent(const LEAP_CONFIG_RESPONSE_EVENT* configResponseEvent);
  PlusStatus OnImageEvent(const LEAP_IMAGE_EVENT* imageEvent);
  PlusStatus OnPointMappingChangeEvent(const LEAP_POINT_MAPPING_CHANGE_EVENT* pointMappingChangeEvent);
  PlusStatus OnHeadPoseEvent(const LEAP_HEAD_POSE_EVENT* headPoseEvent);

protected:
  std::string ResultToString(eLeapRS);
  std::string EventToString(_eLeapEventType);

protected:
  bool                              LeapHMDPolicy;
  bool                              RefusePauseResumePolicy;
  bool                              Initialized;
  bool                              ImageInitialized;
  bool                              InvertImage;

  LEAP_CONNECTION                   Connection;
  unsigned int                      PollTimeoutMs;
  LEAP_CONNECTION_MESSAGE           LastMessage;
  LEAP_TRACKING_EVENT               LastTrackingEvent;
  LEAP_HEAD_POSE_EVENT              LastHeadPoseEvent;

  vtkPlusDataSource*                LeftCameraSource;
  vtkPlusDataSource*                RightCameraSource;

  vtkIGSIORecursiveCriticalSection* Mutex;

private:
  vtkPlusLeapMotion(const vtkPlusLeapMotion&);
  void operator=(const vtkPlusLeapMotion&);
};

#endif

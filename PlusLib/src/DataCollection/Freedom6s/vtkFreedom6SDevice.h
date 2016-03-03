/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Chris Wedlake <cwedlake@imaging.robarts.ca>. All rights reserved.
See License.txt for details.

Date:      $Date: 2007/04/19 12:48:53 $
Version:   $Revision: 1.1 $
=========================================================Plus=header=end*/

#ifndef __vtkFreedom6SDevice_h
#define __vtkFreedom6SDevice_h

#include "vtkPlusDevice.h"
#include "freedom6s.h"
#include "vtkHapticForce.h"

class Freedom6S;
class vtkMatrix4x4;

class VTK_EXPORT vtkFreedom6SDevice : public vtkPlusDevice
{
  static const int FREEDOM6S_STRING_SIZE = 50;

public:
  static vtkFreedom6SDevice *New();
  vtkTypeMacro(vtkFreedom6SDevice, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus Probe();

  vtkGetObjectMacro(ForceModel, vtkHapticForce);

  vtkGetMacro(Scale, double);
  vtkSetMacro(Scale, double);

  vtkSetMacro(UseForce, bool);
  vtkGetMacro(UseForce, bool);

  vtkSetMacro(TimerPeriod, int);
  vtkGetMacro(TimerPeriod, int);

protected:
  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus InternalStartRecording();
  virtual PlusStatus InternalStopRecording();
  virtual PlusStatus InternalUpdate();

  virtual PlusStatus NotifyConfigured();

  void ComputeForceTorque(Vector3& force, Vector3& torque);
  int GetVersion();

protected:
  vtkFreedom6SDevice();
  ~vtkFreedom6SDevice();

  Freedom6S* FreedomAPI;
  char SerialNumber[75];
  char LeftHanded[20];
  char Version[FREEDOM6S_STRING_SIZE];
  char VersionDate[FREEDOM6S_STRING_SIZE];

  int TimerPeriod;
  vtkHapticForce* ForceModel;
  vtkMatrix4x4* SendMatrix;
  bool UseForce;
  uint64_t FrameCount;
  int CurrentFrame;
  int NextFrame;
  double Scale;
};


#endif
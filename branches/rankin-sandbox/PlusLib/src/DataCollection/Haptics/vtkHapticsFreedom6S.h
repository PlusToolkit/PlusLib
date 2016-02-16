/*=========================================================================

  Program:   Haptics for VTK
  Module:    $RCSfile: vtkHapticsFreedom6S.h,v $
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: cwedlake $
  Date:      $Date: 2007/04/19 12:48:53 $
  Version:   $Revision: 1.1 $

==========================================================================

Copyright (c) 2000-2005

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
vBE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkHapticsFreedom6D - interfaces VTK with the Freedom6D haptics device
// .SECTION Description
//
// .SECTION see also
// 

#ifndef __vtkHapticsFreedom6S_h
#define __vtkHapticsFreedom6S_h

#include "vtkObject.h"
#include "vtkCriticalSection.h"

#include <stdlib.h>
#include <stdio.h>
#include "freedom6s/f6s.h"
#include "freedom6s/freedom6s.h"

#include <math.h>
#include <windows.h>
#include <time.h>

#include <vector>

#include "vtkTracker.h"
#include "vtkTrackerTool.h"

#include "vtkHapticForce.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"


#define STR_SIZE 50
#define aHeart 0.25
#define MAX_FORCE 0.6
#define POS2MM 1000 // Position is coming back in Meters. Wanted in mms

#define VTK_FREEDOM6S_NTOOLS 1

class VTK_EXPORT vtkHapticsFreedom6S : public vtkTracker
{
public:
    static vtkHapticsFreedom6S *New();
    vtkTypeMacro(vtkHapticsFreedom6S,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);

  double GetScale();
  void SetScale(double scale);
  int Probe();
  vtkHapticForce * GetForceModel() { return forceModel; };
  int GetSerialPort() { return 1; };
  void SetSerialPort(int port) {};

  
protected:
  vtkCriticalSection *UpdateMutex;
  vtkHapticsFreedom6S();
  ~vtkHapticsFreedom6S();


  int InternalStartTracking();
  int InternalStopTracking();
  void InternalUpdate();
//BTX
  void computeForceTorque(Vector3& force, Vector3& torque);
//ETX
  int GetVersion();
  

private:
  //BTX
  Freedom6S f6s;
  int DeviceOn;
  char SerialNumber[75];
  char LeftHanded[20];
  char Version[STR_SIZE];
  char VersionDate[STR_SIZE];

  int Started;
  int TimerPeriod;
  vtkHapticForce * forceModel;
  vtkMatrix4x4 * SendMatrix;

  int UseForce;
  unsigned long frameCount;

  int currentFrame;
  int nextFrame;

  double sizeHIP;
    double Scale;
  int timeSlice;
  //ETX
};


#endif
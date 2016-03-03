/*=========================================================================

  Program:   Haptics for VTK
  Module:    $RCSfile: vtkHapticsDevice.h,v $
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
// .NAME vtkHapticsDevice - interfaces VTK with the Freedom6D haptics device
// .SECTION Description
//
// .SECTION see also
// 
#ifndef __vtkHapticsDevice_h
#define __vtkHapticsDevice_h

#include "vtkObject.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkHapticForce.h"

class VTK_EXPORT vtkHapticsDevice : public vtkObject 
{
public:
    static vtkHapticsDevice *New();
    vtkTypeMacro(vtkHapticsDevice,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);

    virtual int InternalStartDevice() { return 1; };
    virtual int InternalStopDevice() { return 1; };
  virtual int InternalUpdate() { return 1;};
  vtkHapticForce * GetForceModel() { return forceModel; };
//BTX
  friend static void *vtkHapticsThread(vtkMultiThreader::ThreadInfo *data);
//ETX
  void StopDevice();
  void StartDevice();

    double GetInternalUpdateRate() { return this->InternalUpdateRate; };
  void Update();
    vtkGetMacro(LastUpdateTime,double);
    vtkGetObjectMacro(Transform,vtkTransform);

protected:
  vtkHapticsDevice();
  ~vtkHapticsDevice();

  double InternalUpdateRate;
  vtkCriticalSection * UpdateMutex;
  vtkCriticalSection * RequestUpdateMutex;
  vtkTransform * Transform;
  vtkMatrix4x4 * Position;
  int Initialized;
  unsigned long LastUpdateTime;
  vtkTimeStamp UpdateTime;
  vtkHapticForce * forceModel;
    
private:
  vtkMultiThreader * Threader;
  int ThreadId;

};


#endif
/*=========================================================================

  Program:   Haptics for VTK
  Module:    $RCSfile: vtkHapticsFreedom6S.cxx,v $
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
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkCriticalSection.h"
#include "vtkHapticsFreedom6S.h"

//----------------------------------------------------------------------------
vtkHapticsFreedom6S* vtkHapticsFreedom6S::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkHapticsFreedom6S");
  if(ret)
    {
    return (vtkHapticsFreedom6S*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkHapticsFreedom6S;
}

//----------------------------------------------------------------------------
vtkHapticsFreedom6S::vtkHapticsFreedom6S()
{
    this->SendMatrix = vtkMatrix4x4::New();
  this->TimerPeriod = 1000;
  this->Scale = 1.0;
  this->UseForce=1;
  this->currentFrame = 0;
  this->nextFrame = 1;
    this->SetNumberOfTools(1);
  this->frameCount = 0;
  this->forceModel = vtkHapticForce::New();
  this->GetTool(0)->SetToolPartNumber("00000001");
  this->GetTool(0)->SetToolManufacturer("Freedom6S");
  this->GetTool(0)->SetToolType("00000000");
  this->DeviceOn=0;

}

//----------------------------------------------------------------------------
vtkHapticsFreedom6S::~vtkHapticsFreedom6S() 
{
  this->SendMatrix->Delete();
}
  
//----------------------------------------------------------------------------
void vtkHapticsFreedom6S::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkTracker::PrintSelf(os,indent.GetNextIndent());
  
  os << indent << "Force Model: ";
  forceModel->PrintSelf(os,indent.GetNextIndent());

  if (this->SerialNumber != NULL)
  os << indent << "Serial Number: " << this->SerialNumber << endl;
  else 
  os << indent << "Serial Number: (NULL)" << endl;

  if (this->LeftHanded != NULL)
  os << indent << "Is Left Handed: " << this->LeftHanded << endl;
  else 
  os << indent << "Is Left Handed: (NULL)" << endl;

  if (this->Version != NULL)
  os << indent << "Device Version: " << this->Version << endl;
  else 
  os << indent << "Device Version: (NULL)" << endl;

  if (this->VersionDate != NULL)
  os << indent << "Device Version Date: " << this->VersionDate << endl;
  else 
  os << indent << "Device Version Date: (NULL)" << endl;

  os << indent << "Use Force: " << (this->UseForce ? "Yes" : "No") << endl;
  
}

//----------------------------------------------------------------------------
int vtkHapticsFreedom6S::Probe()
{
  return 1;
}

//----------------------------------------------------------------------------
int vtkHapticsFreedom6S::InternalStartTracking() 
{
  if (this->DeviceOn==1) {
    cout << "ALREADY STARED" << endl;
    return 0;
  }
  
  
  if (!this->f6s.Initialize()) {
    cout << "Error initializing Freedom6S library: " << f6s.GetLastError() << endl;
    return 0;
  }
  if (this->f6s.GetLastError() < 0)
    cout << "F6S: "<< this->f6s.GetLastErrorString() << endl;
    this->f6s.ComputeJointVel(0.001,20);
  if (this->f6s.GetLastError() < 0)
       cout << "F6S: "<< this->f6s.GetLastErrorString() << endl;
    this->f6s.SetHoldDist(0.03f);
  if (this->f6s.GetLastError() < 0)
    cout << "F6S: "<< this->f6s.GetLastErrorString() << endl;

  F6sDeviceInfo *pDevInfo;
  this->f6s.GetDeviceInfo(&pDevInfo);
  strcpy(SerialNumber, pDevInfo->m_strSerialNo);
  strcpy(LeftHanded,(pDevInfo->m_bLeftDevice ? "yes" : "no"));
  this->f6s.SetTimer(this->TimerPeriod);
  this->GetVersion();
  this->GetTool(0)->SetToolManufacturer(SerialNumber);
  this->DeviceOn=1;
  return 1;
}

//----------------------------------------------------------------------------
void vtkHapticsFreedom6S::InternalUpdate() 
{


  if (!this->DeviceOn) {return;}

  // what does this do?
    this->f6s.SleepTimer();  

  timeSlice = timeSlice + 1; //for force interpolation

    this->frameCount = this->frameCount + 1;

  int flags = 0;
  this->f6s.UpdateKinematics();
  double matrix[16];
    this->f6s.GetPositionMatrixGL(matrix);  // transform from handle to GL
  matrix[12] *= POS2MM*this->Scale; //x
  matrix[13] *= POS2MM*this->Scale; //y
  matrix[14] *= POS2MM*this->Scale; //z

  this->SendMatrix->DeepCopy(matrix);
  this->SendMatrix->Transpose();
  //SendMatrix->PrintSelf(cout, vtkIndent());

    this->ToolUpdate(0,this->SendMatrix,flags,this->frameCount);

  // force-feedback
  //================================================================================
  Vector3 linearVec, angularVec;

  f6s.GetVelocity(linearVec, angularVec);

  Vector3 force(0,0,0), torque(0,0,0);
  this->computeForceTorque(force, torque); // why are we computing torque as it is NEVER set to anything

  if (!UseForce){
    force = force*0;
  }
  //else {
  //  force = force*3;  //Why 3? should this be user settable?
  //}
        
  // Output to device
  f6s.SetForceTorque(force, torque); // torque always = {0,0,0}
//  */
}

//--------------------------------------------------------------------------------------
void vtkHapticsFreedom6S::computeForceTorque(Vector3& force, Vector3& torque)
{
  if (forceModel->GetNumberOfFrames() == 0 ) {return;}
  if (this->GetTool(0)->GetTransform()->GetMatrix() == NULL) {return;}
  double forceSP[3] = {0,0,0};
  int i;
  Vector3 pHome;
  Vector3 pEE(0,0,0);
  int flag = 0;
  double forceSP1[3] = {0,0,0}, forceSP2[3]= {0,0,0};

  if (this->forceModel->GetNumberOfFrames() <=0) return;

  // SHOULD BE THE CURRENT VOLUME INFORMATION
  int dispPeriod = forceModel->GetNumberOfFrames();


  for(i=0;i<3;i++){
    force[i] = 0;
    torque[i] = 0;
  }

  Vector3 p = f6s.TransformPointToHome(pEE);

  for(i=0; i<3; i++){
    pHome[i] = p[i]*POS2MM*this->Scale;
  }

  double pointSP[3] = {pHome[0]/aHeart, pHome[1]/aHeart, pHome[2]/aHeart};

  vtkMatrix4x4 * temp = vtkMatrix4x4::New();
  temp->DeepCopy(this->GetTool(0)->GetTransform()->GetMatrix());

  this->currentFrame = (nextFrame % forceModel->GetNumberOfFrames());
  this->nextFrame = (currentFrame + 1) % forceModel->GetNumberOfFrames();

  this->forceModel->GetForceModel(this->currentFrame)->GenerateForce(temp, forceSP1);
  flag = this->forceModel->GetForceModel(this->nextFrame)->GenerateForce(temp, forceSP2);
  temp->Delete();
  
  double alpha = currentFrame/dispPeriod;
  for(i=0;i<3;i++){
    forceSP[i] = (1-alpha)*forceSP1[i] + alpha*forceSP2[i];
  }

  force[0] = forceSP[0];
  force[1] = -forceSP[2];
  force[2] = forceSP[1];
  
  double forceMag = sqrt(force[0]*force[0] + force[1]*force[1] + force[2]*force[2]);

  // Limit the maximum force
  if(forceMag >= MAX_FORCE){
    force.normalize();
    force = force*MAX_FORCE;
  }
  return;
}

int vtkHapticsFreedom6S::InternalStopTracking() 
{
  this->DeviceOn=0;
  this->f6s.Cleanup();
  return this->f6s.GetLastError();
  
}

int vtkHapticsFreedom6S::GetVersion() {
    this->f6s.GetVersion(this->Version, STR_SIZE, this->VersionDate, STR_SIZE);
  return this->f6s.GetLastError();
}

double vtkHapticsFreedom6S::GetScale(){
  return this->Scale;
}

void vtkHapticsFreedom6S::SetScale(double scale) {
  this->Scale = scale;
}
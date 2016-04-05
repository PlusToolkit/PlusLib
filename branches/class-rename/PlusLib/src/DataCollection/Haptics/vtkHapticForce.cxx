/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#include "vtkHapticForce.h"
#include "vtkObjectFactory.h"

#include "vtkForceFeedback.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkHapticForce)

//----------------------------------------------------------------------------
void vtkHapticForce::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
  for(unsigned int i=0; i < this->forceModel.size(); i++)
  {
    this->forceModel[i]->PrintSelf(os, indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
vtkHapticForce::vtkHapticForce()
{
  this->NumberOfFrames=0;
}

//----------------------------------------------------------------------------
vtkHapticForce::~vtkHapticForce()
{
  for(unsigned int i=0; i < this->forceModel.size(); i++)
  {
    forceModel[i]->UnRegister(forceModel[i]);
  }
}

//----------------------------------------------------------------------------
void vtkHapticForce::AddForceModel(vtkForceFeedback * force)
{
  force->Register(force);
  this->forceModel.push_back(force);
  this->NumberOfFrames=forceModel.size();
}

//----------------------------------------------------------------------------
void vtkHapticForce::InsertForceModel(unsigned int position, vtkForceFeedback * force)
{
  if (position >= forceModel.size())
  {
    vtkForceFeedback * copy = force;
    this->forceModel.push_back(copy);
  }
  else if (position < forceModel.size() && position >= 0)
  {
    vtkForceFeedback * copy = force;
    this->forceModel.insert(forceModel.begin()+position, force);
  }
  this->NumberOfFrames=forceModel.size();
}

//----------------------------------------------------------------------------
vtkForceFeedback * vtkHapticForce::GetForceModel(int position)
{
  return this->forceModel[position];
}

//----------------------------------------------------------------------------
int vtkHapticForce::GetNumberOfFrames()
{
  return this->NumberOfFrames;
}

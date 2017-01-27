/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#include "PlusConfigure.h"

#include "vtkPlusHapticForce.h"
#include "vtkObjectFactory.h"

#include "vtkPlusForceFeedback.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusHapticForce)

//----------------------------------------------------------------------------
void vtkPlusHapticForce::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
  for(unsigned int i=0; i < this->forceModel.size(); i++)
  {
    this->forceModel[i]->PrintSelf(os, indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
vtkPlusHapticForce::vtkPlusHapticForce()
{
  this->NumberOfFrames=0;
}

//----------------------------------------------------------------------------
vtkPlusHapticForce::~vtkPlusHapticForce()
{
  for(unsigned int i=0; i < this->forceModel.size(); i++)
  {
    forceModel[i]->UnRegister(forceModel[i]);
  }
}

//----------------------------------------------------------------------------
void vtkPlusHapticForce::AddForceModel(vtkPlusForceFeedback * force)
{
  force->Register(force);
  this->forceModel.push_back(force);
  this->NumberOfFrames=forceModel.size();
}

//----------------------------------------------------------------------------
void vtkPlusHapticForce::InsertForceModel(unsigned int position, vtkPlusForceFeedback * force)
{
  if (position >= forceModel.size())
  {
    vtkPlusForceFeedback * copy = force;
    this->forceModel.push_back(copy);
  }
  else if ( position < forceModel.size() )
  {
    this->forceModel.insert(forceModel.begin()+position, force);
  }
  this->NumberOfFrames=forceModel.size();
}

//----------------------------------------------------------------------------
vtkPlusForceFeedback * vtkPlusHapticForce::GetForceModel(int position)
{
  return this->forceModel[position];
}

//----------------------------------------------------------------------------
int vtkPlusHapticForce::GetNumberOfFrames()
{
  return this->NumberOfFrames;
}

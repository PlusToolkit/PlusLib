/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkTransformCombiner.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTransformCombiner);

//----------------------------------------------------------------------------
vtkTransformCombiner::vtkTransformCombiner()
{
}

//----------------------------------------------------------------------------
vtkTransformCombiner::~vtkTransformCombiner()
{
}

//----------------------------------------------------------------------------
void vtkTransformCombiner::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  /*
  os << indent << "Member: " << this->Member << "\n";
  this->Member2->PrintSelf(os,indent.GetNextIndent());
  */
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformCombiner::SetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkMatrix4x4* matrix)
{
  return PLUS_FAIL;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformCombiner::SetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus status)
{
  return PLUS_FAIL;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformCombiner::GetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkTransform** transform)
{
  return PLUS_FAIL;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformCombiner::GetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus &status)
{
  return PLUS_FAIL;
}

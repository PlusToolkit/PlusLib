/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#include "vtkForceFeedback.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkForceFeedback)

//----------------------------------------------------------------------------
void vtkForceFeedback::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkForceFeedback::vtkForceFeedback()
{
}

//----------------------------------------------------------------------------
vtkForceFeedback::~vtkForceFeedback()
{
}

//----------------------------------------------------------------------------
int vtkForceFeedback::GenerateForce(vtkMatrix4x4 * hapticPosition, double force[3])
{
  return 0;
}

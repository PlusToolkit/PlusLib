/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#include "PlusConfigure.h"

#include "vtkPlusForceFeedback.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusForceFeedback)

//----------------------------------------------------------------------------
void vtkPlusForceFeedback::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkPlusForceFeedback::vtkPlusForceFeedback()
{
}

//----------------------------------------------------------------------------
vtkPlusForceFeedback::~vtkPlusForceFeedback()
{
}

//----------------------------------------------------------------------------
int vtkPlusForceFeedback::GenerateForce(vtkMatrix4x4 * hapticPosition, double force[3])
{
  return 0;
}

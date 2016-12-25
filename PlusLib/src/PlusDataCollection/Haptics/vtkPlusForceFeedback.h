/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkPlusForceFeedback_h
#define __vtkPlusForceFeedback_h

#include "PlusConfigure.h"
#include "vtkPlusHapticsExport.h"

#include "vtkObject.h"

class vtkMatrix4x4;

class vtkPlusHapticsExport vtkPlusForceFeedback : public vtkObject
{
public:
  static vtkPlusForceFeedback *New();

  vtkTypeMacro(vtkPlusForceFeedback,vtkObject);

  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual int GenerateForce(vtkMatrix4x4 * hapticPosition, double force[3]);
  ~vtkPlusForceFeedback();

protected:
  vtkPlusForceFeedback();
};

#endif

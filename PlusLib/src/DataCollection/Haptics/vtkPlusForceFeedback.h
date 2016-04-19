/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkPlusForceFeedback_h
#define __vtkPlusForceFeedback_h

#include "PlusConfigure.h"
#include "PlusHapticsExport.h"

#include "vtkObject.h"

class vtkMatrix4x4;

class PlusHapticsExport vtkPlusForceFeedback : public vtkObject
{
public:
  static vtkPlusForceFeedback *New();

  vtkTypeMacro(vtkPlusForceFeedback,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int GenerateForce(vtkMatrix4x4 * hapticPosition, double force[3]);
  ~vtkPlusForceFeedback();

protected:
  vtkPlusForceFeedback();
};

#endif

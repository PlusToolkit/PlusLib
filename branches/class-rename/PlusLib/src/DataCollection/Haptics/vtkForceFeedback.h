/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkForceFeedback_h
#define __vtkForceFeedback_h

#include "PlusConfigure.h"
#include "PlusHapticsExport.h"

#include "vtkObject.h"

class vtkMatrix4x4;

class PlusHapticsExport vtkForceFeedback : public vtkObject
{
public:
  static vtkForceFeedback *New();

  vtkTypeMacro(vtkForceFeedback,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int GenerateForce(vtkMatrix4x4 * hapticPosition, double force[3]);
  ~vtkForceFeedback();

protected:
  vtkForceFeedback();
};

#endif

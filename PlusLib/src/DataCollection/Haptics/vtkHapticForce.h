/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkHapticForce_h
#define __vtkHapticForce_h

#include "PlusConfigure.h"
#include "PlusHapticsExport.h"

#include "vtkObject.h"
#include <vector>

class vtkForceFeedback;

class PlusHapticsExport vtkHapticForce : public vtkObject
{
public:
  static vtkHapticForce *New();

  vtkTypeMacro(vtkHapticForce,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);
  void AddForceModel(vtkForceFeedback * force);
  void InsertForceModel(unsigned int position, vtkForceFeedback * force);
  vtkForceFeedback * GetForceModel(int position);
  int GetNumberOfFrames();

protected:
  vtkHapticForce();
  ~vtkHapticForce();

private:
  std::vector<vtkForceFeedback *>  forceModel;
  int NumberOfFrames;
};

#endif

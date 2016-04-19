/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkPlusHapticForce_h
#define __vtkPlusHapticForce_h

#include "PlusConfigure.h"
#include "PlusHapticsExport.h"

#include "vtkObject.h"
#include <vector>

class vtkPlusForceFeedback;

class PlusHapticsExport vtkPlusHapticForce : public vtkObject
{
public:
  static vtkPlusHapticForce *New();

  vtkTypeMacro(vtkPlusHapticForce,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);
  void AddForceModel(vtkPlusForceFeedback * force);
  void InsertForceModel(unsigned int position, vtkPlusForceFeedback * force);
  vtkPlusForceFeedback * GetForceModel(int position);
  int GetNumberOfFrames();

protected:
  vtkPlusHapticForce();
  ~vtkPlusHapticForce();

private:
  std::vector<vtkPlusForceFeedback *>  forceModel;
  int NumberOfFrames;
};

#endif

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkPlusHapticForce_h
#define __vtkPlusHapticForce_h

#include "PlusConfigure.h"
#include "vtkPlusHapticsExport.h"

#include "vtkObject.h"
#include <vector>

class vtkPlusForceFeedback;

class vtkPlusHapticsExport vtkPlusHapticForce : public vtkObject
{
public:
  static vtkPlusHapticForce *New();
  vtkTypeMacro(vtkPlusHapticForce,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

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

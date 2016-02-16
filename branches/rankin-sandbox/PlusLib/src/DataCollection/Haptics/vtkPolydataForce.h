/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkPolydataForce_h
#define __vtkPolydataForce_h

#include "PlusConfigure.h"
#include "PlusHapticsExport.h"

#include "vtkForceFeedback.h"

class vtkPolyData;

class PlusHapticsExport vtkPolydataForce : public vtkForceFeedback
{
public:
  static vtkPolydataForce *New();
  vtkTypeMacro(vtkPolydataForce, vtkForceFeedback);

  void PrintSelf(ostream& os, vtkIndent indent);

  int GenerateForce(vtkMatrix4x4 * transformMatrix, double force[3]);
  int SetGamma(double gamma);
  void SetInput(vtkPolyData * poly);

protected:
  vtkPolydataForce();
  virtual ~vtkPolydataForce();
  double CalculateDistance(double x, double y, double z);
  void CalculateForce(double x, double y, double z, double force[3]);

private:
  vtkPolyData * poly;
  double gammaSigmoid;
  double scaleForce;
  double lastPos[3];
};

#endif

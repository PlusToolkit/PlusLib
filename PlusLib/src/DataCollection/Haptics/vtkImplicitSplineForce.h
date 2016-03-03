/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#ifndef __vtkImplicitSplineForce_h
#define __vtkImplicitSplineForce_h

#include "PlusConfigure.h"
#include "PlusHapticsExport.h"

#include "vtkForceFeedback.h"

class vtkMatrix4x4;

#define DIM_BASE 20
#define NUM_INTERVALU_S DIM_BASE+10
#define NUM_INTERVALV_S DIM_BASE
#define NUM_INTERVALW_S DIM_BASE+25
#define DimCPoint_U NUM_INTERVALU_S+3
#define DimCPoint_V NUM_INTERVALV_S+3
#define DimCPoint_W NUM_INTERVALW_S+3
#define DimKnot_U DimCPoint_U+2
#define DimKnot_V DimCPoint_V+2
#define DimKnot_W DimCPoint_W+2

class PlusHapticsExport vtkImplicitSplineForce : public vtkForceFeedback
{
public:
  static vtkImplicitSplineForce *New();
  vtkTypeMacro(vtkImplicitSplineForce,vtkForceFeedback);

  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInput(int splineId);
  void SetInput(char * controlPnt);
  int GenerateForce(vtkMatrix4x4 * transformMatrix, double force[3]);
  int SetGamma(double gamma);

protected:
  vtkImplicitSplineForce();
  virtual ~vtkImplicitSplineForce();

  int fnGaussValueDeriv(double a, double x, double& value, double& deriv);
  int fnSigmoidValueDeriv(double a, double x, double& value, double& deriv);
  int CalculateDistanceDerivativeBasis(double x, double y, double z, int n, int direction, double *gradient);
  double BasisFunction3(int k, double *knot, double u, int K);
  double BasisFunction3DerivativeD(int k, double *knot, double u, int K);
  double CalculateDistanceBasis(double x, double y, double z, int n, int direction);
  int CalculateKnotIu(double u, double *knot, int K, int n);
  int ReadFile3DBSplineKnots(char *fname);
  int ReadFileControlPoints(char *fname);

  double controlQ3D[DimCPoint_W][DimCPoint_V][DimCPoint_U];
  double knot1[NUM_INTERVALU_S+1];
  double knot2[NUM_INTERVALV_S+1];
  double knot3[NUM_INTERVALW_S+1];

  double knot1b[DimKnot_U];
  double knot2b[DimKnot_V];
  double knot3b[DimKnot_W];

  double gammaSigmoid;
  double scaleForce;
  char SplineKnots[255];
  char ControlPoints[255];
};

#endif

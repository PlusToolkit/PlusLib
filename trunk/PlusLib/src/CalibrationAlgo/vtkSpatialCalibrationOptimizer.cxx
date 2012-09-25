/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkSpatialCalibrationOptimizer.h"
#include "vtkTransformRepository.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkSpatialCalibrationOptimizer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSpatialCalibrationOptimizer);

//-----------------------------------------------------------------------------

vtkSpatialCalibrationOptimizer::vtkSpatialCalibrationOptimizer()
{
  this->PivotPointToMarkerTransformMatrix = NULL;
  this->CalibrationError = -1.0;
  this->Minimizer = NULL;
  this->MarkerToReferenceTransformMatrixArray = NULL;
  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->ObjectPivotPointCoordinateFrame = NULL;

  vtkSmartPointer<vtkAmoebaMinimizer> minimizer = vtkSmartPointer<vtkAmoebaMinimizer>::New();
  this->SetMinimizer(minimizer);

  vtkSmartPointer<vtkDoubleArray> markerToReferenceTransformMatrixArray = vtkSmartPointer<vtkDoubleArray>::New();
  this->SetMarkerToReferenceTransformMatrixArray(markerToReferenceTransformMatrixArray);
  this->MarkerToReferenceTransformMatrixArray->SetNumberOfComponents(16);

  Initialize();
}

//-----------------------------------------------------------------------------

vtkSpatialCalibrationOptimizer::~vtkSpatialCalibrationOptimizer()
{
  this->SetPivotPointToMarkerTransformMatrix(NULL);
  this->SetMarkerToReferenceTransformMatrixArray(NULL);
  this->SetMinimizer(NULL);
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::Initialize()
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::Initialize");

  this->MarkerToReferenceTransformMatrixArray->SetNumberOfTuples(0);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::InsertNextCalibrationPoint(vtkMatrix4x4* aMarkerToReferenceTransformMatrix)
{
  //LOG_TRACE("vtkSpatialCalibrationOptimizer::InsertNextCalibrationPoint");

  this->MarkerToReferenceTransformMatrixArray->InsertNextTuple(*aMarkerToReferenceTransformMatrix->Element);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData)
{
  LOG_TRACE("(vtkSpatialCalibrationOptimizer)vtkImageToProbeCalibrationMatrixEvaluationFunction");

  vtkSpatialCalibrationOptimizer *self = (vtkSpatialCalibrationOptimizer*)userData;

  int i;
  int n = self->MarkerToReferenceTransformMatrixArray->GetNumberOfTuples();

  double x = self->Minimizer->GetParameterValue("x");
  double y = self->Minimizer->GetParameterValue("y");
  double z = self->Minimizer->GetParameterValue("z");
  double nx,ny,nz,sx,sy,sz,sxx,syy,szz;

  double matrix[4][4];

  sx = sy = sz = 0.0;
  sxx = syy = szz = 0.0;

  for (i = 0; i < n; i++)
  {
    self->MarkerToReferenceTransformMatrixArray->GetTuple(i,*matrix);

    nx = matrix[0][0]*x + matrix[0][1]*y + matrix[0][2]*z + matrix[0][3];
    ny = matrix[1][0]*x + matrix[1][1]*y + matrix[1][2]*z + matrix[1][3];
    nz = matrix[2][0]*x + matrix[2][1]*y + matrix[2][2]*z + matrix[2][3];

    sx += nx;
    sy += ny;
    sz += nz;

    sxx += nx*nx;
    syy += ny*ny;
    szz += nz*nz;
  }

  double r;

  if (n > 1)
  {
    r = sqrt((sxx - sx*sx/n)/(n-1) +
      (syy - sy*sy/n)/(n-1) +
      (szz - sz*sz/n)/(n-1));
  }
  else
  {
    r = 0;
  }

  self->Minimizer->SetFunctionValue(r);
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::DoCalibrationOptimization(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::DoCalibrationOptimization");

  return PLUS_FAIL;
}

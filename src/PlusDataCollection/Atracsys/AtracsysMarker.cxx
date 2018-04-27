/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "AtracsysMarker.h"

using namespace Atracsys;

ATRACSYS_ERROR Marker::GetID(int& id)
{
  return SUCCESS;
}

ATRACSYS_ERROR Marker::GetGeometryID(int& geometryID)
{
  return SUCCESS;
}

ATRACSYS_ERROR Marker::GetGeometryPrecsenceMask(int& geometryMask)
{
  return SUCCESS;
}

ATRACSYS_ERROR Marker::GetTransformToTracker(vtkSmartPointer<vtkMatrix4x4> MarkerToTracker)
{
  return SUCCESS;
}

ATRACSYS_ERROR Marker::GetFiducialRegistrationErrorMm(float& fre)
{
  return SUCCESS;
}
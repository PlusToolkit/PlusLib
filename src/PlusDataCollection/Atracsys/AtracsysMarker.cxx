/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "AtracsysMarker.h"

using namespace Atracsys;

Marker::Marker(int geometryId, vtkSmartPointer<vtkMatrix4x4> toolToTracker, int gpm, float freMm)
{
  this->GeometryId = geometryId;
  this->ToolToTracker = toolToTracker;
  this->GeometryPresenceMask = gpm;
  this->FreMm = freMm;
}

int Marker::GetGeometryID()
{
  return this->GeometryId;
}

int Marker::GetGeometryPrecsenceMask()
{
  return this->GeometryPresenceMask;
}

vtkSmartPointer<vtkMatrix4x4> Marker::GetTransformToTracker()
{
  return this->ToolToTracker;
}

float Marker::GetFiducialRegistrationErrorMm()
{
  return this->FreMm;
}
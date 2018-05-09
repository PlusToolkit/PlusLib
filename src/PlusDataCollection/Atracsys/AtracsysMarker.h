/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __AtracsysMarker_h
#define __AtracsysMarker_h

#include "AtracsysConstants.h"
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>

namespace Atracsys
{
  class Marker
  {
  public:
    Marker(int geometryId, vtkSmartPointer<vtkMatrix4x4> toolToTracker, int gpm, float freMm);
    int GetGeometryID();
    int GetGeometryPrecsenceMask();
    vtkSmartPointer<vtkMatrix4x4> GetTransformToTracker();
    float GetFiducialRegistrationErrorMm();
  private:
    int Id; /*!< Tracking id */
    int GeometryId;
    vtkSmartPointer<vtkMatrix4x4> ToolToTracker;
    int GeometryPresenceMask; /*!< Presence mask of fiducials expressed as
                                 * their geometrical indexes */
    float FreMm; /*!< Registration mean ATRACSYS_ERROR (unit mm) */
  };
}

#endif
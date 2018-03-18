/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __AtracsysMarker_h
#define __AtracsysMarker_h

#include "AtracsysConstants.h"
#include <vtkSmartPointer.h>
#include "vtkMatrix4x4.h"

namespace Atracsys
{
  class Marker
  {
  public:
    ATRACSYS_ERROR GetID(int& id);
    ATRACSYS_ERROR GetGeometryID(int& geometryID);
    ATRACSYS_ERROR GetGeometryPrecsenceMask(int& geometryMask);
    ATRACSYS_ERROR GetTransformToTracker(vtkSmartPointer<vtkMatrix4x4> MarkerToTracker);
    ATRACSYS_ERROR GetFiducialRegistrationErrorMm(float& fre);
  private:
    int id; /*!< Tracking id */
    int geometryId; /*!< Geometric id, i.e. the unique id of the used
                       * geometry. */
    int geometryPresenceMask; /*!< Presence mask of fiducials expressed as
                                 * their geometrical indexes */
    int fiducialCorresp[FTK_MAX_FIDUCIALS]; /*!< Correspondence between
                                               * geometry index and 3D
                                               * fiducials indexes or
                                               * INVALID_ID */
    float rotation[3][3]; /*!< Rotation matrix: format [row][column] */
    float translationMM[3]; /*!< translation vector (unit mm) */
    float registrationErrorMM; /*!< Registration mean ATRACSYS_ERROR (unit mm) */
  };
}

#endif
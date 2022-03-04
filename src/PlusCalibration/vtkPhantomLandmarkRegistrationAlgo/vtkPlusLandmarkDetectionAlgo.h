/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusLandmarkDetectionAlgo_h
#define __vtkPlusLandmarkDetectionAlgo_h

#include "PlusConfigure.h"
#include "vtkPlusCalibrationExport.h"

#include "vtkIGSIOLandmarkDetectionAlgo.h"

class vtkMatrix4x4;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
\class vtkPlusLandmarkDetectionAlgo
\brief Landmark detection algorithm detects when a calibrated stylus is pivoting around its tip.
The stylus pivoting point (landmark) is computed assuming that the stylus is calibrated.
\ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusCalibrationExport vtkPlusLandmarkDetectionAlgo : public vtkIGSIOLandmarkDetectionAlgo
{
public:
  vtkTypeMacro( vtkPlusLandmarkDetectionAlgo, vtkIGSIOTransformRepository);
  static vtkPlusLandmarkDetectionAlgo* New();

  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig) override;

protected:
  vtkPlusLandmarkDetectionAlgo();
  virtual  ~vtkPlusLandmarkDetectionAlgo();
};

#endif

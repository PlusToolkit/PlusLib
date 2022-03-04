/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusPivotCalibrationAlgo_h
#define __vtkPlusPivotCalibrationAlgo_h

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusCalibrationExport.h"

// IGSIO includes
#include <vtkIGSIOPivotCalibrationAlgo.h>

//-----------------------------------------------------------------------------

/*!
  \class vtkPlusPivotCalibrationAlgo
  \brief Pivot calibration algorithm to calibrate a stylus. It determines the pose of the stylus tip relative to the marker attached to the stylus.

  The stylus tip position is computed by robust LSQR method, which detects and ignores outliers (that have much larger reprojection error than other points).

  The stylus pose is computed assuming that the marker is attached on the center of one of the stylus axes, which is often a good approximation.
  The axis that points towards the marker is the PivotPoint coordinate system's -Z axis (so that points in front of the stylus have positive Z coordinates
  in the PivotPoint coordinate system). The X axis of the PivotPoint coordinate system is
  aligned with the marker coordinate system's X axis (unless the Z axis of the PivotPoint coordinate system is parallel with the marker coordinate
  system's X axis; in this case the X axis of the PivotPoint coordinate system is aligned with the marker coordinate system's Y axis). The Y axis
  of the PivotPoint coordinate system is chosen to be the cross product of the Z and X axes.

  The method detects outlier points (points that have larger than 3x error than the standard deviation) and ignores them when computing the pivot point
  coordinates and the calibration error.

  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusCalibrationExport vtkPlusPivotCalibrationAlgo : public vtkIGSIOPivotCalibrationAlgo
{
public:
  vtkTypeMacro(vtkPlusPivotCalibrationAlgo, vtkIGSIOPivotCalibrationAlgo);
  static vtkPlusPivotCalibrationAlgo* New();

  /*!
  * Read configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig) override;

protected:
  vtkPlusPivotCalibrationAlgo();
  virtual ~vtkPlusPivotCalibrationAlgo();
};

#endif

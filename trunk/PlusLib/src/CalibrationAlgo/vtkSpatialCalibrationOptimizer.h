/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkSpatialCalibrationOptimizer_h
#define __vtkSpatialCalibrationOptimizer_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkAmoebaMinimizer.h"
#include "vtkDoubleArray.h"
#include "vtkMatrix4x4.h"

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
  \class vtkSpatialCalibrationOptimizer 
  \brief Pivot calibration algorithm to calibrate a stylus. It determines the pose of the stylus tip relative to the marker attached to the stylus.
  
  The stylus tip position computation is straightforward. The stylus pose is computed assuming that the marker is attached on the center of one of
  the stylus axes, which is often a good approximation.
  The axis that points towards the marker is the PivotPoint coordinate system's X axis. The Y axis of the PivotPoint coordinate system is
  aligned with the marker coordinate system's Y axis (unless the X axis of the PivotPoint coordinate system is parallel with the marker coordinate
  system's Y axis; in this case the Y axis of the PivotPoint coordinate system is aligned with the marker coordinate system's Z axis). The Z axis
  of the PivotPoint coordinate system is chosen to be the cross product of the X and Y axes.
  
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkSpatialCalibrationOptimizer : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkSpatialCalibrationOptimizer,vtkObject);
  static vtkSpatialCalibrationOptimizer *New();

  /*! Initialize algorithm - clear the MarkerToReferenceTransformMatrix array */
  PlusStatus Initialize();

  /*!
    Insert acquired point to calibration point list
    \param aMarkerToReferenceTransformMatrix New calibration point (tool to reference transform)
  */
  PlusStatus InsertNextCalibrationPoint(vtkMatrix4x4* aMarkerToReferenceTransformMatrix);

  /*!
    Calibrate (call the minimizer and set the result)
    \param aTransformRepository Transform repository to save the results into
  */
  PlusStatus DoCalibrationOptimization(vtkTransformRepository* aTransformRepository = NULL);


public:

  vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(PivotPointToMarkerTransformMatrix, vtkMatrix4x4); 

  vtkGetStringMacro(ObjectMarkerCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(ObjectPivotPointCoordinateFrame);

protected:

  vtkSetObjectMacro(PivotPointToMarkerTransformMatrix, vtkMatrix4x4);

  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

  vtkSetObjectMacro(MarkerToReferenceTransformMatrixArray, vtkDoubleArray);

  vtkSetStringMacro(ObjectMarkerCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ObjectPivotPointCoordinateFrame);

protected:
  vtkSpatialCalibrationOptimizer();
  virtual  ~vtkSpatialCalibrationOptimizer();

protected:
  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData);

protected:
  /*! Pivot point to marker transform (eg. stylus tip to stylus) - the result of the calibration */
  vtkMatrix4x4*       PivotPointToMarkerTransformMatrix;

  /*! Uncertainty (standard deviation), error of the calibration result in mm */
  double              CalibrationError;

  /*! Minimizer algorithm object */
  vtkAmoebaMinimizer* Minimizer;

  /*! Array of the input points */
  vtkDoubleArray*     MarkerToReferenceTransformMatrixArray;

  /*! Name of the object marker coordinate frame (eg. Stylus) */
  char*               ObjectMarkerCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*               ReferenceCoordinateFrame;

  /*! Name of the object pivot point coordinate frame (eg. StylusTip) */
  char*               ObjectPivotPointCoordinateFrame;

};

#endif

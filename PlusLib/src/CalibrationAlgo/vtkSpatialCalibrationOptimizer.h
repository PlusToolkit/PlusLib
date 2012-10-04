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

#include "vnl/vnl_matrix.h"
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_trace.h>


#include <vcl_iostream.h>

#include "FidPatternRecognitionCommon.h"


class vtkTransformRepository;
class vtkXMLDataElement;
class NWire;

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
private:
  enum OptimizationMethod {
    OptimizationMethod_Method1,
    OptimizationMethod_Method2
  };

public:
  vtkTypeRevisionMacro(vtkSpatialCalibrationOptimizer,vtkObject);
  static vtkSpatialCalibrationOptimizer *New();

  /*! Calibrate (call the minimizer) */
  PlusStatus Update(OptimizationMethod aMethod);

  /*! Provides to the class the information necessary make the optimization */
  PlusStatus SetOptimizerData(std::vector< vnl_vector<double> > *DataPositionsInImageFrame, std::vector< vnl_vector<double> > *DataPositionsInProbeFrame, vnl_matrix<double> *ImageToProbeTransformMatrixVnl);

  /*! Provides to the class the information necessary make the optimization */
  PlusStatus SetOptimizerDataUsingNWires(std::vector< vnl_vector<double> > *SegmentedPointsInImageFrame, std::vector<NWire> *NWires, std::vector< vnl_matrix<double> > *probeToPhantomTransforms, vnl_matrix<double> *ImageToProbeTransformMatrixVnl);

  /*! Get optimized Image to Probe matrix */
  vnl_matrix<double> GetOptimizedImageToProbeTransformMatrix();

protected:
  /* Helper functions */
  static void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *vtkSpatialCalibrationOptimizerPointer);
  static void vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *vtkSpatialCalibrationOptimizerPointer);
  static vnl_matrix<double> TransformParametersToTransformMatrix(const vnl_vector<double> &transformParameters);
  static vnl_vector<double> TransformMatrixToParametersVector(const vnl_matrix<double> &transformMatrix);
  static vnl_double_3 RotationMatrixToRotationVersor (const vnl_double_3x3 &aRotationMatrix);
  static vnl_double_3x3 RotationVersorToRotationMatrix (const vnl_double_3 &aRotationVersor);
  static vnl_matrix<double> VectorToMatrix(const vnl_double_3 &vnlVector);
  static double PointToLineDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2 ); 
  static void vtkOptimizationMetricFunction(void *userData);

  /*! Set minimizer */
  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

  vtkSpatialCalibrationOptimizer();
  virtual  ~vtkSpatialCalibrationOptimizer();

  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData);

  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *userData);

protected:
  /*! Minimizer algorithm object */
  vtkAmoebaMinimizer* Minimizer;

  /*! Positions of segmented points in image frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInImageFrame;

  /*! Positions of segmented points in probe frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInProbeFrame;

  /*! Positions of ALL the segmented points in image frame - input to the second cost function (distance to wires) */
  std::vector< vnl_vector<double> > SegmentedPointsInImageFrame;

  /* Contains all the probe to phantom transforms used during calibration */
  std::vector< vnl_matrix<double> > ProbeToPhantomTransforms;

  /*! List of NWires used for calibration and error computation */
  std::vector<NWire> NWires;

  /*! Store the seed for the optimization process */
  vnl_matrix<double> ImageToProbeSeedTransformMatrixVnl;

  /*! Store the result of the optimization process */
  vnl_matrix<double> ImageToProbeTransformMatrixVnl;
};

#endif

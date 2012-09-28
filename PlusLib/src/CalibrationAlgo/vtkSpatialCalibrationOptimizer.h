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
public:
  vtkTypeRevisionMacro(vtkSpatialCalibrationOptimizer,vtkObject);
  static vtkSpatialCalibrationOptimizer *New();

  /*! Initialize algorithm - clear the MarkerToReferenceTransformMatrix array */
  PlusStatus Initialize();

  /*!
    Insert acquired point to calibration point list
    \param aMarkerToReferenceTransformMatrix New calibration point (tool to reference transform)
  */

   /*!
    Calibrate (call the minimizer)
  */
  PlusStatus Optimize(int method);

   /*! Set the result */
  PlusStatus DoCalibrationOptimization(vtkTransformRepository* aTransformRepository = NULL);

  /*! Provides to the class the information necessary make the optimization 
   */
  PlusStatus SetOptimizerData(std::vector< vnl_vector<double> > *DataPositionsInImageFrame, std::vector< vnl_vector<double> > *DataPositionsInProbeFrame, vnl_matrix<double> *imageToProbeTransformMatrixVnl);

  /*! Provides to the class the information necessary make the optimization 
   */
  PlusStatus SetOptimizerData2(std::vector< vnl_vector<double> > *SegmentedPointsInImageFrame, std::vector<NWire> *NWires, std::vector< vnl_matrix<double> > *probeToPhantomTransforms, vnl_matrix<double> *imageToProbeTransformMatrixVnl);
 

  vnl_matrix<double> GetOptimizedImageToProbeTransformMatrix(){return this->imageToProbeTransformMatrixVnl;};

public:


protected:

    /*! Positions of segmented points in image frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInImageFrame;

  /*! Positions of segmented points in probe frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInProbeFrame;

  /*! Positions of ALL the segmented points in image frame - input to the second cost function (distance to wires) */
  std::vector< vnl_vector<double> > SegmentedPointsInImageFrame;

  /* Contains all the probe to phantom transforms used during calibration */
  std::vector< vnl_matrix<double> > probeToPhantomTransforms;

  /*! List of NWires used for calibration and error computation */
  std::vector<NWire> NWires;

  /*! Store the seed for the optimization process */
  vnl_matrix<double> imageToProbeSeedTransformMatrixVnl;

  /*! Store the result of the optimization process */
  vnl_matrix<double> imageToProbeTransformMatrixVnl;

protected:
  vtkSpatialCalibrationOptimizer();
  virtual  ~vtkSpatialCalibrationOptimizer();

protected:
  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData);

    /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *userData);
  

protected:
  /*! Pivot point to marker transform (eg. stylus tip to stylus) - the result of the calibration */
  vtkMatrix4x4*       PivotPointToMarkerTransformMatrix;

  /*! Uncertainty (standard deviation), error of the calibration result in mm */
  double              CalibrationError;

  /*! Minimizer algorithm object */
  vtkAmoebaMinimizer* Minimizer;

  
  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

};

#endif

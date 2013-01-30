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
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_cross.h>

#include <vcl_iostream.h>

#include "FidPatternRecognitionCommon.h"
#include "itkFiducialTransformComputation.h"

#include <algorithm>
#include <PlusMath.h>


#include <set>

#define AMOEBA_ROTATION_PARAMETERS_SCALE 0.01
#define AMOEBA_TRANSLATION_PARAMETERS_SCALE 0.1
#define AMOEBA_SCALE_PARAMETERS_SCALE 0.005 
#define AMOEBA_MAX_ITERATIONS 2000
#define AMOEBA_TOLERANCE 1e-9

#define LM_ROTATION_PARAMETERS_SCALE 1
#define LM_TRANSLATION_PARAMETERS_SCALE 0.005
#define LM_SCALE_PARAMETERS_SCALE 1
#define LM_MAX_ITERATIONS 1200
#define LM_STEP 1e-11
#define LM_FUNCTION_TOLERANCE 1e-10
#define LM_GRADIENT_TOLERANCE 1e-5
#define LM_PARAMETERS_TOLERANCE 1e-8

class vtkTransformRepository;
class vtkXMLDataElement;
class NWire;

//-----------------------------------------------------------------------------

/*!
  \class vtkSpatialCalibrationOptimizer 
  \brief Optimizing an image to probe transform.
  \ingroup PlusLibCalibrationAlgo
*/
class vtkSpatialCalibrationOptimizer : public vtkObject
{

public:

    /* Choose one of the posible metrics*/
  enum ImageToProbeCalibrationCostFunctionType {
    MINIMIZATION_3D,
    MINIMIZATION_2D
  };

  /* Choose one of the optimization methods*/
  enum ImageToProbeCalibrationOptimizationMethodType{
    NO_OPTIMIZATION, 
    VTK_AMOEBA_MINIMIZER, 
    ITK_LEVENBERG_MARQUARD,
	  FIDUCIALS_SIMILARITY
  };

  
    /*! Number of parameters in the optimization process */
  static double NumberOfParameters;

  static vnl_double_3x3 RotationVersorToRotationMatrix (const vnl_double_3 &aRotationVersor);  
  static double PointToWireDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2);
  static vnl_matrix<double> TransformParametersToTransformMatrix(const vnl_vector<double> &transformParameters);

  vtkTypeRevisionMacro(vtkSpatialCalibrationOptimizer,vtkObject);
  static vtkSpatialCalibrationOptimizer *New();



  ImageToProbeCalibrationCostFunctionType CurrentImageToProbeCalibrationCostFunction;
  ImageToProbeCalibrationOptimizationMethodType CurrentImageToProbeCalibrationOptimizationMethod;

  PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig );

  /*! Calibrate (call the minimizer) */
  PlusStatus Update();

  /*! Provides to the class the information necessary make the optimization */
  PlusStatus SetInputDataForMiddlePointMethod(std::vector< vnl_vector<double> > *DataPositionsInImageFrame, std::vector< vnl_vector<double> > *DataPositionsInProbeFrame, vnl_matrix<double> *ImageToProbeTransformMatrixVnl, std::set<int>* outliers);

  /*! Provides to the class the information necessary make the optimization */
  PlusStatus SetOptimizerDataUsingNWires(std::vector< vnl_vector<double> > *SegmentedPointsInImageFrame, std::vector<NWire> *NWires, std::vector< vnl_matrix<double> > *probeToPhantomTransforms, vnl_matrix<double> *ImageToProbeTransformMatrixVnl, std::set<int>* outliers);

  /*! Get optimized Image to Probe matrix */
  vnl_matrix<double> GetOptimizedImageToProbeTransformMatrix();

  static char* GetCalibrationCostFunctionAsString(ImageToProbeCalibrationCostFunctionType type);

  static char* GetCalibrationOptimizationMethodAsString(ImageToProbeCalibrationOptimizationMethodType type);

  std::vector<double> GetOptimizationResults()
  {
	  return optimizationResults;
  };

protected:
  /* Helper functions */
  static void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *vtkSpatialCalibrationOptimizerPointer);
  static void vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *vtkSpatialCalibrationOptimizerPointer);
  static vnl_vector<double> TransformMatrixToParametersVector(const vnl_matrix<double> &transformMatrix);
  static vnl_double_3 RotationMatrixToRotationVersor (const vnl_double_3x3 &aRotationMatrix);
  static vnl_matrix<double> VectorToMatrix(const vnl_double_3 &vnlVector);
  static double PointToLineDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2 ); 
  static void vtkOptimizationMetricFunction(void *userData);
  PlusStatus ShowTransformation(const vnl_matrix<double> &transformationMatrix);
  void StoreAndShowResults();
  void SetVtkAmoebaMinimizerInitialParameters(vtkAmoebaMinimizer *minimizer, const vnl_vector<double> &parametersVector);
  void ComputeRmsError(const vnl_matrix<double> &transformationMatrix, double *rmsError, double *rmsErrorSD);
  
  /*! Set minimizer */
  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

  vtkSpatialCalibrationOptimizer();
  virtual  ~vtkSpatialCalibrationOptimizer();

  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData);

  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *userData);

  /* Run the Levenberg Marquard Optimization*/
  int itkRunLevenbergMarquardOptimization( bool useGradient, 
                double fTolerance, double gTolerance, double xTolerance, 
                double epsilonFunction, int maxIterations );

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

  /* Store the parameters of the optimization  [r1 r2 r3 t1 t2 t3 sx sy] */
  vnl_vector<double> parametersVector; 
   
  /*! Number of segmented points that will be used during the optimization process. It is used by Levenberg Marquard Algorithm */
  int NumberOfResiduals;

  double rotationParametersScale;
  double translationParametersScale;
  double scalesParametersScale;
  int amoebaMaxIterations;
  double amoebaTolerance;


  double LMFunctionTolerance;  // Function value tolerance
  double LMGradienteTolerance;  // Gradient magnitude tolerance
  double LMParametersTolerance;  // Search space tolerance
  double LMEpsilonFunction;  // Step
  int    LMMaxIterations;  // Maximum number of iterations

  /*! store the residuals used during the optimization */
  std::vector<double> minimizationResiduals;

  /*! store the results of the optimization [numberOfOptimizedParameters currentCostFunction
                                             q1 q2 q3 q4 t1 t2 t3 sx ratio crossProduct rmsError rmsErrorSD ... <-- initial 
                                             q1 q2 q3 q4 t1 t2 t3 sx ratio crossProduct rmsError rmsErrorSD ... <-- optimized
                                             angleDifference ]*/
  std::vector<double> optimizationResults;
  // bool quaternionsRepresentation;
};

#endif

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkSpatialCalibrationOptimizer_h
#define __vtkSpatialCalibrationOptimizer_h

#include "PlusConfigure.h"

#include "vtkObject.h"
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
#include "itkScaleVersor3DTransform.h"
#include "itkSimilarity3DTransform.h"

#include <algorithm>
#include <PlusMath.h>



#include <set>

class vtkTransformRepository;
class vtkXMLDataElement;
class NWire;

//-----------------------------------------------------------------------------

/*!
  \class vtkSpatialCalibrationOptimizer 
  \brief Optimizing an image to probe transform.

  It uses the OptimizationMethod (NONE/2D/3D) and IsotropicPixelSpacing (TRUE/FALSE) attributes of the 
  probe calibration algorithm element to configure the optimization.

  \ingroup PlusLibCalibrationAlgo
*/
class vtkSpatialCalibrationOptimizer : public vtkObject
{

public:
  
  /* Choose one of the posible metrics*/
  enum OptimizationMethodType 
  {
    MINIMIZE_NONE,
    MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D,
    MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D
  };
  
  static double PointToWireDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2);

  vtkTypeRevisionMacro(vtkSpatialCalibrationOptimizer,vtkObject);
  static vtkSpatialCalibrationOptimizer *New();

  PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig );

  /*! Returns true if optimization is requested (the OptimizationMethod is defined and not "NONE") */
  bool Enabled();

  /*! Calibrate (call the minimizer) */
  PlusStatus Update();

  /*! Provides to the class the information necessary make the optimization */
  PlusStatus SetInputDataForMiddlePointMethod(std::vector< vnl_vector<double> > *DataPositionsInImageFrame, std::vector< vnl_vector<double> > *DataPositionsInProbeFrame, vnl_matrix<double> *ImageToProbeTransformMatrixVnl, std::set<int>* outliers);

  /*! Provides to the class the information necessary make the optimization */
  PlusStatus SetOptimizerDataUsingNWires(std::vector< vnl_vector<double> > *SegmentedPointsInImageFrame, std::vector<NWire> *NWires, std::vector< vnl_matrix<double> > *probeToPhantomTransforms, vnl_matrix<double> *ImageToProbeTransformMatrixVnl, std::set<int>* outliers);

  /*! Get optimized Image to Probe matrix */
  vnl_matrix<double> GetOptimizedImageToProbeTransformMatrix();

  void ComputeRmsError(const vnl_matrix<double> &transformationMatrix, double &rmsError, double &rmsErrorSD);

  bool GetIsotropicPixelSpacing() { return this->IsotropicPixelSpacing; }
  void SetIsotropicPixelSpacing(bool isotropicPixelSpacing) { this->IsotropicPixelSpacing=isotropicPixelSpacing; }

  OptimizationMethodType GetOptimizationMethod() { return this->OptimizationMethod; }
  void SetOptimizationMethod(OptimizationMethodType optimizationMethod) { this->OptimizationMethod=optimizationMethod; }
  static char* GetOptimizationMethodAsString(OptimizationMethodType type);

protected:

  PlusStatus ShowTransformation(const vnl_matrix<double> &transformationMatrix);
  void StoreAndShowResults();  
  
  vtkSpatialCalibrationOptimizer();
  virtual  ~vtkSpatialCalibrationOptimizer();

protected:
  /*! If true then X and Y pixel spacing is forced to be the same during the optimization */
  bool IsotropicPixelSpacing;

  /*! Cost function to minimize during the optimization */
  OptimizationMethodType OptimizationMethod;

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
   
  double RotationParametersScale;
  double TranslationParametersScale;
  double ScalesParametersScale; 

  /*! store the residuals used during the optimization */
  std::vector<double> MinimizationResiduals;

};

#endif

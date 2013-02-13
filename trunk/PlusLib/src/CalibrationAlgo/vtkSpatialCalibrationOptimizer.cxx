/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkSpatialCalibrationOptimizer.h"
#include "vtkTransformRepository.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"
#include "itkAmoebaOptimizer.h"

#include "vtkLine.h"
#include "vtkPlane.h"

typedef  itk::AmoebaOptimizer  OptimizerType;

//-----------------------------------------------------------------------------
class DistanceToWiresCostFunction : public itk::SingleValuedCostFunction 
{
public:

  typedef DistanceToWiresCostFunction       Self;
  typedef itk::SingleValuedCostFunction     Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;
  itkNewMacro( Self );
  itkTypeMacro( DistanceToWiresCostFunction, SingleValuedCostFunction );

  typedef Superclass::ParametersType              ParametersType;
  typedef Superclass::DerivativeType              DerivativeType;

  typedef vnl_vector<double>                      VectorType;
  typedef vnl_matrix<double>                      MatrixType;

  typedef itk::VersorRigid3DTransform< double > RigidTransformType;

  typedef double MeasureType ;

  DistanceToWiresCostFunction() {};


  DistanceToWiresCostFunction(vtkSpatialCalibrationOptimizer* calibrationOptimizer) 
  {
    m_CalibrationOptimizer=calibrationOptimizer;
  }

  static PlusStatus GetTransformMatrix(vnl_matrix<double>& imageToProbeTransform_vnl, const ParametersType & imageToProbeTransformParameters)
  {
    imageToProbeTransform_vnl.set_size(4,4);
    imageToProbeTransform_vnl.set_identity();

    if (imageToProbeTransformParameters.GetSize()!=7 && imageToProbeTransformParameters.GetSize()!=8)
    {
      LOG_ERROR("GetTransformMatrix expects 7 or 8 parameters");
      return PLUS_FAIL;
    }

    RigidTransformType::Pointer rigidTransform=RigidTransformType::New();
    rigidTransform->SetParameters(imageToProbeTransformParameters);
    imageToProbeTransform_vnl.update(rigidTransform->GetMatrix().GetVnlMatrix());
    imageToProbeTransform_vnl.put(0,3,rigidTransform->GetOffset()[0]);
    imageToProbeTransform_vnl.put(1,3,rigidTransform->GetOffset()[1]);
    imageToProbeTransform_vnl.put(2,3,rigidTransform->GetOffset()[2]);

    bool isotropicPixelSpacing=(imageToProbeTransformParameters.GetSize()==7);
    if (isotropicPixelSpacing)
    {
      // X and Y pixel spacing are the same
      imageToProbeTransform_vnl.scale_column(0,imageToProbeTransformParameters(6));
      imageToProbeTransform_vnl.scale_column(1,imageToProbeTransformParameters(6));
      imageToProbeTransform_vnl.scale_column(2,imageToProbeTransformParameters(6));
    }
    else
    {
      // X and Y pixel spacing are different
      imageToProbeTransform_vnl.scale_column(0,imageToProbeTransformParameters(6));
      imageToProbeTransform_vnl.scale_column(1,imageToProbeTransformParameters(7));
      imageToProbeTransform_vnl.scale_column(2,(imageToProbeTransformParameters(6)+imageToProbeTransformParameters(7))/2);
    }
    return PLUS_SUCCESS;
  }

  static PlusStatus GetTransformParameters(ParametersType& imageToProbeTransformParameters, const vnl_matrix<double>& imageToProbeTransform_vnl)
  {
    if (imageToProbeTransformParameters.GetSize()!=7 && imageToProbeTransformParameters.GetSize()!=8)
    {
      LOG_ERROR("GetTransformMatrix expects 7 or 8 parameters");
      return PLUS_FAIL;
    }    

    // Decompose the initial calibration matrix to an orthogonal transformation matrix, scaling vector, and translation vector
    vnl_matrix<double> rotationMatrix= imageToProbeTransform_vnl.extract(3,3);
    vnl_svd<double> svd(rotationMatrix);
    vnl_matrix<double> orthogonalizedRotationMatrix(3, 3);
    orthogonalizedRotationMatrix = svd.U() * svd.V().transpose(); 
    double scale[3] = { svd.W(0), svd.W(1), svd.W(2) };

    // Rotation versor (unit quaternion parameters)
    RigidTransformType::Pointer imageToProbeTransformRigid = RigidTransformType::New();
    imageToProbeTransformRigid->SetMatrix(orthogonalizedRotationMatrix );
    RigidTransformType::ParametersType rigidParameters=imageToProbeTransformRigid->GetParameters();
    imageToProbeTransformParameters[0]=rigidParameters[0];
    imageToProbeTransformParameters[1]=rigidParameters[1];
    imageToProbeTransformParameters[2]=rigidParameters[2];

    // Translation
    imageToProbeTransformParameters[3]=imageToProbeTransform_vnl.get(0,3);
    imageToProbeTransformParameters[4]=imageToProbeTransform_vnl.get(1,3);
    imageToProbeTransformParameters[5]=imageToProbeTransform_vnl.get(2,3);
    
    // Scaling
    bool isotropicPixelSpacing=(imageToProbeTransformParameters.GetSize()==7);
    if (isotropicPixelSpacing)
    {
      imageToProbeTransformParameters[6]=(scale[0]+scale[1])/2.0;
    }
    else
    {
      imageToProbeTransformParameters[6]=scale[0];
      imageToProbeTransformParameters[7]=scale[1];
    }
    return PLUS_SUCCESS;
  }

  double GetValue( const ParametersType & imageToProbeTransformParameters ) const
  {
    vnl_matrix<double> imageToProbeTransform_vnl;
    GetTransformMatrix(imageToProbeTransform_vnl, imageToProbeTransformParameters);    
    double rmsError=0.0;
    double rmsErrorSd=0.0;
    m_CalibrationOptimizer->ComputeRmsError(imageToProbeTransform_vnl, rmsError, rmsErrorSd);
    return rmsError;
  }

  void GetDerivative( const ParametersType & parameters, DerivativeType  & derivative ) const
  {
    LOG_ERROR("GetDerivative is not implemented");
  }

  unsigned int GetNumberOfParameters(void) const
  {
    if (m_CalibrationOptimizer->GetIsotropicPixelSpacing())
    {
      return 7; // 3 rotation + 3 translation + 1 scaling
    }
    else
    {
      return 8; // 3 rotation + 3 translation + 2 scaling
    }
  }

private:
  vtkSpatialCalibrationOptimizer* m_CalibrationOptimizer;
}; 

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef CommandIterationUpdate Self;
  typedef itk::Command Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() 
  {
    m_IterationNumber=0;
  }
public:


  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    LOG_DEBUG( "Observer::Execute()");
    const OptimizerType* optimizer = dynamic_cast< const OptimizerType* >( object );
    if( m_FunctionEvent.CheckEvent( &event ) )
    {
      LOG_DEBUG( m_IterationNumber++ );
      LOG_DEBUG( optimizer->GetCachedValue() );
      LOG_DEBUG( optimizer->GetCachedCurrentPosition());
      OptimizerType::MeasureType errorValue=optimizer->GetCachedValue();
      m_ErrorValues.push_back(errorValue);      
    }
    else if( m_GradientEvent.CheckEvent( &event ) )
    {
      LOG_DEBUG("Gradient " << optimizer->GetCachedDerivative() << "   ");
    }
  }

  const std::vector<double>& GetErrorValues()
  {
    return m_ErrorValues;
  }
private:
  unsigned long m_IterationNumber;
  itk::FunctionEvaluationIterationEvent m_FunctionEvent;
  itk::GradientEvaluationIterationEvent m_GradientEvent;
  std::vector<double> m_ErrorValues;
};

//-----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkSpatialCalibrationOptimizer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSpatialCalibrationOptimizer);

//-----------------------------------------------------------------------------
vtkSpatialCalibrationOptimizer::vtkSpatialCalibrationOptimizer()
: IsotropicPixelSpacing(true)
, RotationParametersScale(1.0)
, TranslationParametersScale(0.005)
, ScalesParametersScale(1.0)
{  
}

//-----------------------------------------------------------------------------
vtkSpatialCalibrationOptimizer::~vtkSpatialCalibrationOptimizer()
{
}

//-----------------------------------------------------------------------------
double vtkSpatialCalibrationOptimizer::PointToWireDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2 )
{
  //norm(cross( (LP2-LP1), (P-LP1) ) ) / norm(LP2-LP1);
  //double d = vnl_cross_3d(LP2-LP1,P-LP1).two_norm() / (LP2-LP1).two_norm();

  // Convert point from vnl to vtk format
  double wireP0[3] = {aLineEndPoint1[0], aLineEndPoint1[1], aLineEndPoint1[2]};
  double wireP1[3] = {aLineEndPoint2[0], aLineEndPoint2[1], aLineEndPoint2[2]};
  double segmentedPoint[3] = {aPoint(0),aPoint(1),aPoint(2)};
  double distanceToWire = sqrt(vtkLine::DistanceToLine(segmentedPoint, wireP0, wireP1));
  return distanceToWire;
}

// --------------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::ComputeRmsError(const vnl_matrix<double> &transformationMatrix, double &aRmsError, double &aRmsErrorSD)
{
  int numberOfFrames = 0;
  double rmsError = 0;
  double px,py,pz,dx,dy,dz =0;
  double squaredDistanceToMiddleWire=0;
  std::vector<double> reprojectionErrors;

  switch (this->OptimizationMethod)
  {

  case MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D:
    {
      numberOfFrames = this->DataPositionsInImageFrame.size();
      for(int i=0;i<numberOfFrames;i++)
      {
        px = transformationMatrix[0][0]*this->DataPositionsInImageFrame[i][0] + transformationMatrix[0][1]*this->DataPositionsInImageFrame[i][1] + 
          transformationMatrix[0][2]*this->DataPositionsInImageFrame[i][2] + transformationMatrix[0][3];
        py = transformationMatrix[1][0]*this->DataPositionsInImageFrame[i][0] + transformationMatrix[1][1]*this->DataPositionsInImageFrame[i][1] + 
          transformationMatrix[1][2]*this->DataPositionsInImageFrame[i][2] + transformationMatrix[1][3];
        pz = transformationMatrix[2][0]*this->DataPositionsInImageFrame[i][0] + transformationMatrix[2][1]*this->DataPositionsInImageFrame[i][1] + 
          transformationMatrix[2][2]*this->DataPositionsInImageFrame[i][2] + transformationMatrix[2][3];

        dx = px - this->DataPositionsInProbeFrame[i][0];
        dy = py - this->DataPositionsInProbeFrame[i][1];
        dz = pz - this->DataPositionsInProbeFrame[i][2];

        squaredDistanceToMiddleWire = dx*dx + dy*dy + dz*dz;
        rmsError += squaredDistanceToMiddleWire;
        reprojectionErrors.push_back(sqrt(squaredDistanceToMiddleWire));
      }
      rmsError = sqrt(rmsError/numberOfFrames);
      break;
    }
  case MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D:
    {
      int nWires = this->NWires.size(); 
      int m = this->SegmentedPointsInImageFrame.size()/(3*nWires); //number of frames
      double residual=0;

      vnl_vector<double> segmentedInProbeFrame_vnl(4), segmentedInPhantomFrame_vnl(4);
      vnl_vector<double> wireFrontPoint_vnl(3), wireBackPoint_vnl(3);
      int currentSegmentedPoint = 0;

      for(int i=0;i<m;i++) // for each frame
      {
        vnl_matrix<double> probeToPhantomTransform_vnl = this->ProbeToPhantomTransforms[i];

        for (int j=0;j<3*nWires;j++)  // for each segmented point
        {
          // Find the projection in the probe frame 
          px = transformationMatrix[0][0]*this->SegmentedPointsInImageFrame[3*nWires*i+j][0] + transformationMatrix[0][1]*this->SegmentedPointsInImageFrame[3*nWires*i+j][1] + 
            transformationMatrix[0][2]*this->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ transformationMatrix[0][3];
          py = transformationMatrix[1][0]*this->SegmentedPointsInImageFrame[3*nWires*i+j][0] + transformationMatrix[1][1]*this->SegmentedPointsInImageFrame[3*nWires*i+j][1] + 
            transformationMatrix[1][2]*this->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ transformationMatrix[1][3];
          pz = transformationMatrix[2][0]*this->SegmentedPointsInImageFrame[3*nWires*i+j][0] + transformationMatrix[2][1]*this->SegmentedPointsInImageFrame[3*nWires*i+j][1] + 
            transformationMatrix[2][2]*this->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ transformationMatrix[2][3];

          segmentedInProbeFrame_vnl[0] = px; 
          segmentedInProbeFrame_vnl[1] = py; 
          segmentedInProbeFrame_vnl[2] = pz; 
          segmentedInProbeFrame_vnl[3] = 1;

          // Transform points from image to phantom frame
          segmentedInPhantomFrame_vnl = probeToPhantomTransform_vnl * segmentedInProbeFrame_vnl;

          // compute distance to wire
          int line = j%3;
          int w = j/3; 

          wireFrontPoint_vnl[0]= this->NWires[w].Wires[line].EndPointFront[0];
          wireFrontPoint_vnl[1]= this->NWires[w].Wires[line].EndPointFront[1];
          wireFrontPoint_vnl[2]= this->NWires[w].Wires[line].EndPointFront[2];
          wireBackPoint_vnl[0]= this->NWires[w].Wires[line].EndPointBack[0];
          wireBackPoint_vnl[1]= this->NWires[w].Wires[line].EndPointBack[1];
          wireBackPoint_vnl[2]= this->NWires[w].Wires[line].EndPointBack[2];

          double distanceToWire = vtkSpatialCalibrationOptimizer::PointToWireDistance(segmentedInPhantomFrame_vnl.extract(3,0), wireFrontPoint_vnl, wireBackPoint_vnl );
          reprojectionErrors.push_back(distanceToWire);
          rmsError += distanceToWire*distanceToWire;
        }
      }
      rmsError = sqrt(rmsError/(3*nWires*m));
      break;
    }
  default:
    LOG_ERROR("Invalid cost function");
  }

  double squareDiffSum = 0;
  int numberOfProjecterPoints = reprojectionErrors.size();
  for (int j=0; j<numberOfProjecterPoints; ++j)
  {
    double diff = reprojectionErrors.at(j) - rmsError;
    squareDiffSum += diff * diff;
  }
  double variance = squareDiffSum / numberOfProjecterPoints;
  aRmsError = rmsError;
  aRmsErrorSD = sqrt(variance);
}

//-----------------------------------------------------------------------------
PlusStatus vtkSpatialCalibrationOptimizer::ShowTransformation(const vnl_matrix<double> &imageToProbeTransformationMatrix)
{
  LOG_INFO("Translation = [" << imageToProbeTransformationMatrix.get(0,3) << " " << imageToProbeTransformationMatrix.get(1,3) << " " << imageToProbeTransformationMatrix.get(2,3) << " ]");

  vnl_matrix<double> rotationMatrix=imageToProbeTransformationMatrix.extract(3,3);
  vnl_svd<double> svd(rotationMatrix);
  vnl_matrix<double> orthogonalizedRotationMatrix(3, 3);
  orthogonalizedRotationMatrix = svd.U() * svd.V().transpose(); 
  double scale[3] = { svd.W(0), svd.W(1), svd.W(2) };
  LOG_INFO("Scale = [" << scale[0] << " " << scale[1] << " " << scale[2] << " ]");

  vnl_vector<double> xAxis=imageToProbeTransformationMatrix.get_column(0);
  xAxis.normalize();
  vnl_vector<double> yAxis=imageToProbeTransformationMatrix.get_column(1);
  yAxis.normalize();
  double xyAxesAngleDeg=vtkMath::DegreesFromRadians(acos(dot_product(xAxis,yAxis)));
  LOG_INFO("XY axes angle = " << xyAxesAngleDeg << " deg");

  double rmsError =0, rmsErrorSD = 0;
  ComputeRmsError(imageToProbeTransformationMatrix, rmsError, rmsErrorSD);
  LOG_INFO("rmsError = " << rmsError << " mm");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::StoreAndShowResults()
{
  LOG_INFO("Cost function = " << GetOptimizationMethodAsString(this->OptimizationMethod));

  LOG_INFO("Without optimization:");
  ShowTransformation(this->ImageToProbeSeedTransformMatrixVnl);

  LOG_INFO("With optimization:");
  ShowTransformation(this->ImageToProbeTransformMatrixVnl);

  vtkSmartPointer<vtkMatrix4x4> imageToProbeSeedTransformMatrixVtk = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> imageToProbeTransformMatrixVtk = vtkSmartPointer<vtkMatrix4x4>::New();
  PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeSeedTransformMatrixVnl,imageToProbeSeedTransformMatrixVtk);
  PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeTransformMatrixVnl,imageToProbeTransformMatrixVtk);
  double angleDifference = PlusMath::GetOrientationDifference(imageToProbeSeedTransformMatrixVtk, imageToProbeTransformMatrixVtk);
  LOG_INFO("Orientation difference between unoptimized and optimized matrices =  " << angleDifference << " deg");

  LOG_INFO("Optimization details:");
  // Print how the residual error changed during the optimization (limit to max. 30 samples)
  int stepSize=std::max<int>(1,this->MinimizationResiduals.size()/30);
  for( int i=0; i < this->MinimizationResiduals.size() ; i+=stepSize )
  {
    LOG_INFO("FunctionEvaluation " << i << ":  rmsError = " << this->MinimizationResiduals[i]);
  }
  if (this->MinimizationResiduals.size()>0)
  {
    LOG_DEBUG("First Iteration: rmsError = " << this->MinimizationResiduals[1] << " pixels");
    LOG_DEBUG("Last Iteration (" << this->MinimizationResiduals.size() << "):  rmsError = " << this->MinimizationResiduals[this->MinimizationResiduals.size()-1] << " pixels");
  }
  else
  {
    LOG_ERROR("No error metrics are available");
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkSpatialCalibrationOptimizer::Update()
{  
  DistanceToWiresCostFunction::Pointer costFunction = new DistanceToWiresCostFunction(this);

  DistanceToWiresCostFunction::ParametersType imageToProbeSeedTransformParameters(costFunction->GetNumberOfParameters());
  DistanceToWiresCostFunction::GetTransformParameters(imageToProbeSeedTransformParameters, this->ImageToProbeSeedTransformMatrixVnl);

  double rmsError=0.0;
  double rmsErrorSd=0.0;
  ComputeRmsError(this->ImageToProbeSeedTransformMatrixVnl, rmsError, rmsErrorSd);
  LOG_INFO("Initial cost function value with unconstrained matrix = " << rmsError );
  {
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeSeedTransformMatrixVnl, vtkMatrix); 
    PlusMath::LogVtkMatrix(vtkMatrix);
  }

  double initialError=costFunction->GetValue(imageToProbeSeedTransformParameters);
  LOG_INFO("Initial cost function value with constrained matrix= " << initialError );
  {
    vnl_matrix<double> imageToProbeTransform_vnl;
    DistanceToWiresCostFunction::GetTransformMatrix(imageToProbeTransform_vnl, imageToProbeSeedTransformParameters);
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    PlusMath::ConvertVnlMatrixToVtkMatrix(imageToProbeTransform_vnl, vtkMatrix); 
    PlusMath::LogVtkMatrix(vtkMatrix);
  }

  OptimizerType::Pointer  optimizer = OptimizerType::New();
  try 
  {
    optimizer->SetCostFunction( costFunction.GetPointer() );
  }
  catch( itk::ExceptionObject & e )
  {
    LOG_ERROR("Exception thrown ! An error ocurred during Optimization: "<<e);
    return PLUS_FAIL;
  }

  optimizer->SetMaximumNumberOfIterations( 1000 );
  optimizer->SetParametersConvergenceTolerance( 0.01 );
  optimizer->SetFunctionConvergenceTolerance( 0.001 );
  
  // Scale the translation components of the transform in the Optimizer
  OptimizerType::ScalesType scales( costFunction->GetNumberOfParameters() );
  switch (costFunction->GetNumberOfParameters())
  {
  case 7:
    scales[0] = this->RotationParametersScale;
    scales[1] = this->RotationParametersScale;
    scales[2] = this->RotationParametersScale;
    scales[3] = this->TranslationParametersScale; 
    scales[4] = this->TranslationParametersScale; 
    scales[5] = this->TranslationParametersScale;
    scales[6] = this->ScalesParametersScale;  
    break;
  case 8:
    scales[0] = this->RotationParametersScale;
    scales[1] = this->RotationParametersScale;
    scales[2] = this->RotationParametersScale;
    scales[3] = this->TranslationParametersScale; 
    scales[4] = this->TranslationParametersScale; 
    scales[5] = this->TranslationParametersScale;
    scales[6] = this->ScalesParametersScale;  
    scales[7] = this->ScalesParametersScale;  
    break;
  default:
    LOG_ERROR("Number of transformation parameters is incorrect");
    return PLUS_FAIL;
  }
  optimizer->SetScales(scales);

  optimizer->SetInitialPosition(imageToProbeSeedTransformParameters);

  CommandIterationUpdate::Pointer observer =  CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
  optimizer->AddObserver( itk::FunctionEvaluationIterationEvent(), observer );
  try 
  {
    optimizer->StartOptimization();
  }
  catch( itk::ExceptionObject & e )
  {
    LOG_ERROR("Exception thrown ! An error ocurred during Optimization: Location = " << e.GetLocation() << "Description = " << e.GetDescription());
    return PLUS_FAIL;
  }

  std::string stopCondition=optimizer->GetStopConditionDescription();
  LOG_INFO("Optimization stopping condition: "<<stopCondition);

  // save residuals (that was stored temporarily in the observer)
  this->MinimizationResiduals = observer->GetErrorValues();

  LOG_DEBUG( "Stop description   = " << optimizer->GetStopConditionDescription());

  // Store the matrix

  costFunction->GetTransformMatrix(this->ImageToProbeTransformMatrixVnl, optimizer->GetCurrentPosition());
  {
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeTransformMatrixVnl, vtkMatrix); 
    PlusMath::LogVtkMatrix(vtkMatrix);
  }

  // Store the optimized parameters and show the results
  StoreAndShowResults();

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::SetInputDataForMiddlePointMethod(std::vector< vnl_vector<double> > *DataPositionsInImageFrame, std::vector< vnl_vector<double> > *DataPositionsInProbeFrame, vnl_matrix<double> *ImageToProbeTransformMatrixVnl,std::set<int>* outliers)
{
  for(int i=0;i<DataPositionsInImageFrame->size();i++)
  { 
    if(outliers->find(i) == outliers->end()) // if is not an outlier
    {
      this->DataPositionsInImageFrame.push_back(DataPositionsInImageFrame->at(i));
      this->DataPositionsInProbeFrame.push_back(DataPositionsInProbeFrame->at(i));
    }
  }
  this->ImageToProbeSeedTransformMatrixVnl= *ImageToProbeTransformMatrixVnl;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::SetOptimizerDataUsingNWires(std::vector< vnl_vector<double> > *SegmentedPointsInImageFrame, std::vector<NWire> *NWires, std::vector< vnl_matrix<double> > *ProbeToPhantomTransforms, vnl_matrix<double> *ImageToProbeTransformMatrixVnl,std::set<int>* outliers)
{
  std::set<int> outliersProbeToPhantomTransforms;

  int numberOfWires = NWires->size();
  for (std::set<int>::const_iterator it = outliers->begin();it != outliers->end(); ++it)
  {
    outliersProbeToPhantomTransforms.insert(*it / numberOfWires);
  }

  LOG_INFO(outliersProbeToPhantomTransforms.size() << " probe to phantom transforms outliers were found");

  for(int i=0;i<ProbeToPhantomTransforms->size();i++)
  { 
    if(outliersProbeToPhantomTransforms.find(i) == outliersProbeToPhantomTransforms.end()) // if is not an outlier
    {
      this->ProbeToPhantomTransforms.push_back(ProbeToPhantomTransforms->at(i));
      for (int j=0;j<3*numberOfWires;j++)
      {
        this->SegmentedPointsInImageFrame.push_back(SegmentedPointsInImageFrame->at(3*numberOfWires*i+j));
      }
    }
  }

  this->NWires = * NWires;
  this->ImageToProbeSeedTransformMatrixVnl= *ImageToProbeTransformMatrixVnl;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vnl_matrix<double> vtkSpatialCalibrationOptimizer::GetOptimizedImageToProbeTransformMatrix()
{
  return this->ImageToProbeTransformMatrixVnl;
}

//----------------------------------------------------------------------------
bool vtkSpatialCalibrationOptimizer::Enabled()
{
  if (this->OptimizationMethod==MINIMIZE_NONE)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
char* vtkSpatialCalibrationOptimizer::GetOptimizationMethodAsString(OptimizationMethodType type)
{
  switch (type)
  {
  case MINIMIZE_NONE: return "NONE";
  case MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D: return "3D";
  case MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D: return "2D";
  default:
    LOG_ERROR("Unknown cost function to perform calibration: "<<type);
    return "unknown";
  }
}

PlusStatus vtkSpatialCalibrationOptimizer::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::ReadConfiguration"); 
  if ( aConfig == NULL )
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  // vtkSpatialCalibrationOptimizer section
  const char* optimizationMethod=aConfig->GetAttribute("OptimizationMethod");
  if (optimizationMethod==NULL)
  {
    // no optimization method is defined, assume no optimization is needed
    this->OptimizationMethod=MINIMIZE_NONE;
  }
  else if (STRCASECMP(optimizationMethod, GetOptimizationMethodAsString(MINIMIZE_NONE)) == 0)
  {
    this->OptimizationMethod=MINIMIZE_NONE;
  }
  else if (STRCASECMP(optimizationMethod, GetOptimizationMethodAsString(MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D)) == 0)
  {
    this->OptimizationMethod=MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D;
  }
  else if (STRCASECMP(optimizationMethod, GetOptimizationMethodAsString(MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D)) == 0)
  {
    this->OptimizationMethod=MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D;
  }

  const char* isotropicPixelSpacing = aConfig->GetAttribute("IsotropicPixelSpacing"); 
  if ( isotropicPixelSpacing != NULL ) 
  {
    if ( STRCASECMP("TRUE", isotropicPixelSpacing ) == 0 )
    {
      this->IsotropicPixelSpacing = true; 
    }
    else if ( STRCASECMP("FALSE", isotropicPixelSpacing ) == 0 )
    {
      this->IsotropicPixelSpacing = false; 
    }
    else
    {
      LOG_WARNING("Unable to recognize IsotropicPixelSpacing attribute: " << isotropicPixelSpacing << " - changed to true by default!"); 
      this->IsotropicPixelSpacing = true; 
    }
  }

  if (this->IsotropicPixelSpacing)
  {
    // isotropic
    // TODO: set up the appropriate transform
  }
  else
  {
    // anisotropic
    // TODO: set up the appropriate transform
  }

  return PLUS_SUCCESS;
}

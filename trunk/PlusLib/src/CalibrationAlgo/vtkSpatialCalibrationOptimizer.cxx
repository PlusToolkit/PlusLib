/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkSpatialCalibrationOptimizer.h"
#include "vtkProbeCalibrationAlgo.h"
#include "vtkTransformRepository.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"
#include "itkAmoebaOptimizer.h"

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
  typedef itk::VersorRigid3DTransform< double > RigidTransformType;

  DistanceToWiresCostFunction()
  : m_CalibrationOptimizer(NULL)
  {
  }

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
    double errorMean=0.0;
    double errorStDev=0.0;
    double errorRms=0.0;
    m_CalibrationOptimizer->ComputeError(imageToProbeTransform_vnl, errorMean, errorStDev, errorRms);
    return errorRms;
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

//-----------------------------------------------------------------------------
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
, ProbeCalibrationAlgo(NULL)
{  
}

//-----------------------------------------------------------------------------
vtkSpatialCalibrationOptimizer::~vtkSpatialCalibrationOptimizer()
{
}

//-----------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::SetProbeCalibrationAlgo(vtkProbeCalibrationAlgo* probeCalibrationAlgo)
{
  this->ProbeCalibrationAlgo=probeCalibrationAlgo;
}

//--------------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::ComputeError(const vnl_matrix<double> &transformationMatrix, double &errorMean, double &errorStDev, double &errorRms)
{
  switch (this->OptimizationMethod)
  {
  case MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D:
    this->ProbeCalibrationAlgo->ComputeError3d(transformationMatrix, errorMean, errorStDev, errorRms);
    break;
  case MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D:
    this->ProbeCalibrationAlgo->ComputeError2d(transformationMatrix, errorMean, errorStDev, errorRms);
    break;
  default:
    LOG_ERROR("Invalid cost function");
  }
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

  double errorMean=0.0;
  double errorStDev=0.0;
  double errorRms=0.0;
  ComputeError(imageToProbeTransformationMatrix, errorMean, errorStDev, errorRms);
  LOG_INFO("Error (mm): mean=" << errorMean<< ", stdev="<<errorStDev<<", rms="<<errorRms);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSpatialCalibrationOptimizer::Update()
{  
  DistanceToWiresCostFunction::Pointer costFunction = new DistanceToWiresCostFunction(this);

  DistanceToWiresCostFunction::ParametersType imageToProbeSeedTransformParameters(costFunction->GetNumberOfParameters());
  DistanceToWiresCostFunction::GetTransformParameters(imageToProbeSeedTransformParameters, this->ImageToProbeSeedTransformMatrixVnl);

  double errorMean=0.0;
  double errorStDev=0.0;
  double errorRms=0.0;
  ComputeError(this->ImageToProbeSeedTransformMatrixVnl, errorMean, errorStDev, errorRms);
  LOG_INFO("Initial cost function value with unconstrained matrix = " << errorRms );
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

  optimizer->SetMaximumNumberOfIterations( 30000 ); // default: 500, but often convergence needs 10000+ iterations
  optimizer->SetParametersConvergenceTolerance( 1e-8 ); // default: 1e-8
  optimizer->SetFunctionConvergenceTolerance( 1e-4 ); // default: 1e-4
  
  const double rotationParametersScale=1.0;
  const double translationParametersScale=0.005;
  const double scalesParametersScale=10.0;

  // Scale the translation components of the transform in the Optimizer
  OptimizerType::ScalesType scales( costFunction->GetNumberOfParameters() );
  switch (costFunction->GetNumberOfParameters())
  {
  case 7:
    scales[0] = rotationParametersScale;
    scales[1] = rotationParametersScale;
    scales[2] = rotationParametersScale;
    scales[3] = translationParametersScale; 
    scales[4] = translationParametersScale; 
    scales[5] = translationParametersScale;
    scales[6] = scalesParametersScale;  
    break;
  case 8:
    scales[0] = rotationParametersScale;
    scales[1] = rotationParametersScale;
    scales[2] = rotationParametersScale;
    scales[3] = translationParametersScale; 
    scales[4] = translationParametersScale; 
    scales[5] = translationParametersScale;
    scales[6] = scalesParametersScale;  
    scales[7] = scalesParametersScale;  
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
    LOG_DEBUG("FunctionEvaluation " << i << ":  rmsError = " << this->MinimizationResiduals[i]);
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

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
vnl_matrix<double> vtkSpatialCalibrationOptimizer::GetOptimizedImageToProbeTransformMatrix()
{
  return this->ImageToProbeTransformMatrixVnl;
}

//----------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::SetImageToProbeSeedTransform(const vnl_matrix<double> &imageToProbeTransformMatrixVnl)
{
  this->ImageToProbeSeedTransformMatrixVnl=imageToProbeTransformMatrixVnl;
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

//----------------------------------------------------------------------------
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

  return PLUS_SUCCESS;
}

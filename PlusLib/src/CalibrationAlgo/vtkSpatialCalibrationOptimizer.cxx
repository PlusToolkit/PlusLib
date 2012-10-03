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

#define EPS     2.2204e-16     //similar to matlab
#define BIGEPS  10e+20 * EPS   //as is defines by Bouguet in matlab implementation of Rodrigues

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkSpatialCalibrationOptimizer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSpatialCalibrationOptimizer);

//-----------------------------------------------------------------------------

vtkSpatialCalibrationOptimizer::vtkSpatialCalibrationOptimizer()
{
  this->Minimizer = NULL;

  vtkSmartPointer<vtkAmoebaMinimizer> minimizer = vtkSmartPointer<vtkAmoebaMinimizer>::New();
  this->SetMinimizer(minimizer);
}

//-----------------------------------------------------------------------------

vtkSpatialCalibrationOptimizer::~vtkSpatialCalibrationOptimizer()
{
  this->SetMinimizer(NULL);
}

//-----------------------------------------------------------------------------

vnl_matrix<double> vtkSpatialCalibrationOptimizer::VectorToMatrix(const vnl_double_3 &vnlVector)
{
  vnl_matrix<double> vnlMatrix(3,1);
  vnlMatrix(0,0) = vnlVector(0);
  vnlMatrix(1,0) = vnlVector(1);
  vnlMatrix(2,0) = vnlVector(2);

  return vnlMatrix;
}

//-----------------------------------------------------------------------------

vnl_double_3x3 vtkSpatialCalibrationOptimizer::RotationVectorToRotationMatrix (const vnl_double_3 &rotationVersor)
{
  /* 
  * Adapted from MatLab code by Yves Bouget
  * https://code.google.com/p/calib/source/browse/trunk/lib/rodrigues.m
  *
  *       Copyright (c) March 1993 -- Pietro Perona
  *       California Institute of Technology
  *
  */

  double theta = rotationVersor.two_norm();
  vnl_double_3x3 rotationMatrix;
  if (theta < EPS)
  {
    rotationMatrix.set_identity();
  }
  else
  {
    vnl_double_3 omega = rotationVersor/theta;
    double alpha = cos(theta);
    double beta = sin(theta);
    double gamma = 1-cos(theta);
    vnl_double_3x3 omegav;
    omegav(0,0) = 0.0;           
    omegav(0,1) = -omega(2);
    omegav(0,2) = omega(1);
    omegav(1,0) = omega(2);
    omegav(1,1) = 0.0;
    omegav(1,2) = -omega(0);
    omegav(2,0) = -omega(1);
    omegav(2,1) = omega(0);
    omegav(2,2) = 0.0;
    vnl_double_3x3 A = VectorToMatrix(omega) * VectorToMatrix(omega).transpose();

    vnl_double_3x3 eye; eye.set_identity();
    rotationMatrix = eye*alpha + omegav*beta + A*gamma;
  }

  return rotationMatrix;
}

//-----------------------------------------------------------------------------

vnl_double_3 vtkSpatialCalibrationOptimizer::RotationMatrixToRotationVector (const vnl_double_3x3 &rotationMatrix)
{
  /* 
  * Adapted from MatLab code by Yves Bouget
  * https://code.google.com/p/calib/source/browse/trunk/lib/rodrigues.m
  *
  *       Copyright (c) March 1993 -- Pietro Perona
  *       California Institute of Technology
  *
  */

  vnl_double_3 rotationVersor;
  vnl_double_3x3 R;

  vnl_double_3x3 eye; eye.set_identity();
  vnl_double_3x3 in_x_in; in_x_in = (rotationMatrix.transpose() * rotationMatrix - eye);
  vnl_svd<double> svdInXIn (in_x_in);
  double normInXIn = svdInXIn.sigma_max();
  double det_in = vnl_determinant(rotationMatrix);

  if (normInXIn < BIGEPS &&  fabs(det_in-1)<BIGEPS)
  {
    //project the rotation matrix to SO(3);
    vnl_svd<double> svd (rotationMatrix);
    R = svd.U() * svd.V().transpose();

    double tr = (vnl_trace(R)-1)/2;
    double theta = acos(tr); 

    if (sin(theta) >= 1e-5)
    {
      double  vth = 1/(2*sin(theta));
      vnl_double_3 om1;  om1(0) = R(2,1)-R(1,2); om1(1)= R(0,2)-R(2,0); om1(2)= R(1,0)-R(0,1);
      vnl_double_3 om = vth*om1;
      rotationVersor = om*theta;

    }
    else
    {
      if (tr>0)  //% case norm(om)=0;
      {
        rotationVersor.fill(0);
      }
      else //% case norm(om)=pi;
      {
        //out = theta * (sqrt((diag(R)+1)/2).*[1;2*(R(1,2:3)>=0)'-1]);
        vnl_double_3 diag, leftPart, rightPart;
        diag(0)=R(0,0); diag(1)=R(1,1); diag(2)=R(2,2);
        leftPart(0) = sqrt((diag(0)+1)/2);
        leftPart(1) = sqrt((diag(1)+1)/2);
        leftPart(2) = sqrt((diag(2)+1)/2);
        rightPart(0)=1;
        rightPart(1) = 2*(R(0,1)>=0) - 1;
        rightPart(2) = 2*(R(0,2)>=0) - 1;

        rotationVersor = element_product(leftPart,rightPart)*theta;
      }
    }
  }
  else
  {
    LOG_DEBUG( "Rodrigues: Matrix to versor : (normInXIn < BIGEPS &&  fabs(det_in-1)<BIGEPS)==false" );
  }

  return rotationVersor;
}

//-----------------------------------------------------------------------------

vnl_vector<double> vtkSpatialCalibrationOptimizer::TransformMatrixToParametersVector(const vnl_matrix<double> &transformMatrix)
{
  // transform_vector = [r1 r2 r3 t1 t2 t3 sx sy]
  vnl_vector<double> parametersVector(8);
  vnl_vector<double> rotationVector(3);
  //vnl_matrix<double> rotationMatrix(3,3);
  vnl_vector<double> translationVector(3);
  vnl_matrix<double> RotationMatrix(3,3);
  double scaleX, scaleY, scaleZ;

  RotationMatrix = transformMatrix.extract(3,3,0,0);
  scaleX = RotationMatrix.get_column(0).two_norm();
  scaleY = RotationMatrix.get_column(1).two_norm();
  scaleZ = RotationMatrix.get_column(2).two_norm();

  RotationMatrix.normalize_columns();
  rotationVector = RotationMatrixToRotationVector(RotationMatrix);

  translationVector = transformMatrix.extract(3,1,0,3).get_column(0);

  // fill the transform_vector
  parametersVector.update(rotationVector,0);
  parametersVector.update(translationVector,3);
  parametersVector(6)=scaleX;
  parametersVector(7)=scaleY;

  return parametersVector;
}

//-----------------------------------------------------------------------------

vnl_matrix<double> vtkSpatialCalibrationOptimizer::TransformParametersToTransformMatrix(const vnl_vector<double> &transformParameters)
{
  // transform_vector = [r1 r2 r3 t1 t2 t3 sx sy]
  vnl_matrix<double> transformMatrix(4,4);
  vnl_vector<double> rotationVector(3);
  vnl_matrix<double> rotationMatrix(3,3);

  rotationVector(0) = transformParameters[0];
  rotationVector(1) = transformParameters[1];
  rotationVector(2) = transformParameters[2];
  rotationMatrix = RotationVectorToRotationMatrix(rotationVector);

  // multiply the matrix with the scales
  rotationMatrix.set_column(0, rotationMatrix.get_column(0) * transformParameters[6]);
  rotationMatrix.set_column(1, rotationMatrix.get_column(1) * transformParameters[7]);

  // copy the rotation matrix to imageToProbeTransformMatrix in (0,0)
  transformMatrix.update(rotationMatrix, 0, 0);
  // copy the translation to imageToProbeTransformMatrix
  transformMatrix(0,3) = transformParameters[3];
  transformMatrix(1,3) = transformParameters[4];
  transformMatrix(2,3) = transformParameters[5];
  // set last row
  transformMatrix(3,0) = 0;
  transformMatrix(3,1) = 0;
  transformMatrix(3,2) = 0;
  transformMatrix(3,3) = 1;

  return transformMatrix;
}


//----------------------------------------------------------------------------

void vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction");

  vtkSpatialCalibrationOptimizer *self = (vtkSpatialCalibrationOptimizer*)userData;

  double r1, r2, r3, t1, t2, t3, sx,sy;
  r1 = self->Minimizer->GetParameterValue("r1");
  r2 = self->Minimizer->GetParameterValue("r2");
  r3 = self->Minimizer->GetParameterValue("r3");
  t1 = self->Minimizer->GetParameterValue("t1");
  t2 = self->Minimizer->GetParameterValue("t2");
  t3 = self->Minimizer->GetParameterValue("t3");
  sx = self->Minimizer->GetParameterValue("sx");
  sy = self->Minimizer->GetParameterValue("sy");

  vnl_vector<double> rotationVector(3);
  vnl_matrix<double> rotationMatrix(3,3);

  rotationVector(0) = r1;
  rotationVector(1) = r2;
  rotationVector(2) = r3;
  rotationMatrix = RotationVectorToRotationMatrix(rotationVector);

  int m = self->DataPositionsInImageFrame.size();
  double px, py, pz, dx, dy, dz, d;
  double residual=0;

  for(int i=0;i<m;i++)
  {
    px = sx*rotationMatrix[0][0]*self->DataPositionsInImageFrame[i][0] + sy*rotationMatrix[0][1]*self->DataPositionsInImageFrame[i][1] + rotationMatrix[0][2]*self->DataPositionsInImageFrame[i][2] + t1;
    py = sx*rotationMatrix[1][0]*self->DataPositionsInImageFrame[i][0] + sy*rotationMatrix[1][1]*self->DataPositionsInImageFrame[i][1] + rotationMatrix[1][2]*self->DataPositionsInImageFrame[i][2] + t2;
    pz = sx*rotationMatrix[2][0]*self->DataPositionsInImageFrame[i][0] + sy*rotationMatrix[2][1]*self->DataPositionsInImageFrame[i][1] + rotationMatrix[2][2]*self->DataPositionsInImageFrame[i][2] + t3;
    dx = px - self->DataPositionsInProbeFrame[i][0];
    dy = py - self->DataPositionsInProbeFrame[i][1];
    dz = pz - self->DataPositionsInProbeFrame[i][2];
    d = dx*dx + dy*dy + dz*dz;
    residual += d*d;
  }
  double rmsError = sqrt(residual/m);
  self->Minimizer->SetFunctionValue(residual);
}

//-----------------------------------------------------------------------------

double vtkSpatialCalibrationOptimizer::PointToLineDistance( const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2 )
{
  //norm(cross( (LP2-LP1), (P-LP1) ) ) / norm(LP2-LP1);
  double d = vnl_cross_3d(aLineEndPoint2-aLineEndPoint1,aPoint-aLineEndPoint1).two_norm() / (aLineEndPoint2-aLineEndPoint1).two_norm();
  return d;
}

//-----------------------------------------------------------------------------

void vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *userData)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction2");

  vtkSpatialCalibrationOptimizer *self = (vtkSpatialCalibrationOptimizer*)userData;

  double r1, r2, r3, t1, t2, t3, sx,sy;
  r1 = self->Minimizer->GetParameterValue("r1");
  r2 = self->Minimizer->GetParameterValue("r2");
  r3 = self->Minimizer->GetParameterValue("r3");
  t1 = self->Minimizer->GetParameterValue("t1");
  t2 = self->Minimizer->GetParameterValue("t2");
  t3 = self->Minimizer->GetParameterValue("t3");
  sx = self->Minimizer->GetParameterValue("sx");
  sy = self->Minimizer->GetParameterValue("sy");

  vnl_vector<double> rotationVector(3);
  vnl_matrix<double> rotationMatrix(3,3);

  rotationVector(0) = r1;
  rotationVector(1) = r2;
  rotationVector(2) = r3;
  rotationMatrix = RotationVectorToRotationMatrix(rotationVector);

  int nWires = self->NWires.size(); 
  int m = self->SegmentedPointsInImageFrame.size()/(3*nWires); //number of frames

  double px, py, pz;
  double residual=0;

  vnl_vector<double> segmentedInProbeFrame_vnl(4), segmentedInPhantomFrame_vnl(4);
  vnl_vector<double> wireFrontPoint_vnl(3), wireBackPoint_vnl(3);

  for(int i=0;i<m;i++) // for each frame
  {
    vnl_matrix<double> probeToPhantomTransform_vnl = self->ProbeToPhantomTransforms[i];

    for (int j=0;j<3*nWires;j++)  // for each segmented point
    {
      // Find the projection in the probe frame 
      px = sx*rotationMatrix[0][0]*self->SegmentedPointsInImageFrame[3*nWires*i+j][0] + sy*rotationMatrix[0][1]*self->SegmentedPointsInImageFrame[3*nWires*i+j][1] + rotationMatrix[0][2]*self->SegmentedPointsInImageFrame[3*nWires*i+j][2] + t1;
      py = sx*rotationMatrix[1][0]*self->SegmentedPointsInImageFrame[3*nWires*i+j][0] + sy*rotationMatrix[1][1]*self->SegmentedPointsInImageFrame[3*nWires*i+j][1] + rotationMatrix[1][2]*self->SegmentedPointsInImageFrame[3*nWires*i+j][2] + t2;
      pz = sx*rotationMatrix[2][0]*self->SegmentedPointsInImageFrame[3*nWires*i+j][0] + sy*rotationMatrix[2][1]*self->SegmentedPointsInImageFrame[3*nWires*i+j][1] + rotationMatrix[2][2]*self->SegmentedPointsInImageFrame[3*nWires*i+j][2] + t3;

      segmentedInProbeFrame_vnl[0] = px; segmentedInProbeFrame_vnl[1] = py; segmentedInProbeFrame_vnl[2] = pz; segmentedInProbeFrame_vnl[3] = 1;

      // Transform points from image to phantom frame
      segmentedInPhantomFrame_vnl = probeToPhantomTransform_vnl * segmentedInProbeFrame_vnl;

      // compute distance to wire
      int line = j%3;
      int w = j/3; 

      wireFrontPoint_vnl[0]= self->NWires[w].Wires[line].EndPointFront[0];
      wireFrontPoint_vnl[1]= self->NWires[w].Wires[line].EndPointFront[1];
      wireFrontPoint_vnl[2]= self->NWires[w].Wires[line].EndPointFront[2];
      wireBackPoint_vnl[0]= self->NWires[w].Wires[line].EndPointBack[0];
      wireBackPoint_vnl[1]= self->NWires[w].Wires[line].EndPointBack[1];
      wireBackPoint_vnl[2]= self->NWires[w].Wires[line].EndPointBack[2];

      double d = PointToLineDistance(segmentedInPhantomFrame_vnl.extract(3,0), wireFrontPoint_vnl, wireBackPoint_vnl );
      residual += d*d;
    }
  }

  double rmsError= sqrt(residual/(3*m*nWires));
  self->Minimizer->SetFunctionValue(rmsError);
}

//-----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::Optimize(OptimizationMethod aMethod)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::Optimize");

  vnl_vector<double> parametersVector(8); //[r1 r2 r3 t1 t2 t3 sx sy]
  parametersVector = TransformMatrixToParametersVector(this->ImageToProbeSeedTransformMatrixVnl);

  switch (aMethod)
  {
  case OptimizationMethod_Method1:
    // Set up minimizer and run the optimization
    this->Minimizer->SetFunction(vtkImageToProbeCalibrationMatrixEvaluationFunction,this);
    break;
  case OptimizationMethod_Method2:
    this->Minimizer->SetFunction(vtkImageToProbeCalibrationMatrixEvaluationFunction2,this);
    break;
  }

  double scale = 0.01;
  this->Minimizer->SetFunctionArgDelete(NULL);
  this->Minimizer->SetParameterValue("r1",parametersVector[0]);
  this->Minimizer->SetParameterScale("r1",scale);
  this->Minimizer->SetParameterValue("r2",parametersVector[1]);
  this->Minimizer->SetParameterScale("r2",scale);
  this->Minimizer->SetParameterValue("r3",parametersVector[2]);
  this->Minimizer->SetParameterScale("r3",scale);

  scale = 0.1;
  this->Minimizer->SetParameterValue("t1",parametersVector[3]);
  this->Minimizer->SetParameterScale("t1",scale);
  this->Minimizer->SetParameterValue("t2",parametersVector[4]);
  this->Minimizer->SetParameterScale("t2",scale);
  this->Minimizer->SetParameterValue("t3",parametersVector[5]);
  this->Minimizer->SetParameterScale("t3",scale);

  scale = 0.005;
  this->Minimizer->SetParameterValue("sx",parametersVector[6]);
  this->Minimizer->SetParameterScale("sx",scale);
  this->Minimizer->SetParameterValue("sy",parametersVector[7]);
  this->Minimizer->SetParameterScale("sy",scale);

  LOG_DEBUG("Vector parameters " << parametersVector << " seed");

  this->Minimizer->Minimize();

  double rmsError = this->Minimizer->GetFunctionValue();
  int iteration = this->Minimizer->GetIterations();
  int evaluations = this->Minimizer->GetFunctionEvaluations();

  double r1, r2, r3, t1, t2, t3, sx,sy;
  r1 = this->Minimizer->GetParameterValue("r1");
  r2 = this->Minimizer->GetParameterValue("r2");
  r3 = this->Minimizer->GetParameterValue("r3");
  t1 = this->Minimizer->GetParameterValue("t1");
  t2 = this->Minimizer->GetParameterValue("t2");
  t3 = this->Minimizer->GetParameterValue("t3");
  sx = this->Minimizer->GetParameterValue("sx");
  sy = this->Minimizer->GetParameterValue("sy");

  vnl_vector<double> rotationVector(3);
  vnl_matrix<double> rotationMatrix(3,3);
  rotationVector(0) = r1;
  rotationVector(1) = r2;
  rotationVector(2) = r3;
  rotationMatrix = RotationVectorToRotationMatrix(rotationVector);

  parametersVector[0] = r1; parametersVector[1] = r2; parametersVector[2] = r3;
  parametersVector[3] = t1; parametersVector[4] = t2; parametersVector[5] = t3;
  parametersVector[6] = sx; parametersVector[7] = sy;

  this->ImageToProbeTransformMatrixVnl = TransformParametersToTransformMatrix(parametersVector);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::SetOptimizerData(std::vector< vnl_vector<double> > *DataPositionsInImageFrame, std::vector< vnl_vector<double> > *DataPositionsInProbeFrame, vnl_matrix<double> *ImageToProbeTransformMatrixVnl)
{
  this->DataPositionsInImageFrame = *DataPositionsInImageFrame;
  this->DataPositionsInProbeFrame = *DataPositionsInProbeFrame;
  this->ImageToProbeSeedTransformMatrixVnl= *ImageToProbeTransformMatrixVnl;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::SetOptimizerData2(std::vector< vnl_vector<double> > *SegmentedPointsInImageFrame, std::vector<NWire> *NWires, std::vector< vnl_matrix<double> > *ProbeToPhantomTransforms, vnl_matrix<double> *ImageToProbeTransformMatrixVnl)
{
  this->SegmentedPointsInImageFrame = *SegmentedPointsInImageFrame;
  this->NWires = * NWires;
  this->ProbeToPhantomTransforms = *ProbeToPhantomTransforms;
  this->ImageToProbeSeedTransformMatrixVnl= *ImageToProbeTransformMatrixVnl;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkSpatialCalibrationOptimizer::vtkOptimizationMetricFunction(void *userData)
{
  LOG_TRACE("(vtkSpatialCalibrationOptimizer)vtkTrackerToolCalibrationFunction");

  vtkSpatialCalibrationOptimizer *self = (vtkSpatialCalibrationOptimizer*)userData;
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::DoCalibrationOptimization(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::DoCalibrationOptimization");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

vnl_matrix<double> vtkSpatialCalibrationOptimizer::GetOptimizedImageToProbeTransformMatrix()
{
  return this->ImageToProbeTransformMatrixVnl;
}

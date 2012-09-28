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

vnl_matrix<double> VectorToMatrix(const vnl_double_3 &A)
{
  vnl_matrix<double> M(3,1);
  M(0,0) = A(0);
  M(1,0) = A(1);
  M(2,0) = A(2);

  return M;
}

//-----------------------------------------------------------------------------

vnl_double_3x3 rodrigues (vnl_double_3 rotationVersor)
{
  //adapted from matlab Yves Bouget
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

vnl_double_3 rodrigues (vnl_double_3x3 rotationMatrix)
{
  vnl_double_3 rotationVersor;
  vnl_double_3x3 R;

  vnl_double_3x3 eye; eye.set_identity();
  vnl_double_3x3 in_x_in; in_x_in = (rotationMatrix.transpose() * rotationMatrix - eye);
  vnl_svd<double> svd_in_x_in (in_x_in);
  double norm_in_x_in = svd_in_x_in.sigma_max();
  double det_in = vnl_determinant(rotationMatrix);

  if (norm_in_x_in < BIGEPS &&  fabs(det_in-1)<BIGEPS)
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
        vnl_double_3 diag, left_part, right_part;
        diag(0)=R(0,0); diag(1)=R(1,1); diag(2)=R(2,2);
        left_part(0) = sqrt((diag(0)+1)/2);
        left_part(1) = sqrt((diag(1)+1)/2);
        left_part(2) = sqrt((diag(2)+1)/2);
        right_part(0)=1;
        right_part(1) = 2*(R(0,1)>=0) - 1;
        right_part(2) = 2*(R(0,2)>=0) - 1;

        rotationVersor = element_product(left_part,right_part)*theta;
      }
    }
  }
  else
  {
    LOG_DEBUG( "Rodrigues: Matrix to versor : (norm_in_x_in < BIGEPS &&  fabs(det_in-1)<BIGEPS)==false" );
  }

  return rotationVersor;
}

//-----------------------------------------------------------------------------

vnl_vector<double> transform_matrix_to_parameters_vector(vnl_matrix<double> transform_matrix)
{
  // transform_vector = [r1 r2 r3 t1 t2 t3 sx sy]
  vnl_vector<double> parameters_vector(8);
  vnl_vector<double> rotationVersor(3);
  //vnl_matrix<double> rotationMatrix(3,3);
  vnl_vector<double> translationVector(3);
  vnl_matrix<double> R(3,3);
  double scaleX, scaleY, scaleZ;

  R = transform_matrix.extract(3,3,0,0);
  scaleX = R.get_column(0).two_norm();
  scaleY = R.get_column(1).two_norm();
  scaleZ = R.get_column(2).two_norm();

  R.normalize_columns();
  rotationVersor = rodrigues(R);

  translationVector = transform_matrix.extract(3,1,0,3).get_column(0);

  // fill the transform_vector
  parameters_vector.update(rotationVersor,0);
  parameters_vector.update(translationVector,3);
  parameters_vector(6)=scaleX;
  parameters_vector(7)=scaleY;

  return parameters_vector;
}

//-----------------------------------------------------------------------------

vnl_matrix<double> transform_parameters_to_transform_matrix(vnl_vector<double> transform_parameters)
{
  // transform_vector = [r1 r2 r3 t1 t2 t3 sx sy]
  vnl_matrix<double> transform_matrix(4,4);
  vnl_vector<double> rotationVersor(3);
  vnl_matrix<double> rotationMatrix(3,3);

  rotationVersor(0)=transform_parameters[0]; rotationVersor(1)=transform_parameters[1]; rotationVersor(2)=transform_parameters[2];
  rotationMatrix = rodrigues(rotationVersor);

  // multiply the matrix with the scales
  rotationMatrix.set_column(0, rotationMatrix.get_column(0) * transform_parameters[6]);
  rotationMatrix.set_column(1, rotationMatrix.get_column(1) * transform_parameters[7]);

  // copy the rotation matrix to imageToProbeTransformMatrix in (0,0)
  transform_matrix.update(rotationMatrix, 0, 0);
  // copy the translation to imageToProbeTransformMatrix
  transform_matrix(0,3)=transform_parameters[3]; transform_matrix(1,3)=transform_parameters[4]; transform_matrix(2,3)=transform_parameters[5];
  // set last row
  transform_matrix(3,0)=0; transform_matrix(3,1)=0; transform_matrix(3,2)=0; transform_matrix(3,3)=1;

  return transform_matrix;
}


//----------------------------------------------------------------------------

void vtkImageToProbeCalibrationMatrixEvaluationFunction(void *userData)
{
  LOG_TRACE("(vtkImageToProbeCalibrationMatrixEvaluationFunction");

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
  
  vnl_vector<double> rotationVersor(3);
  vnl_matrix<double> R(3,3);

  rotationVersor(0)=r1; rotationVersor(1)=r2; rotationVersor(2)=r3;
  R = rodrigues(rotationVersor);
 
  int m = self->DataPositionsInImageFrame.size();
  double px, py, pz, dx, dy, dz;
  double residual=0;

  for(int i=0;i<m;i++)
  {
	  px = sx*R[0][0]*self->DataPositionsInImageFrame[i][0] + sy*R[0][1]*self->DataPositionsInImageFrame[i][1] + R[0][2]*self->DataPositionsInImageFrame[i][2]+ t1;
	  py = sx*R[1][0]*self->DataPositionsInImageFrame[i][0] + sy*R[1][1]*self->DataPositionsInImageFrame[i][1] + R[1][2]*self->DataPositionsInImageFrame[i][2]+ t2;
	  pz = sx*R[2][0]*self->DataPositionsInImageFrame[i][0] + sy*R[2][1]  *self->DataPositionsInImageFrame[i][1] + R[2][2]*self->DataPositionsInImageFrame[i][2]+ t3;
	  dx = px - self->DataPositionsInProbeFrame[i][0];
	  dy = py - self->DataPositionsInProbeFrame[i][1];
	  dz = pz - self->DataPositionsInProbeFrame[i][2];
	  residual += dx*dx + dy*dy + dz*dz;
  }
  
  self->Minimizer->SetFunctionValue(residual);
}

//-----------------------------------------------------------------------------

double point_to_line_distance(vnl_double_3 P, vnl_double_3 LP1, vnl_double_3 LP2 )
{
  //norm(cross( (LP2-LP1), (P-LP1) ) ) / norm(LP2-LP1);
  double d = vnl_cross_3d(LP2-LP1,P-LP1).two_norm() / (LP2-LP1).two_norm();
  return d;
}

//-----------------------------------------------------------------------------

void vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *userData)
{
  LOG_TRACE("(vtkImageToProbeCalibrationMatrixEvaluationFunction2");

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
  
  vnl_vector<double> rotationVersor(3);
  vnl_matrix<double> R(3,3);

  rotationVersor(0)=r1; rotationVersor(1)=r2; rotationVersor(2)=r3;
  R= rodrigues(rotationVersor);
 
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
      px = sx*R[0][0]*self->SegmentedPointsInImageFrame[3*nWires*i+j][0] + sy*R[0][1]*self->SegmentedPointsInImageFrame[3*nWires*i+j][1] + R[0][2]*self->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ t1;
      py = sx*R[1][0]*self->SegmentedPointsInImageFrame[3*nWires*i+j][0] + sy*R[1][1]*self->SegmentedPointsInImageFrame[3*nWires*i+j][1] + R[1][2]*self->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ t2;
      pz = sx*R[2][0]*self->SegmentedPointsInImageFrame[3*nWires*i+j][0] + sy*R[2][1]*self->SegmentedPointsInImageFrame[3*nWires*i+j][1] + R[2][2]*self->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ t3;
 
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

      residual += point_to_line_distance(segmentedInPhantomFrame_vnl.extract(3,0), wireFrontPoint_vnl, wireBackPoint_vnl );
    }
  }
  
  self->Minimizer->SetFunctionValue(residual);
}

//-----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::Optimize(int method)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::Optimize");

  vnl_vector<double> parameters_vector(8); //[r1 r2 r3 t1 t2 t3 sx sy]
  parameters_vector = transform_matrix_to_parameters_vector(this->ImageToProbeSeedTransformMatrixVnl);

  switch (method)
  {
    case 1:
      // Set up minimizer and run the optimalization
      this->Minimizer->SetFunction(vtkImageToProbeCalibrationMatrixEvaluationFunction,this);
      break;
    case 2:
      this->Minimizer->SetFunction(vtkImageToProbeCalibrationMatrixEvaluationFunction2,this);
      break;
  }

  double scale = 0.01;
  this->Minimizer->SetFunctionArgDelete(NULL);
  this->Minimizer->SetParameterValue("r1",parameters_vector[0]);
  this->Minimizer->SetParameterScale("r1",scale);
  this->Minimizer->SetParameterValue("r2",parameters_vector[1]);
  this->Minimizer->SetParameterScale("r2",scale);
  this->Minimizer->SetParameterValue("r3",parameters_vector[2]);
  this->Minimizer->SetParameterScale("r3",scale);
  
  scale = 0.5;
  this->Minimizer->SetParameterValue("t1",parameters_vector[3]);
  this->Minimizer->SetParameterScale("t1",scale);
  this->Minimizer->SetParameterValue("t2",parameters_vector[4]);
  this->Minimizer->SetParameterScale("t2",scale);
  this->Minimizer->SetParameterValue("t3",parameters_vector[5]);
  this->Minimizer->SetParameterScale("t3",scale);

  scale = 0.05;
  this->Minimizer->SetParameterValue("sx",parameters_vector[6]);
  this->Minimizer->SetParameterScale("sx",scale);
  this->Minimizer->SetParameterValue("sy",parameters_vector[7]);
  this->Minimizer->SetParameterScale("sy",scale);

  LOG_DEBUG("Vector parameters " << parameters_vector << " seed");

  this->Minimizer->Minimize();

  //this->CalibrationError = this->Minimizer->GetFunctionValue();

  double r1, r2, r3, t1, t2, t3, sx,sy;
  r1 = this->Minimizer->GetParameterValue("r1");
  r2 = this->Minimizer->GetParameterValue("r2");
  r3 = this->Minimizer->GetParameterValue("r3");
  t1 = this->Minimizer->GetParameterValue("t1");
  t2 = this->Minimizer->GetParameterValue("t2");
  t3 = this->Minimizer->GetParameterValue("t3");
  sx = this->Minimizer->GetParameterValue("sx");
  sy = this->Minimizer->GetParameterValue("sy");

  vnl_vector<double> rotationVersor(3);
  vnl_matrix<double> R(3,3);
  rotationVersor(0)=r1; rotationVersor(1)=r2; rotationVersor(2)=r3;
  R= rodrigues(rotationVersor);
  
  parameters_vector[0]=r1; parameters_vector[1]=r2; parameters_vector[2]=r3;
  parameters_vector[3]=t1; parameters_vector[4]=t2; parameters_vector[5]=t3;
  parameters_vector[6]=sx; parameters_vector[7]=sy;

  this->ImageToProbeTransformMatrixVnl = transform_parameters_to_transform_matrix(parameters_vector);
  
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

void vtkOptimizationMetricFunction(void *userData)
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

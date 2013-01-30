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

#include "itkLevenbergMarquardtOptimizer.h"
#include "vtkLine.h"
#include "vtkPlane.h"

#include <float.h> // for DBL_MAX

static const double EPS=2.2204e-16;     //similar to matlab
static const double BIGEPS=10e+20 * EPS;   //as is defines by Bouguet in matlab implementation of Rodrigues

double vtkSpatialCalibrationOptimizer::NumberOfParameters = 0;

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

vnl_double_3x3 vtkSpatialCalibrationOptimizer::RotationVersorToRotationMatrix (const vnl_double_3 &rotationVersor)
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

vnl_double_3 vtkSpatialCalibrationOptimizer::RotationMatrixToRotationVersor (const vnl_double_3x3 &aRotationMatrix)
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
  vnl_double_3x3 rotationMatrix;

  vnl_double_3x3 eye; eye.set_identity();
  vnl_double_3x3 in_x_in; in_x_in = (aRotationMatrix.transpose() * aRotationMatrix - eye);
  vnl_svd<double> svdInXIn (in_x_in);
  double normInXIn = svdInXIn.sigma_max();
  double det_in = vnl_determinant(aRotationMatrix);

  if (normInXIn < BIGEPS &&  fabs(det_in-1)<BIGEPS)
  {
    //project the rotation matrix to SO(3);
    vnl_svd<double> svd (aRotationMatrix);
    rotationMatrix = svd.U() * svd.V().transpose();

    double tr = (vnl_trace(rotationMatrix)-1)/2;
    double theta = acos(tr); 

    if (sin(theta) >= 1e-5)
    {
      double  vth = 1/(2*sin(theta));
      vnl_double_3 om1;  om1(0) = rotationMatrix(2,1)-rotationMatrix(1,2); om1(1)= rotationMatrix(0,2)-rotationMatrix(2,0); om1(2)= rotationMatrix(1,0)-rotationMatrix(0,1);
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
        diag(0)=rotationMatrix(0,0); diag(1)=rotationMatrix(1,1); diag(2)=rotationMatrix(2,2);
        leftPart(0) = sqrt((diag(0)+1)/2);
        leftPart(1) = sqrt((diag(1)+1)/2);
        leftPart(2) = sqrt((diag(2)+1)/2);
        rightPart(0)=1;
        rightPart(1) = 2*(rotationMatrix(0,1)>=0) - 1;
        rightPart(2) = 2*(rotationMatrix(0,2)>=0) - 1;

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

  vnl_vector<double> parametersVector(NumberOfParameters);

  /** if (quaternionsRepresentation)
  {
  */

  // Method used for conversion  is similar to published in: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
  vnl_matrix<double> rotationMatrix= transformMatrix.extract(3,3);
  double scaleX = rotationMatrix.get_column(0).two_norm();
  double scaleY = rotationMatrix.get_column(1).two_norm();

  if ( NumberOfParameters==8)
  {
    parametersVector(7)= scaleY/scaleX;
    // correction of the second column in order to get an isotropic scaling
    rotationMatrix.set_column(1,rotationMatrix.get_column(1)/parametersVector(7)); 
  }

  //double determinant = vnl_determinant(rotationMatrix); 
  double scale = scaleX;    //pow(determinant, 1.0/3 );
  double q1,q2,q3,q4;

  double trace = rotationMatrix(0,0) + rotationMatrix(1,1) + rotationMatrix(2,2); // I removed + 1.0f; see discussion with Ethan
  if( trace > 0 ) {// I changed M_EPSILON to 0
    double s = 0.5f / sqrtf(trace+ scale);
    q4 = 0.25f / s;
    q1 = ( rotationMatrix(2,1) - rotationMatrix(1,2) ) * s;
    q2 = ( rotationMatrix(0,2) - rotationMatrix(2,0) ) * s;
    q3 = ( rotationMatrix(1,0) - rotationMatrix(0,1) ) * s;
  } else {
    if ( rotationMatrix(0,0) > rotationMatrix(1,1) && rotationMatrix(0,0) > rotationMatrix(2,2) ) {
      double s = 2.0f * sqrtf( scale + rotationMatrix(0,0) - rotationMatrix(1,1) - rotationMatrix(2,2));
      q4 = (rotationMatrix(2,1) - rotationMatrix(1,2) ) / s;
      q1 = 0.25f * s;
      q2 = (rotationMatrix(0,1) + rotationMatrix(1,0) ) / s;
      q3 = (rotationMatrix(0,2) + rotationMatrix(2,0) ) / s;
    } else if (rotationMatrix(1,1) > rotationMatrix(2,2)) {
      double s = 2.0f * sqrtf( scale + rotationMatrix(1,1) - rotationMatrix(0,0) - rotationMatrix(2,2));
      q4 = (rotationMatrix(0,2) - rotationMatrix(2,0) ) / s;
      q1 = (rotationMatrix(0,1) + rotationMatrix(1,0) ) / s;
      q2 = 0.25f * s;
      q3 = (rotationMatrix(1,2) + rotationMatrix(2,1) ) / s;
    } else {
      double s = 2.0f * sqrtf( scale + rotationMatrix(2,2) - rotationMatrix(0,0) - rotationMatrix(1,1) );
      q4 = (rotationMatrix(1,0) - rotationMatrix(0,1) ) / s;
      q1 = (rotationMatrix(0,2) + rotationMatrix(2,0) ) / s;
      q2 = (rotationMatrix(1,2) + rotationMatrix(2,1) ) / s;
      q3 = 0.25f * s;
    }   
  }

  double newScale = q1*q1 + q2*q2 + q3*q3 +q4*q4; // for debug

  vnl_vector<double> translationVector(3);
  translationVector = transformMatrix.extract(3,1,0,3).get_column(0);
  parametersVector(0)=q1;
  parametersVector(1)=q2;
  parametersVector(2)=q3;
  parametersVector(3)=q4;
  parametersVector(4)=translationVector(0);
  parametersVector(5)=translationVector(1);
  parametersVector(6)=translationVector(2);

  /*

  }
  else
  {	
  parametersVector = parametersVector.set_size(8);
  // transform_vector = [r1 r2 r3 t1 t2 t3 sx sy]
  vnl_vector<double> rotationVersor(3);
  //vnl_matrix<double> rotationMatrix(3,3);
  vnl_vector<double> translationVector(3);
  vnl_matrix<double> rotationMatrix(3,3);
  double scaleX, scaleY, scaleZ;

  rotationMatrix = transformMatrix.extract(3,3,0,0);
  scaleX = rotationMatrix.get_column(0).two_norm();
  scaleY = rotationMatrix.get_column(1).two_norm();
  scaleZ = rotationMatrix.get_column(2).two_norm();

  rotationMatrix.normalize_columns();
  rotationVersor = RotationMatrixToRotationVersor(rotationMatrix);

  translationVector = transformMatrix.extract(3,1,0,3).get_column(0);

  // fill the transform_vector
  parametersVector.update(rotationVersor,0);
  parametersVector.update(translationVector,3);
  parametersVector(6)=scaleX;
  parametersVector(7)=scaleY;
  }
  */

  return parametersVector;
}


//-----------------------------------------------------------------------------

vnl_matrix<double> vtkSpatialCalibrationOptimizer::TransformParametersToTransformMatrix(const vnl_vector<double> &transformParameters)
{
  // transform_vector = [r1 r2 r3 t1 t2 t3 sx sy]
  vnl_matrix<double> transformMatrix(4,4);
  vnl_vector<double> rotationVersor(3);
  vnl_matrix<double> rotationMatrix(3,3);
  /*
  int numberOfParameters = transformParameters.size();
  if (numberOfParameters == 8)
  {
  rotationVersor(0) = transformParameters[0];
  rotationVersor(1) = transformParameters[1];
  rotationVersor(2) = transformParameters[2];
  rotationMatrix = RotationVersorToRotationMatrix(rotationVersor);

  // multiply the matrix with the scales
  rotationMatrix.set_column(0, rotationMatrix.get_column(0) * transformParameters[6]);
  rotationMatrix.set_column(1, rotationMatrix.get_column(1) * transformParameters[7]);

  // copy the translation to imageToProbeTransformMatrix
  transformMatrix(0,3) = transformParameters[3];
  transformMatrix(1,3) = transformParameters[4];
  transformMatrix(2,3) = transformParameters[5];
  }
  else
  {

  */
  double q1, q2, q3, q4, skew, scale;
  q1= transformParameters[0];
  q2= transformParameters[1];
  q3= transformParameters[2];
  q4= transformParameters[3];

  if (NumberOfParameters == 8)
  {
    skew = transformParameters(7); // scaleY/scaleX
  }
  else
  {
    skew = 1;
  }

  double q1q1 = q1*q1;
  double q1q2 = q1*q2;
  double q1q3 = q1*q3; 
  double q1q4 = q1*q4;

  double q2q2 = q2*q2;
  double q2q3 = q2*q3;
  double q2q4 = q2*q4;

  double q3q3 = q3*q3;
  double q4q3 = q4*q3;
  double q4q4 = q4*q4;

  scale = q1q1 + q2q2 + q3q3 + q4q4;

  rotationMatrix(0,0) = q1q1 - q2q2 - q3q3 + q4q4;  //1 - 2*(q2q2 + q3q3); 
  rotationMatrix(0,1) = skew*2*(q1q2 - q4q3);
  rotationMatrix(0,2) = 2*(q1q3 + q2q4);
  rotationMatrix(1,0) = 2*(q1q2 + q4q3);
  rotationMatrix(1,1) = skew*(q4q4 - q1q1 + q2q2 - q3q3);   //1 - 2*(q1q1 + q3q3);
  rotationMatrix(1,2) = 2*(q2q3 - q1q4);
  rotationMatrix(2,0) = 2*(q1q3 - q2q4);
  rotationMatrix(2,1) = skew*2*(q2q3 + q1q4);
  rotationMatrix(2,2) = q4q4 - q1q1 -q2q2 + q3q3;    //1 - 2*(q1q1 + q2q2);

  // copy the translation to imageToProbeTransformMatrix
  transformMatrix(0,3) = transformParameters[4];
  transformMatrix(1,3) = transformParameters[5];
  transformMatrix(2,3) = transformParameters[6];

  //}

  // copy the rotation matrix to imageToProbeTransformMatrix in (0,0)
  transformMatrix.update(rotationMatrix, 0, 0);

  // set last row
  transformMatrix(3,0) = 0;
  transformMatrix(3,1) = 0;
  transformMatrix(3,2) = 0;
  transformMatrix(3,3) = 1;

  return transformMatrix;
}


class LMDistanceToAllWiresCostFunction : public itk::MultipleValuedCostFunction
{
public:
  typedef LMDistanceToAllWiresCostFunction                    Self;
  typedef itk::MultipleValuedCostFunction   Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;
  itkNewMacro( Self );

  int RangeDimension;
  int SpaceDimension ;

  typedef Superclass::ParametersType              ParametersType;
  typedef Superclass::DerivativeType              DerivativeType;
  typedef Superclass::MeasureType                 MeasureType;

  LMDistanceToAllWiresCostFunction(){}; 

  LMDistanceToAllWiresCostFunction(int numberOfParameters,int numberOfResiduals ):
  m_Measure(numberOfResiduals),
    m_Derivative(numberOfParameters,numberOfResiduals),
    m_TheoreticalData(numberOfResiduals)  
  {

    m_Measure.SetSize(numberOfResiduals);
    m_Derivative.SetSize(numberOfParameters,numberOfResiduals);
    m_TheoreticalData.SetSize(numberOfResiduals);
    this->RangeDimension = numberOfResiduals;
    this->SpaceDimension = numberOfParameters;

  }



  MeasureType GetValue( const ParametersType & parameters ) const
  {


    vnl_vector<double> rotationVersor(3);
    int numParameters = parameters.Size();
    vnl_vector<double> parametersVector(numParameters);
    vnl_matrix<double> imageToProbeTransform_vnl(4,4);
    vnl_matrix<double> phantomToImageTransform_vnl(4,4); 
    vtkSmartPointer<vtkMatrix4x4> phantomToImageVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

    for(int i=0;i<numParameters;i++)
    { 
      parametersVector(i) = parameters[i];
    }

    imageToProbeTransform_vnl= vtkSpatialCalibrationOptimizer::TransformParametersToTransformMatrix(parametersVector);

    int nWires = this->NWires.size(); 
    int m = this->SegmentedPointsInImageFrame.size()/(3*nWires); //number of frames

    vnl_vector<double> segmentedInProbeFrame_vnl(4), segmentedInPhantomFrame_vnl(4);
    vnl_vector<double> wireFrontPoint_vnl(3), wireBackPoint_vnl(3);
    int currentSegmentedPoint = 0;

    for(int i=0;i<m;i++) // for each frame
    {
      vnl_matrix<double> probeToPhantomTransform_vnl = this->ProbeToPhantomTransforms[i];

      /*
      for (int j=0;j<3*nWires;j++)  // for each segmented point
      {
      // Find the projection in the probe frame 
      px = transformMatrix[0][0]*this->SegmentedPointsInImageFrame[3*nWires*i+j][0] + transformMatrix[0][1]*this->SegmentedPointsInImageFrame[3*nWires*i+j][1] + transformMatrix[0][2]*this->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ transformMatrix[0][3];
      py = transformMatrix[1][0]*this->SegmentedPointsInImageFrame[3*nWires*i+j][0] + transformMatrix[1][1]*this->SegmentedPointsInImageFrame[3*nWires*i+j][1] + transformMatrix[1][2]*this->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ transformMatrix[1][3];
      pz = transformMatrix[2][0]*this->SegmentedPointsInImageFrame[3*nWires*i+j][0] + transformMatrix[2][1]*this->SegmentedPointsInImageFrame[3*nWires*i+j][1] + transformMatrix[2][2]*this->SegmentedPointsInImageFrame[3*nWires*i+j][2]+ transformMatrix[2][3];

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
      m_Measure[currentSegmentedPoint++] = distanceToWire;
      }
      */

      double normalVector[3] = { 0.0, 0.0, 1.0 };
      double origin[3] = { 0.0, 0.0, 0.0 };

      for (int j=0;j<3*nWires;j++)  // for each segmented point
      {
        // Get wire endpoints in image coordinate system
        Wire wire = this->NWires[j/3].Wires[j%3];
        double wireEndPointFrontInPhantomFrame[4] = { wire.EndPointFront[0], wire.EndPointFront[1], wire.EndPointFront[2], 1.0 };
        double wireEndPointBackInPhantomFrame[4] = { wire.EndPointBack[0], wire.EndPointBack[1], wire.EndPointBack[2], 1.0 };
        double wireEndPointFrontInImageFrame[4];
        double wireEndPointBackInImageFrame[4];

        phantomToImageTransform_vnl = vnl_inverse( probeToPhantomTransform_vnl * imageToProbeTransform_vnl);
        PlusMath::ConvertVnlMatrixToVtkMatrix(phantomToImageTransform_vnl, phantomToImageVtkTransformMatrix);

        phantomToImageVtkTransformMatrix->MultiplyPoint(wireEndPointFrontInPhantomFrame, wireEndPointFrontInImageFrame);
        phantomToImageVtkTransformMatrix->MultiplyPoint(wireEndPointBackInPhantomFrame, wireEndPointBackInImageFrame);

        double computedPositionInImagePlane[3];
        double t = 0; // Parametric coordinate along the line

        // Compute intersection of wire and image plane
        if ( ( ! vtkPlane::IntersectWithLine(wireEndPointFrontInImageFrame, wireEndPointBackInImageFrame, normalVector, origin, t, computedPositionInImagePlane) )
          && ( wireEndPointFrontInImageFrame[3] * wireEndPointBackInImageFrame[3] < 0 ) ) // This condition to ensure that warning is thrown only if the zero value is returned because both points are on the same side of the image plane (in that case the intersection is still valid although the return value is zero)
        {
          LOG_WARNING("Image plane and wire are parallel!");

          std::vector<double> reprojectionError2D(2, DBL_MAX);

          continue;
        }

        std::vector<double> reprojectionError2D(2);
        reprojectionError2D[0] = this->SegmentedPointsInImageFrame[3*nWires*i+j][0] - computedPositionInImagePlane[0];
        reprojectionError2D[1] = this->SegmentedPointsInImageFrame[3*nWires*i+j][1] - computedPositionInImagePlane[1];

        m_Measure[currentSegmentedPoint++] = sqrt(reprojectionError2D[0]*reprojectionError2D[0] + reprojectionError2D[1]*reprojectionError2D[1]);
      }

    }
    return m_Measure;

  }

  void GetDerivative( const ParametersType & parameters,
    DerivativeType  & derivative ) const
  {

  }

  unsigned int GetNumberOfParameters(void) const
  {
    return SpaceDimension;
  }

  unsigned int GetNumberOfValues(void) const
  {
    return RangeDimension;
  }

  void SetSegmentedPointsInImageFrame( std::vector< vnl_vector<double> > SegmentedPointsInImageFrame)
  {
    this->SegmentedPointsInImageFrame = SegmentedPointsInImageFrame;
  }

  void SetProbeToPhantomTransforms( std::vector< vnl_matrix<double> > ProbeToPhantomTransforms)
  {
    this->ProbeToPhantomTransforms = ProbeToPhantomTransforms;
  }

  void SetNWires( std::vector<NWire> NWires)
  {
    this->NWires = NWires;
  }

private:
  /*! Positions of ALL the segmented points in image frame - input to the second cost function (distance to wires) */
  std::vector< vnl_vector<double> > SegmentedPointsInImageFrame;

  /* Contains all the probe to phantom transforms used during calibration */
  std::vector< vnl_matrix<double> > ProbeToPhantomTransforms;

  /*! List of NWires used for calibration and error computation */
  std::vector<NWire> NWires;

  mutable MeasureType       m_Measure;
  mutable DerivativeType    m_Derivative;
  MeasureType       m_TheoreticalData;

};





class LMDistanceToMiddleWiresCostFunction : public itk::MultipleValuedCostFunction
{
public:
  typedef LMDistanceToMiddleWiresCostFunction                    Self;
  typedef itk::MultipleValuedCostFunction   Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;
  itkNewMacro( Self );

  int RangeDimension;
  int SpaceDimension ;

  typedef Superclass::ParametersType              ParametersType;
  typedef Superclass::DerivativeType              DerivativeType;
  typedef Superclass::MeasureType                 MeasureType;

  LMDistanceToMiddleWiresCostFunction(){}; 

  LMDistanceToMiddleWiresCostFunction(int numberOfParameters,int numberOfResiduals ):
  m_Measure(numberOfResiduals),
    m_Derivative(numberOfParameters,numberOfResiduals),
    m_TheoreticalData(numberOfResiduals)  
  {

    m_Measure.SetSize(numberOfResiduals);
    m_Derivative.SetSize(numberOfParameters,numberOfResiduals);
    m_TheoreticalData.SetSize(numberOfResiduals);
    this->RangeDimension = numberOfResiduals;
    this->SpaceDimension = numberOfParameters;

  }

  MeasureType GetValue( const ParametersType & parameters ) const
  {

    int numberOfFrames = this->DataPositionsInImageFrame.size();
    double px, py, pz, dx, dy, dz;
    double squaredDistanceToMiddleWire = 0;
    double residual=0;

    int numParameters = parameters.Size();
    vnl_vector<double> parametersVector(numParameters);
    vnl_matrix<double> transformationMatrix(4,4);


    for(int i=0;i<numParameters;i++)
    { 
      parametersVector(i) = parameters[i];
    }

    transformationMatrix = vtkSpatialCalibrationOptimizer::TransformParametersToTransformMatrix(parametersVector);

    for(int i=0;i<numberOfFrames;i++)
    {
      px = transformationMatrix[0][0]*this->DataPositionsInImageFrame[i][0] + transformationMatrix[0][1]*this->DataPositionsInImageFrame[i][1] + transformationMatrix[0][2]*this->DataPositionsInImageFrame[i][2] + transformationMatrix[0][3];
      py = transformationMatrix[1][0]*this->DataPositionsInImageFrame[i][0] + transformationMatrix[1][1]*this->DataPositionsInImageFrame[i][1] + transformationMatrix[1][2]*this->DataPositionsInImageFrame[i][2] + transformationMatrix[1][3];
      pz = transformationMatrix[2][0]*this->DataPositionsInImageFrame[i][0] + transformationMatrix[2][1]*this->DataPositionsInImageFrame[i][1] + transformationMatrix[2][2]*this->DataPositionsInImageFrame[i][2] + transformationMatrix[2][3];
      dx = px - this->DataPositionsInProbeFrame[i][0];
      dy = py - this->DataPositionsInProbeFrame[i][1];
      dz = pz - this->DataPositionsInProbeFrame[i][2];
      squaredDistanceToMiddleWire = dx*dx + dy*dy + dz*dz;
      m_Measure(i) = sqrt(squaredDistanceToMiddleWire);
    }

    return m_Measure;

  }



  void GetDerivative( const ParametersType & parameters,
    DerivativeType  & derivative ) const
  {

  }

  unsigned int GetNumberOfParameters(void) const
  {
    return SpaceDimension;
  }

  unsigned int GetNumberOfValues(void) const
  {
    return RangeDimension;
  }

  void SetSDataPositionsInImageFrame( std::vector< vnl_vector<double> > DataPositionsInImageFrame)
  {
    this->DataPositionsInImageFrame = DataPositionsInImageFrame;
  }

  void SetDataPositionsInProbeFrame( std::vector< vnl_vector<double> > DataPositionsInProbeFrame)
  {
    this->DataPositionsInProbeFrame = DataPositionsInProbeFrame;
  }

private:
  /*! Positions of the segmented points in image frame */
  std::vector< vnl_vector<double> > DataPositionsInImageFrame;

  /*! Positions of the segmented points in probe frame, they were calculated using the alpha value */
  std::vector< vnl_vector<double> > DataPositionsInProbeFrame;

  mutable MeasureType       m_Measure;
  mutable DerivativeType    m_Derivative;
  MeasureType       m_TheoreticalData;

};



class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command                               Superclass;
  typedef itk::SmartPointer<Self>                     Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() 
  {
    m_IterationNumber=0;
  }
public:
  typedef itk::LevenbergMarquardtOptimizer   OptimizerType;
  typedef   const OptimizerType   *          OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    LOG_DEBUG( "Observer::Execute() " << "\n");
    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );

    if( m_FunctionEvent.CheckEvent( &event ) )
    {
      LOG_DEBUG( m_IterationNumber++ << "   ");
      LOG_DEBUG( optimizer->GetCachedValue() << "   ");
      LOG_DEBUG( optimizer->GetCachedCurrentPosition());
      minimizationMeasures.push_back(optimizer->GetCachedValue());
    }
    else if( m_GradientEvent.CheckEvent( &event ) )
    {
      LOG_DEBUG("Gradient " << optimizer->GetCachedDerivative() << "   ");
    }
  }

  std::vector<OptimizerType::MeasureType> getMinimizationMeasures()
  {
    return this->minimizationMeasures;
  }
private:
  unsigned long m_IterationNumber;

  itk::FunctionEvaluationIterationEvent m_FunctionEvent;
  itk::GradientEvaluationIterationEvent m_GradientEvent;
  std::vector<OptimizerType::MeasureType> minimizationMeasures;
};


int vtkSpatialCalibrationOptimizer::itkRunLevenbergMarquardOptimization( bool useGradient, 
                                                                        double functionTolerance, double gradienteTolerance, double parametersTolerance, 
                                                                        double epsilonFunction, int maxIterations )
{

  LOG_DEBUG( "Levenberg Marquardt optimizer test \n \n"); 

  typedef  itk::LevenbergMarquardtOptimizer  OptimizerType;

  // Declaration of a itkOptimizer
  OptimizerType::Pointer  optimizer = OptimizerType::New();

  typedef LMDistanceToAllWiresCostFunction::ParametersType ParametersType;
  ParametersType  parameters(this->NumberOfParameters);



  //// assemble the transform vector from the seed matrix

  for(int i=0; i<this->NumberOfParameters; i++){
    parameters[i]=this->parametersVector(i);
  }

  LMDistanceToMiddleWiresCostFunction::Pointer costFunctionMiddleWires = NULL;
  LMDistanceToAllWiresCostFunction::Pointer costFunctionAllWires = NULL;

  switch (this->CurrentImageToProbeCalibrationCostFunction)
  {
  case MINIMIZATION_3D:
    // Set up minimizer and run the optimalization
    costFunctionMiddleWires = new LMDistanceToMiddleWiresCostFunction(this->NumberOfParameters,this->NumberOfResiduals);
    costFunctionMiddleWires->SetDataPositionsInProbeFrame(this->DataPositionsInProbeFrame);
    costFunctionMiddleWires->SetSDataPositionsInImageFrame(this->DataPositionsInImageFrame);

    costFunctionMiddleWires->GetValue(parameters);
    LOG_DEBUG("Number of Values = " << costFunctionMiddleWires->GetNumberOfValues() << "\n");

    try 
    {
      optimizer->SetCostFunction( costFunctionMiddleWires.GetPointer() );
    }
    catch( itk::ExceptionObject & e )
    {
      LOG_DEBUG( "Exception thrown ! " << "\n");
      LOG_DEBUG( "An error ocurred during Optimization" << "\n");
      LOG_DEBUG(  e << "\n");
      return PLUS_FAIL;
    }
    break;

  case MINIMIZATION_2D:
    // Declaration of the CostFunction adaptor
    costFunctionAllWires = new LMDistanceToAllWiresCostFunction(this->NumberOfParameters,this->NumberOfResiduals);
    costFunctionAllWires->SetNWires(this->NWires);
    costFunctionAllWires->SetProbeToPhantomTransforms(this->ProbeToPhantomTransforms);
    costFunctionAllWires->SetSegmentedPointsInImageFrame(this->SegmentedPointsInImageFrame);

    costFunctionAllWires->GetValue(parameters);


    LOG_DEBUG("Number of Values = " << costFunctionAllWires->GetNumberOfValues() << "\n");

    try 
    {
      optimizer->SetCostFunction( costFunctionAllWires.GetPointer() );
    }
    catch( itk::ExceptionObject & e )
    {
      LOG_DEBUG( "Exception thrown ! " << "\n");
      LOG_DEBUG( "An error ocurred during Optimization" << "\n");
      LOG_DEBUG(  e << "\n");
      return PLUS_FAIL;
    }
    break;
  }


  // this following call is equivalent to invoke: costFunction->SetUseGradient( useGradient );
  optimizer->GetUseCostFunctionGradient();
  optimizer->UseCostFunctionGradientOn();
  optimizer->UseCostFunctionGradientOff();
  optimizer->SetUseCostFunctionGradient( useGradient );

  optimizer->SetGradientTolerance(gradienteTolerance);
  optimizer->SetEpsilonFunction(epsilonFunction);
  optimizer->SetNumberOfIterations(maxIterations);
  optimizer->SetValueTolerance(functionTolerance);

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( this->NumberOfParameters );

  scales[0] = this->rotationParametersScale ;
  scales[1] = this->rotationParametersScale;
  scales[2] = this->rotationParametersScale;
  scales[3] = this->translationParametersScale; 
  scales[4] = this->translationParametersScale; 
  scales[5] = this->translationParametersScale;
  scales[6] = this->scalesParametersScale;
  
  if(this->NumberOfParameters==8)
  {
    scales[7] = this->scalesParametersScale;
  }
  

  optimizer->SetScales(scales);
  optimizer->SetInitialPosition(parameters);

  CommandIterationUpdate::Pointer observer =  CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
  optimizer->AddObserver( itk::FunctionEvaluationIterationEvent(), observer );

  try 
  {
    optimizer->StartOptimization();
  }
  catch( itk::ExceptionObject & e )
  {
    LOG_ERROR("Exception thrown ! " << "\n");
    LOG_ERROR( "An error ocurred during Optimization" << "\n");
    LOG_ERROR( "Location    = " << e.GetLocation()    << "\n");
    LOG_ERROR( "Description = " << e.GetDescription() << "\n");
    return PLUS_FAIL;
  }

  OptimizerType::ParametersType finalPosition;
  finalPosition = optimizer->GetCurrentPosition();
  std::vector<OptimizerType::MeasureType> minimizationResiduals = observer->getMinimizationMeasures();
  int numberOfEvaluations = minimizationResiduals.size();
  for(int i=0; i<numberOfEvaluations; i++)
  {
    double squaredDistanceToWires, residual = 0; 
    for (int j=0; j<this->NumberOfResiduals; j++)
    {
      squaredDistanceToWires = minimizationResiduals.at(i)(j)*minimizationResiduals.at(i)(j);
      residual += squaredDistanceToWires;
    }
    double rmsError = sqrt(residual/this->NumberOfResiduals);
    this->minimizationResiduals.push_back(rmsError);
  }

  for(int i=0; i<this->NumberOfParameters; i++)
  {
    this->parametersVector(i) = finalPosition[i];
  }

  LOG_DEBUG( "Stop description   = " << optimizer->GetStopConditionDescription());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkSpatialCalibrationOptimizer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSpatialCalibrationOptimizer);

//-----------------------------------------------------------------------------

vtkSpatialCalibrationOptimizer::vtkSpatialCalibrationOptimizer()
{
  this->Minimizer = NULL;

  vtkSmartPointer<vtkAmoebaMinimizer> minimizer = vtkSmartPointer<vtkAmoebaMinimizer>::New();
  this->CurrentImageToProbeCalibrationOptimizationMethod = NO_OPTIMIZATION;
  this->SetMinimizer(minimizer);
}

//-----------------------------------------------------------------------------

vtkSpatialCalibrationOptimizer::~vtkSpatialCalibrationOptimizer()
{
  this->SetMinimizer(NULL);
}

// --------------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::ComputeRmsError(const vnl_matrix<double> &transformationMatrix, double *aRmsError, double *aRmsErrorSD)
{
  int numberOfFrames = 0;
  double rmsError = 0;
  double px,py,pz,dx,dy,dz =0;
  double squaredDistanceToMiddleWire=0;
  std::vector<double> reprojectionErrors;

  switch (this->CurrentImageToProbeCalibrationCostFunction)
  {
  case MINIMIZATION_3D:
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
  case MINIMIZATION_2D:

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

  double squareDiffSum = 0;
  int numberOfProjecterPoints = reprojectionErrors.size();
  for (int j=0; j<numberOfProjecterPoints; ++j)
  {
    double diff = reprojectionErrors.at(j) - rmsError;
    squareDiffSum += diff * diff;
  }
  double variance = squareDiffSum / numberOfProjecterPoints;
  *aRmsError =rmsError;
  *aRmsErrorSD = sqrt(variance);
}

//----------------------------------------------------------------------------

void vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction(void *vtkSpatialCalibrationOptimizerPointer)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction");

  vtkSpatialCalibrationOptimizer *self = (vtkSpatialCalibrationOptimizer*)vtkSpatialCalibrationOptimizerPointer;

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
  vnl_matrix<double> rotationMatrix(3,3);

  rotationVersor(0) = r1;
  rotationVersor(1) = r2;
  rotationVersor(2) = r3;
  rotationMatrix = RotationVersorToRotationMatrix(rotationVersor);

  int numberOfFrames = self->DataPositionsInImageFrame.size();
  double px, py, pz, dx, dy, dz;
  double squaredDistanceToMiddleWire = 0;
  double residual=0;

  for(int i=0;i<numberOfFrames;i++)
  {
    px = sx*rotationMatrix[0][0]*self->DataPositionsInImageFrame[i][0] + sy*rotationMatrix[0][1]*self->DataPositionsInImageFrame[i][1] + 
      rotationMatrix[0][2]*self->DataPositionsInImageFrame[i][2] + t1;
    py = sx*rotationMatrix[1][0]*self->DataPositionsInImageFrame[i][0] + sy*rotationMatrix[1][1]*self->DataPositionsInImageFrame[i][1] + 
      rotationMatrix[1][2]*self->DataPositionsInImageFrame[i][2] + t2;
    pz = sx*rotationMatrix[2][0]*self->DataPositionsInImageFrame[i][0] + sy*rotationMatrix[2][1]*self->DataPositionsInImageFrame[i][1] + 
      rotationMatrix[2][2]*self->DataPositionsInImageFrame[i][2] + t3;

    dx = px - self->DataPositionsInProbeFrame[i][0];
    dy = py - self->DataPositionsInProbeFrame[i][1];
    dz = pz - self->DataPositionsInProbeFrame[i][2];

    squaredDistanceToMiddleWire = dx*dx + dy*dy + dz*dz;
    residual += squaredDistanceToMiddleWire;
  }
  double rmsError = sqrt(residual/numberOfFrames);
  self->Minimizer->SetFunctionValue(rmsError);
  self->minimizationResiduals.push_back(rmsError);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction2(void *vtkSpatialCalibrationOptimizerPointer)
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::vtkImageToProbeCalibrationMatrixEvaluationFunction2");

  vtkSpatialCalibrationOptimizer *self = (vtkSpatialCalibrationOptimizer*)vtkSpatialCalibrationOptimizerPointer;

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
  vnl_matrix<double> rotationMatrix(3,3);

  rotationVersor(0) = r1;
  rotationVersor(1) = r2;
  rotationVersor(2) = r3;
  rotationMatrix = RotationVersorToRotationMatrix(rotationVersor);

  int nWires = self->NWires.size(); 
  int numberOfFrames = self->SegmentedPointsInImageFrame.size()/(3*nWires); //number of frames

  double px, py, pz;
  double residual=0;

  vnl_vector<double> segmentedInProbeFrame_vnl(4), segmentedInPhantomFrame_vnl(4);
  vnl_vector<double> wireFrontPoint_vnl(3), wireBackPoint_vnl(3);

  for(int i=0;i<numberOfFrames;i++) // for each frame
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

      double d = PointToWireDistance(segmentedInPhantomFrame_vnl.extract(3,0), wireFrontPoint_vnl, wireBackPoint_vnl );
      residual += d*d;
    }
  }

  double rmsError= sqrt(residual/(3*numberOfFrames*nWires));
  self->Minimizer->SetFunctionValue(rmsError);
  self->minimizationResiduals.push_back(rmsError);  // for debugging
}

//-----------------------------------------------------------------------------

void vtkSpatialCalibrationOptimizer::SetVtkAmoebaMinimizerInitialParameters(vtkAmoebaMinimizer *minimizer, const vnl_vector<double> &parametersVector)
{  

  minimizer->SetFunctionArgDelete(NULL);
  double scaleRotation = this->rotationParametersScale ; //vtkSpatialCalibrationOptimizer::GetAmoebaRotationScale();
  minimizer->SetParameterValue("r1",parametersVector[0]);
  minimizer->SetParameterScale("r1",scaleRotation);
  minimizer->SetParameterValue("r2",parametersVector[1]);
  minimizer->SetParameterScale("r2",scaleRotation);
  minimizer->SetParameterValue("r3",parametersVector[2]);
  minimizer->SetParameterScale("r3",scaleRotation);

  double scaleTranslation = this->translationParametersScale; //vtkSpatialCalibrationOptimizer::GetAmoebaTranslationScale();
  minimizer->SetParameterValue("t1",parametersVector[3]);
  minimizer->SetParameterScale("t1",scaleTranslation);
  minimizer->SetParameterValue("t2",parametersVector[4]);
  minimizer->SetParameterScale("t2",scaleTranslation);
  minimizer->SetParameterValue("t3",parametersVector[5]);
  minimizer->SetParameterScale("t3",scaleTranslation);

  double scaleScales= this->scalesParametersScale; //vtkSpatialCalibrationOptimizer::GetAmoebaScaleParameterScale();
  minimizer->SetParameterValue("sx",parametersVector[6]);
  minimizer->SetParameterScale("sx",scaleScales);
  minimizer->SetParameterValue("sy",parametersVector[7]);
  minimizer->SetParameterScale("sy", scaleScales);

  minimizer->SetMaxIterations(this->amoebaMaxIterations);
  minimizer->SetTolerance(this->amoebaTolerance);
  LOG_DEBUG("Vector parameters " << parametersVector << " seed");
}

//-----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::ShowTransformation(const vnl_matrix<double> &transformationMatrix)
{
  vnl_vector<double> parametersVector(8);
  vnl_vector<double> rotationVersor(3);
  vnl_matrix<double> rotationMatrix(3,3);

  parametersVector = TransformMatrixToParametersVector(transformationMatrix);


  rotationMatrix = transformationMatrix.extract(3,3);

  double scaleX, scaleY, scaleZ;
  scaleX = rotationMatrix.get_column(0).two_norm();
  scaleY = rotationMatrix.get_column(1).two_norm();
  scaleZ = rotationMatrix.get_column(2).two_norm();

  LOG_INFO( "Rotation Norm in X = " << scaleX << "\n");
  LOG_INFO( "Rotation Norm in Y = " << scaleY << "\n");
  LOG_INFO( "Rotation Norm in Z = " << scaleZ << "\n"); 
  double normCrossProduct= vnl_cross_3d(rotationMatrix.get_column(0).normalize(),rotationMatrix.get_column(1).normalize()).two_norm();
  LOG_INFO( "Module of cross product between first and second column = " << normCrossProduct<< "\n"); 
  LOG_INFO( "q1 = " << parametersVector(0) << "\n");
  LOG_INFO( "q2 = " << parametersVector(1) << "\n");
  LOG_INFO( "q3 = " << parametersVector(2) << "\n");
  LOG_INFO( "q4 = " << parametersVector(3) << "\n");
  LOG_INFO( "Translation in X = " << parametersVector(4) << "\n");
  LOG_INFO( "Translation in Y = " << parametersVector(5) << "\n");
  LOG_INFO( "Translation in Z = " << parametersVector(6) << "\n");
  LOG_INFO( "skew = " << scaleY/scaleX << "\n");

  this->optimizationResults.push_back(parametersVector(0)); //q1
  this->optimizationResults.push_back(parametersVector(1)); //q2
  this->optimizationResults.push_back(parametersVector(2)); //q3
  this->optimizationResults.push_back(parametersVector(3)); //q4
  this->optimizationResults.push_back(parametersVector(4)); //t1
  this->optimizationResults.push_back(parametersVector(5)); //t2
  this->optimizationResults.push_back(parametersVector(6)); //t3
  this->optimizationResults.push_back(scaleX); //sx
  this->optimizationResults.push_back(scaleY/scaleX); //ratio 
  this->optimizationResults.push_back(normCrossProduct); // cross product between first and secon column
  //}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkSpatialCalibrationOptimizer::StoreAndShowResults()
{
  this->optimizationResults.clear();
  // Show the results
  for( int i=this->minimizationResiduals.size()-30; i < this->minimizationResiduals.size() ; i++ ){
    LOG_INFO( "FunctionEvaluation " << i << ":  rmsError = " << this->minimizationResiduals[i] << "\n");
  }

  this->optimizationResults.push_back(this->NumberOfParameters);
  this->optimizationResults.push_back(this->CurrentImageToProbeCalibrationCostFunction);

  LOG_INFO( "Optimization Method = " << GetCalibrationOptimizationMethodAsString(this->CurrentImageToProbeCalibrationOptimizationMethod) << "\n");
  LOG_INFO("Cost function = " << GetCalibrationCostFunctionAsString(this->CurrentImageToProbeCalibrationCostFunction) << "\n");

  LOG_INFO( "Number Of Optimized Parameters " << this->NumberOfParameters << "\n"); 
  LOG_INFO("Initial parameters \n");
  ShowTransformation(this->ImageToProbeSeedTransformMatrixVnl);
  double rmsError =0, rmsErrorSD = 0;
  ComputeRmsError(this->ImageToProbeSeedTransformMatrixVnl, &rmsError, &rmsErrorSD);
  LOG_INFO( "Without optimization: rmsError = " << rmsError << " mm \n");
  this->optimizationResults.push_back(rmsError);
  this->optimizationResults.push_back(rmsErrorSD);

  LOG_INFO( "First Iteration " << 1 << ": rmsError = " << this->minimizationResiduals[1] << " pixels \n");
  LOG_INFO( "Last Iteration " << this->minimizationResiduals.size() << ":  rmsError = " << this->minimizationResiduals[this->minimizationResiduals.size()-1] << " pixels \n");

  LOG_INFO("Optimized parameters \n");
  ShowTransformation(this->ImageToProbeTransformMatrixVnl);
  ComputeRmsError(this->ImageToProbeTransformMatrixVnl, &rmsError, &rmsErrorSD);
  LOG_INFO( "With optimization: rmsError = " << rmsError << " mm \n");
  this->optimizationResults.push_back(rmsError);
  this->optimizationResults.push_back(rmsErrorSD);

  vtkSmartPointer<vtkMatrix4x4> imageToProbeSeedTransformMatrixVtk = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> imageToProbeTransformMatrixVtk = vtkSmartPointer<vtkMatrix4x4>::New();
  PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeSeedTransformMatrixVnl,imageToProbeSeedTransformMatrixVtk);
  PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeTransformMatrixVnl,imageToProbeTransformMatrixVtk);
  double angleDifference = PlusMath::GetOrientationDifference(imageToProbeSeedTransformMatrixVtk, imageToProbeTransformMatrixVtk);
  LOG_INFO("Orientation difference between both matrices =  " << angleDifference  <<  "\n");
  this->optimizationResults.push_back(angleDifference);
}

//----------------------------------------------------------------------------

PlusStatus vtkSpatialCalibrationOptimizer::Update()
{
  LOG_TRACE("vtkSpatialCalibrationOptimizer::Optimize( " << CurrentImageToProbeCalibrationOptimizationMethod << ")");

  this->parametersVector = TransformMatrixToParametersVector(this->ImageToProbeSeedTransformMatrixVnl);

  switch(this->CurrentImageToProbeCalibrationOptimizationMethod)
  {
  case VTK_AMOEBA_MINIMIZER:
    {
    SetVtkAmoebaMinimizerInitialParameters(this->Minimizer, this->parametersVector);

    switch (this->CurrentImageToProbeCalibrationCostFunction)
    {
    case MINIMIZATION_3D:
      // Set up minimizer and run the optimalization
      this->Minimizer->SetFunction(vtkImageToProbeCalibrationMatrixEvaluationFunction,this);
      break;
    case MINIMIZATION_2D:
      this->Minimizer->SetFunction(vtkImageToProbeCalibrationMatrixEvaluationFunction2,this);
      break;
    }

    this->Minimizer->Minimize();

    this->parametersVector[0] = this->Minimizer->GetParameterValue("r1");
    this->parametersVector[1] = this->Minimizer->GetParameterValue("r2");
    this->parametersVector[2] = this->Minimizer->GetParameterValue("r3");
    this->parametersVector[3] = this->Minimizer->GetParameterValue("t1");
    this->parametersVector[4] = this->Minimizer->GetParameterValue("t2");
    this->parametersVector[5] = this->Minimizer->GetParameterValue("t3");
    this->parametersVector[6] = this->Minimizer->GetParameterValue("sx");
    this->parametersVector[7] = this->Minimizer->GetParameterValue("sy");

    break;
    }
  case ITK_LEVENBERG_MARQUARD:
    {
    //bool useGradient = false;
    itkRunLevenbergMarquardOptimization( false,this->LMFunctionTolerance,
      this->LMGradienteTolerance,this->LMParametersTolerance, 
      this->LMEpsilonFunction,this->LMMaxIterations );

    break;
    }
  case FIDUCIALS_SIMILARITY:
    {
    std::vector< itk::Point<double,3> > fixedPoints, movingPoints;
    itk::Point<double,3> auxiliar;
    for (int i=0; i<this->DataPositionsInImageFrame.size();i++)
    {
      auxiliar[0]= this->DataPositionsInProbeFrame[i][0];
      auxiliar[1]= this->DataPositionsInProbeFrame[i][1];
      auxiliar[2]= this->DataPositionsInProbeFrame[i][2];
      fixedPoints.push_back(auxiliar);  

      auxiliar[0]= this->DataPositionsInImageFrame[i][0];
      auxiliar[1]= this->DataPositionsInImageFrame[i][1];
      auxiliar[2]= this->DataPositionsInImageFrame[i][2];
      movingPoints.push_back(auxiliar);
    }

    itkFiducialTransformComputation *fiducialTransformComputation = new itkFiducialTransformComputation();
    fiducialTransformComputation->computeTransform(fixedPoints,movingPoints,"Similarity");
    this->ImageToProbeTransformMatrixVnl = fiducialTransformComputation->GetImageToProbeTransformMatrixVnl();

    LOG_INFO( " Fiducial Similarity Transform \n");
    LOG_INFO( "rms error = " << fiducialTransformComputation->GetRmsError() << " mm \n");
    LOG_INFO( "scale factor = " << fiducialTransformComputation->GetScaleFactor() << "\n");
    this->CurrentImageToProbeCalibrationCostFunction = MINIMIZATION_3D;
    double rmsError =0, rmsErrorSD = 0;
    ComputeRmsError(this->ImageToProbeSeedTransformMatrixVnl, &rmsError, &rmsErrorSD);
    LOG_INFO( "Without optimization: rmsError = " << rmsError << " mm \n");
    return PLUS_SUCCESS;
    break;
    }
  }


  // Store the matrix
  this->ImageToProbeTransformMatrixVnl = TransformParametersToTransformMatrix(this->parametersVector);

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
  this->NumberOfResiduals = this->DataPositionsInImageFrame.size();

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

  //this->SegmentedPointsInImageFrame = *SegmentedPointsInImageFrame;
  this->NWires = * NWires;
  //this->ProbeToPhantomTransforms = *ProbeToPhantomTransforms;
  this->ImageToProbeSeedTransformMatrixVnl= *ImageToProbeTransformMatrixVnl;
  this->NumberOfResiduals = this->SegmentedPointsInImageFrame.size();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

vnl_matrix<double> vtkSpatialCalibrationOptimizer::GetOptimizedImageToProbeTransformMatrix()
{
  return this->ImageToProbeTransformMatrixVnl;
}


char* vtkSpatialCalibrationOptimizer::GetCalibrationCostFunctionAsString(ImageToProbeCalibrationCostFunctionType type)
{
  switch (type)
  {
  case MINIMIZATION_3D: return "MINIMIZATION_3D";
  case MINIMIZATION_2D: return "MINIMIZATION_2D";
  default:
    LOG_ERROR("Unknown cost function to perform calibration: "<<type);
    return "unknown";
  }
}

char* vtkSpatialCalibrationOptimizer::GetCalibrationOptimizationMethodAsString( ImageToProbeCalibrationOptimizationMethodType type)
{
  switch (type)
  {
  case ITK_LEVENBERG_MARQUARD: return "ITK_LEVENBERG_MARQUARD";
  case VTK_AMOEBA_MINIMIZER: return "VTK_AMOEBA_MINIMIZER";
  case FIDUCIALS_SIMILARITY: return "FIDUCIALS_SIMILARITY";
  default:
    return "NO_OPTIMIZATION";
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
  vtkXMLDataElement* spatialCalibrationOptimizerElement = aConfig->FindNestedElementWithName("vtkSpatialCalibrationOptimizer"); 


  if (spatialCalibrationOptimizerElement->GetAttribute("OptimizerMinimizationMethod"))
  {
    if (STRCASECMP(spatialCalibrationOptimizerElement->GetAttribute("OptimizerMinimizationMethod"),
      GetCalibrationOptimizationMethodAsString(VTK_AMOEBA_MINIMIZER)) == 0)
    {
      this->CurrentImageToProbeCalibrationOptimizationMethod = VTK_AMOEBA_MINIMIZER;
      this->rotationParametersScale = AMOEBA_ROTATION_PARAMETERS_SCALE;
      this->translationParametersScale = AMOEBA_TRANSLATION_PARAMETERS_SCALE;
      this->scalesParametersScale = AMOEBA_SCALE_PARAMETERS_SCALE;
      this->amoebaMaxIterations = AMOEBA_MAX_ITERATIONS;
      this->amoebaTolerance = AMOEBA_TOLERANCE;
    }
    else if (STRCASECMP(spatialCalibrationOptimizerElement->GetAttribute("OptimizerMinimizationMethod"), 
      GetCalibrationOptimizationMethodAsString(ITK_LEVENBERG_MARQUARD)) == 0)
    {
      this->CurrentImageToProbeCalibrationOptimizationMethod=ITK_LEVENBERG_MARQUARD;
      this->rotationParametersScale = LM_ROTATION_PARAMETERS_SCALE;
      this->translationParametersScale = LM_TRANSLATION_PARAMETERS_SCALE;
      this->scalesParametersScale = LM_SCALE_PARAMETERS_SCALE;
      this->LMFunctionTolerance = LM_FUNCTION_TOLERANCE; 
      this->LMGradienteTolerance = LM_GRADIENT_TOLERANCE; 
      this->LMParametersTolerance= LM_PARAMETERS_TOLERANCE; 
      this->LMEpsilonFunction = LM_STEP;
      this->LMMaxIterations = LM_MAX_ITERATIONS;
    }
    else if (STRCASECMP(spatialCalibrationOptimizerElement->GetAttribute("OptimizerMinimizationMethod"), 
      GetCalibrationOptimizationMethodAsString(FIDUCIALS_SIMILARITY)) == 0)
    {
      this->CurrentImageToProbeCalibrationOptimizationMethod=FIDUCIALS_SIMILARITY;
    }
    else
    {
      this->CurrentImageToProbeCalibrationOptimizationMethod=NO_OPTIMIZATION;
    }
  }
  else
  {
    this->CurrentImageToProbeCalibrationOptimizationMethod=NO_OPTIMIZATION;
  }


  if (spatialCalibrationOptimizerElement->GetAttribute("OptimizerCostFunction"))
  {
    if (STRCASECMP(spatialCalibrationOptimizerElement->GetAttribute("OptimizerCostFunction"),
      GetCalibrationCostFunctionAsString(MINIMIZATION_3D)) == 0)
    {
      this->CurrentImageToProbeCalibrationCostFunction=MINIMIZATION_3D;
    }
    else if (STRCASECMP(spatialCalibrationOptimizerElement->GetAttribute("OptimizerCostFunction"), 
      GetCalibrationCostFunctionAsString(MINIMIZATION_2D)) == 0)
    {
      this->CurrentImageToProbeCalibrationCostFunction=MINIMIZATION_2D;
    }
  }

  int numberOfParameters = -1;
  if (spatialCalibrationOptimizerElement->GetScalarAttribute("NumberOfParameters", numberOfParameters))
  {
    NumberOfParameters = numberOfParameters;
  }

  return PLUS_SUCCESS;
}

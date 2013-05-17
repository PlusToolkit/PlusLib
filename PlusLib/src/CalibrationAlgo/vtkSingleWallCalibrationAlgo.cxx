/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "TrackedFrame.h"
#include "vnl/algo/vnl_qr.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vtkLineSegmentationAlgo.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkSingleWallCalibrationAlgo.h"
#include "vtkSmartPointer.h"
#include "vtkTrackedFrameList.h"
#include <iostream>

//------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkSingleWallCalibrationAlgo, "$Revision: 1.0$");
vtkStandardNewMacro(vtkSingleWallCalibrationAlgo);

//------------------------------------------------------------------------

vtkSingleWallCalibrationAlgo::vtkSingleWallCalibrationAlgo()
: TrackedFrameList(NULL)
, LineSegmenter(NULL)
, ImageToProbeTransformation(NULL)
, MmPerPixelX(1.0)
, MmPerPixelY(1.0)
, ImageCoordinateFrame(NULL)
, ProbeCoordinateFrame(NULL)
, ReferenceCoordinateFrame(NULL)
, UpToDate(false)
{
  vtkMatrix4x4* aMatrix = vtkMatrix4x4::New();
  aMatrix->Identity();
  this->SetImageToProbeTransformation(aMatrix);

  this->SetLineSegmenter(vtkLineSegmentationAlgo::New());
}

//------------------------------------------------------------------------

vtkSingleWallCalibrationAlgo::~vtkSingleWallCalibrationAlgo()
{
  this->SetImageToProbeTransformation(NULL);
  this->SetLineSegmenter(NULL);
}

//----------------------------------------------------------------------------

void vtkSingleWallCalibrationAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------

PlusStatus vtkSingleWallCalibrationAlgo::ExtractVectorsOfImageAndNormalOfWall(const vnl_vector<double>& anXEstimate, vtkSingleWallCalibrationAlgo::ExtractResult& aResult)
{
  int l = 0;
  double v1[3] = {0};
  double n1[3] = {0};
  double u1[3] = {0};

  int l1 = 0;
  int l2 = 0;

  for (int i = 0 ; i < 6; i++)
  {
    l1 = (i)*3;
    l2 = (i)*3+2;
    l = 0;

    for (int j=l1; j < l2+1; j++)
    {
      n1[l] = anXEstimate[j];
      l = l+1;
    }

    double vNormal = sqrt(n1[0]*n1[0] + n1[1] *n1[1] + n1[2]*n1[2]);  

    if (n1[0] > 0)
    {
      n1[0] = n1[0]/vNormal;
      n1[1] = n1[1]/vNormal;
      n1[2] = n1[2]/vNormal;
    }
    else 
    {
      n1[0] = -n1[0]/vNormal;
      n1[1] = -n1[1]/vNormal;
      n1[2] = -n1[2]/vNormal;
    }

    for (int l=0; l < 3; l++)
    {
      aResult.NormalVectorsOfWallPlane[l][i] = n1[l];
    }
  }

  for (int i=0; i < 3; i++)
  {
    u1[0] = anXEstimate[i];
    u1[1] = anXEstimate[3+i];
    u1[2] = anXEstimate[6+i];
    v1[0] = anXEstimate[i+9];
    v1[1] = anXEstimate[i+12];
    v1[2] = anXEstimate[i+15];

    double uNormal = sqrt(u1[0]*u1[0] + u1[1]*u1[1] + u1[2]*u1[2]);
    double vNormal = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);

    if (u1[0] > 0) 
    {
      u1[0] = u1[0] / uNormal;
      u1[1] = u1[1] / uNormal;
      u1[2] = u1[2] / uNormal;
    }
    else 
    {
      u1[0] = -u1[0] / uNormal;
      u1[1] = -u1[1] / uNormal;
      u1[2] = -u1[2] / uNormal;
    }
    if (v1[0] > 0) 
    {
      v1[0] = v1[0] / vNormal;
      v1[1] = v1[1] / vNormal;
      v1[2] = v1[2] / vNormal;
    }
    else 
    {
      v1[0] = -v1[0]/vNormal;
      v1[1] = -v1[1]/vNormal;
      v1[2] = -v1[2]/vNormal;
    }

    for (int l=0; l < 3 ; l++)
    {
      aResult.LateralDirectionsOfImagePlane[l][i] = u1[l];
      aResult.AxialDirectionsOfWallPlane[l][i] = v1[l];
    }
  }

  double normf1(0.0);
  double normf2(0.0);
  for (int i=0; i < 9; i++)
  {
    normf1 = normf1 + anXEstimate[i] * anXEstimate[i];
    normf2 = normf2 + anXEstimate[i+9]*anXEstimate[i+9];
  }
  normf1 = sqrt(normf1);
  normf2 = sqrt(normf2);

  aResult.PixelPerMMInYToPixelPerMMInX = (normf2)/(normf1);

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------

PlusStatus vtkSingleWallCalibrationAlgo::Calibrate()
{
  if( this->UpToDate )
  {
    return PLUS_SUCCESS;
  }

  if( this->Validate() != PLUS_SUCCESS )
  {
    LOG_ERROR("Algorithm data has not been initialized. Cannot continue.");
    return PLUS_FAIL;
  }

  LOG_INFO("Starting single wall image to probe calculation with " << this->TrackedFrameList->GetNumberOfTrackedFrames() << " images.");

  vtkMatrix4x4* aProbeToReferenceTransform = vtkMatrix4x4::New();
  vtkMatrix4x4* initialRotationMatrixInverse = vtkMatrix4x4::New();
  std::vector< vtkMatrix4x4* > relativeRotationFromInitialPosition;
  relativeRotationFromInitialPosition.resize(this->TrackedFrameList->GetNumberOfTrackedFrames());
  for( int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++imageIndex )
  {
    relativeRotationFromInitialPosition[imageIndex] = vtkMatrix4x4::New();
  }

  if( this->LineSegmenter->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to complete line segmentation.");
    return PLUS_FAIL;
  }

  {
    TrackedFrame* firstFrame = this->TrackedFrameList->GetTrackedFrame(0);
    PlusTransformName probeToReferenceName(this->GetProbeCoordinateFrame(), this->GetReferenceCoordinateFrame());
    if( firstFrame->GetCustomFrameTransform(probeToReferenceName, aProbeToReferenceTransform) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to locate ProbeToReferenceTransform in first frame of tracked frame list.");
      return PLUS_FAIL;
    }
    initialRotationMatrixInverse->DeepCopy(aProbeToReferenceTransform);
    (*initialRotationMatrixInverse)[0][3] = 0.0;
    (*initialRotationMatrixInverse)[1][3] = 0.0;
    (*initialRotationMatrixInverse)[2][3] = 0.0;
    (*initialRotationMatrixInverse)[3][3] = 1.0;
    (*initialRotationMatrixInverse)[3][0] = 0.0;
    (*initialRotationMatrixInverse)[3][1] = 0.0;
    (*initialRotationMatrixInverse)[3][2] = 0.0;
    initialRotationMatrixInverse->Invert();
  }

  for( int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++imageIndex )
  {
    TrackedFrame* aFrame = this->TrackedFrameList->GetTrackedFrame(imageIndex);
    PlusTransformName probeToReferenceName(this->GetProbeCoordinateFrame(), this->GetReferenceCoordinateFrame());
    if( aFrame->GetCustomFrameTransform(probeToReferenceName, aProbeToReferenceTransform) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable to locate ProbeToReferenceTransform in frame " << imageIndex << " of tracked frame list.");
      return PLUS_FAIL;
    }
    (*aProbeToReferenceTransform)[0][3] = 0.0;
    (*aProbeToReferenceTransform)[1][3] = 0.0;
    (*aProbeToReferenceTransform)[2][3] = 0.0;
    (*aProbeToReferenceTransform)[3][3] = 1.0;
    (*aProbeToReferenceTransform)[3][0] = 0.0;
    (*aProbeToReferenceTransform)[3][1] = 0.0;
    (*aProbeToReferenceTransform)[3][2] = 0.0;
    vtkMatrix4x4::Multiply4x4(aProbeToReferenceTransform, initialRotationMatrixInverse, relativeRotationFromInitialPosition[imageIndex]);
    this->CleanMatrix(relativeRotationFromInitialPosition[imageIndex]);
  }

  vnl_matrix<double> A(this->TrackedFrameList->GetNumberOfTrackedFrames(), 18);
  A = A * 0.0;
  vnl_vector<double> aRow(18);

  std::vector<vtkLineSegmentationAlgo::LineParameters> lineParameters;
  this->LineSegmenter->GetDetectedLineParameters(lineParameters);

  for (int imageIndex = 1; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); imageIndex++)
  {
    int k = 0;
    for (int l1 = 0; l1 < 3; l1++)
    {
      for (int l2 = 0; l2 < 3; l2++)
      {
        aRow[k] = (*relativeRotationFromInitialPosition[imageIndex])[l2][l1];
        aRow[k+9] = lineParameters[imageIndex].Slope() * 
          (*relativeRotationFromInitialPosition[imageIndex])[l2][l1];
        k++;
      }
    }
    A.set_row(imageIndex-1, aRow);
  }

  //------------ Now Solve for X ------------------
  vnl_matrix<double> A2vnl(this->TrackedFrameList->GetNumberOfTrackedFrames(), 17);
  A2vnl = A2vnl * 0.0;
  vnl_matrix<double> B2vnl(this->TrackedFrameList->GetNumberOfTrackedFrames(), 1);
  B2vnl = B2vnl * 0.0;

  for (int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++imageIndex)
  {
    B2vnl[imageIndex][0] = -A[imageIndex][0];
  }

  for (int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++imageIndex)
  {
    for (int l2 = 0; l2 < 17; l2++)
    {
      A2vnl[imageIndex][l2] = A[imageIndex][l2+1];
    }
  }

  vnl_matrix<double> A2Tvnl = A2vnl.transpose();
  vnl_matrix<double> lambdaIdentityMatrix(17, 17);
  lambdaIdentityMatrix.set_identity();
  lambdaIdentityMatrix = lambdaIdentityMatrix * 0.5;

  vnl_qr<double> q( (A2Tvnl*A2vnl) + lambdaIdentityMatrix );
  vnl_matrix<double> inverse = q.inverse();
  vnl_matrix<double> A2TB2 = A2Tvnl*B2vnl;

  vnl_vector<double> C(17);
  for (int i = 0; i < 17; i++)
  {
    C[i] = A2TB2[0][i];
  } 

  vnl_matrix<double> tmp4 = inverse*A2TB2;  // estimation of X

  vnl_vector<double> X2_est(17);
  for (int i=0; i < 17; i++)
  {
    X2_est[i] = *tmp4[i];
  }

  vnl_vector<double> X_est0(18);
  X_est0[0] = 1;
  for (int i = 1; i < 18; i++)
  {
    X_est0[i] = X2_est[i-1];
  }

  double scale_est = 0;
  for(int i=0; i < 9; i++)
  {
    scale_est = scale_est + pow(X_est0[i],2);
  }

  scale_est = sqrt(scale_est);
  scale_est = 1/scale_est;


  vnl_vector<double> X_est(18);
  for (int i=0; i < 18; i++)
  {
    X_est[i] = X_est0[i]*scale_est;
  }

  vtkSingleWallCalibrationAlgo::ExtractResult unv;
  unv.NormalVectorsOfWallPlane.set_size(3, 6);
  unv.LateralDirectionsOfImagePlane.set_size(3, 3);
  unv.AxialDirectionsOfWallPlane.set_size(3, 3);
  if( this->ExtractVectorsOfImageAndNormalOfWall(X_est, unv) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to extract x.");
    return PLUS_FAIL;
  }

  vnl_vector<double> nEstimate(3);
  vnl_vector<double> initialUEstimate(3);
  vnl_vector<double> initialVEstimate(3);

  for(int i = 0; i < 3; i++)
  {
    nEstimate[i] = unv.NormalVectorsOfWallPlane[i][0];
    initialUEstimate[i] = unv.LateralDirectionsOfImagePlane[i][1];
    initialVEstimate[i] = -unv.AxialDirectionsOfWallPlane[i][0];
  }

  vnl_vector<double> initialZEstimate = vnl_cross_3d(initialUEstimate, initialVEstimate);
  initialZEstimate.normalize();

  vtkMatrix4x4* initialImageToReferenceEstimate = vtkMatrix4x4::New(); 
  initialImageToReferenceEstimate->Identity();

  for (int row = 0; row < 3; row++)
  {
    (*initialImageToReferenceEstimate)[row][0] = initialUEstimate[row];
    (*initialImageToReferenceEstimate)[row][1] = initialVEstimate[row];
    (*initialImageToReferenceEstimate)[row][2] = initialZEstimate[row];
  }

  vtkMatrix4x4::Multiply4x4(initialRotationMatrixInverse, initialImageToReferenceEstimate, this->ImageToProbeTransformation);

  vtkMatrix4x4* imageToReferenceMatrix = vtkMatrix4x4::New();
  imageToReferenceMatrix->Identity();

  vnl_matrix<double> uEstimate(3, this->TrackedFrameList->GetNumberOfTrackedFrames());
  vnl_matrix<double> vEstimate(3, this->TrackedFrameList->GetNumberOfTrackedFrames());
  vnl_matrix<double> ni(this->TrackedFrameList->GetNumberOfTrackedFrames(), 3);
  vnl_vector<double> Ti(3);
  vnl_vector<double> Ui(3);
  vnl_vector<double> Vi(3);
  vnl_matrix<double> ci(this->TrackedFrameList->GetNumberOfTrackedFrames(), 1);
  double Sx( this->MmPerPixelX );
  double Sy( this->MmPerPixelY );

  for( int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++imageIndex )
  {
    TrackedFrame* aFrame = this->TrackedFrameList->GetTrackedFrame(imageIndex);
    PlusTransformName probeToReferenceName(this->GetProbeCoordinateFrame(), this->GetReferenceCoordinateFrame());
    if( aFrame->GetCustomFrameTransform(probeToReferenceName, aProbeToReferenceTransform) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable to locate ProbeToReferenceTransform in frame " << imageIndex << " of tracked frame list.");
      return PLUS_FAIL;
    }
    (*aProbeToReferenceTransform)[3][3] = 1.0;
    (*aProbeToReferenceTransform)[3][0] = 0.0;
    (*aProbeToReferenceTransform)[3][1] = 0.0;
    (*aProbeToReferenceTransform)[3][2] = 0.0;
    vtkMatrix4x4::Multiply4x4(this->ImageToProbeTransformation, aProbeToReferenceTransform, imageToReferenceMatrix);

    uEstimate[0][imageIndex] = (*imageToReferenceMatrix)[0][0];
    uEstimate[1][imageIndex] = (*imageToReferenceMatrix)[1][0];
    uEstimate[2][imageIndex] = (*imageToReferenceMatrix)[2][0]; 
    vEstimate[0][imageIndex] = (*imageToReferenceMatrix)[0][1];
    vEstimate[1][imageIndex] = (*imageToReferenceMatrix)[1][1];
    vEstimate[2][imageIndex] = (*imageToReferenceMatrix)[2][1];

    ni[imageIndex][0] = nEstimate[0] * (*aProbeToReferenceTransform)[0][0] + 
      nEstimate[1] * (*aProbeToReferenceTransform)[1][0] + 
      nEstimate[2] * (*aProbeToReferenceTransform)[2][0];
    ni[imageIndex][1] = nEstimate[0] * (*aProbeToReferenceTransform)[0][1] + 
      nEstimate[1] * (*aProbeToReferenceTransform)[1][1] + 
      nEstimate[2] * (*aProbeToReferenceTransform)[2][1];
    ni[imageIndex][2] = nEstimate[0] * (*aProbeToReferenceTransform)[0][2] + 
      nEstimate[1] * (*aProbeToReferenceTransform)[1][2] + 
      nEstimate[2] * (*aProbeToReferenceTransform)[2][2];

    Ti[0] = (*aProbeToReferenceTransform)[0][3]; 
    Ti[1] = (*aProbeToReferenceTransform)[1][3]; 
    Ti[2] = (*aProbeToReferenceTransform)[2][3];
    Ui[0] = uEstimate[0][imageIndex];
    Ui[1] = uEstimate[1][imageIndex];
    Ui[2] = uEstimate[2][imageIndex];
    Vi[0] = vEstimate[0][imageIndex];
    Vi[1] = vEstimate[1][imageIndex];
    Vi[2] = vEstimate[2][imageIndex];

    ci[imageIndex][0] = dot_product(Ti, nEstimate) + 
      Sx * lineParameters[imageIndex].lineOriginPoint_Image[0] * dot_product(Ui, nEstimate) + 
      Sy * lineParameters[imageIndex].lineOriginPoint_Image[1] * dot_product(Vi, nEstimate);
  }

  vnl_matrix<double> g(this->TrackedFrameList->GetNumberOfTrackedFrames(), 4);

  for (int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); imageIndex++)
  {
    for(int j=0; j < 4; j++)
    {
      g[imageIndex][j] = ni[imageIndex][j];
    }
    g[imageIndex][3] = -1;
  }

  vnl_qr<double> q2(g.transpose() * g);
  vnl_matrix<double> invGTG = q2.inverse();
  vnl_matrix<double> estimate = -invGTG * g.transpose();
  estimate = estimate * ci;

  // Not sure if these will be needed later
  //LOG_INFO("Estimated n: " << nEstimate[0] << ", " << nEstimate[1] << ", " << nEstimate[2] );
  //LOG_INFO("Estimated d (parameter of the plane): " << -estimate[3][0] );

  (*this->ImageToProbeTransformation)[0][3] = estimate[0][0];
  (*this->ImageToProbeTransformation)[1][3] = estimate[1][0];
  (*this->ImageToProbeTransformation)[2][3] = estimate[2][0];

  LOG_INFO("Ending single wall image to probe calculation with " << this->TrackedFrameList->GetNumberOfTrackedFrames() << " images.");

  // Clean up interim matrices
  initialRotationMatrixInverse->Delete();
  for( int imageIndex = 0; imageIndex < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++imageIndex )
  {
    relativeRotationFromInitialPosition[imageIndex]->Delete();
  }
  initialImageToReferenceEstimate->Delete();
  aProbeToReferenceTransform->Delete();

  this->UpToDate = true;

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------

PlusStatus vtkSingleWallCalibrationAlgo::Validate() const
{
  if( this->TrackedFrameList == NULL )
  {
    LOG_ERROR("Tracked frame list not initialized.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------

const vtkMatrix4x4& vtkSingleWallCalibrationAlgo::GetImageToProbeTransformation() const
{
  return *ImageToProbeTransformation;
}

//------------------------------------------------------------------------

PlusStatus vtkSingleWallCalibrationAlgo::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  if( aConfig == NULL )
  {
    LOG_ERROR("Null configuration sent to vtkSingleWallCalibrationAlgo::ReadConfiguration.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* singleWallElement = aConfig->FindNestedElementWithName("vtkSingleWallCalibrationAlgo");
  if( singleWallElement == NULL )
  {
    LOG_ERROR("Unable to find \'vtkSingleWallCalibrationAlgo\' element. Cannot proceed with calibration.");
    return PLUS_FAIL;
  }

  if( LineSegmenter->ReadConfiguration(aConfig) != PLUS_SUCCESS )
  {
    LOG_WARNING("Unable to perform line segmentation configuration. Defaults will be used.");
  }

  double mmPerPixelX(1.0);
  if( !singleWallElement->GetScalarAttribute("MmPerPixelInX", mmPerPixelX) )
  {
    LOG_WARNING("No attribute \'MmPerPixelInX\' defined in \'vtkSingleWallCalibrationAlgo\' element. Defaulting to " << mmPerPixelX )
  }
  this->MmPerPixelX = mmPerPixelX;

  double mmPerPixelY(1.0);
  if( !singleWallElement->GetScalarAttribute("MmPerPixelInY", mmPerPixelY) )
  {
    LOG_WARNING("No attribute \'MmPerPixelInY\' defined in \'vtkSingleWallCalibrationAlgo\' element. Defaulting to " << mmPerPixelY )
  }
  this->MmPerPixelY = mmPerPixelY;

  // Image coordinate frame name
  const char* imageCoordinateFrame = singleWallElement->GetAttribute("ImageCoordinateFrame");
  if (imageCoordinateFrame == NULL)
  {
    LOG_ERROR("ImageCoordinateFrame is not specified in vtkSingleWallCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetImageCoordinateFrame(imageCoordinateFrame);

  // Probe coordinate frame name
  const char* probeCoordinateFrame = singleWallElement->GetAttribute("ProbeCoordinateFrame");
  if (probeCoordinateFrame == NULL)
  {
    LOG_ERROR("ProbeCoordinateFrame is not specified in vtkSingleWallCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetProbeCoordinateFrame(probeCoordinateFrame);

  // Reference coordinate frame name
  const char* referenceCoordinateFrame = singleWallElement->GetAttribute("ReferenceCoordinateFrame");
  if (referenceCoordinateFrame == NULL)
  {
    LOG_ERROR("ReferenceCoordinateFrame is not specified in vtkSingleWallCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetReferenceCoordinateFrame(referenceCoordinateFrame);

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------

void vtkSingleWallCalibrationAlgo::SetTrackedFrameList( vtkTrackedFrameList* aTrackedFrameList )
{
  this->UpToDate = false;
  this->TrackedFrameList = aTrackedFrameList;
  this->LineSegmenter->SetTrackedFrameList(this->TrackedFrameList);
}

//------------------------------------------------------------------------

void vtkSingleWallCalibrationAlgo::CleanMatrix( vtkMatrix4x4* aMatrix )
{
  for( int row = 0; row < 4; ++row )
  {
    for( int col = 0; col < 4; ++col )
    {
      if( (*aMatrix)[row][col] < 0.0000001 )
      {
        aMatrix->SetElement(row, col, 0.0);
      }
    }
  }
}

//-----------------------------------------------------------------------------

std::string vtkSingleWallCalibrationAlgo::GetResultString(int precision/* = 3*/)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::GetResultString");

  std::ostringstream matrixStringStream;
  matrixStringStream << "Image to probe transform:" << std::endl;

  // Print matrix rows
  vnl_matrix_fixed<double, 4, 4> aMatrix;
  for( int row = 0; row < 4; ++row)
  {
    for( int col = 0; col < 4; ++col)
    {
      aMatrix[row][col] = (*this->ImageToProbeTransformation)[row][col];
    }
  }
  
  PlusMath::PrintMatrix(aMatrix, matrixStringStream, precision+2);
  matrixStringStream << std::endl;

  double pixelSizeX = aMatrix.get_column(0).magnitude();
  double pixelSizeY = aMatrix.get_column(1).magnitude();
  matrixStringStream << "Image pixel size (mm):" << std::endl;
  matrixStringStream << "  " << std::fixed << std::setprecision(precision+1) << pixelSizeX << " x " << pixelSizeY << std::endl;

  return matrixStringStream.str();
}
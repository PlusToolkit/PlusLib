/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "float.h"
#include "vtkProbeCalibrationAlgo.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransformRepository.h"

#include "PlusMath.h"
#include "FidPatternRecognitionCommon.h"

#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkPlane.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"

#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"
#include "vtkPoints.h"

static const int MIN_NUMBER_OF_VALID_CALIBRATION_FRAMES=1; // minimum number of successfully calibrated frames required for calibration

vtkCxxRevisionMacro(vtkProbeCalibrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkProbeCalibrationAlgo);

//----------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
vtkProbeCalibrationAlgo::vtkProbeCalibrationAlgo() 
{
  this->CalibrationDate = NULL;
  this->ImageCoordinateFrame = NULL;
  this->ProbeCoordinateFrame = NULL;
  this->PhantomCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;

  vtkSmartPointer<vtkMatrix4x4> imageToProbeTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  this->ImageToProbeTransformMatrix = NULL;
  this->SetImageToProbeTransformMatrix(imageToProbeTransformMatrix); 

  this->DataPositionsInImageFrame.clear();
  this->DataPositionsInProbeFrame.clear();

  this->ErrorConfidenceLevel = 0.95;

  this->ValidationMiddleWirePositionsInPhantomFrame.clear();
  this->ValidationReprojectionError3Ds.clear();
  this->SortedValidationReprojectionError3DIndices.clear();
  this->ValidationReprojectionError3DMean = -1.0;
  this->ValidationReprojectionError3DStdDev = -1.0;

  this->CalibrationMiddleWirePositionsInPhantomFrame.clear();
  this->CalibrationReprojectionError3Ds.clear();
  this->SortedCalibrationReprojectionError3DIndices.clear();
  this->CalibrationReprojectionError3DMean = -1.0;
  this->CalibrationReprojectionError3DStdDev = -1.0;

  this->ValidationReprojectionError2Ds.clear();
  this->SortedValidationReprojectionError2DIndices.clear();
  this->ValidationReprojectionError2DMeans.clear();
  this->ValidationReprojectionError2DStdDevs.clear();

  this->CalibrationReprojectionError2Ds.clear();
  this->SortedCalibrationReprojectionError2DIndices.clear();
  this->CalibrationReprojectionError2DMeans.clear();
  this->CalibrationReprojectionError2DStdDevs.clear();

  this->NWires.clear();

  this->SpatialCalibrationOptimizer = NULL;
  this->SpatialCalibrationOptimizer = vtkSpatialCalibrationOptimizer::New();
}

//----------------------------------------------------------------------------
vtkProbeCalibrationAlgo::~vtkProbeCalibrationAlgo() 
{
  this->SetImageToProbeTransformMatrix(NULL);

  if (this->SpatialCalibrationOptimizer)
  {
    this->SpatialCalibrationOptimizer->Delete();
    this->SpatialCalibrationOptimizer = NULL;
  }
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::ReadConfiguration"); 
  if ( aConfig == NULL )
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  // vtkPivotCalibrationAlgo section
  vtkXMLDataElement* probeCalibrationElement = aConfig->FindNestedElementWithName("vtkProbeCalibrationAlgo"); 

  if (probeCalibrationElement == NULL)
  {
    LOG_ERROR("Unable to find vtkProbeCalibrationAlgo element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Image coordinate frame name
  const char* imageCoordinateFrame = probeCalibrationElement->GetAttribute("ImageCoordinateFrame");
  if (imageCoordinateFrame == NULL)
  {
    LOG_ERROR("ImageCoordinateFrame is not specified in vtkProbeCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetImageCoordinateFrame(imageCoordinateFrame);

  // Probe coordinate frame name
  const char* probeCoordinateFrame = probeCalibrationElement->GetAttribute("ProbeCoordinateFrame");
  if (probeCoordinateFrame == NULL)
  {
    LOG_ERROR("ProbeCoordinateFrame is not specified in vtkProbeCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetProbeCoordinateFrame(probeCoordinateFrame);

  // Phantom coordinate frame name
  const char* phantomCoordinateFrame = probeCalibrationElement->GetAttribute("PhantomCoordinateFrame");
  if (phantomCoordinateFrame == NULL)
  {
    LOG_ERROR("PhantomCoordinateFrame is not specified in vtkProbeCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetPhantomCoordinateFrame(phantomCoordinateFrame);

  // Reference coordinate frame name
  const char* referenceCoordinateFrame = probeCalibrationElement->GetAttribute("ReferenceCoordinateFrame");
  if (referenceCoordinateFrame == NULL)
  {
    LOG_ERROR("ReferenceCoordinateFrame is not specified in vtkProbeCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetReferenceCoordinateFrame(referenceCoordinateFrame);

  
  // optimization options

  // vtkSpatialCalibrationOptimizer section
  vtkXMLDataElement* spatialCalibrationOptimizerElement = aConfig->FindNestedElementWithName("vtkSpatialCalibrationOptimizer");   
  
  if (spatialCalibrationOptimizerElement != NULL)
  {
    if (this->SpatialCalibrationOptimizer->ReadConfiguration(aConfig) != PLUS_SUCCESS)
    {
      LOG_ERROR("vtkSpatialCalibrationOptimizer is not well specified in vtkSpatialCalibrationOptimizer element of the configuration!");
      return PLUS_FAIL;     
    }   
  }



  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::Calibrate( vtkTrackedFrameList* validationTrackedFrameList, vtkTrackedFrameList* calibrationTrackedFrameList, vtkTransformRepository* transformRepository, const std::vector<NWire> &nWires )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::Calibrate");

  return Calibrate(validationTrackedFrameList, -1, -1, calibrationTrackedFrameList, -1, -1, transformRepository, nWires);
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::Calibrate( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, vtkTransformRepository* transformRepository, const std::vector<NWire> &nWires )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::Calibrate(validation: " << validationStartFrame << "-" << validationEndFrame << ", calibration: " << calibrationStartFrame << "-" << calibrationEndFrame << ")"); 

  // Set range boundaries
  if (validationStartFrame < 0)
  {
    validationStartFrame = 0;
  }

  // Check if TrackedFrameLists are MF oriented BRIGHTNESS images
  if (vtkTrackedFrameList::VerifyProperties(validationTrackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to perform calibration - validation tracked frame list is invalid"); 
    return PLUS_FAIL; 
  }
  if (vtkTrackedFrameList::VerifyProperties(calibrationTrackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to perform calibration - calibration tracked frame list is invalid"); 
    return PLUS_FAIL; 
  }

  int numberOfValidationFrames = validationTrackedFrameList->GetNumberOfTrackedFrames(); 
  if (validationEndFrame < 0 || validationEndFrame >= numberOfValidationFrames)
  {
    validationEndFrame = numberOfValidationFrames;
  }

  if (calibrationStartFrame < 0)
  {
    calibrationStartFrame = 0;
  }

  int numberOfCalibrationFrames = calibrationTrackedFrameList->GetNumberOfTrackedFrames(); 
  if (calibrationEndFrame < 0 || calibrationEndFrame >= numberOfCalibrationFrames)
  {
    calibrationEndFrame = numberOfCalibrationFrames;
  }

  this->NWires = nWires;

  this->DataPositionsInImageFrame.clear();
  this->DataPositionsInProbeFrame.clear();

  this->SegmentedPointsInImageFrame.clear();
  this->ProbeToPhantomTransforms.clear();

  this->ValidationMiddleWirePositionsInPhantomFrame.clear();
  this->CalibrationMiddleWirePositionsInPhantomFrame.clear();

  // Add tracked frames for calibration and validation
  for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    LOG_DEBUG("\n----------------------------------");
    LOG_DEBUG("Add frame #" << frameNumber << " for validation data");
    if ( AddPositionsPerImage(validationTrackedFrameList->GetTrackedFrame(frameNumber), transformRepository, true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add validation position failed on frame #" << frameNumber);
      continue;
    }
  }    

  for (int frameNumber = calibrationStartFrame; frameNumber < calibrationEndFrame; ++frameNumber)
  {
    LOG_DEBUG("\n----------------------------------");
    LOG_DEBUG("Add frame #" << frameNumber << " for calibration data");
    if ( AddPositionsPerImage(calibrationTrackedFrameList->GetTrackedFrame(frameNumber), transformRepository, false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add calibration position failed on frame #" << frameNumber);
      continue;
    }
  }

  if ( this->DataPositionsInImageFrame.empty() )
  {
    LOG_ERROR("Unable to perform calibration - calibration data is empty!"); 
    return PLUS_FAIL; 
  }

  // Do calibration for all dimensions and assemble output matrix
  const int m = this->DataPositionsInImageFrame.size();
  const int n = this->DataPositionsInImageFrame.begin()->size();

  // If we attempt to run least square with not enough points then vnl_lsqr crashes. Return with an error if there are very few frames to avoid crashing.
  if ( m/this->NWires.size() < MIN_NUMBER_OF_VALID_CALIBRATION_FRAMES)
  {
    LOG_ERROR("Unable to perform calibration - there are " << m/this->NWires.size() << " frames with segmented points and minimum " << MIN_NUMBER_OF_VALID_CALIBRATION_FRAMES << " frames are needed"); 
    return PLUS_FAIL; 
  }

  vnl_matrix<double> imageToProbeTransformMatrixVnl;
  imageToProbeTransformMatrixVnl.set_size(n, n);
  imageToProbeTransformMatrixVnl.fill(0);
  std::set<int> outliers;
  for (int row = 0; row < n; ++row)
  {
    std::vector<double> probePositionRowVector(m,0);
    vnl_vector<double> nonOutliers(m);
    for (int i=0; i < m; ++i)
    {
      probePositionRowVector[i] = this->DataPositionsInProbeFrame[i][row];
      nonOutliers.put(i,i);
    }

    vnl_vector<double> resultVector(n,0);
    if ( PlusMath::LSQRMinimize(this->DataPositionsInImageFrame, probePositionRowVector, resultVector, NULL, NULL, &nonOutliers) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to run LSQRMinimize!"); 
      return PLUS_FAIL;
    }


    for (int i=0;i<m;i++)
    {
      bool found = false;
      int index=0;
      while(!found && (index<=nonOutliers.size()))
      {
        found= i==nonOutliers.get(index++);
      }
      if(!found)
      {
        outliers.insert(i);
      }
    }

    imageToProbeTransformMatrixVnl.set_row(row, resultVector);
  }

  LOG_INFO(outliers.size() << " outliers points were found");
  // Validate calibration result and set it to member variable and transform repository
  SetAndValidateImageToProbeTransform( imageToProbeTransformMatrixVnl, transformRepository, true );

  switch (this->SpatialCalibrationOptimizer->CurrentImageToProbeCalibrationOptimizationMethod)
  {
  case vtkSpatialCalibrationOptimizer::NO_OPTIMIZATION:
    break;

  default:

    // Convert the transform to vnl
    PlusMath::ConvertVtkMatrixToVnlMatrix(this->ImageToProbeTransformMatrix,imageToProbeTransformMatrixVnl);

    switch (this->SpatialCalibrationOptimizer->CurrentImageToProbeCalibrationCostFunction)
    {
    case vtkSpatialCalibrationOptimizer::MINIMIZATION_2D:
      this->SpatialCalibrationOptimizer->SetOptimizerDataUsingNWires(&this->SegmentedPointsInImageFrame,&this->NWires,&this->ProbeToPhantomTransforms,&imageToProbeTransformMatrixVnl,&outliers);
      break;
    case vtkSpatialCalibrationOptimizer::MINIMIZATION_3D:
      this->SpatialCalibrationOptimizer->SetInputDataForMiddlePointMethod(&this->DataPositionsInImageFrame,&this->DataPositionsInProbeFrame,&imageToProbeTransformMatrixVnl,&outliers);
      break;
    }

    switch (this->SpatialCalibrationOptimizer->CurrentImageToProbeCalibrationOptimizationMethod)
    {
    case vtkSpatialCalibrationOptimizer::FIDUCIALS_SIMILARITY:
      this->SpatialCalibrationOptimizer->SetInputDataForMiddlePointMethod(&this->DataPositionsInImageFrame,&this->DataPositionsInProbeFrame,&imageToProbeTransformMatrixVnl,&outliers);
      break;
    }

    this->SpatialCalibrationOptimizer->Update();
    imageToProbeTransformMatrixVnl = this->SpatialCalibrationOptimizer->GetOptimizedImageToProbeTransformMatrix();
    SetAndValidateImageToProbeTransform( imageToProbeTransformMatrixVnl, transformRepository, false );
    break;
  }




  // Log the calibration result and error
  LOG_INFO("Image to probe transform matrix = ");
  PlusMath::LogVtkMatrix(this->ImageToProbeTransformMatrix, 6);

  // Compute 3D reprojection errors
  if ( ComputeReprojectionErrors3D(validationTrackedFrameList, validationStartFrame, validationEndFrame, transformRepository, true) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute validation 3D reprojection errors!");
    return PLUS_FAIL;
  }

  PlusTransformName imageToProbeTransformName(this->ImageCoordinateFrame, this->ProbeCoordinateFrame);
  transformRepository->SetTransformError(imageToProbeTransformName, this->ValidationReprojectionError3DMean);
  LOG_INFO("Validation 3D Reprojection Error - Mean: " << this->ValidationReprojectionError3DMean << "mm, StDdev: " << this->ValidationReprojectionError3DStdDev << "mm");

  if ( ComputeReprojectionErrors3D(calibrationTrackedFrameList, calibrationStartFrame, calibrationEndFrame, transformRepository, false) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute calibration 3D reprojection errors!");
    return PLUS_FAIL;
  }

  LOG_INFO("Calibration 3D Reprojection Error - Mean: " << this->CalibrationReprojectionError3DMean << "mm, StDdev: " << this->CalibrationReprojectionError3DStdDev << "mm");

  // Compute 2D reprojection errors
  if ( ComputeReprojectionErrors2D(validationTrackedFrameList, validationStartFrame, validationEndFrame, transformRepository, true) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute validation 2D reprojection errors!");
    return PLUS_FAIL;
  }
  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    LOG_INFO("Validation 2D Reprojection Error for wire #" << i << ":" << this->NWires[i/3].Wires[i%3].Name << " - Mean: (" << this->ValidationReprojectionError2DMeans[i][0] << "px, " << this->ValidationReprojectionError2DMeans[i][1] << "px), StdDev: (" << this->ValidationReprojectionError2DStdDevs[i][0] << "px, " << this->ValidationReprojectionError2DStdDevs[i][1] << "px)");
  }

  if ( ComputeReprojectionErrors2D(calibrationTrackedFrameList, calibrationStartFrame, calibrationEndFrame, transformRepository, false) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute calibration 2D reprojection errors!");
    return PLUS_FAIL;
  }
  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    LOG_INFO("Calibration 2D Reprojection Error for wire #" << i << ":" << this->NWires[i/3].Wires[i%3].Name << " - Mean: (" << this->CalibrationReprojectionError2DMeans[i][0] << "px, " << this->CalibrationReprojectionError2DMeans[i][1] << "px), StdDev: (" << this->CalibrationReprojectionError2DStdDevs[i][0] << "px, " << this->CalibrationReprojectionError2DStdDevs[i][1] << "px)");
  }

  // Save the calibration results and error reports into a file 
  if ( SaveCalibrationResultAndErrorReportToXML(validationTrackedFrameList, validationStartFrame, validationEndFrame, calibrationTrackedFrameList, calibrationStartFrame, calibrationEndFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save report!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::AddPositionsPerImage( TrackedFrame* trackedFrame, vtkTransformRepository* transformRepository, bool isValidation )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::AddPositionsPerImage(" << (isValidation?"validation":"calibration") << ")");

  // Get segmentation points and check its validity
  vtkPoints* segmentedPointsVtk = trackedFrame->GetFiducialPointsCoordinatePx();

  if (segmentedPointsVtk == NULL)
  {
    LOG_WARNING("Segmentation has not been run on frame!");
    return PLUS_FAIL;
  }
  if (segmentedPointsVtk->GetNumberOfPoints() == 0)
  {
    LOG_DEBUG("Segmentation failed on frame, so it will be ignored");
    return PLUS_SUCCESS;
  }
  if (segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
  {
    LOG_ERROR("Frame does not contain N-Wires only!");
    return PLUS_FAIL;
  }

  bool valid = false;

  // Assemble matrices and add them to the calibration input
  PlusTransformName probeToReferenceTransformName(this->ProbeCoordinateFrame, this->ReferenceCoordinateFrame);
  vtkSmartPointer<vtkMatrix4x4> probeToReferenceVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  transformRepository->SetTransforms(*trackedFrame); 
  if ( (transformRepository->GetTransform(probeToReferenceTransformName, probeToReferenceVtkTransformMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
  {
    std::string transformName; 
    probeToReferenceTransformName.GetTransformName(transformName); 
    LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
    return PLUS_FAIL;
  }

  // Convert segmented points to vnl
  std::vector<vnl_vector<double> > segmentedPoints;
  for (int i=0; i<segmentedPointsVtk->GetNumberOfPoints(); i++)
  {
    vnl_vector<double> vnlPoint(4,0);
    double point[3];
    segmentedPointsVtk->GetPoint(i, point);
    vnlPoint[0] = point[0];
    vnlPoint[1] = point[1];
    vnlPoint[2] = 0.0;
    vnlPoint[3] = 1.0;
    segmentedPoints.push_back(vnlPoint);
  }

  // Get phantom registration matrix and convert it to vnl
  PlusTransformName phantomToReferenceTransformName(this->PhantomCoordinateFrame, this->ReferenceCoordinateFrame);
  vtkSmartPointer<vtkMatrix4x4> phantomToReferenceTransformMatrixVtk = vtkSmartPointer<vtkMatrix4x4>::New();
  if ( (transformRepository->GetTransform(phantomToReferenceTransformName, phantomToReferenceTransformMatrixVtk, &valid) != PLUS_SUCCESS) || (!valid) )
  {
    LOG_ERROR("No valid transform found from phantom to reference");
    return PLUS_FAIL;
  }

  vnl_matrix<double> phantomToReferenceTransformMatrix(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(phantomToReferenceTransformMatrixVtk, phantomToReferenceTransformMatrix);

  // Get reference to probe transform in vnl
  vnl_matrix<double> probeToReferenceTransformMatrix(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(probeToReferenceVtkTransformMatrix, probeToReferenceTransformMatrix); 

  vnl_matrix_inverse<double> inverseMatrix(probeToReferenceTransformMatrix);
  vnl_matrix<double> referenceToProbeTransformMatrix = inverseMatrix.inverse();

  // Make sure the last row in homogeneous transform is [0 0 0 1]
  vnl_vector<double> lastRow(4,0);
  lastRow.put(3, 1);
  referenceToProbeTransformMatrix.set_row(3, lastRow);
  LOG_DEBUG("Reference to probe transform = \n" << referenceToProbeTransformMatrix);

  std::vector< vnl_vector<double> > middleWirePositionsInPhantomFramePerImage;

  if (!isValidation)
  {
      // Store all the probe to phantom transforms, used only in 2D minimization
      PlusTransformName probeToPhantomTransformName(this->ProbeCoordinateFrame, this->PhantomCoordinateFrame);
      vtkSmartPointer<vtkMatrix4x4> probeToPhantomVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

      if ( (transformRepository->GetTransform(probeToPhantomTransformName, probeToPhantomVtkTransformMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
      {
        std::string transformName; 
        probeToPhantomTransformName.GetTransformName(transformName); 
        LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
        return PLUS_FAIL;
      }
      // Get  probe to phantome transform in vnl
      vnl_matrix<double> probeToPhantomTransformMatrix(4,4);
      PlusMath::ConvertVtkMatrixToVnlMatrix(probeToPhantomVtkTransformMatrix, probeToPhantomTransformMatrix); 
      this->ProbeToPhantomTransforms.push_back(probeToPhantomTransformMatrix);
  
  }
 
  // Calculate wire position in probe coordinate system using the segmentation result and the phantom geometry
  for (int n = 0; n < this->NWires.size(); ++n)
  {
    // Calcuate the alpha value
    vnl_vector<double> vectorCi2Xi = segmentedPoints[n*3 + 1] - segmentedPoints[n*3];
    vnl_vector<double> vectorCi2Cii = segmentedPoints[n*3 + 2] - segmentedPoints[n*3];
    double alpha = (double)vectorCi2Xi.magnitude() / vectorCi2Cii.magnitude();

    // Compute middle point position in phantom frame using alpha and the imaginary intersection points
    vnl_vector<double> positionInPhantomFrame(4);
    vnl_vector<double> intersectPosW12(4);
    vnl_vector<double> intersectPosW32(4);

    for (int i=0; i<3; ++i)
    {
      intersectPosW12[i] = this->NWires[n].IntersectPosW12[i];
      intersectPosW32[i] = this->NWires[n].IntersectPosW32[i];
    }

    intersectPosW12[3] = 1.0;
    intersectPosW32[3] = 1.0;

    positionInPhantomFrame = intersectPosW12 + alpha * ( intersectPosW32 - intersectPosW12 );
    positionInPhantomFrame[3] = 1.0;

    middleWirePositionsInPhantomFramePerImage.push_back( positionInPhantomFrame );

    LOG_DEBUG("NWire #" << n);
    LOG_DEBUG("  Segmented point #" << n*3 << " = " << segmentedPoints[n*3]);
    LOG_DEBUG("  Segmented point #" << n*3+1 << " = " << segmentedPoints[n*3+1] << " (middle wire)");
    LOG_DEBUG("  Segmented point #" << n*3+2 << " = " << segmentedPoints[n*3+2]);
    LOG_DEBUG("  Alpha = " << alpha);
    LOG_DEBUG("  Middle wire position in phantom frame = " << positionInPhantomFrame);

    if (!isValidation)
    {
      // Compute middle point position in probe frame
      vnl_vector<double> positionInProbeFrame = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix * positionInPhantomFrame;

      LOG_DEBUG("  Middle wire position in probe frame = " << positionInProbeFrame);

      // Store into the list of positions in the image frame and the probe frame
      this->DataPositionsInImageFrame.push_back( segmentedPoints[n*3+1] );
      this->DataPositionsInProbeFrame.push_back( positionInProbeFrame );


	     // Store all the segmented points, used only in 2D minimization
       this->SegmentedPointsInImageFrame.push_back(segmentedPoints[n*3]);
       this->SegmentedPointsInImageFrame.push_back(segmentedPoints[n*3+1]);
       this->SegmentedPointsInImageFrame.push_back(segmentedPoints[n*3+2]);

    }
  }

  if (isValidation)
  {
    this->ValidationMiddleWirePositionsInPhantomFrame.push_back( middleWirePositionsInPhantomFramePerImage );
  }
  else
  {
    this->CalibrationMiddleWirePositionsInPhantomFrame.push_back( middleWirePositionsInPhantomFramePerImage );
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkProbeCalibrationAlgo::SetAndValidateImageToProbeTransform( const vnl_matrix<double> &imageToProbeTransformMatrixVnl, vtkTransformRepository* transformRepository, bool ensureOrthogonal )
{
  // Convert transform to vtk
  vtkSmartPointer<vtkMatrix4x4> imageToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  for ( int i = 0; i < 3; i++ )
  {
    for ( int j = 0; j < 4; j++ )
    {
      imageToProbeMatrix->SetElement(i, j, imageToProbeTransformMatrixVnl.get(i, j) ); 
    }
  }

  // Make sure the last row in homogeneous transform is [0 0 0 1]
  imageToProbeMatrix->SetElement(3, 0, 0.0);
  imageToProbeMatrix->SetElement(3, 1, 0.0);
  imageToProbeMatrix->SetElement(3, 2, 0.0);
  imageToProbeMatrix->SetElement(3, 3, 1.0);

  // Check orthogonality
  if ( ! IsImageToProbeTransformOrthogonal() )
  {
    LOG_WARNING("ImageToProbeTransform is not orthogonal! The result is probably not satisfactory");
  }

  if( ensureOrthogonal )
  {
    // Make the z vector have about the same length as x an y, so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
    double xVector[3] = {imageToProbeMatrix->GetElement(0,0),imageToProbeMatrix->GetElement(1,0),imageToProbeMatrix->GetElement(2,0)}; 
    double yVector[3] = {imageToProbeMatrix->GetElement(0,1),imageToProbeMatrix->GetElement(1,1),imageToProbeMatrix->GetElement(2,1)};  
    double zVector[3] = {0,0,0}; 

    vtkMath::Cross(xVector, yVector, zVector); 

    vtkMath::Normalize(zVector);
    double normZ = (vtkMath::Norm(xVector)+vtkMath::Norm(yVector))/2;  
    vtkMath::MultiplyScalar(zVector, normZ);

    imageToProbeMatrix->SetElement(0, 2, zVector[0]);
    imageToProbeMatrix->SetElement(1, 2, zVector[1]);
    imageToProbeMatrix->SetElement(2, 2, zVector[2]);
  }

  // Set result matrix
  this->ImageToProbeTransformMatrix->DeepCopy( imageToProbeMatrix );

  // Save results into transform repository
  PlusTransformName imageToProbeTransformName(this->ImageCoordinateFrame, this->ProbeCoordinateFrame);
  transformRepository->SetTransform(imageToProbeTransformName, this->ImageToProbeTransformMatrix);
  transformRepository->SetTransformPersistent(imageToProbeTransformName, true);
  transformRepository->SetTransformDate(imageToProbeTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 
}


//-----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::ComputeReprojectionErrors3D( vtkTrackedFrameList* trackedFrameList, int startFrame, int endFrame, vtkTransformRepository* transformRepository, bool isValidation )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::ComputeReprojectionErrors3D");

  // Initialize objects
  std::vector< std::vector<double> >* reprojectionError3Ds = NULL;
  std::vector< std::vector<int> >* sortedReprojectionError3DIndices = NULL;
  std::vector< std::vector< vnl_vector<double> > >* middleWirePositionsInPhantomFrame = NULL;
  double* reprojectionError3DMean = NULL;
  double* reprojectionError3DStdDev = NULL;

  if (isValidation)
  {
    reprojectionError3Ds = &(this->ValidationReprojectionError3Ds);
    sortedReprojectionError3DIndices = &(this->SortedValidationReprojectionError3DIndices);
    middleWirePositionsInPhantomFrame = &(this->ValidationMiddleWirePositionsInPhantomFrame);
    reprojectionError3DMean = &(this->ValidationReprojectionError3DMean);
    reprojectionError3DStdDev = &(this->ValidationReprojectionError3DStdDev);
  }
  else
  {
    reprojectionError3Ds = &(this->CalibrationReprojectionError3Ds);
    sortedReprojectionError3DIndices = &(this->SortedCalibrationReprojectionError3DIndices);
    middleWirePositionsInPhantomFrame = &(this->CalibrationMiddleWirePositionsInPhantomFrame);
    reprojectionError3DMean = &(this->CalibrationReprojectionError3DMean);
    reprojectionError3DStdDev = &(this->CalibrationReprojectionError3DStdDev);
  }

  for (int i=0; i<reprojectionError3Ds->size(); ++i)
  {
    reprojectionError3Ds->at(i).clear();
    sortedReprojectionError3DIndices->at(i).clear();
  }
  reprojectionError3Ds->clear();
  sortedReprojectionError3DIndices->clear();

  reprojectionError3Ds->resize( this->NWires.size() );
  sortedReprojectionError3DIndices->resize( this->NWires.size() );


  std::vector< std::vector< vnl_vector<double> > >::iterator middleWirePositionsIt = middleWirePositionsInPhantomFrame->begin();

  for (int frameNumber = startFrame; frameNumber < endFrame; ++frameNumber)
  {
    TrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(frameNumber);
    transformRepository->SetTransforms(*trackedFrame); 

    // Get segmentation points and check its validity
    vtkPoints* segmentedPointsVtk = trackedFrame->GetFiducialPointsCoordinatePx();

    if (segmentedPointsVtk == NULL || segmentedPointsVtk->GetNumberOfPoints() == 0 || segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
    {
      continue;
    }

    // Compute 3D error for each NWire
    bool valid = false;

    for (int n = 0; n < this->NWires.size(); ++n)
    {
      // Transform point to phantom frame
      double point[3];
      segmentedPointsVtk->GetPoint(3*n+1, point);
      double segmentedPointInImageFrame[4];
      segmentedPointInImageFrame[0] = point[0];
      segmentedPointInImageFrame[1] = point[1];
      segmentedPointInImageFrame[2] = 0.0;
      segmentedPointInImageFrame[3] = 1.0;

      // Assemble matrices and add them to the calibration input
      PlusTransformName imageToPhantomTransformName(this->ImageCoordinateFrame, this->PhantomCoordinateFrame);
      vtkSmartPointer<vtkMatrix4x4> imageToPhantomVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

      if ( (transformRepository->GetTransform(imageToPhantomTransformName, imageToPhantomVtkTransformMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
      {
        std::string transformName; 
        imageToPhantomTransformName.GetTransformName(transformName); 
        LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
        return PLUS_FAIL;
      }

      double segmentedPointInPhantomFrame[4];
      imageToPhantomVtkTransformMatrix->MultiplyPoint(segmentedPointInImageFrame, segmentedPointInPhantomFrame);

      double computedPointInPhantomFrame[4];
      for (int i=0; i<4; ++i)
      {
        computedPointInPhantomFrame[i] = middleWirePositionsIt->at(n).get(i);
      }

      reprojectionError3Ds->at(n).push_back( sqrt( vtkMath::Distance2BetweenPoints(segmentedPointInPhantomFrame, computedPointInPhantomFrame) ) );

    } // For all NWires

    // Only advance to the next frame element if the segmentation was successful on this frame
    ++middleWirePositionsIt;

  } // For all frames

  // Compute the sorted indices array
  std::vector< std::vector<double> > tempReprojectionError3Ds( reprojectionError3Ds->size() );
  for (int i=0; i<reprojectionError3Ds->size(); ++i)
  {
    tempReprojectionError3Ds[i] = reprojectionError3Ds->at(i);
  }

  for (int i=0; i<reprojectionError3Ds->size(); ++i)
  {
    sortedReprojectionError3DIndices->at(i).resize( reprojectionError3Ds->at(i).size() );
    std::vector<double>::iterator tempReprojectionError3DsBeginIt = tempReprojectionError3Ds[i].begin();

    for (int j=0; j<reprojectionError3Ds->at(i).size(); ++j)
    {
      std::vector<double>::iterator reprojectionError3DMinIt = std::min_element(tempReprojectionError3DsBeginIt, tempReprojectionError3Ds[i].end());
      int minIndex = (int)std::distance(tempReprojectionError3DsBeginIt,reprojectionError3DMinIt);
      (*sortedReprojectionError3DIndices)[i][j] = minIndex;
      (*reprojectionError3DMinIt) = DBL_MAX;
    }
  }

  int numberOfTopRankedData = ROUND( (double)reprojectionError3Ds->begin()->size() * this->ErrorConfidenceLevel );

  // Compute mean and standard deviation
  double sum = 0;
  for (int i=0; i<reprojectionError3Ds->size(); ++i)
  {
    for (int j=0; j<numberOfTopRankedData; ++j)
    {
      sum += reprojectionError3Ds->at(i)[ (*sortedReprojectionError3DIndices)[i][j] ];
    }
  }
  (*reprojectionError3DMean) = sum / (reprojectionError3Ds->size() * numberOfTopRankedData);
    
  double squareDiffSum = 0;
  for (int i=0; i<reprojectionError3Ds->size(); ++i)
  {
    for (int j=0; j<numberOfTopRankedData; ++j)
    {
      double diff = reprojectionError3Ds->at(i)[ (*sortedReprojectionError3DIndices)[i][j] ] - (*reprojectionError3DMean);
      squareDiffSum += diff * diff;
    }
  }
  double variance = squareDiffSum / (reprojectionError3Ds->size() * numberOfTopRankedData);
  (*reprojectionError3DStdDev) = sqrt(variance);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::ComputeReprojectionErrors2D( vtkTrackedFrameList* trackedFrameList, int startFrame, int endFrame, vtkTransformRepository* transformRepository, bool isValidation )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::ComputeReprojectionErrors2D");

  // Initialize objects
  std::vector< std::vector< std::vector<double> > >* reprojectionError2Ds = NULL;
  std::vector< std::vector<int> >* sortedReprojectionError2DIndices = NULL;
  std::vector< std::vector<double> >* reprojectionError2DMeans = NULL;
  std::vector< std::vector<double> >* reprojectionError2DStdDevs = NULL;

  if (isValidation)
  {
    reprojectionError2Ds = &(this->ValidationReprojectionError2Ds);
    sortedReprojectionError2DIndices = &(this->SortedValidationReprojectionError2DIndices);
    reprojectionError2DMeans = &(this->ValidationReprojectionError2DMeans);
    reprojectionError2DStdDevs = &(this->ValidationReprojectionError2DStdDevs);
  }
  else
  {
    reprojectionError2Ds = &(this->CalibrationReprojectionError2Ds);
    sortedReprojectionError2DIndices = &(this->SortedCalibrationReprojectionError2DIndices);
    reprojectionError2DMeans = &(this->CalibrationReprojectionError2DMeans);
    reprojectionError2DStdDevs = &(this->CalibrationReprojectionError2DStdDevs);
  }

  for (int i=0; i<reprojectionError2Ds->size(); ++i)
  {
    for (int j=0; j<reprojectionError2Ds->at(i).size(); ++j)
    {
      (*reprojectionError2Ds)[i][j].clear();
    }
    reprojectionError2Ds->at(i).clear();
    sortedReprojectionError2DIndices->at(i).clear();
    reprojectionError2DMeans->at(i).clear();
    reprojectionError2DStdDevs->at(i).clear();
  }
  reprojectionError2Ds->clear();
  sortedReprojectionError2DIndices->clear();
  reprojectionError2DMeans->clear();
  reprojectionError2DStdDevs->clear();

  reprojectionError2Ds->resize( this->NWires.size() * 3 );
  sortedReprojectionError2DIndices->resize( this->NWires.size() * 3 );
  reprojectionError2DMeans->resize( this->NWires.size() * 3 );
  reprojectionError2DStdDevs->resize( this->NWires.size() * 3 );


  bool valid = false;
  std::vector<double>allReprojection2DErrors;

  for (int frameNumber = startFrame; frameNumber < endFrame; ++frameNumber)
  {
    TrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(frameNumber);
    transformRepository->SetTransforms(*trackedFrame);

    // Get segmentation points and check its validity
    vtkPoints* segmentedPointsVtk = trackedFrame->GetFiducialPointsCoordinatePx();

    if (segmentedPointsVtk == NULL || segmentedPointsVtk->GetNumberOfPoints() == 0 || segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
    {
      continue;
    }

    // Define actual image plane in phantom frame
    PlusTransformName imageToPhantomTransformName(this->ImageCoordinateFrame, this->PhantomCoordinateFrame);
    vtkSmartPointer<vtkMatrix4x4> imageToPhantomVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if ( (transformRepository->GetTransform(imageToPhantomTransformName, imageToPhantomVtkTransformMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
    {
      std::string transformName; 
      imageToPhantomTransformName.GetTransformName(transformName); 
      LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
      return PLUS_FAIL;
    }

    vtkSmartPointer<vtkMatrix4x4> phantomToImageVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Invert(imageToPhantomVtkTransformMatrix, phantomToImageVtkTransformMatrix);

    double normalVector[3] = { 0.0, 0.0, 1.0 };
    double origin[3] = { 0.0, 0.0, 0.0 };

    // Compute 2D reprojection error for each wire
    for (int i=0; i<segmentedPointsVtk->GetNumberOfPoints(); ++i)
    {
      // Get wire endpoints in image coordinate system
      Wire wire = this->NWires[i/3].Wires[i%3];
      double wireEndPointFrontInPhantomFrame[4] = { wire.EndPointFront[0], wire.EndPointFront[1], wire.EndPointFront[2], 1.0 };
      double wireEndPointBackInPhantomFrame[4] = { wire.EndPointBack[0], wire.EndPointBack[1], wire.EndPointBack[2], 1.0 };
      double wireEndPointFrontInImageFrame[4];
      double wireEndPointBackInImageFrame[4];
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
        reprojectionError2Ds->at(i).push_back( reprojectionError2D );

        continue;
      }

      double* segmentedPositionInImagePlane = segmentedPointsVtk->GetPoint(i);

      std::vector<double> reprojectionError2D(2);
      reprojectionError2D[0] = segmentedPositionInImagePlane[0] - computedPositionInImagePlane[0];
      reprojectionError2D[1] = segmentedPositionInImagePlane[1] - computedPositionInImagePlane[1];

      reprojectionError2Ds->at(i).push_back( reprojectionError2D );
      allReprojection2DErrors.push_back(sqrt(reprojectionError2D[0]*reprojectionError2D[0] + reprojectionError2D[1] * reprojectionError2D[1]));
    }

  } // For all frames

  double totalRmsError2D =0 ,totalRmsError2DSD = 0;
  int numberOfReprojections = allReprojection2DErrors.size();
  for(int i=0;i<numberOfReprojections;i++)
  {
    totalRmsError2D += allReprojection2DErrors.at(i)*allReprojection2DErrors.at(i);
  }
  totalRmsError2D = sqrt(totalRmsError2D/numberOfReprojections);
  
	  // estimate the standar desviation
  for(int i=0;i<numberOfReprojections;i++)
  {
	  double diff = allReprojection2DErrors.at(i) - totalRmsError2D;
    totalRmsError2DSD += diff * diff;
  }
  totalRmsError2DSD = sqrt(totalRmsError2DSD/numberOfReprojections);

  if (isValidation)
  {
    this->ValidationRmsError2D = totalRmsError2D;
	LOG_INFO("\n Validation 2D rms error = " << totalRmsError2D << " pixels");
	this->ValidationRmsError2DSD = totalRmsError2DSD;
	LOG_INFO("\n Validation 2D standard desviation = " << totalRmsError2DSD << " pixels");
  }
  else
  {
	this->CalibrationRmsError2D = totalRmsError2D;
	LOG_INFO("\n Calibration 2D rms error = " << totalRmsError2D << " pixels");

    this->CalibrationRmsError2DSD = totalRmsError2DSD;
	LOG_INFO("\n Calibration 2D standard desviation = " << totalRmsError2DSD << " pixels");
  }


  // Create error vector containing the sum of squares of X and Y errors (so that the minimum can be searched)
  std::vector< std::vector<double> > tempReprojectionError2Ds( reprojectionError2Ds->size() );
  for (int i=0; i<reprojectionError2Ds->size(); ++i)
  {
    std::vector<double> tempReprojectionError2DsPerWire( reprojectionError2Ds->at(i).size() );
    for (int j=0; j<reprojectionError2Ds->at(i).size(); ++j)
    {
      tempReprojectionError2DsPerWire[j] = (*reprojectionError2Ds)[i][j][0] * (*reprojectionError2Ds)[i][j][0] + (*reprojectionError2Ds)[i][j][1] * (*reprojectionError2Ds)[i][j][1];
    }
    tempReprojectionError2Ds[i] = tempReprojectionError2DsPerWire;
  }

  // Compute the sorted indices array for each wire
  for (int i=0; i<reprojectionError2Ds->size(); ++i)
  {
    sortedReprojectionError2DIndices->at(i).resize( reprojectionError2Ds->at(i).size() );
    std::vector<double>::iterator tempReprojectionError2DsBeginIt = tempReprojectionError2Ds[i].begin();

    for (int j=0; j<reprojectionError2Ds->at(i).size(); ++j)
    {
      std::vector<double>::iterator reprojectionError2DMinIt = std::min_element(tempReprojectionError2DsBeginIt, tempReprojectionError2Ds[i].end());
      int minIndex = (int)std::distance(tempReprojectionError2DsBeginIt,reprojectionError2DMinIt);
      (*sortedReprojectionError2DIndices)[i][j] = minIndex;
      (*reprojectionError2DMinIt) = DBL_MAX;
    }
  }

  int numberOfTopRankedData = ROUND( (double)reprojectionError2Ds->begin()->size() * this->ErrorConfidenceLevel );

  // Compute statistics for each wire
  for (int i=0; i<reprojectionError2Ds->size(); ++i)
  {
    // Compute mean and standard deviation for X and Y
    for (int k=0; k<2; ++k)
    {
      double sum = 0;
      for (int j=0; j<numberOfTopRankedData; ++j)
      {
        sum += reprojectionError2Ds->at(i)[ (*sortedReprojectionError2DIndices)[i][j] ][k];
      }
      reprojectionError2DMeans->at(i).push_back( sum / numberOfTopRankedData );
      
      double squareDiffSum = 0;
      for (int j=0; j<numberOfTopRankedData; ++j)
      {
        double diff = reprojectionError2Ds->at(i)[ (*sortedReprojectionError2DIndices)[i][j] ][k] - (*reprojectionError2DMeans)[i][k];
        squareDiffSum += diff * diff;
      }
      double variance = squareDiffSum / numberOfTopRankedData;
      reprojectionError2DStdDevs->at(i).push_back( sqrt(variance) );
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::GetReprojectionError2DStatistics(double &xMean, double &yMean, double &xStdDev, double &yStdDev, int wireNumber, bool isValidation)
{
  xMean = yMean = xStdDev = yStdDev = -1.0;

  int wireIndex = wireNumber - 1; 

  if (wireIndex < 0 || wireIndex >= this->NWires.size()*3)
  {
    LOG_ERROR("Invalid wire number: " << wireNumber);
    return PLUS_FAIL;
  }

  if (isValidation)
  {
    xMean = this->ValidationReprojectionError2DMeans[wireIndex][0];
    yMean = this->ValidationReprojectionError2DMeans[wireIndex][1];
    xStdDev = this->ValidationReprojectionError2DStdDevs[wireIndex][0];
    yStdDev = this->ValidationReprojectionError2DStdDevs[wireIndex][1];
  }
  else
  {
    xMean = this->CalibrationReprojectionError2DMeans[wireIndex][0];
    yMean = this->CalibrationReprojectionError2DMeans[wireIndex][1];
    xStdDev = this->CalibrationReprojectionError2DStdDevs[wireIndex][0];
    yStdDev = this->CalibrationReprojectionError2DStdDevs[wireIndex][1];
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

std::string vtkProbeCalibrationAlgo::GetResultString(int precision/* = 3*/)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::GetResultString");

  std::ostringstream matrixStringStream;
  matrixStringStream << "Image to probe transform:" << std::endl;

  // Print matrix rows
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      matrixStringStream << std::fixed << std::setprecision(precision) << std::setw(precision+3) << std::right << this->ImageToProbeTransformMatrix->GetElement(i,j) << " ";
    }

    matrixStringStream << std::endl;
  }

  std::ostringstream errorsStringStream;

  errorsStringStream << std::fixed << std::setprecision(precision) << "3D Reprojection Error (mm)" << std::endl <<
    " Mean: " << this->ValidationReprojectionError3DMean <<
    ", StdDev: " << this->ValidationReprojectionError3DStdDev << std::endl << std::endl;

  errorsStringStream << "2D Reprojection Errors (px)" << std::endl;;
  for (int i=0; i<this->NWires.size()*3; ++i)
  {
      errorsStringStream << std::fixed << std::setprecision(precision-1) <<
      "Wire #" << i << " (" << this->NWires[i/3].Wires[i%3].Name << ")" << std::endl <<
      " M:(" << this->ValidationReprojectionError2DMeans[i][0] << "," << this->ValidationReprojectionError2DMeans[i][1] << ")" <<
      " SD:(" << this->ValidationReprojectionError2DStdDevs[i][0] << "," << this->ValidationReprojectionError2DStdDevs[i][1] << ")" << std::endl;
  }

  std::ostringstream resultStringStream;
  resultStringStream << matrixStringStream.str() << std::endl << errorsStringStream.str();

  //resultStringStream << std::endl << "Error confidence: " << (int)(this->ErrorConfidenceLevel*100) << "%";

  return resultStringStream.str();
}

//----------------------------------------------------------------------------

bool vtkProbeCalibrationAlgo::IsImageToProbeTransformOrthogonal() const
{
  LOG_TRACE("vtkProbeCalibrationAlgo::IsImageToProbeTransformOrthogonal");

  // Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
  double xVector[3] = { this->ImageToProbeTransformMatrix->GetElement(0,0), this->ImageToProbeTransformMatrix->GetElement(1,0), this->ImageToProbeTransformMatrix->GetElement(2,0) }; 
  double yVector[3] = { this->ImageToProbeTransformMatrix->GetElement(0,1), this->ImageToProbeTransformMatrix->GetElement(1,1), this->ImageToProbeTransformMatrix->GetElement(2,1) };  
  double zVector[3] = { this->ImageToProbeTransformMatrix->GetElement(0,2), this->ImageToProbeTransformMatrix->GetElement(1,2), this->ImageToProbeTransformMatrix->GetElement(2,2) };  

  double dotProductXY = vtkMath::Dot(xVector, yVector);
  double dotProductXZ = vtkMath::Dot(xVector, zVector);
  double dotProductYZ = vtkMath::Dot(yVector, zVector);

  if (dotProductXY > 0.001) 
  {
    LOG_WARNING("Calibration result axes are not orthogonal (dot product of X and Y axes is " << dotProductXY << ")");
    return false; 
  }

  if (dotProductYZ > 0.001) 
  {
    LOG_WARNING("Calibration result axes are not orthogonal (dot product of Y and Z axes is " << dotProductYZ << ")");
    return false; 
  }

  if (dotProductXZ > 0.001) 
  {
    LOG_WARNING("Calibration result axes are not orthogonal (dot product of X and Z axes is " << dotProductXZ << ")");
    return false; 
  }

  return true; 
}


//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::SaveCalibrationResultAndErrorReportToXML(vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::SaveCalibrationResultsAndErrorReportsToXML");

  std::string calibrationResultFileName = std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) + ".Calibration.results.xml";
  std::string calibrationResultFileNameWithPath = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/") + calibrationResultFileName;

  // ProbeCalibrationResult
  vtkSmartPointer<vtkXMLDataElement> probeCalibrationResult = vtkSmartPointer<vtkXMLDataElement>::New(); 
  // ProbeCalibrationResult
  probeCalibrationResult->SetName("ProbeCalibrationResult"); 
  probeCalibrationResult->SetAttribute("version", "2.0"); 

  // CalibrationFile
  vtkSmartPointer<vtkXMLDataElement> calibrationFile = vtkSmartPointer<vtkXMLDataElement>::New(); 
  calibrationFile->SetName("CalibrationFile"); 
  calibrationFile->SetAttribute("Timestamp", vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()); 
  calibrationFile->SetAttribute("FileName", calibrationResultFileName.c_str()); 

  PlusStatus status = this->GetXMLCalibrationResultAndErrorReport(validationTrackedFrameList, validationStartFrame, validationEndFrame, calibrationTrackedFrameList, calibrationStartFrame, calibrationEndFrame, probeCalibrationResult); 
  
  if ( status == PLUS_SUCCESS )
  {
    probeCalibrationResult->AddNestedElement( calibrationFile );
    PlusCommon::PrintXML(calibrationResultFileNameWithPath.c_str(), probeCalibrationResult); 
  }

  return status; 
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::GetXMLCalibrationResultAndErrorReport(vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, vtkXMLDataElement* probeCalibrationResult)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::GetXMLCalibrationResultAndErrorReport");

  std::string calibrationResultFileName = std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) + ".Calibration.results.xml";
  std::string calibrationResultFileNameWithPath = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/") + calibrationResultFileName;

  if ( probeCalibrationResult == NULL )
  {
    LOG_ERROR("Unable to get xml calibration result and error report - xml data element is NULL"); 
    return PLUS_FAIL; 
  }

  if ( validationTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get xml calibration result and error report - validationTrackedFrameList is NULL"); 
    return PLUS_FAIL; 
  }

  if ( calibrationTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get xml calibration result and error report - calibrationTrackedFrameList is NULL"); 
    return PLUS_FAIL; 
  }

  // CalibrationResults
  vtkSmartPointer<vtkXMLDataElement> calibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
  calibrationResults->SetName("CalibrationResults"); 

  // Image to Probe transform
  double imageToProbeTransformVector[16]={0}; 
  vtkMatrix4x4::DeepCopy(imageToProbeTransformVector, this->ImageToProbeTransformMatrix); 

  vtkSmartPointer<vtkXMLDataElement> imageToProbeTransformElement = vtkSmartPointer<vtkXMLDataElement>::New();
  imageToProbeTransformElement->SetName("Transform"); 
  imageToProbeTransformElement->SetAttribute("From", this->ImageCoordinateFrame); 
  imageToProbeTransformElement->SetAttribute("To", this->ProbeCoordinateFrame); 
  imageToProbeTransformElement->SetVectorAttribute("Matrix", 16, imageToProbeTransformVector); 

  calibrationResults->AddNestedElement( imageToProbeTransformElement );


  // Error report
  vtkSmartPointer<vtkXMLDataElement> errorReport = vtkSmartPointer<vtkXMLDataElement>::New(); 
  errorReport->SetName("ErrorReport");

  // ReprojectionError3D
  vtkSmartPointer<vtkXMLDataElement> reprojectionError3DStatistics = vtkSmartPointer<vtkXMLDataElement>::New(); 
  reprojectionError3DStatistics->SetName("ReprojectionError3DStatistics");
  reprojectionError3DStatistics->SetDoubleAttribute("ValidationMeanMm", this->ValidationReprojectionError3DMean); 
  reprojectionError3DStatistics->SetDoubleAttribute("ValidationStdDevMm", this->ValidationReprojectionError3DStdDev); 
  reprojectionError3DStatistics->SetDoubleAttribute("CalibrationMeanMm", this->CalibrationReprojectionError3DMean); 
  reprojectionError3DStatistics->SetDoubleAttribute("CalibrationStdDevMm", this->CalibrationReprojectionError3DStdDev); 

  // ReprojectionError2D
  vtkSmartPointer<vtkXMLDataElement> reprojectionError2DStatistics = vtkSmartPointer<vtkXMLDataElement>::New(); 
  reprojectionError2DStatistics->SetName("ReprojectionError2DStatistics");

  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    vtkSmartPointer<vtkXMLDataElement> wire = vtkSmartPointer<vtkXMLDataElement>::New(); 
    wire->SetName("Wire");
    wire->SetAttribute("Name", this->NWires[i/3].Wires[i%3].Name.c_str());

    double validationMean2D[2] = { this->ValidationReprojectionError2DMeans[i][0], this->ValidationReprojectionError2DMeans[i][1] };
    double validationStdDev2D[2] = { this->ValidationReprojectionError2DStdDevs[i][0], this->ValidationReprojectionError2DStdDevs[i][1] };
    double calibrationMean2D[2] = { this->CalibrationReprojectionError2DMeans[i][0], this->CalibrationReprojectionError2DMeans[i][1] };
    double calibrationStdDev2D[2] = { this->CalibrationReprojectionError2DStdDevs[i][0], this->CalibrationReprojectionError2DStdDevs[i][1] };
    wire->SetVectorAttribute("ValidationMeanPx", 2, validationMean2D);
    wire->SetVectorAttribute("ValidationStdDevPx", 2, validationStdDev2D);
    wire->SetVectorAttribute("CalibrationMeanPx", 2, calibrationMean2D);
    wire->SetVectorAttribute("CalibrationStdDevPx", 2, calibrationStdDev2D);

    reprojectionError2DStatistics->AddNestedElement( wire );
  }

  // ValidationData
  vtkSmartPointer<vtkXMLDataElement> validationData = vtkSmartPointer<vtkXMLDataElement>::New(); 
  validationData->SetName("ValidationData");

  int numberOfSegmentedFramesSoFar = 0;
  for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    vtkPoints* segmentedPointsVtk = validationTrackedFrameList->GetTrackedFrame(frameNumber)->GetFiducialPointsCoordinatePx();

    // Frame
    vtkSmartPointer<vtkXMLDataElement> frame = vtkSmartPointer<vtkXMLDataElement>::New(); 
    frame->SetName("Frame");
    frame->SetIntAttribute("Index", frameNumber);

    if (segmentedPointsVtk == NULL)
    {
      frame->SetAttribute("SegmentationStatus", "HasNotBeenRun");
      validationData->AddNestedElement( frame );
      continue;
    }
    if (segmentedPointsVtk->GetNumberOfPoints() == 0)
    {
      frame->SetAttribute("SegmentationStatus", "Failed");
      validationData->AddNestedElement( frame );
      continue;
    }
    if (segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
    {
      frame->SetAttribute("SegmentationStatus", "InvalidPatterns");
      validationData->AddNestedElement( frame );
      continue;
    }
    frame->SetAttribute("SegmentationStatus", "OK");

    // SegmentedPoints
    vtkSmartPointer<vtkXMLDataElement> segmentedPoints = vtkSmartPointer<vtkXMLDataElement>::New(); 
    segmentedPoints->SetName("SegmentedPoints");

    int numberOfSegmentedPoints = segmentedPointsVtk->GetNumberOfPoints();
    for (int i=0; i<numberOfSegmentedPoints; i++)
    {
      double point[3];
      segmentedPointsVtk->GetPoint(i, point);

      vtkSmartPointer<vtkXMLDataElement> pointElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      pointElement->SetName("Point");
      pointElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedPoints->AddNestedElement( pointElement );
    }

    // ReprojectionError3Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError3Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError3Ds->SetName("ReprojectionError3DList");

    for (int i = 0; i < numberOfSegmentedPoints/3; ++i)
    {
      vtkSmartPointer<vtkXMLDataElement> reprojectionError3DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError3DElement->SetName("ReprojectionError3D");
      reprojectionError3DElement->SetAttribute("WireName", this->NWires[i].Wires[1].Name.c_str());
      reprojectionError3DElement->SetDoubleAttribute("ErrorMm", this->ValidationReprojectionError3Ds[i][numberOfSegmentedFramesSoFar]);
      reprojectionError3Ds->AddNestedElement( reprojectionError3DElement );
    }

    // ReprojectionError2Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError2Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError2Ds->SetName("ReprojectionError2DList");

    for (int i = 0; i < numberOfSegmentedPoints; ++i)
    {
      double reprojectionError2D[2] = { this->ValidationReprojectionError2Ds[i][numberOfSegmentedFramesSoFar][0], this->ValidationReprojectionError2Ds[i][numberOfSegmentedFramesSoFar][1] };
      vtkSmartPointer<vtkXMLDataElement> reprojectionError2DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError2DElement->SetName("ReprojectionError2D");
      reprojectionError2DElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      reprojectionError2DElement->SetVectorAttribute("ErrorPx", 2, reprojectionError2D);
      reprojectionError2Ds->AddNestedElement( reprojectionError2DElement );
    }

    frame->AddNestedElement( segmentedPoints );
    frame->AddNestedElement( reprojectionError3Ds );
    frame->AddNestedElement( reprojectionError2Ds );

    vtkSmartPointer<vtkXMLDataElement> trackedFrame = vtkSmartPointer<vtkXMLDataElement>::New(); 
    if ( validationTrackedFrameList->GetTrackedFrame(frameNumber)->PrintToXML(trackedFrame) == PLUS_SUCCESS )
    {
      frame->AddNestedElement(trackedFrame); 
    }

    validationData->AddNestedElement( frame );

    numberOfSegmentedFramesSoFar++;
  }

  // CalibrationData
  vtkSmartPointer<vtkXMLDataElement> calibrationData = vtkSmartPointer<vtkXMLDataElement>::New(); 
  calibrationData->SetName("CalibrationData");

  numberOfSegmentedFramesSoFar = 0;
  for (int frameNumber = calibrationStartFrame; frameNumber < calibrationEndFrame; ++frameNumber)
  {
    vtkPoints* segmentedPointsVtk = calibrationTrackedFrameList->GetTrackedFrame(frameNumber)->GetFiducialPointsCoordinatePx();

    // Frame
    vtkSmartPointer<vtkXMLDataElement> frame = vtkSmartPointer<vtkXMLDataElement>::New(); 
    frame->SetName("Frame");
    frame->SetIntAttribute("Index", frameNumber);

    if (segmentedPointsVtk == NULL)
    {
      frame->SetAttribute("SegmentationStatus", "HasNotBeenRun");
      calibrationData->AddNestedElement( frame );
      continue;
    }
    if (segmentedPointsVtk->GetNumberOfPoints() == 0)
    {
      frame->SetAttribute("SegmentationStatus", "Failed");
      calibrationData->AddNestedElement( frame );
      continue;
    }
    if (segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
    {
      frame->SetAttribute("SegmentationStatus", "InvalidPatterns");
      calibrationData->AddNestedElement( frame );
      continue;
    }
    frame->SetAttribute("SegmentationStatus", "OK");

    // SegmentedPoints
    vtkSmartPointer<vtkXMLDataElement> segmentedPoints = vtkSmartPointer<vtkXMLDataElement>::New(); 
    segmentedPoints->SetName("SegmentedPoints");

    int numberOfSegmentedPoints = segmentedPointsVtk->GetNumberOfPoints();
    for (int i=0; i<numberOfSegmentedPoints; i++)
    {
      double point[3];
      segmentedPointsVtk->GetPoint(i, point);

      vtkSmartPointer<vtkXMLDataElement> pointElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      pointElement->SetName("Point");
      pointElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedPoints->AddNestedElement( pointElement );
    }

    // MiddleWire
    vtkSmartPointer<vtkXMLDataElement> middleWires = vtkSmartPointer<vtkXMLDataElement>::New(); 
    middleWires->SetName("MiddleWires");

    for (int i = numberOfSegmentedFramesSoFar*numberOfSegmentedPoints/3; i < (numberOfSegmentedFramesSoFar+1)*numberOfSegmentedPoints/3; ++i)
    {
      vtkSmartPointer<vtkXMLDataElement> middleWire = vtkSmartPointer<vtkXMLDataElement>::New(); 
      middleWire->SetName("MiddleWire");

      double middleWirePositionInImageFrame[4];
      double middleWirePositionInProbeFrame[4];
      for (int k=0; k<4; ++k)
      {
        middleWirePositionInImageFrame[k] = this->DataPositionsInImageFrame[i].get(k);
        middleWirePositionInProbeFrame[k] = this->DataPositionsInProbeFrame[i].get(k);
      }
      middleWire->SetVectorAttribute("PositionInImageFrame", 4, middleWirePositionInImageFrame);
      middleWire->SetVectorAttribute("PositionInProbeFrame", 4, middleWirePositionInProbeFrame);
      middleWires->AddNestedElement( middleWire );
    }

    // ReprojectionError3Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError3Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError3Ds->SetName("ReprojectionError3DList");

    for (int i = 0; i < numberOfSegmentedPoints/3; ++i)
    {
      vtkSmartPointer<vtkXMLDataElement> reprojectionError3DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError3DElement->SetName("ReprojectionError3D");
      reprojectionError3DElement->SetAttribute("WireName", this->NWires[i].Wires[1].Name.c_str());
      reprojectionError3DElement->SetDoubleAttribute("ErrorMm", this->CalibrationReprojectionError3Ds[i][numberOfSegmentedFramesSoFar]);
      reprojectionError3Ds->AddNestedElement( reprojectionError3DElement );
    }

    // ReprojectionError2Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError2Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError2Ds->SetName("ReprojectionError2DList");

    for (int i = 0; i < numberOfSegmentedPoints; ++i)
    {
      double reprojectionError2D[2] = { this->CalibrationReprojectionError2Ds[i][numberOfSegmentedFramesSoFar][0], this->CalibrationReprojectionError2Ds[i][numberOfSegmentedFramesSoFar][1] };
      vtkSmartPointer<vtkXMLDataElement> reprojectionError2DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError2DElement->SetName("ReprojectionError2D");
      reprojectionError2DElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      reprojectionError2DElement->SetVectorAttribute("ErrorPx", 2, reprojectionError2D);
      reprojectionError2Ds->AddNestedElement( reprojectionError2DElement );
    }

    frame->AddNestedElement( segmentedPoints );
    frame->AddNestedElement( middleWires );
    frame->AddNestedElement( reprojectionError3Ds );
    frame->AddNestedElement( reprojectionError2Ds );

    vtkSmartPointer<vtkXMLDataElement> trackedFrame = vtkSmartPointer<vtkXMLDataElement>::New(); 
    if ( calibrationTrackedFrameList->GetTrackedFrame(frameNumber)->PrintToXML(trackedFrame) == PLUS_SUCCESS )
    {
      frame->AddNestedElement(trackedFrame); 
    }

    calibrationData->AddNestedElement( frame );

    numberOfSegmentedFramesSoFar++;
  }

  errorReport->AddNestedElement( reprojectionError3DStatistics );
  errorReport->AddNestedElement( reprojectionError2DStatistics );
  errorReport->AddNestedElement( validationData );
  errorReport->AddNestedElement( calibrationData );

  probeCalibrationResult->AddNestedElement( calibrationResults );
  probeCalibrationResult->AddNestedElement( errorReport );

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::GetCalibrationReport( std::vector<double> *optimizedResults, std::vector<double> *calibError,
                                                            std::vector<double> *validError,vnl_matrix<double> *imageToProbeTransformMatrixVnl) 
{
                                                            // Write the results to be easily processed
 
  *optimizedResults = this->SpatialCalibrationOptimizer->GetOptimizationResults();

  calibError->push_back(this->CalibrationRmsError2D);
  calibError->push_back(this->CalibrationRmsError2DSD);
  validError->push_back(this->ValidationRmsError2D);
  validError->push_back(this->ValidationRmsError2DSD);
  
  //*imageToProbeTransformMatrixVnl = this->SpatialCalibrationOptimizer->GetOptimizedImageToProbeTransformMatrix();
  // Convert the transform to vnl
  PlusMath::ConvertVtkMatrixToVnlMatrix(this->ImageToProbeTransformMatrix,*imageToProbeTransformMatrixVnl);

  return PLUS_SUCCESS;
}
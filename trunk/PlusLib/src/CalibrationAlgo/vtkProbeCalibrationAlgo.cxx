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
#include "vtkLine.h"
#include "vtkPlane.h"

static const int MIN_NUMBER_OF_VALID_CALIBRATION_FRAMES=10; // minimum number of successfully calibrated frames required for calibration
static const double DEFAULT_ERROR_CONFIDENCE_INTERVAL=0.95; // this fraction of the data is taken into account when computing mean and standard deviation in the final calibration error report

vtkCxxRevisionMacro(vtkProbeCalibrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkProbeCalibrationAlgo);

//----------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
vtkProbeCalibrationAlgo::vtkProbeCalibrationAlgo() 
: CalibrationDate(NULL)
, ImageCoordinateFrame(NULL)
, ProbeCoordinateFrame(NULL)
, PhantomCoordinateFrame(NULL)
, ReferenceCoordinateFrame(NULL)
, ErrorConfidenceLevel(DEFAULT_ERROR_CONFIDENCE_INTERVAL)
{
  this->SpatialCalibrationOptimizer = vtkSpatialCalibrationOptimizer::New();
  this->SpatialCalibrationOptimizer->SetProbeCalibrationAlgo(this);
}

//----------------------------------------------------------------------------
vtkProbeCalibrationAlgo::~vtkProbeCalibrationAlgo() 
{
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

  // Optimization options
  if (this->SpatialCalibrationOptimizer->ReadConfiguration(probeCalibrationElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkSpatialCalibrationOptimizer is not well specified in vtkSpatialCalibrationOptimizer element of the configuration!");
    return PLUS_FAIL;     
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
PlusStatus vtkProbeCalibrationAlgo::ComputeImageToProbeTransformByLinearLeastSquaresMethod(vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix, std::set<int> &outliers)
{
  // Do calibration for all dimensions and assemble output matrix
  const int n = 4; // number of point dimensions + 1 (homogeneous coordinate system representation: x, y, z, 1)
  const int numberOfNWiresOnEachFrame=this->NWires.size();
  const int numberOfFrames=PreProcessedWirePositions[CALIBRATION_ALL].FramePositions.size();
  const int m = numberOfFrames*numberOfNWiresOnEachFrame; // number of all middle line intersection points on all frames

  // If we attempt to run least square with not enough points then vnl_lsqr crashes. Return with an error if there are very few frames to avoid crashing.
  if ( numberOfFrames < MIN_NUMBER_OF_VALID_CALIBRATION_FRAMES)
  {
    LOG_ERROR("Unable to perform calibration - there are " << numberOfFrames << " frames with segmented points and minimum " << MIN_NUMBER_OF_VALID_CALIBRATION_FRAMES << " frames are needed"); 
    return PLUS_FAIL; 
  }

  vnl_sparse_matrix<double> middleWireIntersectionPointsPos_Image(m,n);
  {
    int outputColumnIndex=0;
    for (int frameIndex=0; frameIndex<this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions.size(); ++frameIndex)
    {
      for (int nWireIndex=0; nWireIndex<numberOfNWiresOnEachFrame; ++nWireIndex)
      {
        const vnl_vector_fixed<double,4> middleWireIntersectionPointPos_Image=PreProcessedWirePositions[CALIBRATION_ALL].FramePositions[frameIndex].AllWiresIntersectionPointsPos_Image[nWireIndex*3+1];
        middleWireIntersectionPointsPos_Image(outputColumnIndex,0)=middleWireIntersectionPointPos_Image[0];
        middleWireIntersectionPointsPos_Image(outputColumnIndex,1)=middleWireIntersectionPointPos_Image[1];
        middleWireIntersectionPointsPos_Image(outputColumnIndex,2)=middleWireIntersectionPointPos_Image[2];
        middleWireIntersectionPointsPos_Image(outputColumnIndex,3)=middleWireIntersectionPointPos_Image[3];
        outputColumnIndex++;
      }
    }
  }

  imageToProbeTransformMatrix.fill(0);

  for (int row = 0; row < n; ++row)
  {
    vnl_vector<double> probePositionRowVector(m,0);
    {
      int posIndex=0;
      for (int frameIndex=0; frameIndex<this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions.size(); ++frameIndex)
      {
        for (int nWireIndex=0; nWireIndex<numberOfNWiresOnEachFrame; ++nWireIndex)
        {
          probePositionRowVector[posIndex] = this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions[frameIndex].MiddleWireIntersectionPointsPos_Probe[nWireIndex][row];
          posIndex++;
        }
      }
    }

    // Fill the nonOutliers vector with the indices of all the points  (0, 1, 2, 3, ... m)
    vnl_vector<unsigned int> nonOutliers(m);
    for (int i=0; i < m; ++i)
    {
      nonOutliers.put(i,i);
    }

    vnl_vector<double> resultVector(n,0);
    if ( PlusMath::LSQRMinimize(middleWireIntersectionPointsPos_Image, probePositionRowVector, resultVector, NULL, NULL, &nonOutliers) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to run LSQRMinimize!"); 
      return PLUS_FAIL;
    }

    // Update outlier list
    for (int pointIndex=0;pointIndex<m;pointIndex++)
    {
      bool validPoint = false; // not an outlier
      for (int validPointListIndex=0; validPointListIndex<=nonOutliers.size(); validPointListIndex++)
      {
        if (nonOutliers.get(validPointListIndex) == pointIndex)
        {
          validPoint=true;
          break;
        }
      }
      if (!validPoint)
      {
        outliers.insert(pointIndex);
      }
    }

    // Save result into ImageToProbe matrix
    imageToProbeTransformMatrix.set_row(row, resultVector);
  }

  // Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
  // Make the z vector have about the same length as x an y, so that when a 3D widget is transformed using this transform, the aspect ratio is maintained

  double xVector[3] = {imageToProbeTransformMatrix(0,0),imageToProbeTransformMatrix(1,0),imageToProbeTransformMatrix(2,0)}; 
  double yVector[3] = {imageToProbeTransformMatrix(0,1),imageToProbeTransformMatrix(1,1),imageToProbeTransformMatrix(2,1)};  
  double zVector[3] = {0,0,0}; 
  vtkMath::Cross(xVector, yVector, zVector); 
  vtkMath::Normalize(zVector);
  double normZ = (vtkMath::Norm(xVector)+vtkMath::Norm(yVector))/2;  
  vtkMath::MultiplyScalar(zVector, normZ);
  imageToProbeTransformMatrix(0,2)=zVector[0];
  imageToProbeTransformMatrix(1,2)=zVector[1];
  imageToProbeTransformMatrix(2,2)=zVector[2];

  LOG_DEBUG(outliers.size() << " outliers points were found");

  return PLUS_SUCCESS;
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

  this->PreProcessedWirePositions[CALIBRATION_ALL].Clear();
  this->PreProcessedWirePositions[VALIDATION_ALL].Clear();
  this->PreProcessedWirePositions[CALIBRATION_NOT_OUTLIER].Clear();

  // Add tracked frames for calibration and validation
  for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    LOG_DEBUG("\n----------------------------------");
    LOG_DEBUG("Add frame #" << frameNumber << " for validation data");
    if ( AddPositionsPerImage(validationTrackedFrameList->GetTrackedFrame(frameNumber), transformRepository, VALIDATION_ALL) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add validation position failed on frame #" << frameNumber);
      continue;
    }
  }    

  for (int frameNumber = calibrationStartFrame; frameNumber < calibrationEndFrame; ++frameNumber)
  {
    LOG_DEBUG("\n----------------------------------");
    LOG_DEBUG("Add frame #" << frameNumber << " for calibration data");
    if ( AddPositionsPerImage(calibrationTrackedFrameList->GetTrackedFrame(frameNumber), transformRepository, CALIBRATION_ALL) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add calibration position failed on frame #" << frameNumber);
      continue;
    }
  }

  if ( PreProcessedWirePositions[CALIBRATION_ALL].FramePositions.empty() )
  {
    LOG_ERROR("Unable to perform calibration - calibration data is empty!"); 
    return PLUS_FAIL; 
  }

  vnl_matrix_fixed<double,4,4> imageToProbeTransformMatrix;
  std::set<int> outliers;
  if (ComputeImageToProbeTransformByLinearLeastSquaresMethod(imageToProbeTransformMatrix, outliers)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration with linear least squares method failed");
    return PLUS_FAIL;
  }
  // Validate calibration result and set it to member variable and transform repository
  SetAndValidateImageToProbeTransform( imageToProbeTransformMatrix, transformRepository);

  if (this->SpatialCalibrationOptimizer->Enabled())
  {
    LOG_INFO("Additional calibration optimization is requested");
    UpdateNonOutlierData(outliers);
    this->SpatialCalibrationOptimizer->SetImageToProbeSeedTransform(imageToProbeTransformMatrix);
    this->SpatialCalibrationOptimizer->Update();
    imageToProbeTransformMatrix = this->SpatialCalibrationOptimizer->GetOptimizedImageToProbeTransformMatrix();
    SetAndValidateImageToProbeTransform( imageToProbeTransformMatrix, transformRepository);
  }

  // Log the calibration result and error
  LOG_INFO("Image to probe transform matrix = ");
  PlusMath::LogMatrix(this->ImageToProbeTransformMatrix, 6);

  // Compute 3D reprojection errors
  if ( ComputeReprojectionErrors3D(VALIDATION_ALL, imageToProbeTransformMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute validation 3D reprojection errors!");
    return PLUS_FAIL;
  }

  PlusTransformName imageToProbeTransformName(this->ImageCoordinateFrame, this->ProbeCoordinateFrame);
  transformRepository->SetTransformError(imageToProbeTransformName, this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DMean);
  LOG_INFO("Validation 3D Reprojection Error (OPE): Mean: " << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DMean << "mm, StDdev: " << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DStdDev << "mm");

  if ( ComputeReprojectionErrors3D(CALIBRATION_ALL, imageToProbeTransformMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute calibration 3D reprojection errors!");
    return PLUS_FAIL;
  }

  LOG_INFO("Calibration 3D Reprojection Error (OPE): Mean: " << this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError3DMean << "mm, StDdev: " << this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError3DStdDev << "mm");

  // Compute 2D reprojection errors
  if ( ComputeReprojectionErrors2D(VALIDATION_ALL, imageToProbeTransformMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute validation 2D reprojection errors!");
    return PLUS_FAIL;
  }
  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    LOG_INFO("Validation 2D Reprojection Error (IPE) for wire " << this->NWires[i/3].Wires[i%3].Name << ": "
      << "Mean: (" << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DMeans[i][0] << "px, " 
      << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DMeans[i][1] << "px), "
      << "StdDev: (" << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][0] << "px, " 
      << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][1] << "px)");
  }

  if ( ComputeReprojectionErrors2D(CALIBRATION_ALL, imageToProbeTransformMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute calibration 2D reprojection errors!");
    return PLUS_FAIL;
  }
  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    LOG_INFO("Calibration 2D Reprojection Error (IPE) for wire " << this->NWires[i/3].Wires[i%3].Name <<": "
      << "Mean: (" << this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DMeans[i][0] << "px, " 
      << this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DMeans[i][1] << "px), StdDev: (" 
      << this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][0] << "px, " 
      << this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][1] << "px)");
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
PlusStatus vtkProbeCalibrationAlgo::AddPositionsPerImage( TrackedFrame* trackedFrame, vtkTransformRepository* transformRepository, PreProcessedWirePositionIdType datasetType)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::AddPositionsPerImage(type=" <<datasetType<< ")");

  // Get position of segmented fiducial points in the image
  std::vector<vnl_vector<double> > segmentedWireIntersectionPointsPos_Image;
  {
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
    // Convert to vnl  
    for (int i=0; i<segmentedPointsVtk->GetNumberOfPoints(); i++)
    {
      vnl_vector<double> vnlPoint(4,0);
      double* point=segmentedPointsVtk->GetPoint(i);
      vnlPoint[0] = point[0];
      vnlPoint[1] = point[1];
      vnlPoint[2] = 0.0;
      vnlPoint[3] = 1.0;
      segmentedWireIntersectionPointsPos_Image.push_back(vnlPoint);
    }
  }

  // Compute PhantomToProbe transform 
  vnl_matrix_fixed<double,4,4> phantomToProbeTransformMatrix;
  {
    PlusTransformName probeToReferenceTransformName(this->ProbeCoordinateFrame, this->ReferenceCoordinateFrame);
    vtkSmartPointer<vtkMatrix4x4> probeToReferenceVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    transformRepository->SetTransforms(*trackedFrame);
    bool transformValid = false;
    if ( (transformRepository->GetTransform(probeToReferenceTransformName, probeToReferenceVtkTransformMatrix, &transformValid) != PLUS_SUCCESS) || (!transformValid) )
    {
      std::string transformName; 
      probeToReferenceTransformName.GetTransformName(transformName); 
      LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
      return PLUS_FAIL;
    }

    // Get phantom registration matrix and convert it to vnl
    PlusTransformName phantomToReferenceTransformName(this->PhantomCoordinateFrame, this->ReferenceCoordinateFrame);
    vtkSmartPointer<vtkMatrix4x4> phantomToReferenceTransformMatrixVtk = vtkSmartPointer<vtkMatrix4x4>::New();
    if ( (transformRepository->GetTransform(phantomToReferenceTransformName, phantomToReferenceTransformMatrixVtk, &transformValid) != PLUS_SUCCESS) || (!transformValid) )
    {
      LOG_ERROR("No valid transform found from phantom to reference");
      return PLUS_FAIL;
    }

    vnl_matrix_fixed<double,4,4> phantomToReferenceTransformMatrix;
    PlusMath::ConvertVtkMatrixToVnlMatrix(phantomToReferenceTransformMatrixVtk, phantomToReferenceTransformMatrix);

    // Get reference to probe transform in vnl
    vnl_matrix_fixed<double,4,4> probeToReferenceTransformMatrix;
    PlusMath::ConvertVtkMatrixToVnlMatrix(probeToReferenceVtkTransformMatrix, probeToReferenceTransformMatrix); 

    vnl_matrix_fixed<double,4,4> referenceToProbeTransformMatrix = vnl_inverse(probeToReferenceTransformMatrix);

    // Make sure the last row in homogeneous transform is [0 0 0 1]
    vnl_vector<double> lastRow(4,0);
    lastRow.put(3, 1);
    referenceToProbeTransformMatrix.set_row(3, lastRow);    
    LOG_DEBUG("Reference to probe transform = \n" << referenceToProbeTransformMatrix);

    phantomToProbeTransformMatrix = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix;
  }

  // Create a new element to store all the position information for the frame
  NWirePositionType framePosition;

  {
    // Store all the probe to phantom transforms, used only in 2D minimization
    PlusTransformName probeToPhantomTransformName(this->ProbeCoordinateFrame, this->PhantomCoordinateFrame);
    vtkSmartPointer<vtkMatrix4x4> probeToPhantomVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

    bool transformValid = false;
    if ( (transformRepository->GetTransform(probeToPhantomTransformName, probeToPhantomVtkTransformMatrix, &transformValid) != PLUS_SUCCESS) || (!transformValid) )
    {
      std::string transformName; 
      probeToPhantomTransformName.GetTransformName(transformName); 
      LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
      return PLUS_FAIL;
    }
    // Get  probe to phantome transform in vnl
    vnl_matrix_fixed<double,4,4> probeToPhantomTransformMatrix;
    PlusMath::ConvertVtkMatrixToVnlMatrix(probeToPhantomVtkTransformMatrix, probeToPhantomTransformMatrix); 
    
    framePosition.ProbeToPhantomTransform=probeToPhantomTransformMatrix;
  }

  // Calculate wire position in probe coordinate system using the segmentation result and the phantom geometry
  std::vector< vnl_vector_fixed<double,4> > middleWireIntersectionPointsPos_Phantom;
  for (int nWireIndex = 0; nWireIndex < this->NWires.size(); ++nWireIndex)
  {
    int wire1Index=nWireIndex*3+0;
    int wire2Index=nWireIndex*3+1;
    int wire3Index=nWireIndex*3+2;

    // Calcuate the alpha value (how far the middle point is from the two side points)
    vnl_vector<double> wire12vector_Image = segmentedWireIntersectionPointsPos_Image[wire2Index] - segmentedWireIntersectionPointsPos_Image[wire1Index];
    vnl_vector<double> wire13vector_Image = segmentedWireIntersectionPointsPos_Image[wire3Index] - segmentedWireIntersectionPointsPos_Image[wire1Index];
    double alpha = (double)wire12vector_Image.magnitude() / wire13vector_Image.magnitude();

    // Compute middle point position in phantom frame using alpha and the imaginary intersection points
    vnl_vector_fixed<double,4> middleWireIntersectionPointPos_Phantom(4);
    vnl_vector_fixed<double,4> intersectPosW12_Phantom(this->NWires[nWireIndex].IntersectPosW12[0], this->NWires[nWireIndex].IntersectPosW12[1], this->NWires[nWireIndex].IntersectPosW12[2], 1.0);
    vnl_vector_fixed<double,4> intersectPosW32_Phantom(this->NWires[nWireIndex].IntersectPosW32[0], this->NWires[nWireIndex].IntersectPosW32[1], this->NWires[nWireIndex].IntersectPosW32[2], 1.0);
    middleWireIntersectionPointPos_Phantom = intersectPosW12_Phantom + alpha * ( intersectPosW32_Phantom - intersectPosW12_Phantom );

    LOG_DEBUG("NWire #" << nWireIndex);
    LOG_DEBUG("  Segmented point #" << wire1Index << " = " << segmentedWireIntersectionPointsPos_Image[wire1Index]);
    LOG_DEBUG("  Segmented point #" << wire2Index << " = " << segmentedWireIntersectionPointsPos_Image[wire2Index] << " (middle wire)");
    LOG_DEBUG("  Segmented point #" << wire3Index << " = " << segmentedWireIntersectionPointsPos_Image[wire3Index]);
    LOG_DEBUG("  Alpha = " << alpha);
    LOG_DEBUG("  Middle wire position in phantom frame = " << middleWireIntersectionPointPos_Phantom);

    {
      // Compute middle point position in probe frame
      vnl_vector<double> middleWireIntersectionPointPos_Probe =  phantomToProbeTransformMatrix * middleWireIntersectionPointPos_Phantom;

      LOG_DEBUG("  Middle wire position in probe frame = " << middleWireIntersectionPointPos_Probe);

      // Store into the list of middle wire intersections in the probe frame
      framePosition.MiddleWireIntersectionPointsPos_Probe.push_back( middleWireIntersectionPointPos_Probe );

      // Store all the segmented points, used only in 2D minimization
      framePosition.AllWiresIntersectionPointsPos_Image.push_back(segmentedWireIntersectionPointsPos_Image[wire1Index]);
      framePosition.AllWiresIntersectionPointsPos_Image.push_back(segmentedWireIntersectionPointsPos_Image[wire2Index]);
      framePosition.AllWiresIntersectionPointsPos_Image.push_back(segmentedWireIntersectionPointsPos_Image[wire3Index]);
    }
  }

  this->PreProcessedWirePositions[datasetType].FramePositions.push_back(framePosition);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::SetAndValidateImageToProbeTransform( const vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix, vtkTransformRepository* transformRepository)
{
  // Check orthogonality
  vnl_matrix_fixed<double,3,3> imageToProbeTransformMatrixRot=imageToProbeTransformMatrix.extract(3,3);

  vnl_vector_fixed<double,3> xVector = imageToProbeTransformMatrixRot.get_column(0); 
  vnl_vector_fixed<double,3> yVector = imageToProbeTransformMatrixRot.get_column(1); 
  xVector.normalize();
  yVector.normalize();  
  double angleXYdeg = fabs(vtkMath::DegreesFromRadians(acos(dot_product<double,3>(xVector,yVector))));
  const double angleToleranceDeg=1.0;
  if (fabs(angleXYdeg-90.0)>angleToleranceDeg) 
  {
    LOG_WARNING("ImageToProbe transformation matrix X and Y axes are not orthogonal. XY axis angle = " << angleXYdeg << " deg");
  }

  // Set result matrix
  this->ImageToProbeTransformMatrix=imageToProbeTransformMatrix;

  // Save results into transform repository
  vtkSmartPointer<vtkMatrix4x4> imageToProbeMatrixVtk=vtkSmartPointer<vtkMatrix4x4>::New();
  PlusMath::ConvertVnlMatrixToVtkMatrix(imageToProbeTransformMatrix, imageToProbeMatrixVtk);
  PlusTransformName imageToProbeTransformName(this->ImageCoordinateFrame, this->ProbeCoordinateFrame);
  transformRepository->SetTransform(imageToProbeTransformName, imageToProbeMatrixVtk);
  transformRepository->SetTransformPersistent(imageToProbeTransformName, true);
  transformRepository->SetTransformDate(imageToProbeTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationAlgo::ComputeReprojectionErrors3D(PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::ComputeReprojectionErrors3D");

  std::vector<double> reprojectionErrors;
  ComputeError3d(reprojectionErrors, datasetType, imageToProbeTransformMatrix);

  int numberOfNWiresPerFrame=this->NWires.size();
  this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError3Ds.clear();
  this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError3Ds.resize(numberOfNWiresPerFrame);
  for (int frameIndex=0; frameIndex<reprojectionErrors.size()/numberOfNWiresPerFrame; frameIndex++)
  {
    for (int nWireIndex=0; nWireIndex<numberOfNWiresPerFrame; nWireIndex++)
    {
      this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError3Ds.at(nWireIndex).push_back(reprojectionErrors[frameIndex*numberOfNWiresPerFrame+nWireIndex]);
    }
  }

  PlusMath::ComputePercentile(reprojectionErrors,this->ErrorConfidenceLevel, 
    this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError3DMax,
    this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError3DMean, 
    this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError3DStdDev); 
  
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationAlgo::ComputeReprojectionErrors2D(PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::ComputeReprojectionErrors2D");

  // Initialize objects

  double errorMean=0.0;
  double errorStdev=0.0;
  ComputeError2d(datasetType, imageToProbeMatrix, errorMean, errorStdev, this->PreProcessedWirePositions[datasetType].NWireErrors.RmsError2D,
    &(this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2Ds) );   
  
  this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DMeans.clear();
  this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DStdDevs.clear();
  const int numberOfFrames=this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2Ds.size();
  for (int wire=0; wire<3*this->NWires.size(); wire++)
  {
    std::vector<double> xErrors;
    std::vector<double> yErrors;
    for (int frameIndex=0; frameIndex<numberOfFrames; frameIndex++)
    {
      xErrors.push_back(this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2Ds[frameIndex][wire][0]);
      yErrors.push_back(this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2Ds[frameIndex][wire][1]);
    }
    double xErrorMean=0.0;
    double xErrorStdev=0.0;
    PlusMath::ComputeMeanAndStdev(xErrors, xErrorMean, xErrorStdev);
    double yErrorMean=0.0;
    double yErrorStdev=0.0;
    PlusMath::ComputeMeanAndStdev(yErrors, yErrorMean, yErrorStdev);
    vnl_vector_fixed<double,2> errorMean(xErrorMean, yErrorMean);
    this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DMeans.push_back(errorMean);
    vnl_vector_fixed<double,2> errorStdev(xErrorStdev, yErrorStdev);
    this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DStdDevs.push_back(errorStdev);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationAlgo::GetReprojectionError2DStatistics(double &xMean, double &yMean, double &xStdDev, double &yStdDev, int wireNumber, bool isValidation)
{
  PreProcessedWirePositionIdType datasetType = isValidation ? VALIDATION_ALL : CALIBRATION_ALL;    

  xMean = yMean = xStdDev = yStdDev = -1.0;

  int wireIndex = wireNumber - 1; 

  if (wireIndex < 0 || wireIndex >= this->NWires.size()*3)
  {
    LOG_ERROR("Invalid wire number: " << wireNumber);
    return PLUS_FAIL;
  }

  xMean = this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DMeans[wireIndex][0];
  yMean = this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DMeans[wireIndex][1];
  xStdDev = this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DStdDevs[wireIndex][0];
  yStdDev = this->PreProcessedWirePositions[datasetType].NWireErrors.ReprojectionError2DStdDevs[wireIndex][1];

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkProbeCalibrationAlgo::GetResultString(int precision/* = 3*/)
{
  LOG_TRACE("vtkProbeCalibrationAlgo::GetResultString");

  std::ostringstream matrixStringStream;
  matrixStringStream << "Image to probe transform:" << std::endl;

  // Print matrix rows
  PlusMath::PrintMatrix(this->ImageToProbeTransformMatrix, matrixStringStream, precision+2);
  matrixStringStream << std::endl;

  double pixelSizeX=this->ImageToProbeTransformMatrix.get_column(0).magnitude();
  double pixelSizeY=this->ImageToProbeTransformMatrix.get_column(1).magnitude();
  matrixStringStream << "Image pixel size (mm):" << std::endl;
  matrixStringStream << "  " << std::fixed << std::setprecision(precision+1) << pixelSizeX << " x " << pixelSizeY << std::endl;

  std::ostringstream errorsStringStream;

  errorsStringStream << std::fixed << std::setprecision(precision) << "3D Reprojection Error (mm)" << std::endl <<
    " Mean: " << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DMean <<
    ", StdDev: " << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DStdDev << std::endl << std::endl;

  errorsStringStream << "2D Reprojection Errors (px)" << std::endl;;
  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    errorsStringStream << std::fixed << std::setprecision(precision-1) <<
      "Wire #" << i << " (" << this->NWires[i/3].Wires[i%3].Name << ")" << std::endl <<
      " M:(" << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DMeans[i][0] 
    << "," << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DMeans[i][1] << ")" <<
      " SD:(" << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][0] 
    << "," << this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][1] << ")" << std::endl;
  }

  std::ostringstream resultStringStream;
  resultStringStream << matrixStringStream.str() << std::endl << errorsStringStream.str();

  //resultStringStream << std::endl << "Error confidence: " << (int)(this->ErrorConfidenceLevel*100) << "%";

  return resultStringStream.str();
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
  vtkSmartPointer<vtkXMLDataElement> imageToProbeTransformElement = vtkSmartPointer<vtkXMLDataElement>::New();
  imageToProbeTransformElement->SetName("Transform"); 
  imageToProbeTransformElement->SetAttribute("From", this->ImageCoordinateFrame); 
  imageToProbeTransformElement->SetAttribute("To", this->ProbeCoordinateFrame); 
  double *imageToProbeTransformVector=this->ImageToProbeTransformMatrix.data_block();
  imageToProbeTransformElement->SetVectorAttribute("Matrix", 16, imageToProbeTransformVector); 

  calibrationResults->AddNestedElement( imageToProbeTransformElement );


  // Error report
  vtkSmartPointer<vtkXMLDataElement> errorReport = vtkSmartPointer<vtkXMLDataElement>::New(); 
  errorReport->SetName("ErrorReport");

  // ReprojectionError3D
  vtkSmartPointer<vtkXMLDataElement> reprojectionError3DStatistics = vtkSmartPointer<vtkXMLDataElement>::New(); 
  reprojectionError3DStatistics->SetName("ReprojectionError3DStatistics");
  reprojectionError3DStatistics->SetDoubleAttribute("ValidationMeanMm", this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DMean); 
  reprojectionError3DStatistics->SetDoubleAttribute("ValidationStdDevMm", this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DStdDev); 
  reprojectionError3DStatistics->SetDoubleAttribute("CalibrationMeanMm", this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError3DMean); 
  reprojectionError3DStatistics->SetDoubleAttribute("CalibrationStdDevMm", this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError3DStdDev); 

  // ReprojectionError2D
  vtkSmartPointer<vtkXMLDataElement> reprojectionError2DStatistics = vtkSmartPointer<vtkXMLDataElement>::New(); 
  reprojectionError2DStatistics->SetName("ReprojectionError2DStatistics");

  for (int i=0; i<this->NWires.size()*3; ++i)
  {
    vtkSmartPointer<vtkXMLDataElement> wire = vtkSmartPointer<vtkXMLDataElement>::New(); 
    wire->SetName("Wire");
    wire->SetAttribute("Name", this->NWires[i/3].Wires[i%3].Name.c_str());

    double validationMean2D[2] = { this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DMeans[i][0], this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DMeans[i][1] };
    double validationStdDev2D[2] = { this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][0], this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][1] };
    double calibrationMean2D[2] = { this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DMeans[i][0], this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DMeans[i][1] };
    double calibrationStdDev2D[2] = { this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][0], this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2DStdDevs[i][1] };
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
    vtkSmartPointer<vtkXMLDataElement> segmentedWireIntersectionPointsPos_Image = vtkSmartPointer<vtkXMLDataElement>::New(); 
    segmentedWireIntersectionPointsPos_Image->SetName("SegmentedPoints");

    int numberOfSegmentedPoints = segmentedPointsVtk->GetNumberOfPoints();
    for (int i=0; i<numberOfSegmentedPoints; i++)
    {
      double point[3];
      segmentedPointsVtk->GetPoint(i, point);

      vtkSmartPointer<vtkXMLDataElement> pointElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      pointElement->SetName("Point");
      pointElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedWireIntersectionPointsPos_Image->AddNestedElement( pointElement );
    }

    // ReprojectionError3Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError3Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError3Ds->SetName("ReprojectionError3DList");

    for (int i = 0; i < numberOfSegmentedPoints/3; ++i)
    {
      vtkSmartPointer<vtkXMLDataElement> reprojectionError3DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError3DElement->SetName("ReprojectionError3D");
      reprojectionError3DElement->SetAttribute("WireName", this->NWires[i].Wires[1].Name.c_str());
      reprojectionError3DElement->SetDoubleAttribute("ErrorMm", this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3Ds[i][numberOfSegmentedFramesSoFar]);
      reprojectionError3Ds->AddNestedElement( reprojectionError3DElement );
    }

    // ReprojectionError2Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError2Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError2Ds->SetName("ReprojectionError2DList");

    for (int i = 0; i < numberOfSegmentedPoints; ++i)
    {
      double reprojectionError2D[2] = { this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2Ds[numberOfSegmentedFramesSoFar][i][0], this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError2Ds[numberOfSegmentedFramesSoFar][i][1] };
      vtkSmartPointer<vtkXMLDataElement> reprojectionError2DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError2DElement->SetName("ReprojectionError2D");
      reprojectionError2DElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      reprojectionError2DElement->SetVectorAttribute("ErrorPx", 2, reprojectionError2D);
      reprojectionError2Ds->AddNestedElement( reprojectionError2DElement );
    }

    frame->AddNestedElement( segmentedWireIntersectionPointsPos_Image );
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
    vtkSmartPointer<vtkXMLDataElement> segmentedWireIntersectionPointsPos_Image = vtkSmartPointer<vtkXMLDataElement>::New(); 
    segmentedWireIntersectionPointsPos_Image->SetName("SegmentedPoints");

    int numberOfSegmentedPoints = segmentedPointsVtk->GetNumberOfPoints();
    for (int i=0; i<numberOfSegmentedPoints; i++)
    {
      double point[3];
      segmentedPointsVtk->GetPoint(i, point);

      vtkSmartPointer<vtkXMLDataElement> pointElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      pointElement->SetName("Point");
      pointElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedWireIntersectionPointsPos_Image->AddNestedElement( pointElement );
    }

    // MiddleWire
    vtkSmartPointer<vtkXMLDataElement> middleWires = vtkSmartPointer<vtkXMLDataElement>::New(); 
    middleWires->SetName("MiddleWires");

    for (int nWireIndex = 0; nWireIndex<this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions[numberOfSegmentedFramesSoFar].MiddleWireIntersectionPointsPos_Probe.size(); ++nWireIndex)
    {
      vtkSmartPointer<vtkXMLDataElement> middleWire = vtkSmartPointer<vtkXMLDataElement>::New(); 
      middleWire->SetName("MiddleWire");

      double middleWirePositionInImageFrame[4];
      double middleWirePositionInProbeFrame[4];
      for (int k=0; k<4; ++k)
      {
        middleWirePositionInImageFrame[k] = this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions[numberOfSegmentedFramesSoFar].AllWiresIntersectionPointsPos_Image[nWireIndex*3+1].get(k);
        middleWirePositionInProbeFrame[k] = this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions[numberOfSegmentedFramesSoFar].MiddleWireIntersectionPointsPos_Probe[nWireIndex].get(k);
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
      reprojectionError3DElement->SetDoubleAttribute("ErrorMm", this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError3Ds[i][numberOfSegmentedFramesSoFar]);
      reprojectionError3Ds->AddNestedElement( reprojectionError3DElement );
    }

    // ReprojectionError2Ds
    vtkSmartPointer<vtkXMLDataElement> reprojectionError2Ds = vtkSmartPointer<vtkXMLDataElement>::New(); 
    reprojectionError2Ds->SetName("ReprojectionError2DList");

    for (int i = 0; i < numberOfSegmentedPoints; ++i)
    {
      double reprojectionError2D[2] = { this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2Ds[numberOfSegmentedFramesSoFar][i][0], this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.ReprojectionError2Ds[numberOfSegmentedFramesSoFar][i][1] };
      vtkSmartPointer<vtkXMLDataElement> reprojectionError2DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      reprojectionError2DElement->SetName("ReprojectionError2D");
      reprojectionError2DElement->SetAttribute("WireName", this->NWires[i/3].Wires[i%3].Name.c_str());
      reprojectionError2DElement->SetVectorAttribute("ErrorPx", 2, reprojectionError2D);
      reprojectionError2Ds->AddNestedElement( reprojectionError2DElement );
    }

    frame->AddNestedElement( segmentedWireIntersectionPointsPos_Image );
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
PlusStatus vtkProbeCalibrationAlgo::GetCalibrationReport(std::vector<double> *calibError, std::vector<double> *validError,vnl_matrix_fixed<double,4,4> *imageToProbeTransformMatrix) 
{
  (*imageToProbeTransformMatrix)=this->ImageToProbeTransformMatrix;
  calibError->push_back(this->PreProcessedWirePositions[CALIBRATION_ALL].NWireErrors.RmsError2D);
  validError->push_back(this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.RmsError2D);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::UpdateNonOutlierData(const std::set<int>& outliers)
{
  for(int frameIndex=0;frameIndex<this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions.size();++frameIndex)
  { 
    bool outlier=false;
    for (int nWireIndex=0; nWireIndex<this->NWires.size(); ++nWireIndex)
    {
      if(outliers.find(frameIndex*this->NWires.size()+nWireIndex) != outliers.end())
      {
        // any of the nWires is an outlier, so skip the whole frame
        outlier=true;
        break;
      }
    }
    if (outlier)
    {
      continue;
    }
    // not outlier frame, copy to the non-outlier list
    this->PreProcessedWirePositions[CALIBRATION_NOT_OUTLIER].FramePositions.push_back(this->PreProcessedWirePositions[CALIBRATION_ALL].FramePositions[frameIndex]);
  }
}

//-----------------------------------------------------------------------------
double vtkProbeCalibrationAlgo::PointToWireDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2 )
{
  // Convert point from vnl to vtk format
  double wireP0[3] = {aLineEndPoint1[0], aLineEndPoint1[1], aLineEndPoint1[2]};
  double wireP1[3] = {aLineEndPoint2[0], aLineEndPoint2[1], aLineEndPoint2[2]};
  double segmentedPoint[3] = {aPoint(0),aPoint(1),aPoint(2)};
  double distanceToWire = sqrt(vtkLine::DistanceToLine(segmentedPoint, wireP0, wireP1));
  return distanceToWire;
}

//--------------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::ComputeError2d(const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix, double &errorMean, double &errorStDev, double &errorRms)
{
  ComputeError2d(CALIBRATION_NOT_OUTLIER, imageToProbeMatrix, errorMean, errorStDev, errorRms);   
}

//--------------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::ComputeError2d(PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix, 
  double &errorMean, double &errorStDev, double &errorRms,
  std::vector< std::vector< vnl_vector_fixed<double,2> > >* reprojectionError2Ds /*=NULL*/)
{
  std::vector<double> reprojectionErrors;

  int nWires = this->NWires.size(); // number of N-wires in the calibration pattern

  vnl_vector_fixed<double,4> segmentedInPhantomFrame_vnl;
  segmentedInPhantomFrame_vnl(3)=1;
  vnl_vector_fixed<double,4> wireFrontPoint_Phantom;
  vnl_vector_fixed<double,4> wireBackPoint_Phantom;  
  wireFrontPoint_Phantom(3)=1;
  wireBackPoint_Phantom(3)=1;
  vnl_vector_fixed<double,4> wireFrontPoint_Image;
  vnl_vector_fixed<double,4> wireBackPoint_Image;
  wireFrontPoint_Image(3)=1;
  wireBackPoint_Image(3)=1;

  int numberOfFrames=this->PreProcessedWirePositions[datasetType].FramePositions.size();
  if (reprojectionError2Ds!=NULL)
  {
    reprojectionError2Ds->clear();
    reprojectionError2Ds->resize(numberOfFrames);
  }

  for(int frameIndex=0;frameIndex<numberOfFrames;frameIndex++) // for each frame
  {
    vnl_matrix_fixed<double,4,4> probeToPhantomTransform_vnl = this->PreProcessedWirePositions[datasetType].FramePositions[frameIndex].ProbeToPhantomTransform;
    vnl_matrix_fixed<double,4,4> phantomToImageTransform_vnl = vnl_inverse(imageToProbeMatrix)*vnl_inverse(probeToPhantomTransform_vnl);
    vtkSmartPointer<vtkMatrix4x4> phantomToImageTransform=vtkSmartPointer<vtkMatrix4x4>::New();
    PlusMath::ConvertVnlMatrixToVtkMatrix(phantomToImageTransform_vnl, phantomToImageTransform);

    double normalVector[3] = { 0.0, 0.0, 1.0 };
    double origin[3] = { 0.0, 0.0, 0.0 };

    for (int nWireIndex=0; nWireIndex<nWires; nWireIndex++) // for each segmented N-wire
    {
      for (int wireIndex=0;wireIndex<3;wireIndex++)  // for each segmented point
      {
        // Compute the wire intersection point position in the image
        Wire wire = this->NWires[nWireIndex].Wires[wireIndex];
        double wireEndPointFrontInPhantomFrame[4] = { wire.EndPointFront[0], wire.EndPointFront[1], wire.EndPointFront[2], 1.0 };
        double wireEndPointBackInPhantomFrame[4] = { wire.EndPointBack[0], wire.EndPointBack[1], wire.EndPointBack[2], 1.0 };
        double wireEndPointFrontInImageFrame[4]={0};
        double wireEndPointBackInImageFrame[4]={0};
        phantomToImageTransform->MultiplyPoint(wireEndPointFrontInPhantomFrame, wireEndPointFrontInImageFrame);
        phantomToImageTransform->MultiplyPoint(wireEndPointBackInPhantomFrame, wireEndPointBackInImageFrame);

        double computedPositionInImagePlane[3];
        double t = 0; // Parametric coordinate along the line

        // Compute intersection of wire and image plane
        if ( ( ! vtkPlane::IntersectWithLine(wireEndPointFrontInImageFrame, wireEndPointBackInImageFrame, normalVector, origin, t, computedPositionInImagePlane) )
          && ( wireEndPointFrontInImageFrame[3] * wireEndPointBackInImageFrame[3] < 0 ) ) // This condition to ensure that warning is thrown only if the zero value is returned because both points are on the same side of the image plane (in that case the intersection is still valid although the return value is zero)
        {
          LOG_WARNING("Image plane and wire are parallel!");
          if (reprojectionError2Ds!=NULL)
          {
            vnl_vector_fixed<double,2> reprojectionError2D(2, DBL_MAX);
            (*reprojectionError2Ds)[frameIndex].push_back( reprojectionError2D );
          }
          continue;
        }

        // Get the segmented intersection position in the image
        vnl_vector_fixed<double,4> segmentedPoint_Image=this->PreProcessedWirePositions[datasetType].FramePositions[frameIndex].AllWiresIntersectionPointsPos_Image[3*nWireIndex+wireIndex];

        vnl_vector_fixed<double,2> reprojectionError2D;
        reprojectionError2D[0] = segmentedPoint_Image[0] - computedPositionInImagePlane[0];
        reprojectionError2D[1] = segmentedPoint_Image[1] - computedPositionInImagePlane[1];

        if (reprojectionError2Ds!=NULL)
        {
          (*reprojectionError2Ds)[frameIndex].push_back( reprojectionError2D );
        }
        reprojectionErrors.push_back(reprojectionError2D.magnitude());
      }
    }
  }

  PlusMath::ComputeMeanAndStdev(reprojectionErrors, errorMean, errorStDev);
  PlusMath::ComputeRms(reprojectionErrors, errorRms);
}

//--------------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::ComputeError3d(std::vector<double> &reprojectionErrors, PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix)
{
  reprojectionErrors.clear();
  int numberOfFrames = this->PreProcessedWirePositions[datasetType].FramePositions.size();
  vnl_vector_fixed<double,4> segmentedInProbeFrame_vnl;
  vnl_vector_fixed<double,4> pointErrorVector;
  for (int frameIndex=0; frameIndex<numberOfFrames; frameIndex++)
  {
    for(int nWireIndex=0; nWireIndex<this->NWires.size(); nWireIndex++)
    {
      // Find the projection in the probe frame
      segmentedInProbeFrame_vnl=imageToProbeMatrix*this->PreProcessedWirePositions[datasetType].FramePositions[frameIndex].AllWiresIntersectionPointsPos_Image[nWireIndex*3+1];

      pointErrorVector=segmentedInProbeFrame_vnl-this->PreProcessedWirePositions[datasetType].FramePositions[frameIndex].MiddleWireIntersectionPointsPos_Probe[nWireIndex];
      reprojectionErrors.push_back(pointErrorVector.magnitude());
    }
  }
}

//--------------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::ComputeError3d(const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix, double &errorMean, double &errorStDev, double &errorRms)
{
  std::vector<double> reprojectionErrors;
  ComputeError3d(reprojectionErrors, CALIBRATION_NOT_OUTLIER, imageToProbeMatrix);
  PlusMath::ComputeMeanAndStdev(reprojectionErrors, errorMean, errorStDev);
  PlusMath::ComputeRms(reprojectionErrors, errorRms);
}

//--------------------------------------------------------------------------------
double vtkProbeCalibrationAlgo::GetCalibrationReprojectionError3DMean()
{
  return this->PreProcessedWirePositions[VALIDATION_ALL].NWireErrors.ReprojectionError3DMean;
}

//--------------------------------------------------------------------------------
void vtkProbeCalibrationAlgo::GetImageToProbeTransformMatrix(vtkMatrix4x4* imageToProbeMatrix)
{
  PlusMath::ConvertVnlMatrixToVtkMatrix(this->ImageToProbeTransformMatrix, imageToProbeMatrix);
}

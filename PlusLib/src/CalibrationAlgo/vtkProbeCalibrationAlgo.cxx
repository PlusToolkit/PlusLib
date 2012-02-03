/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
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
  this->TransducerOriginCoordinateFrame = NULL; // TODO move to fCal
  this->TransducerOriginPixelCoordinateFrame = NULL; // TODO move to fCal

	vtkSmartPointer<vtkTransform> transformImageToProbe = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToProbe = NULL;
	this->SetTransformImageToProbe(transformImageToProbe); 

	vtkSmartPointer<vtkTransform> transformImageToTransducerOriginPixel = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToTransducerOriginPixel = NULL;
	this->SetTransformImageToTransducerOriginPixel(transformImageToTransducerOriginPixel);

  this->DataPositionsInImageFrame.clear();
  this->DataPositionsInProbeFrame.clear();
  this->MiddleWirePositionsInPhantomFrame.clear();

  this->ValidationDataConfidenceLevel = 0.95;

  this->ReprojectionError3DList.clear();
  this->SortedReprojectionError3DIndices.clear();
  this->ReprojectionError3DMean = -1.0;
  this->ReprojectionError3DStdDev = -1.0;

  this->ReprojectionError2DList.clear();
  this->SortedReprojectionError2DIndices.clear();
  this->ReprojectionError2DMean.clear();
  this->ReprojectionError2DStdDev.clear();
}

//----------------------------------------------------------------------------
vtkProbeCalibrationAlgo::~vtkProbeCalibrationAlgo() 
{
	this->SetTransformImageToTransducerOriginPixel(NULL);
	this->SetTransformImageToProbe(NULL);
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

  // Transducer origin (mm) coordinate frame name
  const char* transducerOriginCoordinateFrame = probeCalibrationElement->GetAttribute("TransducerOriginCoordinateFrame");
  if (transducerOriginCoordinateFrame == NULL)
  {
	  LOG_ERROR("TransducerOriginCoordinateFrame is not specified in vtkProbeCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetTransducerOriginCoordinateFrame(transducerOriginCoordinateFrame);

  // Transducer origin (pixel) coordinate frame name
  const char* transducerOriginPixelCoordinateFrame = probeCalibrationElement->GetAttribute("TransducerOriginPixelCoordinateFrame");
  if (transducerOriginPixelCoordinateFrame == NULL)
  {
	  LOG_ERROR("TransducerOriginPixelCoordinateFrame is not specified in vtkProbeCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetTransducerOriginPixelCoordinateFrame(transducerOriginPixelCoordinateFrame);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::Calibrate( vtkTrackedFrameList* validationTrackedFrameList, vtkTrackedFrameList* calibrationTrackedFrameList, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires )
{
	LOG_TRACE("vtkProbeCalibrationAlgo::Calibrate");

  return Calibrate(validationTrackedFrameList, -1, -1, calibrationTrackedFrameList, -1, -1, transformRepository, nWires);
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::Calibrate( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::Calibrate(validation: " << validationStartFrame << "-" << validationEndFrame << ", calibration: " << calibrationStartFrame << "-" << calibrationEndFrame << ")"); 

  // Set range boundaries
  if (validationStartFrame < 0)
  {
    validationStartFrame = 0;
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

  this->DataPositionsInImageFrame.clear();
  this->DataPositionsInProbeFrame.clear();
  this->MiddleWirePositionsInPhantomFrame.clear();

  // Add tracked frames for calibration and validation
	for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    LOG_DEBUG("\n----------------------------------");
    LOG_DEBUG("Add frame #" << frameNumber << " for validation data");
    if ( AddPositionsPerImage(validationTrackedFrameList->GetTrackedFrame(frameNumber), transformRepository, nWires, true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add validation position failed on frame #" << frameNumber);
      continue;
    }
  }		

	for (int frameNumber = calibrationStartFrame; frameNumber < calibrationEndFrame; ++frameNumber)
  {
    LOG_DEBUG("\n----------------------------------");
    LOG_DEBUG("Add frame #" << frameNumber << " for calibration data");
    if ( AddPositionsPerImage(calibrationTrackedFrameList->GetTrackedFrame(frameNumber), transformRepository, nWires, false) != PLUS_SUCCESS )
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

  vnl_matrix<double> imageToProbeTransformMatrixVnl;
  imageToProbeTransformMatrixVnl.set_size(n, n);
  imageToProbeTransformMatrixVnl.fill(0);

  for (int row = 0; row < n; ++row)
  {
    std::vector<double> probePositionRowVector(m,0);
    for (int i=0; i < m; ++i)
    {
      probePositionRowVector[i] = this->DataPositionsInProbeFrame[i][row];
    }

    vnl_vector<double> resultVector(n,0);
    if ( PlusMath::LSQRMinimize(this->DataPositionsInImageFrame, probePositionRowVector, resultVector) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }

    imageToProbeTransformMatrixVnl.set_row(row, resultVector);
  }

  // Validate calibration result and set it to member variable and transform repository
  SetAndValidateImageToProbeTransform( imageToProbeTransformMatrixVnl, transformRepository );

	// Log the calibration result and error
  LOG_INFO("Image to probe transform matrix = ");
  PlusMath::LogVtkMatrix(this->TransformImageToProbe->GetMatrix(), 6);

  // Compute 3D reprojection errors
  if ( ComputeReprojectionErrors3D(validationTrackedFrameList, validationStartFrame, validationEndFrame, transformRepository, nWires) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute 3D reprojection errors!");
    return PLUS_FAIL;
  }

  PlusTransformName imageToProbeTransformName(this->ImageCoordinateFrame, this->ProbeCoordinateFrame);
  transformRepository->SetTransformError(imageToProbeTransformName, this->ReprojectionError3DMean);
  LOG_INFO("3D Reprojection Error - Mean: " << this->ReprojectionError3DMean << "mm, StDdev: " << this->ReprojectionError3DStdDev << "mm");

  // Compute 2D reprojection errors
  if ( ComputeReprojectionErrors2D(validationTrackedFrameList, validationStartFrame, validationEndFrame, transformRepository, nWires) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to compute 2D reprojection errors!");
    return PLUS_FAIL;
  }

  LOG_INFO("2D Reprojection Error - Mean: (" << this->ReprojectionError2DMean[0] << "px, " << this->ReprojectionError2DMean[1] << "px), StdDev: (" << this->ReprojectionError2DStdDev[0] << "px, " << this->ReprojectionError2DStdDev[1] << "px)");


	// Save the calibration results and error reports into a file 
	if ( SaveCalibrationResultAndErrorReportToXML(validationTrackedFrameList, validationStartFrame, validationEndFrame, calibrationTrackedFrameList, calibrationStartFrame, calibrationEndFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save report!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::AddPositionsPerImage( TrackedFrame* trackedFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires, bool isValidation )
{
  LOG_TRACE("vtkProbeCalibrationAlgo::AddPositionsPerImage(" << (isValidation?"validation":"calibration") << ")");

  if (this->TransformImageToTransducerOriginPixel == NULL)
  {
    LOG_ERROR("Invalid Image to User image transform!");
    return PLUS_FAIL;
  }

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
	std::vector<vnl_vector<double>> segmentedPoints;
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

  // Convert Image to User image transform to vnl
  vnl_matrix<double> imageToTransducerOriginPixelTransformMatrix(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToTransducerOriginPixel->GetMatrix(), imageToTransducerOriginPixelTransformMatrix); 

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


  // Calculate wire position in probe coordinate system using the segmentation result and the phantom geometry
  for (int n = 0; n < nWires.size(); ++n)
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
      intersectPosW12[i] = nWires[n].IntersectPosW12[i];
      intersectPosW32[i] = nWires[n].IntersectPosW32[i];
    }

    intersectPosW12[3] = 1.0;
    intersectPosW32[3] = 1.0;

    positionInPhantomFrame = intersectPosW12 + alpha * ( intersectPosW32 - intersectPosW12 );
    positionInPhantomFrame[3] = 1.0;

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
    }
    else
    {
      // Store segmented point positions in phantom frame
      middleWirePositionsInPhantomFramePerImage.push_back( positionInPhantomFrame );
    }
  }

  if (isValidation)
  {
    this->MiddleWirePositionsInPhantomFrame.push_back( middleWirePositionsInPhantomFramePerImage );
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkProbeCalibrationAlgo::SetAndValidateImageToProbeTransform( vnl_matrix<double> imageToProbeTransformMatrixVnl, vtkTransformRepository* transformRepository )
{
  // Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	imageToProbeTransformMatrixVnl.set_row(3, lastRow);

	// Convert transform to vtk
	vtkSmartPointer<vtkMatrix4x4> imageToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 4; j++ )
		{
			imageToProbeMatrix->SetElement(i, j, imageToProbeTransformMatrixVnl.get(i, j) ); 
		}
	}

  // Check orthogonality
  if ( ! IsImageToProbeTransformOrthogonal() )
  {
    LOG_WARNING("User image to probe transform matrix orthogonality test failed! The result is probably not satisfactory");
  }

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

  // Set result matrix
  this->TransformImageToProbe->SetMatrix( imageToProbeMatrix );

  // Save results into transform repository
  PlusTransformName imageToProbeTransformName(this->ImageCoordinateFrame, this->ProbeCoordinateFrame);
  transformRepository->SetTransform(imageToProbeTransformName, this->TransformImageToProbe->GetMatrix());
  transformRepository->SetTransformPersistent(imageToProbeTransformName, true);
  transformRepository->SetTransformDate(imageToProbeTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());

  double* imageToProbeScale = this->TransformImageToProbe->GetScale();
  vtkSmartPointer<vtkTransform> transducerOriginPixelToTransducerOriginTransform = vtkSmartPointer<vtkTransform>::New();
  transducerOriginPixelToTransducerOriginTransform->Identity();
  transducerOriginPixelToTransducerOriginTransform->Scale(imageToProbeScale);
  PlusTransformName transducerOriginPixelToTransducerOriginTransformName(this->TransducerOriginPixelCoordinateFrame, this->TransducerOriginCoordinateFrame);
  transformRepository->SetTransform(transducerOriginPixelToTransducerOriginTransformName, transducerOriginPixelToTransducerOriginTransform->GetMatrix());
  transformRepository->SetTransformPersistent(transducerOriginPixelToTransducerOriginTransformName, true);
  transformRepository->SetTransformDate(transducerOriginPixelToTransducerOriginTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());

  // Set result matrix
  this->TransformImageToProbe->SetMatrix( imageToProbeMatrix );

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 
}

//-----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::ComputeReprojectionErrors3D( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires )
{
	LOG_TRACE("vtkProbeCalibrationAlgo::ComputeReprojectionErrors3D");

  this->ReprojectionError3DList.clear();
  this->SortedReprojectionError3DIndices.clear();

  std::vector< std::vector< vnl_vector<double> > >::iterator middleWirePositionsIt = this->MiddleWirePositionsInPhantomFrame.begin();

  for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    TrackedFrame* trackedFrame = validationTrackedFrameList->GetTrackedFrame(frameNumber);
    transformRepository->SetTransforms(*trackedFrame); 

    // Get segmentation points and check its validity
    vtkPoints* segmentedPointsVtk = trackedFrame->GetFiducialPointsCoordinatePx();

    if (segmentedPointsVtk == NULL || segmentedPointsVtk->GetNumberOfPoints() == 0 || segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
    {
      continue;
    }

    // Compute 3D error for each NWire
    bool valid = false;

    for (int n = 0; n < nWires.size(); ++n)
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

      this->ReprojectionError3DList.push_back( sqrt( vtkMath::Distance2BetweenPoints(segmentedPointInPhantomFrame, computedPointInPhantomFrame) ) );

    } // For all NWires

    // Only advance to the next frame element if the segmentation was successful on this frame
    ++middleWirePositionsIt;

  } // For all frames

  // Compute the sorted indices array
  std::vector<double> tempReprojectionError3DList( this->ReprojectionError3DList );
  this->SortedReprojectionError3DIndices.resize( this->ReprojectionError3DList.size() );

  std::vector<double>::iterator tempReprojectionError3DListBeginIt = tempReprojectionError3DList.begin();
  for (int i=0; i<this->ReprojectionError3DList.size(); ++i)
  {
    std::vector<double>::iterator reprojectionError3DMinIt = std::min_element(tempReprojectionError3DListBeginIt, tempReprojectionError3DList.end());
    int minIndex = (int)std::distance(tempReprojectionError3DListBeginIt,reprojectionError3DMinIt);
    this->SortedReprojectionError3DIndices[i] = minIndex;
    (*reprojectionError3DMinIt) = DBL_MAX;
  }

  // Compute mean and standard deviation
  int numberOfTopRankedData = ROUND( (double)this->ReprojectionError3DList.size() * this->ValidationDataConfidenceLevel );
  double sum = 0;
  for (int i=0; i<numberOfTopRankedData; ++i)
  {
    sum += this->ReprojectionError3DList[ this->SortedReprojectionError3DIndices[i] ];
  }
  this->ReprojectionError3DMean = sum / this->ReprojectionError3DList.size();
  
  double squareDiffSum = 0;
  for (int i=0; i<numberOfTopRankedData; ++i)
  {
    double diff = this->ReprojectionError3DList[ this->SortedReprojectionError3DIndices[i] ] - this->ReprojectionError3DMean;
    squareDiffSum += diff * diff;
  }
  double variance = squareDiffSum / this->ReprojectionError3DList.size();
  this->ReprojectionError3DStdDev = sqrt(variance);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkProbeCalibrationAlgo::ComputeReprojectionErrors2D( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires )
{
	LOG_TRACE("vtkProbeCalibrationAlgo::ComputeReprojectionErrors2D");

  this->ReprojectionError2DList.clear();
  this->SortedReprojectionError2DIndices.clear();
  this->ReprojectionError2DMean.clear();
  this->ReprojectionError2DStdDev.clear();

  bool valid = false;

  for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    TrackedFrame* trackedFrame = validationTrackedFrameList->GetTrackedFrame(frameNumber);
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
      Wire wire = nWires[i/3].Wires[i%3];
      double wireEndPointFrontInPhantomFrame[4] = { wire.EndPointFront[0], wire.EndPointFront[1], wire.EndPointFront[2], 1.0 };
      double wireEndPointBackInPhantomFrame[4] = { wire.EndPointBack[0], wire.EndPointBack[1], wire.EndPointBack[2], 1.0 };
      double wireEndPointFrontInImageFrame[4];
      double wireEndPointBackInImageFrame[4];
      phantomToImageVtkTransformMatrix->MultiplyPoint(wireEndPointFrontInPhantomFrame, wireEndPointFrontInImageFrame);
      phantomToImageVtkTransformMatrix->MultiplyPoint(wireEndPointBackInPhantomFrame, wireEndPointBackInImageFrame);

      double computedPositionInImagePlane[3];
	    double t = 0; // Parametric coordinate along the line

      // Compute intersection of wire and image plane
      if ( ! vtkPlane::IntersectWithLine(wireEndPointFrontInImageFrame, wireEndPointBackInImageFrame, normalVector, origin, t, computedPositionInImagePlane) )
	    {
		    LOG_WARNING("Image plane and wire are parallel!");

        std::vector<double> reprojectionError2D(2, DBL_MAX);
        this->ReprojectionError2DList.push_back( reprojectionError2D );

        continue;
	    }

      double* segmentedPositionInImagePlane = segmentedPointsVtk->GetPoint(i);

      std::vector<double> reprojectionError2D(2);
      reprojectionError2D[0] = fabs( segmentedPositionInImagePlane[0]-computedPositionInImagePlane[0] );
      reprojectionError2D[1] = fabs( segmentedPositionInImagePlane[1]-computedPositionInImagePlane[1] );
      this->ReprojectionError2DList.push_back( reprojectionError2D );
    }

  } // For all frames

  // Create error vector containing the sum of squares of X and Y errors (so that the minimum can be searched)
  std::vector<double> tempReprojectionError2DList( this->ReprojectionError2DList.size() );
  for (int i=0; i<this->ReprojectionError2DList.size(); ++i)
  {
    tempReprojectionError2DList[i] = this->ReprojectionError2DList[i][0] * this->ReprojectionError2DList[i][0] + this->ReprojectionError2DList[i][1] * this->ReprojectionError2DList[i][1];
  }

  // Compute the sorted indices array
  this->SortedReprojectionError2DIndices.resize( this->ReprojectionError2DList.size() );

  std::vector<double>::iterator tempReprojectionError2DListBeginIt = tempReprojectionError2DList.begin();
  for (int i=0; i<this->ReprojectionError2DList.size(); ++i)
  {
    std::vector<double>::iterator reprojectionError2DMinIt = std::min_element(tempReprojectionError2DListBeginIt, tempReprojectionError2DList.end());
    int minIndex = (int)std::distance(tempReprojectionError2DListBeginIt,reprojectionError2DMinIt);
    this->SortedReprojectionError2DIndices[i] = minIndex;
    (*reprojectionError2DMinIt) = DBL_MAX;
  }

  int numberOfTopRankedData = ROUND( (double)this->ReprojectionError2DList.size() * this->ValidationDataConfidenceLevel );

  // Compute mean and standard deviation for X
  double sumX = 0;
  for (int i=0; i<numberOfTopRankedData; ++i)
  {
    sumX += this->ReprojectionError2DList[ this->SortedReprojectionError2DIndices[i] ][0];
  }
  this->ReprojectionError2DMean.push_back( sumX / this->ReprojectionError2DList.size() );
  
  double squareDiffSumX = 0;
  for (int i=0; i<numberOfTopRankedData; ++i)
  {
    double diff = this->ReprojectionError2DList[ this->SortedReprojectionError2DIndices[i] ][0] - this->ReprojectionError2DMean[0];
    squareDiffSumX += diff * diff;
  }
  double varianceX = squareDiffSumX / this->ReprojectionError2DList.size();
  this->ReprojectionError2DStdDev.push_back( sqrt(varianceX) );

  // Compute mean and standard deviation for Y
  double sumY = 0;
  for (int i=0; i<numberOfTopRankedData; ++i)
  {
    sumY += this->ReprojectionError2DList[ this->SortedReprojectionError2DIndices[i] ][1];
  }
  this->ReprojectionError2DMean.push_back( sumY / this->ReprojectionError2DList.size() );
  
  double squareDiffSumY = 0;
  for (int i=0; i<numberOfTopRankedData; ++i)
  {
    double diff = this->ReprojectionError2DList[ this->SortedReprojectionError2DIndices[i] ][1] - this->ReprojectionError2DMean[1];
    squareDiffSumY += diff * diff;
  }
  double varianceY = squareDiffSumY / this->ReprojectionError2DList.size();
  this->ReprojectionError2DStdDev.push_back( sqrt(varianceY) );

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
			matrixStringStream << std::fixed << std::setprecision(precision) << std::setw(precision+3) << std::right << this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j) << " ";
		}

		matrixStringStream << std::endl;
	}

	std::ostringstream errorsStringStream;

  errorsStringStream << std::fixed << std::setprecision(precision) << "3D Reprojection Error" << std::endl << 
    "  Mean: " << this->ReprojectionError3DMean << "mm" << std::endl << 
    "  StdDev: " << this->ReprojectionError3DStdDev << "mm" << std::endl;
  errorsStringStream << std::fixed << std::setprecision(precision) << "2D Reprojection Error" << std::endl << 
    "  Mean: (" << this->ReprojectionError2DMean[0] << "px, " << this->ReprojectionError2DMean[1] << "px)" << std::endl << 
    "  StdDev: (" << this->ReprojectionError2DStdDev[0] << "px, " << this->ReprojectionError2DStdDev[1] << "px)" << std::endl;

	std::ostringstream resultStringStream;
	resultStringStream << matrixStringStream.str() << std::endl << errorsStringStream.str() << std::endl;

	resultStringStream << "Validation confidence: " << (int)(this->ValidationDataConfidenceLevel*100) << "%";

	return resultStringStream.str();
}

//----------------------------------------------------------------------------

bool vtkProbeCalibrationAlgo::IsImageToProbeTransformOrthogonal()
{
	LOG_TRACE("vtkProbeCalibrationAlgo::IsImageToProbeTransformOrthogonal");

  vtkMatrix4x4* imageToProbeMatrix = this->TransformImageToProbe->GetMatrix(); 

  // Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
  double xVector[3] = {imageToProbeMatrix->GetElement(0,0),imageToProbeMatrix->GetElement(1,0),imageToProbeMatrix->GetElement(2,0)}; 
  double yVector[3] = {imageToProbeMatrix->GetElement(0,1),imageToProbeMatrix->GetElement(1,1),imageToProbeMatrix->GetElement(2,1)};  
  double zVector[3] = {imageToProbeMatrix->GetElement(0,2),imageToProbeMatrix->GetElement(1,2),imageToProbeMatrix->GetElement(2,2)};  

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
	probeCalibrationResult->SetName("ProbeCalibrationResult"); 
	probeCalibrationResult->SetAttribute("version", "2.0"); 

  // CalibrationFile
	vtkSmartPointer<vtkXMLDataElement> calibrationFile = vtkSmartPointer<vtkXMLDataElement>::New(); 
	calibrationFile->SetName("CalibrationFile"); 
	calibrationFile->SetAttribute("Timestamp", vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()); 
	calibrationFile->SetAttribute("FileName", calibrationResultFileName.c_str()); 


  // CalibrationResults
	vtkSmartPointer<vtkXMLDataElement> calibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	calibrationResults->SetName("CalibrationResults"); 

  // Image to Probe transform
  double imageToProbeTransformVector[16]={0}; 
  vtkMatrix4x4::DeepCopy(imageToProbeTransformVector, this->TransformImageToProbe->GetMatrix()); 

  vtkSmartPointer<vtkXMLDataElement> imageToProbeTransformElement = vtkSmartPointer<vtkXMLDataElement>::New();
  imageToProbeTransformElement->SetName("Transform"); 
  imageToProbeTransformElement->SetAttribute("From", this->ImageCoordinateFrame); 
  imageToProbeTransformElement->SetAttribute("To", this->ProbeCoordinateFrame); 
  imageToProbeTransformElement->SetVectorAttribute("Matrix", 16, imageToProbeTransformVector); 

  calibrationResults->AddNestedElement( imageToProbeTransformElement );


  // Error report
	vtkSmartPointer<vtkXMLDataElement> errorReport = vtkSmartPointer<vtkXMLDataElement>::New(); 
	errorReport->SetName("ErrorReport");
  errorReport->SetDoubleAttribute("ReprojectionError3DMeanMm", this->ReprojectionError3DMean); 
  errorReport->SetDoubleAttribute("ReprojectionError3DStdDevMm", this->ReprojectionError3DStdDev); 
  double mean2D[2] = { this->ReprojectionError2DMean[0], this->ReprojectionError2DMean[1] };
  double stddev2D[2] = { this->ReprojectionError2DStdDev[0], this->ReprojectionError2DStdDev[1] };
  errorReport->SetVectorAttribute("ReprojectionError2DMeanPx", 2, mean2D);
  errorReport->SetVectorAttribute("ReprojectionError2DStdDevPx", 2, stddev2D);

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
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedPoints->AddNestedElement( pointElement );
	  }

    // ReprojectionError3DList
	  vtkSmartPointer<vtkXMLDataElement> reprojectionError3DList = vtkSmartPointer<vtkXMLDataElement>::New(); 
	  reprojectionError3DList->SetName("ReprojectionError3DList");

    for (int i = numberOfSegmentedFramesSoFar*numberOfSegmentedPoints/3; i < (numberOfSegmentedFramesSoFar+1)*numberOfSegmentedPoints/3; ++i)
    {
	    vtkSmartPointer<vtkXMLDataElement> reprojectionError3D = vtkSmartPointer<vtkXMLDataElement>::New(); 
	    reprojectionError3D->SetName("ReprojectionError3D");
      reprojectionError3D->SetDoubleAttribute("ErrorMm", this->ReprojectionError3DList[i]);
      reprojectionError3DList->AddNestedElement( reprojectionError3D );
    }

    // ReprojectionError2DList
	  vtkSmartPointer<vtkXMLDataElement> reprojectionError2DList = vtkSmartPointer<vtkXMLDataElement>::New(); 
	  reprojectionError2DList->SetName("ReprojectionError2DList");

    for (int i = numberOfSegmentedFramesSoFar*numberOfSegmentedPoints; i < (numberOfSegmentedFramesSoFar+1)*numberOfSegmentedPoints; ++i)
    {
      double reprojectionError2D[2] = { this->ReprojectionError2DList[i][0], this->ReprojectionError2DList[i][1] };
	    vtkSmartPointer<vtkXMLDataElement> reprojectionError2DElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
	    reprojectionError2DElement->SetName("ReprojectionError2D");
      reprojectionError2DElement->SetVectorAttribute("ErrorPx", 2, reprojectionError2D);
      reprojectionError2DList->AddNestedElement( reprojectionError2DElement );
    }

    frame->AddNestedElement( segmentedPoints );
    frame->AddNestedElement( reprojectionError3DList );
    frame->AddNestedElement( reprojectionError2DList );

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

    frame->AddNestedElement( segmentedPoints );
    frame->AddNestedElement( middleWires );

    calibrationData->AddNestedElement( frame );

    numberOfSegmentedFramesSoFar++;
  }

  errorReport->AddNestedElement( validationData );
  errorReport->AddNestedElement( calibrationData );


  probeCalibrationResult->AddNestedElement( calibrationFile );
  probeCalibrationResult->AddNestedElement( calibrationResults );
  probeCalibrationResult->AddNestedElement( errorReport );

  probeCalibrationResult->PrintXML(calibrationResultFileNameWithPath.c_str()); 

  return PLUS_SUCCESS;
}

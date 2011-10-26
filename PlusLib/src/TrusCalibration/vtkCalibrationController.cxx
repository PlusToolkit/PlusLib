/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkCalibrationController.h"

#include "PlusMath.h"
#include "vtkTracker.h"

#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkMatrix4x4.h"
#include "vtkDirectory.h"
#include "vtkImageImport.h"
#include "vtkXMLUtilities.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <itkImageDuplicator.h>
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkTriangle.h"
#include "vtkPlane.h"
#include "vtkLine.h"
#include "vtksys/SystemTools.hxx"
#include "PlusVideoFrame.h"

#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"

#include "vnl/vnl_cross.h"

vtkCxxRevisionMacro(vtkCalibrationController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkCalibrationController);

//----------------------------------------------------------------------------
void vtkCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
vtkCalibrationController::vtkCalibrationController() 
  : SegmentationProgressCallbackFunction(NULL)
  , MinElevationBeamwidthAndFocalZoneInUSImageFrame(2,0)
{
  this->EnableSegmentationAnalysisOff();
  this->InitializedOff(); 
  this->CalibrationDoneOff(); 

  this->CalibrationDate = NULL; 
  this->CalibrationTimestamp = NULL; 
  
	for ( int i = 0; i < NUMBER_OF_IMAGE_DATA_TYPES; i++ )
	{
		vtkTrackedFrameList *trackedFrameList = vtkTrackedFrameList::New();
    trackedFrameList->SetDefaultFrameTransformName("Probe"); // TODO change this when plus#310 and plus#311 are done (it will come from the config file)
		this->TrackedFrameListContainer.push_back(trackedFrameList); 

		ImageDataInfo imageDataInfo; 
		imageDataInfo.NumberOfImagesToAcquire = 0; 
		imageDataInfo.NumberOfSegmentedImages = 0; 
		imageDataInfo.OutputSequenceMetaFileSuffix.clear(); 
    imageDataInfo.InputSequenceMetaFileName.clear(); 
		ImageDataInfoContainer.push_back(imageDataInfo); 
	}

	this->US3DBeamwidthDataReadyOff();
  this->NumUS3DBeamwidthProfileData = -1;

  this->SetCenterOfRotationPx(0,0);

	this->SetCurrentPRE3DdistributionID(0); 

	this->SetIncorporatingUS3DBeamProfile(0);

	this->SetAxialPositionOfCrystalSurfaceInTRUSImageFrame(-1);

	this->CalibrationConfigFileNameWithPath = NULL;
  this->CalibrationResultFileNameWithPath = NULL;
	this->US3DBeamProfileDataFileNameAndPath = NULL; 
	this->SegmentationAnalysisFileNameWithTimeStamp = NULL; 
	this->SegmentationErrorLogFileNameWithTimeStamp = NULL;
	this->CalibrationResultFileSuffix = NULL;
	
	vtkSmartPointer<vtkTransform> transformProbeToReference = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformProbeToReference = NULL;
	this->SetTransformProbeToReference(transformProbeToReference); 

	vtkSmartPointer<vtkTransform> transformReferenceToTemplateHolderHome = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformReferenceToTemplateHolderHome = NULL;
	this->SetTransformReferenceToTemplateHolderHome(transformReferenceToTemplateHolderHome); 

	vtkSmartPointer<vtkTransform> transformTemplateHolderToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHolderToTemplate = NULL;
	this->SetTransformTemplateHolderToTemplate(transformTemplateHolderToTemplate); 

  vtkSmartPointer<vtkTransform> transformTemplateHolderToPhantom = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHolderToPhantom = NULL;
	this->SetTransformTemplateHolderToPhantom(transformTemplateHolderToPhantom); 

	vtkSmartPointer<vtkTransform> transformUserImageToProbe = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformUserImageToProbe = NULL;
	this->SetTransformUserImageToProbe(transformUserImageToProbe); 

	vtkSmartPointer<vtkTransform> transformImageToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToTemplate = NULL;
	this->SetTransformImageToTemplate(transformImageToTemplate); 

	vtkSmartPointer<vtkTransform> transformImageToUserImage = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToUserImage = NULL;
	this->SetTransformImageToUserImage(transformImageToUserImage); 

	vtkSmartPointer<vtkTransform> transformTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHomeToTemplate = NULL;
	this->SetTransformTemplateHomeToTemplate(transformTemplateHomeToTemplate); 

  this->TransformImageToTemplate->Identity(); 
  this->TransformImageToTemplate->PostMultiply(); 
  this->TransformImageToTemplate->Concatenate(this->TransformImageToUserImage); 
  this->TransformImageToTemplate->Concatenate(this->TransformUserImageToProbe); 
  this->TransformImageToTemplate->Concatenate(this->TransformProbeToReference); 
  this->TransformImageToTemplate->Concatenate(this->TransformReferenceToTemplateHolderHome); 
  this->TransformImageToTemplate->Concatenate(this->TransformTemplateHolderToTemplate);
  this->TransformImageToTemplate->Update(); 

	// Initialize data containers
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_size(0,0);
	this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_size(0,0); 
	this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);

  // Former Phantom class
	mIsUSBeamwidthAndWeightFactorsTableReady = false;

  mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);
	mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN.set_size(0,0);
	mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.set_size(2);
	mUS3DBeamwidthAtNearestAxialDepth.set_size(3);
	mUS3DBeamwidthAtFarestAxialDepth.set_size(3);
  mValidationDataConfidenceLevel = 0.95;
  mOutlierDetectionThreshold = 3.0;

  // 1. We set the maximum tolerance to be the number of times of the current 
	//    minimum magnitude of the US beamwidth typically measured at the 
	//    elevation plane's focal zone. 
	// 2. According to ultrasound physics, the beamwidth loses most of its
	//    resolution after grows to two times that of at focal zone. So 10% 
	//    above the twice of the minimum beamwidth may serve a good cutoff
	//    point to quality control the imaging data for a reliable calibration.
  mNumOfTimesOfMinBeamWidth = 2.1;


  this->PhantomToReferenceTransform = NULL;
}

//----------------------------------------------------------------------------
vtkCalibrationController::~vtkCalibrationController() 
{
	for ( int i = 0; i < NUMBER_OF_IMAGE_DATA_TYPES; i++ )
	{
		if ( this->TrackedFrameListContainer[i] != NULL )
		{
			this->TrackedFrameListContainer[i]->Delete();  
			this->TrackedFrameListContainer[i] = NULL; 
		}
	}

  // Former ProbeCalibrationController and FreehandCalibraitonController members
	this->SetTransformImageToTemplate(NULL);
	this->SetTransformImageToUserImage(NULL);
	this->SetTransformUserImageToProbe(NULL);
	this->SetTransformProbeToReference(NULL);
	this->SetTransformReferenceToTemplateHolderHome(NULL);
	this->SetTransformTemplateHolderToTemplate(NULL);
  this->SetTransformTemplateHolderToPhantom(NULL); 
	this->SetTransformTemplateHomeToTemplate(NULL);

  if ( this->PhantomToReferenceTransform != NULL )
  {
    this->PhantomToReferenceTransform->Delete(); 
    this->PhantomToReferenceTransform = NULL; 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::Initialize()
{
	LOG_TRACE("vtkCalibrationController::Initialize"); 

  this->ResetDataContainers(); 

  // Initialize the calibration component
  if (ComputeNWireInstersections() != PLUS_SUCCESS) {
    LOG_ERROR("Computing intersections of NWire wires failed!");
    return PLUS_FAIL;
  }

	// This will pass the US 3D beamwidth data and their predefined
	// weights to the calibration component.
	if( this->US3DBeamwidthDataReady )
	{
		this->FillUltrasoundBeamwidthAndWeightFactorsTable();
	}

	this->InitializedOn(); 

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::Calibrate( vtkTrackedFrameList* validationTrackedFrameList, vtkTrackedFrameList* calibrationTrackedFrameList, const char* defaultTransformName )
{
	LOG_TRACE("vtkCalibrationController::Calibrate");

  return Calibrate(validationTrackedFrameList, -1, -1, calibrationTrackedFrameList, -1, -1, defaultTransformName);
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::Calibrate( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, const char* defaultTransformName )
{
  LOG_TRACE("vtkCalibrationController::Calibrate(validation: " << validationStartFrame << "-" << validationEndFrame << ", calibration: " << calibrationStartFrame << "-" << calibrationEndFrame << ")"); 

  if ( ! this->Initialized )
  {
	  if ( Initialize() != PLUS_SUCCESS )
    {
      LOG_ERROR("Initialization failed!");
      return PLUS_FAIL;
    }
  }

  this->ResetDataContainers(); 

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

  // Add tracked frames for calibration and validation
	for (int frameNumber = validationStartFrame; frameNumber < validationEndFrame; ++frameNumber)
  {
    LOG_DEBUG(" Add frame #" << frameNumber << " for validation data");
    if ( AddPositionsPerImage(validationTrackedFrameList->GetTrackedFrame(frameNumber), defaultTransformName, true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add validation position failed on frame #" << frameNumber);
      continue;
    }
  }		

	for (int frameNumber = calibrationStartFrame; frameNumber < calibrationEndFrame; ++frameNumber)
  {
    LOG_DEBUG(" Add frame #" << frameNumber << " for calibration data");
    if ( AddPositionsPerImage(calibrationTrackedFrameList->GetTrackedFrame(frameNumber), defaultTransformName, false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Add calibration position failed on frame #" << frameNumber);
      continue;
    }
  }

  // Compute calibration results
  if (ComputeCalibrationResults() != PLUS_SUCCESS)
  {
    LOG_ERROR("Computing calibration results failed!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::AddPositionsPerImage( TrackedFrame* trackedFrame, const char* defaultTransformName, bool isValidation )
{
  LOG_TRACE("vtkCalibrationController::AddPositionsPerImage(" << (isValidation?"validation":"calibration") << ")");

  if (this->TransformImageToUserImage == NULL)
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

  if (segmentedPointsVtk->GetNumberOfPoints() % 3 != 0)
  {
    LOG_ERROR("Frame does not contain N-Wires only!");
    return PLUS_FAIL;
  }

  if (segmentedPointsVtk->GetNumberOfPoints() == 0)
  {
    LOG_DEBUG("Segmentation failed on frame, so it will be ignored");
    return PLUS_SUCCESS;
  }

  // Assemble matrices and add them to the calibration input
  double probeToReferenceTransformVector[16]; 
  if ( trackedFrame->GetCustomFrameTransform(defaultTransformName, probeToReferenceTransformVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Cannot get frame transform '" << defaultTransformName << "' from tracked frame!");
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
  vnl_matrix<double> imageToUserImageTransformMatrix(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToUserImage->GetMatrix(), imageToUserImageTransformMatrix); 

  // Convert phantom registration matrix to vnl
	vnl_matrix<double> phantomToReferenceTransformMatrix(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(this->PhantomToReferenceTransform->GetMatrix(), phantomToReferenceTransformMatrix);

  // Get reference to probe transform
  vtkSmartPointer<vtkMatrix4x4> probeToReferenceVtkTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  probeToReferenceVtkTransformMatrix->DeepCopy(probeToReferenceTransformVector); 
  vnl_matrix<double> probeToReferenceTransformMatrix(4,4);

  PlusMath::ConvertVtkMatrixToVnlMatrix(probeToReferenceVtkTransformMatrix, probeToReferenceTransformMatrix); 

  vnl_matrix_inverse<double> inverseMatrix(probeToReferenceTransformMatrix);
  vnl_matrix<double> referenceToProbeTransformMatrix = inverseMatrix.inverse();

  // Make sure the last row in homogeneous transform is [0 0 0 1]
  vnl_vector<double> lastRow(4,0);
  lastRow.put(3, 1);
  referenceToProbeTransformMatrix.set_row(3, lastRow);


  // Calculate wire position in probe coordinate system using the segmentation result and the phantom geometry
  std::vector<NWire> nWires = this->PatternRecognition.GetFidLineFinder()->GetNWires();

  for (int n = 0; n < segmentedPoints.size() / 3; ++n)
  {
    // Convert the segmented position of the middle wire from the original image to the predefined ultrasound image frame
    vnl_vector<double> middleWirePositionInUserImageFrame = imageToUserImageTransformMatrix * segmentedPoints[n*3 + 1];

    // Add weights to the positions if required (see mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM member description)
    if ( true == mIsUSBeamwidthAndWeightFactorsTableReady && !isValidation )
    {
      // Get and round the axial depth in the US Image Frame for the segmented data point (in pixels and along the Y-axis)
      const int axialDepthInActualImageFrameRounded = floor( middleWirePositionInUserImageFrame.get(1) + 0.5 );
      mWeightsForDataPositions.push_back( GetBeamwidthWeightForBeamwidthMagnitude(axialDepthInActualImageFrameRounded) );
    }

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
      intersectPosW12[i] = nWires[n].intersectPosW12[i];
      intersectPosW32[i] = nWires[n].intersectPosW32[i];
    }

    intersectPosW12[3] = 1.0;
    intersectPosW32[3] = 1.0;

    positionInPhantomFrame = intersectPosW12 + alpha * ( intersectPosW32 - intersectPosW12 );
    positionInPhantomFrame[3] = 1.0;

    // Compute middle point position in probe frame
    vnl_vector<double> positionInProbeFrame = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix * positionInPhantomFrame;

    LOG_DEBUG("Segmented point #" << n*3 << " = " << segmentedPoints[n*3]);
    LOG_DEBUG("Segmented point #" << n*3+1 << " = " << segmentedPoints[n*3+1]);
    LOG_DEBUG("Segmented point #" << n*3+2 << " = " << segmentedPoints[n*3+2]);
    LOG_DEBUG("Middle wire position in user image frame = " << middleWirePositionInUserImageFrame);
    LOG_DEBUG("Alpha = " << alpha);
    LOG_DEBUG("Middle wire position in phantom frame = " << positionInPhantomFrame);
    LOG_DEBUG("Reference to probe transform = \n" << referenceToProbeTransformMatrix);
    LOG_DEBUG("Middle wire position in probe frame = " << positionInProbeFrame << "\n\n");

    if (!isValidation)
    {
      // Store into the list of positions in the image frame and the probe frame
      mDataPositionsInUSImageFrame.push_back( middleWirePositionInUserImageFrame );
      mDataPositionsInUSProbeFrame.push_back( positionInProbeFrame );
    }
    else
    { //TODO clean up
      vnl_vector<double> NWireStartinUSProbeFrame = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix * intersectPosW12;
      vnl_vector<double> NWireEndinUSProbeFrame = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix * intersectPosW32;

      // The parallel wires position in US Probe frame 
      // 1. Parallel wires share the same X, Y coordinates as the N-wire joints
      //    in the phantom (template) frame.
      // 2. The Z-axis of the N-wire joints is not used in the computing.

      // Wire N1 corresponds to mNWireJointTopLayerBackWall 
      vnl_vector<double> NWireJointForN1InUSProbeFrame = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix * intersectPosW12; //any point of wire 1 of this layer

      // Wire N3 corresponds to mNWireJointTopLayerFrontWall
      vnl_vector<double> NWireJointForN3InUSProbeFrame = referenceToProbeTransformMatrix * phantomToReferenceTransformMatrix * intersectPosW32; //any point of wire 3 of this layer

      // Store into the list of positions in the US image frame
      mValidationPositionsInUSImageFrame.push_back( middleWirePositionInUserImageFrame );

      // Store into the list of positions in the US probe frame
      mValidationPositionsInUSProbeFrame.push_back( positionInProbeFrame );
      mValidationPositionsNWireStartInUSProbeFrame.push_back( NWireStartinUSProbeFrame );
      mValidationPositionsNWireEndInUSProbeFrame.push_back( NWireEndinUSProbeFrame );

      for (int i=0; i<2; i++)
      {
        // all the matrices are expected to have the same length, so we need to add each value 
        // as many times as many layer we have - this really have to be cleaned up
        if (n==0)
        {
          // Collect the wire locations (the two parallel wires of 
          // each of the N-shape) for independent Line-Reconstruction 
          // Error (LRE) validation.
          // Note: N1, N3, N4, and N6 are the parallel wires here.
          vnl_vector<double> N1SegmentedPositionInOriginalImageFrame( segmentedPoints[0] );
          vnl_vector<double> N3SegmentedPositionInOriginalImageFrame( segmentedPoints[2] );

          // Convert the segmented image positions from the original image to the predefined ultrasound image frame.
          vnl_vector<double> N1SegmentedPositionInUserImageFrame = imageToUserImageTransformMatrix * N1SegmentedPositionInOriginalImageFrame;
          vnl_vector<double> N3SegmentedPositionInUserImageFrame = imageToUserImageTransformMatrix * N3SegmentedPositionInOriginalImageFrame;

          mValidationPositionsNWire1InUSImageFrame.push_back( N1SegmentedPositionInUserImageFrame );
          mValidationPositionsNWire3InUSImageFrame.push_back( N3SegmentedPositionInUserImageFrame );
          mValidationPositionsNWire1InUSProbeFrame.push_back( NWireJointForN1InUSProbeFrame );
          mValidationPositionsNWire3InUSProbeFrame.push_back( NWireJointForN3InUSProbeFrame );
        }
        else
        {
          vnl_vector<double> N4SegmentedPositionInOriginalImageFrame( segmentedPoints[3] );
          vnl_vector<double> N6SegmentedPositionInOriginalImageFrame( segmentedPoints[5] );

          vnl_vector<double> N4SegmentedPositionInUserImageFrame = imageToUserImageTransformMatrix * N4SegmentedPositionInOriginalImageFrame;
          vnl_vector<double> N6SegmentedPositionInUserImageFrame = imageToUserImageTransformMatrix * N6SegmentedPositionInOriginalImageFrame;

          mValidationPositionsNWire4InUSImageFrame.push_back( N4SegmentedPositionInUserImageFrame );
          mValidationPositionsNWire6InUSImageFrame.push_back( N6SegmentedPositionInUserImageFrame );
          mValidationPositionsNWire4InUSProbeFrame.push_back( NWireJointForN1InUSProbeFrame );
          mValidationPositionsNWire6InUSProbeFrame.push_back( NWireJointForN3InUSProbeFrame );
        }
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkTrackedFrameList* vtkCalibrationController::GetTrackedFrameList( IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::GetTrackedFrameList"); 
	return this->TrackedFrameListContainer[dataType]; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkCalibrationController::ReadConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to read configuration"); 
		return PLUS_FAIL; 
	}

  // Setting the fiducial pattern recognition
  this->PatternRecognition.ReadConfiguration(configData);

	// Tracked frame specifications
	//********************************************************************
  std::vector<vtkTrackedFrameList*>::iterator it;
  for (it = this->TrackedFrameListContainer.begin(); it != this->TrackedFrameListContainer.end(); ++it)
  {
	  if ((*it)->ReadConfiguration(configData) != PLUS_SUCCESS)
	  {
      LOG_ERROR("TrackedFrameList configuration cannot be read!");
	    return PLUS_FAIL;
	  }
  }

	// Calibration controller specifications
	//********************************************************************
	if (this->ReadCalibrationControllerConfiguration(configData) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadCalibrationControllerConfiguration( vtkXMLDataElement* rootElement )
{
	LOG_TRACE("vtkCalibrationController::ReadCalibrationControllerConfiguration"); 

  if ( rootElement == NULL) 
	{
		LOG_WARNING("Invalid root XML configuration data element!"); 
		return PLUS_FAIL; 
	}

	vtkSmartPointer<vtkXMLDataElement> usCalibration = rootElement->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL)
  {
    LOG_ERROR("Cannot find USCalibration element in XML tree!");
    return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	if (calibrationController == NULL)
  {
    LOG_ERROR("Cannot find CalibrationController element in XML tree!");
    return PLUS_FAIL;
  }

	// Path to output calibration results
	const char* outputPath = calibrationController->GetAttribute("OutputPath"); 
	if ( outputPath != NULL) 
	{
    std::string fullOutputPath = vtksys::SystemTools::CollapseFullPath(outputPath, vtkPlusConfig::GetInstance()->GetProgramDirectory()); 
		vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
		if ( dir->Open(fullOutputPath.c_str()) == 0 ) 
		{	
			dir->MakeDirectory(fullOutputPath.c_str()); 
		}
    vtkPlusConfig::GetInstance()->SetOutputDirectory(fullOutputPath.c_str()); 
	}
	else
	{
		// Set to the current working directory
		vtkPlusConfig::GetInstance()->SetOutputDirectory(vtksys::SystemTools::GetCurrentWorkingDirectory().c_str()); 
	}
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ReadProbeCalibrationConfiguration( vtkXMLDataElement* rootElement )
{
	LOG_TRACE("vtkCalibrationController::ReadProbeCalibrationConfiguration");

	if (rootElement == NULL) 
	{	
		LOG_WARNING("Unable to read ProbeCalibration XML data element!"); 
		return PLUS_FAIL; 
	}

	vtkSmartPointer<vtkXMLDataElement> usCalibration = rootElement->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL)
  {
    LOG_ERROR("Cannot find USCalibration element in XML tree!");
    return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	if (calibrationController == NULL)
  {
    LOG_ERROR("Unable to find calibration controller tag in configuration file!"); 
    return PLUS_FAIL; 
  }

	vtkSmartPointer<vtkXMLDataElement> probeCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration"); 
	if (probeCalibration == NULL)
  {
    LOG_ERROR("Unable to find probe calibration tag in configuration file!"); 
    return PLUS_FAIL; 
  }

	//Transform: from image home position to user defined image home position
  double imageToUserImageTransform[16] = {0}; 
	if ( probeCalibration->GetVectorAttribute("TransformImageToUserImage", 16, imageToUserImageTransform) )
	{
		this->TransformImageToUserImage->SetMatrix(imageToUserImageTransform); 
	}

	// Sets the suffix of the calibration result file
	const char* calibrationResultFileSuffix = probeCalibration->GetAttribute("CalibrationResultFileSuffix"); 
	if ( calibrationResultFileSuffix != NULL) 
	{
		this->SetCalibrationResultFileSuffix(calibrationResultFileSuffix); 
	}
	else 
	{
		this->SetCalibrationResultFileSuffix(".Calibration.results"); 
	}

  // RandomStepperMotionData1 data set specifications
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData1 = probeCalibration->FindNestedElementWithName("RandomStepperMotionData1"); 
	if ( randomStepperMotionData1 != NULL) 
	{
		int numberOfImagesToUse = -1;
		if ( randomStepperMotionData1->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			this->ImageDataInfoContainer[RANDOM_STEPPER_MOTION_1].NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = randomStepperMotionData1->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			this->ImageDataInfoContainer[RANDOM_STEPPER_MOTION_1].OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}
	}
	else
	{
		LOG_DEBUG("Unable to find RandomStepperMotionData1 XML data element, default 200 is used"); 
		this->ImageDataInfoContainer[RANDOM_STEPPER_MOTION_1].NumberOfImagesToAcquire = 200;
	}

	// RandomStepperMotionData2 data set specifications
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData2 = probeCalibration->FindNestedElementWithName("RandomStepperMotionData2"); 
	if ( randomStepperMotionData2 != NULL) 
	{
		int numberOfImagesToUse = -1;
		if ( randomStepperMotionData2->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			this->ImageDataInfoContainer[RANDOM_STEPPER_MOTION_2].NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = randomStepperMotionData2->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			this->ImageDataInfoContainer[RANDOM_STEPPER_MOTION_2].OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}
	}
	else
	{
		LOG_DEBUG("Unable to find RandomStepperMotionData2 XML data element, default 100 is used"); 
		this->ImageDataInfoContainer[RANDOM_STEPPER_MOTION_2].NumberOfImagesToAcquire = 100; 
	}

	// US3DBeamwidth specifications
	vtkSmartPointer<vtkXMLDataElement> us3DBeamProfile = probeCalibration->FindNestedElementWithName("US3DBeamProfile"); 
	if ( us3DBeamProfile != NULL) 
	{
		// To incorporate the ultrasound beam profile (3D beam width)
		int incorporatingUS3DBeamProfile = 0;
		if ( us3DBeamProfile->GetScalarAttribute("IncorporatingUS3DBeamProfile", incorporatingUS3DBeamProfile) ) 
		{
			this->SetIncorporatingUS3DBeamProfile(incorporatingUS3DBeamProfile); 
		}

		// The US-3D-beam-profile data file name and path (if choose to incorporate the beam profile)
		const char* configFile = us3DBeamProfile->GetAttribute("ConfigFile"); 
		if ( configFile != NULL) 
		{
			this->SetUS3DBeamProfileDataFileNameAndPath(configFile); 
		}

		if( this->GetIncorporatingUS3DBeamProfile() > 0 && this->GetIncorporatingUS3DBeamProfile() < 4 )
		{
			this->LoadUS3DBeamProfileData();
		}
	}
	else
	{
		LOG_WARNING("Unable to find US3DBeamProfile XML data element"); 
	}

  // CalibrationResult specifications
  vtkSmartPointer<vtkXMLDataElement> calibrationResult = probeCalibration->FindNestedElementWithName("CalibrationResult"); 

  if ( calibrationResult != NULL )
  {
    // Read calibration date
    const char* calibrationDate = calibrationResult->GetAttribute("Date"); 
    if ( calibrationDate != NULL )
    {
      this->SetCalibrationDate(calibrationDate); 
    }

    //Image center of rotation in pixels
    double centerOfRotationPx[2] = {0}; 
    if ( calibrationResult->GetVectorAttribute("CenterOfRotationPx", 2, centerOfRotationPx) )
    {
      this->SetCenterOfRotationPx(centerOfRotationPx); 
    }

    double phantomToProbeDistanceInMm[2] = {0}; 
    if ( calibrationResult->GetVectorAttribute("PhantomToProbeDistanceInMm", 2, phantomToProbeDistanceInMm) )
    {
      this->SetPhantomToProbeDistanceInMm(phantomToProbeDistanceInMm); 
    }

    // TransformUserImageHomeToProbeHome
    double transformUserImageToProbe[16] = {0}; 
    if ( calibrationResult->GetVectorAttribute("TransformUserImageToProbe", 16, transformUserImageToProbe) )
    {
      this->TransformUserImageToProbe->SetMatrix(transformUserImageToProbe); 
    }

    // TransformProbeHomeToTemplateHolderHome
    double tReferenceToTemplateHolderHome[16] = {0}; 
    if ( calibrationResult->GetVectorAttribute("TransformReferenceToTemplateHolderHome", 16, tReferenceToTemplateHolderHome) )
    {
      this->TransformReferenceToTemplateHolderHome->SetMatrix(tReferenceToTemplateHolderHome); 
    }

    // TransformTemplateHolderHomeToTemplateHome
    double transformReferenceToTemplateHolderHome[16] = {0}; 
    if ( calibrationResult->GetVectorAttribute("TransformReferenceToTemplateHolderHome", 16, transformReferenceToTemplateHolderHome) )
    {
      this->TransformReferenceToTemplateHolderHome->SetMatrix(transformReferenceToTemplateHolderHome); 
    }

    // TransformImageHomeToUserImageHome
    double transformImageToUserImage[16] = {0}; 
    if ( calibrationResult->GetVectorAttribute("TransformImageToUserImage", 16, transformImageToUserImage) )
    {
      this->TransformImageToUserImage->SetMatrix(transformImageToUserImage); 
    }

    // Update LRE values
    this->LineReconstructionErrors.clear();
    
    // Add wire #1 LRE to map
    double LRE_w1[7]={0}; 
    if ( calibrationResult->GetVectorAttribute("LRE-W1", 7, LRE_w1) )
    {
      std::vector<double> vectorLRE_w1;
      for ( int i = 0; i < 7; ++i )
      {
        vectorLRE_w1.push_back(LRE_w1[i]); 
      }

      this->LineReconstructionErrors[1] = vectorLRE_w1; 
    }

    // Add wire #3 LRE to map
    double LRE_w3[7]={0}; 
    if ( calibrationResult->GetVectorAttribute("LRE-W3", 7, LRE_w3) )
    {
      std::vector<double> vectorLRE_w3;
      for ( int i = 0; i < 7; ++i )
      {
        vectorLRE_w3.push_back(LRE_w3[i]); 
      }

      this->LineReconstructionErrors[3] = vectorLRE_w3; 
    }

    // Add wire #4 LRE to map
    double LRE_w4[7]={0}; 
    if ( calibrationResult->GetVectorAttribute("LRE-W4", 7, LRE_w4) )
    {
      std::vector<double> vectorLRE_w4;
      for ( int i = 0; i < 7; ++i )
      {
        vectorLRE_w4.push_back(LRE_w4[i]); 
      }

      this->LineReconstructionErrors[4] = vectorLRE_w4; 
    }

    // Add wire #6 LRE to map
    double LRE_w6[7]={0}; 
    if ( calibrationResult->GetVectorAttribute("LRE-W6", 7, LRE_w6) )
    {
      std::vector<double> vectorLRE_w6;
      for ( int i = 0; i < 7; ++i )
      {
        vectorLRE_w6.push_back(LRE_w6[i]); 
      }

      this->LineReconstructionErrors[6] = vectorLRE_w6; 
    }

    this->CalibrationDoneOn(); 

  }

	// Custom transforms
	vtkSmartPointer<vtkXMLDataElement> phantomDefinition = rootElement->FindNestedElementWithName("PhantomDefinition");
	if (phantomDefinition == NULL)
  {
		LOG_ERROR("No phantom definition is found in the XML tree!");
		return PLUS_FAIL;
	}
	vtkSmartPointer<vtkXMLDataElement> customTransforms = phantomDefinition->FindNestedElementWithName("CustomTransforms"); 
	if (customTransforms == NULL) 
  {
		LOG_ERROR("Custom transforms are not found in phantom model");
    return PLUS_FAIL;
	}
  
  double templateHolderToPhantomTransformVector[16]={0}; 
  if (customTransforms->GetVectorAttribute("TemplateHolderToPhantomTransform", 16, templateHolderToPhantomTransformVector)) 
  {
    vtkSmartPointer<vtkTransform> transformTemplateHolderToPhantom = vtkSmartPointer<vtkTransform>::New(); 
    transformTemplateHolderToPhantom->SetMatrix(templateHolderToPhantomTransformVector); 
    this->SetTransformTemplateHolderToPhantom( transformTemplateHolderToPhantom  ); 
  }
  else
	{
		LOG_ERROR("Unable to read template origin from template holder from template model file!"); 
	}

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ReadFreehandCalibrationConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkCalibrationController::ReadFreehandCalibrationConfiguration"); 

  // Read probe calibration
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_PROBE, toolType);

  vtkSmartPointer<vtkXMLDataElement> probeDefinition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType.c_str());
	if (probeDefinition == NULL) {
		LOG_ERROR("No probe definition is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = probeDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in probe definition!");
		return PLUS_FAIL;
	}

  // Read calibration matrix
	double* userImageToProbeTransformVector = new double[16]; 
	if (calibration->GetVectorAttribute("MatrixValue", 16, userImageToProbeTransformVector)) {
    vtkSmartPointer<vtkTransform> userImageToProbeTransform = vtkSmartPointer<vtkTransform>::New();
    userImageToProbeTransform->Identity();
    userImageToProbeTransform->SetMatrix(userImageToProbeTransformVector);
    this->SetTransformUserImageToProbe(userImageToProbeTransform);
	}
	delete[] userImageToProbeTransformVector;

  // Read calibration date
  const char* date = calibration->GetAttribute("Date");
  if ((date != NULL) && (STRCASECMP(date, "") != 0)) {
    this->SetCalibrationDate(date);
  }

  // Find and load calibration configuration
	vtkSmartPointer<vtkXMLDataElement> usCalibration = aConfig->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
		LOG_ERROR("No calibration configuration is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	if (calibrationController == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}

	// Probe Calibration specifications
	vtkSmartPointer<vtkXMLDataElement> probeCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration");
	if (probeCalibration == NULL) {	
		LOG_WARNING("Unable to read ProbeCalibration XML data element!"); 
		return PLUS_FAIL; 
	} 

	// Sets the suffix of the calibration result file
	const char* calibrationResultFileSuffix = probeCalibration->GetAttribute("CalibrationResultFileSuffix"); 
	if ( calibrationResultFileSuffix != NULL) {
		this->SetCalibrationResultFileSuffix(calibrationResultFileSuffix); 
	} else {
		this->SetCalibrationResultFileSuffix(".Calibration.results"); 
	}
  
  // Freehand Calibration specifications
	vtkSmartPointer<vtkXMLDataElement> freehandCalibration = calibrationController->FindNestedElementWithName("FreehandCalibration");
	if (freehandCalibration == NULL) {	
		LOG_WARNING("Unable to read FreehandCalibration XML data element!"); 
		return PLUS_FAIL; 
	} 

	// FreehandMotionData2 data set specifications
	vtkSmartPointer<vtkXMLDataElement> freehandMotionData_2 = freehandCalibration->FindNestedElementWithName("FreehandMotionData2"); 
	if (freehandMotionData_2 != NULL) {
		ImageDataInfo imageDataInfo = this->GetImageDataInfo(FREEHAND_MOTION_2);
		int numberOfImagesToUse = -1;
		if (freehandMotionData_2->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse)) {
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse;
    }

    if (imageDataInfo.NumberOfImagesToAcquire == 0) {
      LOG_ERROR("Unable to read NumberOfImagesToAcquire XML data element (or zero)");
      return PLUS_FAIL;
    }

    const char* sequenceMetaFile = freehandMotionData_2->GetAttribute("OutputSequenceMetaFileSuffix"); 
    if ( sequenceMetaFile != NULL) 
    {
      imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
    }

		this->SetImageDataInfo(FREEHAND_MOTION_2, imageDataInfo); 
	} else {
		LOG_WARNING("Unable to find RandomStepperMotionData2 XML data element"); 
	}

	// FreehandMotionData1 data set specifications
	vtkSmartPointer<vtkXMLDataElement> freehandMotionData_1 = freehandCalibration->FindNestedElementWithName("FreehandMotionData1"); 
	if (freehandMotionData_1 != NULL) {
		ImageDataInfo imageDataInfo = this->GetImageDataInfo(FREEHAND_MOTION_1);
		int numberOfImagesToUse = -1;
		if (freehandMotionData_1->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse)) {
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

    if (imageDataInfo.NumberOfImagesToAcquire == 0) {
      LOG_ERROR("Unable to read NumberOfImagesToAcquire XML data element (or zero)");
      return PLUS_FAIL;
    }

    const char* sequenceMetaFile = freehandMotionData_1->GetAttribute("OutputSequenceMetaFileSuffix"); 
    if ( sequenceMetaFile != NULL) 
    {
      imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
    }

		this->SetImageDataInfo(FREEHAND_MOTION_1, imageDataInfo); 
	} else {
		LOG_WARNING("Unable to find RandomStepperMotionData1 XML data element"); 
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::WriteConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkCalibrationController::WriteConfiguration");

  // Save temporal calibration !!!TODO!!! we should have a separate algorithm class for this with its own read/write configuration functions
  // double videoTimeOffset = 0.0;
  // if ((this->DataCollector != NULL) && (this->DataCollector->GetVideoSource() != NULL) && (this->DataCollector->GetVideoSource()->GetBuffer() != NULL)) {
  //   videoTimeOffset = this->DataCollector->GetVideoSource()->GetBuffer()->GetLocalTimeOffset();
  // }

  // vtkSmartPointer<vtkXMLDataElement> imageAcquisition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "USDataCollection", "ImageAcquisition", NULL, NULL);
  // imageAcquisition->SetDoubleAttribute("LocalTimeOffset", videoTimeOffset);

	// Save spatial calibration result
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_PROBE, toolType);

  // Find probe definition element
	vtkSmartPointer<vtkXMLDataElement> probeDefinition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType.c_str());
	if (probeDefinition == NULL) {
		LOG_ERROR("No probe definition is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = probeDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in probe definition!");
		return PLUS_FAIL;
	}

	// Assemble and save transform
  double tUserImageToProbe[16] = {0}; 
  vtkMatrix4x4::DeepCopy(tUserImageToProbe, this->TransformUserImageToProbe->GetMatrix() ); 
  calibration->SetVectorAttribute("MatrixValue", 16, tUserImageToProbe); 

	// Save matrix name, date and error
	calibration->SetAttribute("MatrixName", "ImageToProbe");
	calibration->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());
	calibration->SetDoubleAttribute("Error", this->PointLineDistanceErrorAnalysisVector[0]); // TODO find the best error number

  // TRUS results
	vtkSmartPointer<vtkXMLDataElement> usCalibration = aConfig->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
		LOG_ERROR("No calibration configuration is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	if (calibrationController == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> probeCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration");
  if ( probeCalibration == NULL )
  {
    LOG_ERROR("Failed to write results to ProbeCalibration XML data element - element not found!"); 
    return PLUS_FAIL; 
  }

  if ( this->CalibrationDone )
  {
    vtkSmartPointer<vtkXMLDataElement> calibrationResult = probeCalibration->FindNestedElementWithName("CalibrationResult");

    if ( calibrationResult == NULL )
    {
      calibrationResult = vtkSmartPointer<vtkXMLDataElement>::New(); 
      calibrationResult->SetName("CalibrationResult"); 
      calibrationResult->SetParent(probeCalibration); 
      probeCalibration->AddNestedElement(calibrationResult); 
    }

    if ( this->CalibrationDate != NULL )
    {
      calibrationResult->SetAttribute("Date", this->CalibrationDate ); 
    }

    calibrationResult->SetVectorAttribute("CenterOfRotationPx", 2, this->CenterOfRotationPx); 
    calibrationResult->SetVectorAttribute("PhantomToProbeDistanceInMm", 2, this->PhantomToProbeDistanceInMm); 

    std::vector<double> LRE_w1; 
    if ( this->GetLineReconstructionErrorAnalysisVector(1, LRE_w1) == PLUS_SUCCESS )
    {
      calibrationResult->SetVectorAttribute("LRE-W1", 7, &LRE_w1[0] ); 
    }

    std::vector<double> LRE_w3; 
    if ( this->GetLineReconstructionErrorAnalysisVector(3, LRE_w3) == PLUS_SUCCESS )
    {
      calibrationResult->SetVectorAttribute("LRE-W3", 7, &LRE_w3[0] ); 
    }

    std::vector<double> LRE_w4; 
    if ( this->GetLineReconstructionErrorAnalysisVector(4, LRE_w4) == PLUS_SUCCESS )
    {
      calibrationResult->SetVectorAttribute("LRE-W4", 7, &LRE_w4[0] ); 
    }

    std::vector<double> LRE_w6; 
    if ( this->GetLineReconstructionErrorAnalysisVector(6, LRE_w6) == PLUS_SUCCESS )
    {
      calibrationResult->SetVectorAttribute("LRE-W6", 7, &LRE_w6[0] ); 
    }

    // TransformImageToTemplate
    double tImageToTemplate[16] = {0}; 
    vtkMatrix4x4::DeepCopy(tImageToTemplate, this->TransformImageToTemplate->GetMatrix() ); 
    calibrationResult->SetVectorAttribute("TransformImageToTemplate", 16, tImageToTemplate); 

    // TransformUserImageToProbe
    calibrationResult->SetVectorAttribute("TransformUserImageToProbe", 16, tUserImageToProbe); 

    // TransformReferenceToTemplateHolderHome
    double tReferenceToTemplateHolderHome[16] = {0}; 
    vtkMatrix4x4::DeepCopy(tReferenceToTemplateHolderHome, this->TransformReferenceToTemplateHolderHome->GetMatrix() ); 
    calibrationResult->SetVectorAttribute("TransformReferenceToTemplateHolderHome", 16, tReferenceToTemplateHolderHome); 

    // TransformTemplateHolderToTemplate
    double tTemplateHolderToTemplate[16] = {0}; 
    vtkMatrix4x4::DeepCopy(tTemplateHolderToTemplate, this->TransformTemplateHolderToTemplate->GetMatrix() ); 
    calibrationResult->SetVectorAttribute("TransformTemplateHolderToTemplate", 16, tTemplateHolderToTemplate); 

    // TransformImageToUserImage
    double tImageToUserImage[16] = {0}; 
    vtkMatrix4x4::DeepCopy(tImageToUserImage, this->TransformImageToUserImage->GetMatrix() ); 
    calibrationResult->SetVectorAttribute("TransformImageToUserImage", 16, tImageToUserImage); 
  }

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ResetFreehandCalibration()
{
	LOG_TRACE("vtkCalibrationController::ResetFreehandCalibration");

	// Empty tracked frame containers
	this->TrackedFrameListContainer[FREEHAND_MOTION_1]->Clear();
	this->TrackedFrameListContainer[FREEHAND_MOTION_2]->Clear();

  // Reset segmented image counters
  this->ImageDataInfoContainer[FREEHAND_MOTION_1].NumberOfSegmentedImages = 0;
  this->ImageDataInfoContainer[FREEHAND_MOTION_2].NumberOfSegmentedImages = 0;

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::DoCalibration()
{
	LOG_TRACE("vtkCalibrationController::DoCalibration"); 

	mHasBeenCalibrated = false;

  if ( mDataPositionsInUSImageFrame.empty() )
  {
    LOG_ERROR("Unable to perform calibration - calibration data is empty!"); 
    return PLUS_FAIL; 
  }

  // Apply beamwidth weights (TODO do this when adding the position)
	if (mIsUSBeamwidthAndWeightFactorsTableReady == true)
	{
		if ( mWeightsForDataPositions.size() != mDataPositionsInUSImageFrame.size() )
		{
			LOG_ERROR("The number of weights and beamwidth data does not match the number of data for calibration!");
			return PLUS_FAIL;
		}

    for (int i=0; i < mWeightsForDataPositions.size(); ++i)
    {
      mDataPositionsInUSImageFrame[i] *= mWeightsForDataPositions[i]; // It can happen that it is done multiple times - see TODO few lines above
      mDataPositionsInUSProbeFrame[i] *= mWeightsForDataPositions[i];
    }
  }

  // Do calibration for all dimensions and assemble output matrix
  const int m = mDataPositionsInUSImageFrame.size();
  const int n = mDataPositionsInUSImageFrame.begin()->size();

  mTransformUSImageFrame2USProbeFrameMatrix4x4.set_size(n, n);
  mTransformUSImageFrame2USProbeFrameMatrix4x4.fill(0);

  for (int row = 0; row < n; ++row)
  {
    std::vector<double> probePositionRowVector(m,0);
    for (int i=0; i < m; ++i)
    {
      probePositionRowVector[i] = mDataPositionsInUSProbeFrame[i][row];
    }

    vnl_vector<double> resultVector(n,0);
    if ( PlusMath::LSQRMinimize(mDataPositionsInUSImageFrame, probePositionRowVector, resultVector) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }

    mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row(row, resultVector);
  }

	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row(3, lastRow);

	mHasBeenCalibrated = true;

	// Validate the calibration accuracy
	this->compute3DPointReconstructionError();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkCalibrationController::IsUserImageToProbeTransformOrthogonal()
{
	LOG_TRACE("vtkCalibrationController::IsUserImageToProbeTransformOrthogonal");

  vtkMatrix4x4* userImageToProbeMatrix = this->TransformUserImageToProbe->GetMatrix(); 

  // Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
  double xVector[3] = {userImageToProbeMatrix->GetElement(0,0),userImageToProbeMatrix->GetElement(1,0),userImageToProbeMatrix->GetElement(2,0)}; 
  double yVector[3] = {userImageToProbeMatrix->GetElement(0,1),userImageToProbeMatrix->GetElement(1,1),userImageToProbeMatrix->GetElement(2,1)};  
  double zVector[3] = {userImageToProbeMatrix->GetElement(0,2),userImageToProbeMatrix->GetElement(1,2),userImageToProbeMatrix->GetElement(2,2)};  

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
PlusStatus vtkCalibrationController::ComputeCalibrationResults()
{
	LOG_TRACE("vtkCalibrationController::ComputeCalibrationResults"); 

  if ( ! this->Initialized ) 
	{
		this->Initialize(); 
	}

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 
  this->SetCalibrationTimestamp(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S").c_str()); 

	// Do final calibration 
	if ( DoCalibration() != PLUS_SUCCESS )
  {
    LOG_ERROR("Calibration failed!");
    return PLUS_FAIL;
  }

	vtkSmartPointer<vtkMatrix4x4> userImageToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	// convert transform to vtk
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 4; j++ )
		{
			userImageToProbeMatrix->SetElement(i, j, mTransformUSImageFrame2USProbeFrameMatrix4x4.get(i, j) ); 
		}
	}
	
  // Check orthogonality
  if ( ! IsUserImageToProbeTransformOrthogonal() )
  {
    LOG_WARNING("User image to probe transform matrix orthogonality test failed! The result is probably not satisfactory");
  }

	// Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
	double xVector[3] = {userImageToProbeMatrix->GetElement(0,0),userImageToProbeMatrix->GetElement(1,0),userImageToProbeMatrix->GetElement(2,0)}; 
	double yVector[3] = {userImageToProbeMatrix->GetElement(0,1),userImageToProbeMatrix->GetElement(1,1),userImageToProbeMatrix->GetElement(2,1)};  
	double zVector[3] = {0,0,0}; 

  vtkMath::Cross(xVector, yVector, zVector); 
					
	// make the z vector have about the same length as x an y,
	// so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
	vtkMath::Normalize(zVector);
	double normZ = (vtkMath::Norm(xVector)+vtkMath::Norm(yVector))/2;  
	vtkMath::MultiplyScalar(zVector, normZ);
	
	userImageToProbeMatrix->SetElement(0, 2, zVector[0]);
	userImageToProbeMatrix->SetElement(1, 2, zVector[1]);
	userImageToProbeMatrix->SetElement(2, 2, zVector[2]);

  // Set result matrix
  this->TransformUserImageToProbe->SetMatrix( userImageToProbeMatrix );

  // Compute the independent point and line reconstruction errors
	if ( this->computeIndependentPointLineReconstructionError() != PLUS_SUCCESS )
  {
    LOG_ERROR("Computing PREs failed!");
    return PLUS_FAIL;
  }

	// STEP-4. Print the final calibration results and error reports 
  LOG_INFO("Calibration result transform matrix = ");
  PlusMath::LogVtkMatrix(this->TransformUserImageToProbe->GetMatrix());

  // Point-Line Distance Error Analysis for Validation Positions in US probe frame
  LOG_INFO("Point-Line Distance Error - mean: " << this->PointLineDistanceErrorAnalysisVector[0] << ", rms: " << this->PointLineDistanceErrorAnalysisVector[1] << ", std: " << this->PointLineDistanceErrorAnalysisVector[2]);
  LOG_INFO("  Validation data confidence level: " << this->PointLineDistanceErrorAnalysisVector[3]);

	// STEP-5. Save the calibration results and error reports into a file 
	SaveCalibrationResultsAndErrorReportsToXML();

  // Save calibration
  if ( WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS )
  {
	  LOG_ERROR("Freehand calibration result could not be saved into session configuration data!");
	  return PLUS_FAIL;
  }

	this->CalibrationDoneOn(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::UpdateLineReconstructionErrorAnalysisVectors()
{
  LOG_TRACE("vtkCalibrationController::UpdateLineReconstructionErrorAnalysisVectors"); 

  this->LineReconstructionErrors.clear(); 

  // Add wire #1 LRE to map
  this->LineReconstructionErrors[1] = this->mNWire1AbsLREAnalysisInUSProbeFrame; 

  // Add wire #3 LRE to map
  this->LineReconstructionErrors[3] = this->mNWire3AbsLREAnalysisInUSProbeFrame; 

  // Add wire #4 LRE to map
  this->LineReconstructionErrors[4] = this->mNWire4AbsLREAnalysisInUSProbeFrame; 

  // Add wire #6 LRE to map
  this->LineReconstructionErrors[6] = this->mNWire6AbsLREAnalysisInUSProbeFrame; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::GetLineReconstructionErrorAnalysisVector(int wireNumber, std::vector<double> &LRE)
{
	LOG_TRACE("vtkCalibrationController::GetLineReconstructionErrorAnalysisVector (wire #" << wireNumber << ")"); 

  if ( this->LineReconstructionErrors.size() == 0 )
  {
    if ( this->UpdateLineReconstructionErrorAnalysisVectors() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get line reconstruction error for wire " << wireNumber ); 
      return PLUS_FAIL; 
    }
  }
  
  std::map<int, std::vector<double> >::iterator lreIterator = this->LineReconstructionErrors.find(wireNumber); 
  if ( lreIterator != this->LineReconstructionErrors.end() )
  {
    LRE = lreIterator->second; 
  }
  else
  {
    LOG_WARNING("Unable to get LRE for wire #" << wireNumber ); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
vnl_matrix<double> vtkCalibrationController::GetLineReconstructionErrorMatrix(int wireNumber)
{
	//LOG_TRACE("vtkCalibrationController::GetLineReconstructionErrorMatrix (wire #" << wireNumber << ")"); 
	vnl_matrix<double> mLREOrigInUSProbeFrameMatrix; 
	switch(wireNumber)
	{
	case 1: // wire #1
		{
			mLREOrigInUSProbeFrameMatrix = this->mNWire1LREOrigInUSProbeFrameMatrix4xN;
		}
		break; 
	case 3: // wire #3
		{
			mLREOrigInUSProbeFrameMatrix = this->mNWire3LREOrigInUSProbeFrameMatrix4xN;
		}
		break; 
	case 4: // wire #4
		{
			mLREOrigInUSProbeFrameMatrix = this->mNWire4LREOrigInUSProbeFrameMatrix4xN;
		}
		break; 
	case 6: // wire #6
		{
			mLREOrigInUSProbeFrameMatrix = this->mNWire6LREOrigInUSProbeFrameMatrix4xN;
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE matrix for wire #" << wireNumber ); 
	}

	return mLREOrigInUSProbeFrameMatrix; 
}

//-----------------------------------------------------------------------------

std::string vtkCalibrationController::GetResultString()
{
	LOG_TRACE("vtkCalibrationController::GetResultString");

	std::ostringstream matrixStringStream;
	matrixStringStream << "Image to probe transform:" << std::endl;

	// Print matrix rows
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrixStringStream << std::fixed << std::setprecision(3) << std::setw(6) << std::right << this->GetTransformUserImageToProbe()->GetMatrix()->GetElement(i,j) << " ";
		}

		matrixStringStream << std::endl;
	}

	std::ostringstream errorsStringStream;

	errorsStringStream << "Point-line distance errors" << std::endl << "(mean, rms, std):" << std::endl;
	errorsStringStream << std::fixed << std::setprecision(3) << "  " << this->PointLineDistanceErrorAnalysisVector[0] << ", " << this->PointLineDistanceErrorAnalysisVector[1] << ", " << this->PointLineDistanceErrorAnalysisVector[2] << std::endl;

	std::ostringstream resultStringStream;
	resultStringStream << matrixStringStream.str() << errorsStringStream.str() << std::endl;

	resultStringStream << "Validation data confidence:" << std::endl;
	resultStringStream << "  " << this->PointLineDistanceErrorAnalysisVector[3];

	return resultStringStream.str();
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ComputeNWireInstersections()
{
	LOG_TRACE("vtkCalibrationController::ComputeNWireInstersections");

	double alphaTopLayerFrontWall = -1.0;
	double alphaTopLayerBackWall = -1.0;
	double alphaBottomLayerFrontWall = -1.0;
	double alphaBottomLayerBackWall = -1.0;

	// Read input NWires and convert them to vnl vectors to easier processing
	LOG_DEBUG("Endpoints of wires = ");

  std::vector<NWire> nWires = this->PatternRecognition.GetFidLineFinder()->GetNWires();
	// List endpoints, check wire ids and NWire geometry correctness (wire order and locations) and compute intersections
	for (std::vector<NWire>::iterator it = nWires.begin(); it != nWires.end(); ++it) {
		int layer = it->wires[0].id / 3;
		int sumLayer = 0;

		for (int i=0; i<3; ++i) {
			vnl_vector<double> endPointFront(3);
			vnl_vector<double> endPointBack(3);

			sumLayer += it->wires[i].id;
		
			for (int j=0; j<3; ++j) {
				endPointFront[j] = it->wires[i].endPointFront[j];
				endPointBack[j] = it->wires[i].endPointBack[j];
			}

			LOG_DEBUG("\t Front endpoint of wire " << i << " on layer " << layer << " = " << endPointFront);
			LOG_DEBUG("\t Back endpoint of wire " << i << " on layer " << layer << " = " << endPointBack);
		}

		if (sumLayer != layer * 9 + 6) {
			LOG_ERROR("Invalid NWire IDs (" << it->wires[0].id << ", " << it->wires[1].id << ", " << it->wires[2].id << ")!");
			return PLUS_FAIL;
		}

		// Check if the middle wire is the diagonal (the other two are parallel to each other and the first and the second, and the second and the third intersect)
		double wire1[3];
		double wire3[3];
		double cross[3];
		if ((it->GetWireById(1) == NULL) || (it->GetWireById(3) == NULL)) {
			LOG_ERROR("No first or third wire found!");
			return PLUS_FAIL;
		}
		vtkMath::Subtract(it->GetWireById(1)->endPointFront, it->GetWireById(1)->endPointBack, wire1);
		vtkMath::Subtract(it->GetWireById(3)->endPointFront, it->GetWireById(3)->endPointBack, wire3);
		vtkMath::Cross(wire1, wire3, cross);
		if (vtkMath::Norm(cross) > 0.001) {
			LOG_ERROR("The first and third wire of layer " << layer << " are not parallel!");
			return PLUS_FAIL;
		}
		double closestTemp[3];
		double parametricCoord1, parametricCoord2;
		if (vtkLine::DistanceBetweenLines(it->wires[0].endPointFront, it->wires[0].endPointBack, it->wires[1].endPointFront, it->wires[1].endPointBack, it->intersectPosW12, closestTemp, parametricCoord1, parametricCoord2) > 0.000001) {
			LOG_ERROR("The first and second wire of layer " << layer << " do not intersect each other!");
			return PLUS_FAIL;
		}
		if (vtkLine::DistanceBetweenLines(it->wires[2].endPointFront, it->wires[2].endPointBack, it->wires[1].endPointFront, it->wires[1].endPointBack, it->intersectPosW32, closestTemp, parametricCoord1, parametricCoord2) > 0.000001) {
			LOG_ERROR("The second and third wire of layer " << layer << " do not intersect each other!");
			return PLUS_FAIL;
		}
	}

	// Log the data pipeline if requested.
	int layer;
	std::vector<NWire>::iterator it;
	for (it = nWires.begin(), layer = 0; it != nWires.end(); ++it, ++layer) {
		LOG_DEBUG("\t Intersection of wire 1 and 2 in layer " << layer << " \t= (" << it->intersectPosW12[0] << ", " << it->intersectPosW12[1] << ", " << it->intersectPosW12[2] << ")");
		LOG_DEBUG("\t Intersection of wire 3 and 2 in layer " << layer << " \t= (" << it->intersectPosW32[0] << ", " << it->intersectPosW32[1] << ", " << it->intersectPosW32[2] << ")");
	}

  this->PatternRecognition.GetFidLineFinder()->SetNWires(nWires);
  this->PatternRecognition.GetFidLabeling()->SetNWires(nWires); // TODO there should be only one NWires in the memory

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

double vtkCalibrationController::GetBeamwidthWeightForBeamwidthMagnitude(int aActualAxialDepth)
{
  LOG_TRACE("vtkCalibrationController::GetBeamwidthWeight");

  // Compute the weight according to the selected method of incorporation
  double beamwidthWeightForActualAxialDepth = -1;

  if( aActualAxialDepth <= mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
  {
    // #1. This is the ultrasound elevation near field which has the the best imaging quality (before the elevation focal zone)
    // We will set the beamwidth at the near field to be the same as that of the elevation focal zone.
    if( 1 == this->IncorporatingUS3DBeamProfile || 3 == this->IncorporatingUS3DBeamProfile )
    { // Option: BWVar or BWTHEVar
      // Filtering is not necessary in the near field
      beamwidthWeightForActualAxialDepth = 1 / ( mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) / 4);
    }
    else 
    { // Option = BWRatio
      beamwidthWeightForActualAxialDepth = sqrt( 1 / mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,0) );
    }
  }
  else if( aActualAxialDepth >= mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
  {
    // #2. Further deep in far field (close to the bottom of the image)
    // Ultrasound diverses quickly in this region and data quality deteriorates

    double beamwidthMagnitudeMm = mUS3DBeamwidthAtFarestAxialDepth.magnitude();

    if( 1 == this->IncorporatingUS3DBeamProfile || 3 == this->IncorporatingUS3DBeamProfile )
    { // Option: BWVar or BWTHEVar
      if( 3 != this->IncorporatingUS3DBeamProfile
        || beamwidthMagnitudeMm < mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) * mNumOfTimesOfMinBeamWidth )
      { // Option: NOT BWTHEVar
        beamwidthWeightForActualAxialDepth = 1 / (beamwidthMagnitudeMm / 4);
      }
    }
    else
    { // Option = BWRatio
      beamwidthWeightForActualAxialDepth = sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
        this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.columns() - 1) );
    }
  }
  else 
  {
    // #3. Ultrasound far field 
    // Here the sound starts to diverse with elevation beamwidth getting
    // larger and larger.  Data quality starts to deteriorate.
    vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
    US3DBeamwidthAtThisAxialDepth.put(0, mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1, aActualAxialDepth - mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
    US3DBeamwidthAtThisAxialDepth.put(1, mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2, aActualAxialDepth - mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
    US3DBeamwidthAtThisAxialDepth.put(2, mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3, aActualAxialDepth - mTheNearestAxialDepthInUSBeamwidthAndWeightTable));

    double beamwidthMagnitudeMm = US3DBeamwidthAtThisAxialDepth.magnitude();

    if( 1 == this->IncorporatingUS3DBeamProfile || 3 == this->IncorporatingUS3DBeamProfile )
    { // Option: BWVar or BWTHEVar
      if( 3 != this->IncorporatingUS3DBeamProfile
        || beamwidthMagnitudeMm < mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) * mNumOfTimesOfMinBeamWidth )
      { // Option: NOT BWTHEVar
        beamwidthWeightForActualAxialDepth = 1 / (beamwidthMagnitudeMm/4);
      }
    }
    else
    { // Option = BWRatio
      beamwidthWeightForActualAxialDepth = sqrt( 1 / mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4, aActualAxialDepth - mTheNearestAxialDepthInUSBeamwidthAndWeightTable) );
    }
  }

  return beamwidthWeightForActualAxialDepth;
}

//-----------------------------------------------------------------------------

void vtkCalibrationController::ResetDataContainers()
{
	LOG_TRACE("vtkCalibrationController::ResetDataContainers");

  // Initialize flags
  this->CalibrationDoneOff();
  this->SetCalibrationDate(NULL);
  
  // Initialize flags
	mArePRE3DsForValidationPositionsReady = false;
	mAreIndependentPointLineReconErrorsReady = false;
  mHasBeenCalibrated = false;
  mAreValidationDataMatricesConstructed = false;

	// Initialize data containers
	mDataPositionsInUSProbeFrame.resize(0);
	mDataPositionsInUSImageFrame.resize(0);
	mOutlierDataPositions.resize(0); 
	
	mWeightsForDataPositions.resize(0);
	mValidationPositionsInUSProbeFrame.resize(0);
	mValidationPositionsNWireStartInUSProbeFrame.resize(0);
	mValidationPositionsNWireEndInUSProbeFrame.resize(0);

	mValidationPositionsNWire1InUSImageFrame.resize(0);
	mValidationPositionsNWire3InUSImageFrame.resize(0);
	mValidationPositionsNWire4InUSImageFrame.resize(0);
	mValidationPositionsNWire6InUSImageFrame.resize(0);

	mValidationPositionsNWire1InUSProbeFrame.resize(0);
	mValidationPositionsNWire3InUSProbeFrame.resize(0);
	mValidationPositionsNWire4InUSProbeFrame.resize(0);
	mValidationPositionsNWire6InUSProbeFrame.resize(0);

	mValidationPositionsInUSImageFrame.resize(0);
	mWeightsForValidationPositions.resize(0);
	mValidationPositionsInUSImageFrameMatrix4xN.set_size(0,0);

	mValidationPositionsNWire1InUSImageFrame4xN.set_size(0,0);
	mValidationPositionsNWire3InUSImageFrame4xN.set_size(0,0);
	mValidationPositionsNWire4InUSImageFrame4xN.set_size(0,0);
	mValidationPositionsNWire6InUSImageFrame4xN.set_size(0,0);

	mValidationPositionsNWire1InUSProbeFrame4xN.set_size(0,0);
	mValidationPositionsNWire3InUSProbeFrame4xN.set_size(0,0);
	mValidationPositionsNWire4InUSProbeFrame4xN.set_size(0,0);
	mValidationPositionsNWire6InUSProbeFrame4xN.set_size(0,0);

	mNWire1LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);
	mNWire3LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);
	mNWire4LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);
	mNWire6LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);

	mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);
	mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);
	mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);
	mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);

	this->PointReconstructionErrorAnalysisVector.resize(0);
	this->PointLineDistanceErrorAnalysisVector.resize(0);
	this->PointReconstructionErrorMatrix.set_size(0,0);
	mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_size(0,0);
	this->PointLineDistanceErrorVector.set_size(0);
	this->PointLineDistanceErrorSortedVector.set_size(0);
}

//-----------------------------------------------------------------------------

void vtkCalibrationController::FillUltrasoundBeamwidthAndWeightFactorsTable()
{
	LOG_TRACE("vtkCalibrationController::FillUltrasoundBeamwidthAndWeightFactorsTable");

	// Populate the beam-width data accordingly
	int numOfTotalBeamWidthData = this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.columns();

	// Fetch the data at the nearest and farest axial depth for fast access 
	mTheNearestAxialDepthInUSBeamwidthAndWeightTable = ROUND(InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(0,0));
	mTheFarestAxialDepthInUSBeamwidthAndWeightTable = ROUND(InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(0, numOfTotalBeamWidthData-1));

	// Populate the 3D beamwidth elements at the nearest and farest axial depth
	mUS3DBeamwidthAtNearestAxialDepth.put(0, InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,0));
	mUS3DBeamwidthAtNearestAxialDepth.put(1, InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,0));
	mUS3DBeamwidthAtNearestAxialDepth.put(2, InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,0));

	mUS3DBeamwidthAtFarestAxialDepth.put(0, InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,numOfTotalBeamWidthData-1));
	mUS3DBeamwidthAtFarestAxialDepth.put(1, InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,numOfTotalBeamWidthData-1));
	mUS3DBeamwidthAtFarestAxialDepth.put(2, InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,numOfTotalBeamWidthData-1));

	mIsUSBeamwidthAndWeightFactorsTableReady = true;

	// Log the data pipeline if requested.
	LOG_DEBUG(" IncorporatingUS3DBeamProfile = " << this->IncorporatingUS3DBeamProfile << " (1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding)");
	LOG_DEBUG(" numOfTotalBeamWidthData = " << numOfTotalBeamWidthData);
	LOG_DEBUG(" MinElevationBeamwidthAndFocalZoneInUSImageFrame [Focal Zone (US Image Frame), Elevation Beamwidth] = " << this->MinElevationBeamwidthAndFocalZoneInUSImageFrame);
	LOG_DEBUG(" SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN = " << this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN);
	LOG_DEBUG(" InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM (interpolated) = " << this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM);
	LOG_DEBUG(" mTheNearestAxialDepthInUSBeamwidthAndWeightTable = " << mTheNearestAxialDepthInUSBeamwidthAndWeightTable);				
	LOG_DEBUG(" mTheFarestAxialDepthInUSBeamwidthAndWeightTable = " << mTheFarestAxialDepthInUSBeamwidthAndWeightTable);
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::compute3DPointReconstructionError()
{
	LOG_TRACE("vtkCalibrationController::compute3DPointReconstructionError");

	// Reset the flag
	if( true == mArePRE3DsForValidationPositionsReady )
	{
		mArePRE3DsForValidationPositionsReady = false;
	}

	// Populate the validation data set matrices if it's not yet been done
	// ===================================================================
	// NOTE: since the validation data set is separated from the calibration 
	//       data and acquired before the calibration, the construction of
	//       the validation data matrices needs to be done once and only 
	//       once (e.g., before the 1st validation task is performed) with
	//       the flag set to true.  This would save the system runtime
	//       during the iterative calibration/validation process.
	if( false == mAreValidationDataMatricesConstructed )
	{
    if (constructValidationDataMatrices() != PLUS_SUCCESS) {
      LOG_ERROR("Failed to construct validation martices!");
      return PLUS_FAIL;
    }
	}

	// The total number of the validation data set.
	const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );

	// Calculate 3D point reconstruction error (PRE3D) in the US Probe Frame
	// ======================================================================

	// Using the calibration matrix, we can obtain the projected positions
	// in the US probe frame after applying the calibration matrix to
	// the validation positions in the US image frame.
	vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix4xN = mTransformUSImageFrame2USProbeFrameMatrix4x4 * mValidationPositionsInUSImageFrameMatrix4xN;

	// Obtain the PRE3D matrix:
	// PRE3D_matrix = ( Projected - True ) positions in US probe frame
	this->PointReconstructionErrorMatrix = ProjectedPositionsInUSProbeFrameMatrix4xN - mValidationPositionsInUSProbeFrameMatrix4xN;

	// Make sure the last row (4th) in the PRE3D matrix is all zeros
	const vnl_vector<double> AllZerosVector( NumberOfValidationPositions, 0 );
	this->PointReconstructionErrorMatrix.set_row(3, AllZerosVector);

	// Sort the PRE3D in an ascending order with respect to the euclidean PRE3D distance
	// =============================================================================

	mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN = this->PointReconstructionErrorMatrix;

	// First calculate the euclidean PRE3D distance from PRE3D
	// vector and store them in the fourth row of the matrix.
	for( int i = 0; i < NumberOfValidationPositions; i++ )
	{
		mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.put( 3, i, this->PointReconstructionErrorMatrix.get_column(i).magnitude() );
	}

	// Populate the last row of the raw PRE3D matrix with the euclidean distance as well.
	this->PointReconstructionErrorMatrix.set_row( 3, mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_row(3) );

	// Sorting the PRE3D matrix w.r.t the  euclidean PRE3D distance (the 4th row)
	// Sorting algorithm employed: Insertion Sort
	for( int i = 0; i < NumberOfValidationPositions; i++ )
  {
		for( int j = i; j > 0 && mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get(3, j-1) > mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> SwapPRE3DColumnVector = mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_column( j-1, mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_column(j) );
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_column( j, SwapPRE3DColumnVector ); 
		}
  }

	// We are only interested at the top-ranked ascending PRE3D values
	// presented in the sorted validation result matrix.
	// Default percentage: 95%
	const int NumberOfTopRankedCalibrationData = ROUND( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel );

	// We only need absolute values of PRE3D to average
	vnl_matrix<double> AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN = mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_n_columns( 0, NumberOfTopRankedCalibrationData ).apply( fabs );

	// Obtain PRE3Ds in X-, Y-, and Z-axis
	vnl_vector<double> AbsPRE3DsinX = AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 0 );
	vnl_vector<double> AbsPRE3DsinY = AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 1 );
	vnl_vector<double> AbsPRE3DsinZ = AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 2 );

	// Perform statistical analysis
	// =============================

	// X-axis PRE3D statistics
	double PRE3D_X_mean = AbsPRE3DsinX.mean();
	double PRE3D_X_rms = AbsPRE3DsinX.rms();
	double PRE3D_X_std = sqrt( 1 / (double)( NumberOfTopRankedCalibrationData - 1 ) * ( AbsPRE3DsinX.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_X_mean,2) ) );

	// Y-axis PRE3D statistics
	double PRE3D_Y_mean = AbsPRE3DsinY.mean();
	double PRE3D_Y_rms = AbsPRE3DsinY.rms();
	double PRE3D_Y_std = sqrt( 1 / (double)( NumberOfTopRankedCalibrationData - 1 ) * ( AbsPRE3DsinY.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_Y_mean,2) ) );

	// Z-axis PRE3D statistics
	double PRE3D_Z_mean = AbsPRE3DsinZ.mean();
	double PRE3D_Z_rms = AbsPRE3DsinZ.rms();
	double PRE3D_Z_std = sqrt( 1 / (double)( NumberOfTopRankedCalibrationData - 1 ) * ( AbsPRE3DsinZ.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_Z_mean,2) ) );
	
	// Consolidate the returning data container (for details see member variable description)
	this->PointReconstructionErrorAnalysisVector.resize(0);
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_X_mean );
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_X_rms );
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_X_std );

	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_Y_mean );
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_Y_rms );
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_Y_std );

	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_Z_mean );
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_Z_rms );
	this->PointReconstructionErrorAnalysisVector.push_back( PRE3D_Z_std );

	this->PointReconstructionErrorAnalysisVector.push_back( mValidationDataConfidenceLevel );

	// Reset the flag
	mArePRE3DsForValidationPositionsReady = true;

	// Log the data pipeline if requested.
	LOG_DEBUG("3D Point Reconstruction Error (PRE3D)");
  LOG_DEBUG("PRE3D mean: (" << this->PointReconstructionErrorAnalysisVector[0] << ", " << this->PointReconstructionErrorAnalysisVector[3] << ", " << this->PointReconstructionErrorAnalysisVector[6] << ")");
  LOG_DEBUG("PRE3D rms: (" << this->PointReconstructionErrorAnalysisVector[1] << ", " << this->PointReconstructionErrorAnalysisVector[4] << ", " << this->PointReconstructionErrorAnalysisVector[7] << ")");
  LOG_DEBUG("PRE3D std: (" << this->PointReconstructionErrorAnalysisVector[2] << ", " << this->PointReconstructionErrorAnalysisVector[5] << ", " << this->PointReconstructionErrorAnalysisVector[8] << ")");
	LOG_DEBUG("Validation Data Confidence Level = " << this->PointReconstructionErrorAnalysisVector[9] << " ( " << NumberOfTopRankedCalibrationData << " top-ranked validation data were used out of the total " << NumberOfValidationPositions << " validation data set for the above statistical analysis)");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vnl_vector<double> vtkCalibrationController::getPointLineReconstructionError(vnl_vector<double> NWirePositionInUSImageFrame, 
			vnl_vector<double> NWirePositionInUSProbeFrame)
{
	LOG_TRACE("vtkCalibrationController::getPointLineReconstructionError");

	vnl_vector<double> NWireProjectedPositionsInUSProbeFrame = mTransformUSImageFrame2USProbeFrameMatrix4x4 * NWirePositionInUSImageFrame;
	vnl_vector<double> NWireLREOrigInUSProbeFrame = NWirePositionInUSProbeFrame - NWireProjectedPositionsInUSProbeFrame;

	NWireLREOrigInUSProbeFrame[3] = 1; 

	return NWireLREOrigInUSProbeFrame; 
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::computeIndependentPointLineReconstructionError()
{
	LOG_TRACE("vtkCalibrationController::computeIndependentPointLineReconstructionError");

	// Reset the flag
	if( true == mAreIndependentPointLineReconErrorsReady )
	{
		mAreIndependentPointLineReconErrorsReady = false;
	}

	// Populate the validation data set matrices if it's not yet been done
	// ===================================================================
	// NOTE: since the validation data set is separated from the calibration 
	//       data and acquired before the calibration, the construction of
	//       the validation data matrices needs to be done once and only 
	//       once (e.g., before the 1st validation task is performed) with
	//       the flag set to true.  This would save the system runtime
	//       during the iterative calibration/validation process.
	if( false == mAreValidationDataMatricesConstructed )
	{
    if (constructValidationDataMatrices() != PLUS_SUCCESS) {
      LOG_ERROR("Failed to construct validation matrices!");
      return PLUS_FAIL;
    }
	}

	// The total number of the validation data set.
	const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );

	// STEP-1. Calculate Parallel-Line Reconstruction Errors in the US Probe Frame
	// NOTES:
	// 1. The parallel lines in use are: Wire N1, N3, N4, and N6;
	// 2. Only X and Y reconstruction errors are computed, since we do not have
	//    ground-truth information along the Z-axis when imaging a line object.

	// Using the calibration matrix, we can obtain the projected positions
	// in the US probe frame after applying the calibration matrix to
	// the validation positions in the US image frame.
	
	// NWire-1 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire1ProjectedPositionsInUSProbeFrameMatrix4xN = mTransformUSImageFrame2USProbeFrameMatrix4x4 * mValidationPositionsNWire1InUSImageFrame4xN;

	// NWire-3 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire3ProjectedPositionsInUSProbeFrameMatrix4xN = mTransformUSImageFrame2USProbeFrameMatrix4x4 * mValidationPositionsNWire3InUSImageFrame4xN;

	// NWire-4 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire4ProjectedPositionsInUSProbeFrameMatrix4xN = mTransformUSImageFrame2USProbeFrameMatrix4x4 * mValidationPositionsNWire4InUSImageFrame4xN;

	// NWire-6 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire6ProjectedPositionsInUSProbeFrameMatrix4xN = mTransformUSImageFrame2USProbeFrameMatrix4x4 * mValidationPositionsNWire6InUSImageFrame4xN;

	// Now we calculate the independent line reconstrution errors (in X and Y axes)
	// for NWires N1, N3, N4, N6.
	// Make sure the 3rd row (z-axis) in the LRE matrix is all zeros
	const vnl_vector<double> AllZerosVector( NumberOfValidationPositions, 0 );

	// NWire-1 LREs
	mNWire1LREOrigInUSProbeFrameMatrix4xN = mValidationPositionsNWire1InUSProbeFrame4xN - NWire1ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire1LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

	// NWire-3 LREs
	mNWire3LREOrigInUSProbeFrameMatrix4xN = mValidationPositionsNWire3InUSProbeFrame4xN - NWire3ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire3LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);
	
	// NWire-4 LREs
	mNWire4LREOrigInUSProbeFrameMatrix4xN = mValidationPositionsNWire4InUSProbeFrame4xN - NWire4ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire4LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

	// NWire-6 LREs
	mNWire6LREOrigInUSProbeFrameMatrix4xN = mValidationPositionsNWire6InUSProbeFrame4xN - NWire6ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire6LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

	// Now we calculate the euclidean LRE distance from LRE
	// vector and store them in the fourth row of the matrix.
	for( int i = 0; i < NumberOfValidationPositions; i++ )
	{
		mNWire1LREOrigInUSProbeFrameMatrix4xN.put( 3, i, mNWire1LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
		mNWire3LREOrigInUSProbeFrameMatrix4xN.put( 3, i, mNWire3LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
		mNWire4LREOrigInUSProbeFrameMatrix4xN.put( 3, i, mNWire4LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
		mNWire6LREOrigInUSProbeFrameMatrix4xN.put( 3, i, mNWire6LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
	}

	// Sorting the LRE matrice w.r.t the  euclidean distance (the 4th row)
	// Sorting algorithm employed: Insertion Sort
	mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN = mNWire1LREOrigInUSProbeFrameMatrix4xN;
	mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN = mNWire3LREOrigInUSProbeFrameMatrix4xN;
	mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN = mNWire4LREOrigInUSProbeFrameMatrix4xN;
	mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN = mNWire6LREOrigInUSProbeFrameMatrix4xN;

	// NWire-1
	for( int i = 0; i < NumberOfValidationPositions; i++ )
  {
		for( int j = i; j > 0 && mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );
			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, Swap3DColumnVector ); 
		}
  }
		
	// NWire-3
	for( int i = 0; i < NumberOfValidationPositions; i++ )
  {
		for( int j = i; j > 0 && mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );
			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, Swap3DColumnVector ); 
		}
  }

	// NWire-4
	for( int i = 0; i < NumberOfValidationPositions; i++ )
  {
		for( int j = i; j > 0 && mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );
			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, Swap3DColumnVector ); 
		}
  }

	// NWire-6
	for( int i = 0; i < NumberOfValidationPositions; i++ )
  {
		for( int j = i; j > 0 && mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );
			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, Swap3DColumnVector ); 
		}			
  }

	// NOTES: 
	// 1. We are only interested in the absolute value of the errors.
	// 2. We will perform analysis on all and the top-ranked ascending LRE values
	//    presented in the sorted validation result matrix.  This will effectively 
	//    get rid of outliers in the validation data set (e.g., caused by erroneous 
	//    segmentation results in the validation data).
	// Default percentage: 95%

	const int NumberOfTopRankedData = floor( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel + 0.5 );

	const vnl_matrix<double> NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN = mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 0, NumberOfTopRankedData ).apply( fabs );
	const vnl_matrix<double> NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN = mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 0, NumberOfTopRankedData ).apply( fabs );
	const vnl_matrix<double> NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN = mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 0, NumberOfTopRankedData ).apply( fabs );
	const vnl_matrix<double> NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN = mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 0, NumberOfTopRankedData ).apply( fabs );

	// Perform statistical analysis

	// NWire-1: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire1AbsLREDistributionInX = NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire1AbsLREDistributionInY = NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire1AbsLREDistributionInEUC = NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire1LRE_X_mean = NWire1AbsLREDistributionInX.mean();
	const double NWire1LRE_X_std = sqrt( 1/(double)( NumberOfTopRankedData - 1 ) *	( NWire1AbsLREDistributionInX.squared_magnitude() - NumberOfTopRankedData*pow( NWire1LRE_X_mean,2 )) );

	const double NWire1LRE_Y_mean = NWire1AbsLREDistributionInY.mean();
	const double NWire1LRE_Y_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) *	( NWire1AbsLREDistributionInY.squared_magnitude() - NumberOfTopRankedData*pow( NWire1LRE_Y_mean,2 )) );

	const double NWire1LRE_EUC_mean = NWire1AbsLREDistributionInEUC.mean();
	const double NWire1LRE_EUC_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire1AbsLREDistributionInEUC.squared_magnitude() - NumberOfTopRankedData*pow( NWire1LRE_EUC_mean,2 )) );

	// NWire-3: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire3AbsLREDistributionInX = NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire3AbsLREDistributionInY = NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire3AbsLREDistributionInEUC = NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire3LRE_X_mean = NWire3AbsLREDistributionInX.mean();
	const double NWire3LRE_X_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire3AbsLREDistributionInX.squared_magnitude() - NumberOfTopRankedData*pow( NWire3LRE_X_mean,2 )) );

	const double NWire3LRE_Y_mean = NWire3AbsLREDistributionInY.mean();
	const double NWire3LRE_Y_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire3AbsLREDistributionInY.squared_magnitude() - NumberOfTopRankedData*pow( NWire3LRE_Y_mean,2 )) );

	const double NWire3LRE_EUC_mean = NWire3AbsLREDistributionInEUC.mean();
	const double NWire3LRE_EUC_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire3AbsLREDistributionInEUC.squared_magnitude() - NumberOfTopRankedData*pow( NWire3LRE_EUC_mean,2 )) );						
		
	// NWire-4: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire4AbsLREDistributionInX = NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire4AbsLREDistributionInY = NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire4AbsLREDistributionInEUC = NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire4LRE_X_mean = NWire4AbsLREDistributionInX.mean();
	const double NWire4LRE_X_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire4AbsLREDistributionInX.squared_magnitude() - NumberOfTopRankedData*pow( NWire4LRE_X_mean,2 )) );

	const double NWire4LRE_Y_mean = NWire4AbsLREDistributionInY.mean();
	const double NWire4LRE_Y_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire4AbsLREDistributionInY.squared_magnitude() - NumberOfTopRankedData*pow( NWire4LRE_Y_mean,2 )) );

	const double NWire4LRE_EUC_mean = NWire4AbsLREDistributionInEUC.mean();
	const double NWire4LRE_EUC_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire4AbsLREDistributionInEUC.squared_magnitude() - NumberOfTopRankedData*pow( NWire4LRE_EUC_mean,2 )) );						

	// NWire-6: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire6AbsLREDistributionInX = NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire6AbsLREDistributionInY = NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire6AbsLREDistributionInEUC = NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire6LRE_X_mean = NWire6AbsLREDistributionInX.mean();
	const double NWire6LRE_X_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire6AbsLREDistributionInX.squared_magnitude() - NumberOfTopRankedData*pow( NWire6LRE_X_mean,2 )) );

	const double NWire6LRE_Y_mean = NWire6AbsLREDistributionInY.mean();
	const double NWire6LRE_Y_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire6AbsLREDistributionInY.squared_magnitude() - NumberOfTopRankedData*pow( NWire6LRE_Y_mean,2 )) );

	const double NWire6LRE_EUC_mean = NWire6AbsLREDistributionInEUC.mean();
	const double NWire6LRE_EUC_std = sqrt( 1 / (double)( NumberOfTopRankedData - 1 ) * ( NWire6AbsLREDistributionInEUC.squared_magnitude() - NumberOfTopRankedData*pow( NWire6LRE_EUC_mean,2 )) );									

	// Consolidate the returning data containers (for details see member variable description)
	// NWire-1 LRE Analysis
	mNWire1AbsLREAnalysisInUSProbeFrame.resize(0);
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_X_mean );
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_X_std );
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_Y_mean );
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_Y_std );
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_EUC_mean );
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_EUC_std );
	mNWire1AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

	// NWire-3 LRE Analysis
	mNWire3AbsLREAnalysisInUSProbeFrame.resize(0);
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_X_mean );
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_X_std );
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_Y_mean );
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_Y_std );
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_EUC_mean );
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_EUC_std );
	mNWire3AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

	// NWire-4 LRE Analysis
	mNWire4AbsLREAnalysisInUSProbeFrame.resize(0);
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_X_mean );
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_X_std );
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_Y_mean );
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_Y_std );
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_EUC_mean );
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_EUC_std );
	mNWire4AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

	// NWire-6 LRE Analysis
	mNWire6AbsLREAnalysisInUSProbeFrame.resize(0);
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_X_mean );
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_X_std );
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_Y_mean );
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_Y_std );
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_EUC_mean );
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_EUC_std );
	mNWire6AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );		

	// STEP-2. Calculate Point-Line Distance Error (PLDE) in the US Probe Frame

	// Using the calibration matrix, we can obtain the projected positions
	// in the US probe frame after applying the calibration matrix to
	// the validation positions in the US image frame.
	const vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix4xN = mTransformUSImageFrame2USProbeFrameMatrix4x4 * mValidationPositionsInUSImageFrameMatrix4xN;
	// We need the 3D coordinates (the first 3 elements) for cross product
	const vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix3xN = ProjectedPositionsInUSProbeFrameMatrix4xN.get_n_rows(0,3);

	// Now we calculate the Point-Line Distance Error (PLDE).  		
	// The PLDE was defined as the absolute point-line distance from the projected
	// positions to the N-Wire (the ground truth), both in the US probe frame.  
	// If there was no error, the PLDE would be zero and the projected postions
	// would reside right on the N-Wire.  The physical position of the N-Wire
	// was measured based on the phantom geometry and converted into the US
	// US probe frame by the optical tracking device affixed on the phantom.
	// NOTE: this data may be used for statistical analysis if desired.
	// FORMAT: vector 1xN (with N being the total number of validation positions)
	this->PointLineDistanceErrorVector.set_size( NumberOfValidationPositions );

	for( int i = 0; i < NumberOfValidationPositions; i++ )
	{
		// Fetch the respective points
		const vnl_vector<double> ThisProjectedPosition3D = 
			ProjectedPositionsInUSProbeFrameMatrix3xN.get_column(i);
		const vnl_vector<double> NWireStartPoint3D = 
			mValidationPositionsNWireStartInUSProbeFrame3xN.get_column(i);
		const vnl_vector<double> NWireEndPoint3D = 
			mValidationPositionsNWireEndInUSProbeFrame3xN.get_column(i);

		// Construct the vectors in 3D space
		const vnl_vector<double> NWireVector3D = 
			NWireEndPoint3D - NWireStartPoint3D;
		const vnl_vector<double> ProjectedPosition2NWireStartVector3D =
			ThisProjectedPosition3D - NWireStartPoint3D;

		// Calculate the point-line distance (using triangle-area principle)
		// Formula: x denotes the cross product
		//		|NWireVector3D x ProjectedPosition2NWireStartVector3D|
		// D =	------------------------------------------------------
		//							|NWireVector3D|
		const double ProjectedPosition2NWireDistance = vnl_cross_3d(NWireVector3D,ProjectedPosition2NWireStartVector3D).magnitude() /	NWireVector3D.magnitude();

		this->PointLineDistanceErrorVector.put(i, ProjectedPosition2NWireDistance );
	}

	// Sort the PLDE in an ascending order

	this->PointLineDistanceErrorSortedVector = this->PointLineDistanceErrorVector;

	// Sorting algorithm employed: Insertion Sort
	for( int i = 0; i < NumberOfValidationPositions; i++ )
  {
		for( int j = i; j > 0 && this->PointLineDistanceErrorSortedVector.get(j-1) > this->PointLineDistanceErrorSortedVector.get(j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const double SwapPLDEValue = this->PointLineDistanceErrorSortedVector.get(j-1);
			
			this->PointLineDistanceErrorSortedVector.put( j-1, this->PointLineDistanceErrorSortedVector.get(j) );

			this->PointLineDistanceErrorSortedVector.put( j, SwapPLDEValue ); 
		}
  }

	// We are only interested at the top-ranked ascending PLDE values
	// presented in the sorted PLDE result vector.
	// Default percentage: 95%
	const int NumberOfTopRankedCalibrationData = ROUND( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel );
	const vnl_vector<double> AscendingTopRankingPLDEsinUSProbeFrame = this->PointLineDistanceErrorSortedVector.extract( NumberOfTopRankedCalibrationData );

	// Perform statistical analysis
	const double PLDE_mean = AscendingTopRankingPLDEsinUSProbeFrame.mean();
	const double PLDE_rms = AscendingTopRankingPLDEsinUSProbeFrame.rms();
	const double PLDE_std = sqrt( 1 / (double)( NumberOfTopRankedCalibrationData - 1 ) * ( AscendingTopRankingPLDEsinUSProbeFrame.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PLDE_mean,2) ) );

	// Consolidate the returning data container (for details see member variable description)
	this->PointLineDistanceErrorAnalysisVector.resize(0);
	this->PointLineDistanceErrorAnalysisVector.push_back( PLDE_mean );
	this->PointLineDistanceErrorAnalysisVector.push_back( PLDE_rms );
	this->PointLineDistanceErrorAnalysisVector.push_back( PLDE_std );
	this->PointLineDistanceErrorAnalysisVector.push_back( mValidationDataConfidenceLevel );

	// Reset the flag
	mAreIndependentPointLineReconErrorsReady = true;

	// Log the data pipeline if requested
  LOG_DEBUG("Wire #1");
  LOG_DEBUG("  LRE mean: (" << mNWire1AbsLREAnalysisInUSProbeFrame[0] << ", " << mNWire1AbsLREAnalysisInUSProbeFrame[1] << ")");
  LOG_DEBUG("  LRE std: (" << mNWire1AbsLREAnalysisInUSProbeFrame[2] << ", " << mNWire1AbsLREAnalysisInUSProbeFrame[3] << ")");
  LOG_DEBUG("  LRE EUC - mean: " << mNWire1AbsLREAnalysisInUSProbeFrame[4] << ", std: " << mNWire1AbsLREAnalysisInUSProbeFrame[5]);
  LOG_DEBUG("  Validation data confidence level: " << mNWire1AbsLREAnalysisInUSProbeFrame[6]);

  LOG_DEBUG("Wire #3");
  LOG_DEBUG("  LRE mean: (" << mNWire3AbsLREAnalysisInUSProbeFrame[0] << ", " << mNWire3AbsLREAnalysisInUSProbeFrame[1] << ")");
  LOG_DEBUG("  LRE std: (" << mNWire3AbsLREAnalysisInUSProbeFrame[2] << ", " << mNWire3AbsLREAnalysisInUSProbeFrame[3] << ")");
  LOG_DEBUG("  LRE EUC - mean: " << mNWire3AbsLREAnalysisInUSProbeFrame[4] << ", std: " << mNWire3AbsLREAnalysisInUSProbeFrame[5]);
  LOG_DEBUG("  Validation data confidence level: " << mNWire3AbsLREAnalysisInUSProbeFrame[6]);

  LOG_DEBUG("Wire #4");
  LOG_DEBUG("  LRE mean: (" << mNWire4AbsLREAnalysisInUSProbeFrame[0] << ", " << mNWire4AbsLREAnalysisInUSProbeFrame[1] << ")");
  LOG_DEBUG("  LRE std: (" << mNWire4AbsLREAnalysisInUSProbeFrame[2] << ", " << mNWire4AbsLREAnalysisInUSProbeFrame[3] << ")");
  LOG_DEBUG("  LRE EUC - mean: " << mNWire4AbsLREAnalysisInUSProbeFrame[4] << ", std: " << mNWire4AbsLREAnalysisInUSProbeFrame[5]);
  LOG_DEBUG("  Validation data confidence level: " << mNWire4AbsLREAnalysisInUSProbeFrame[6]);

  LOG_DEBUG("Wire #6");
  LOG_DEBUG("  LRE mean: (" << mNWire6AbsLREAnalysisInUSProbeFrame[0] << ", " << mNWire6AbsLREAnalysisInUSProbeFrame[1] << ")");
  LOG_DEBUG("  LRE std: (" << mNWire6AbsLREAnalysisInUSProbeFrame[2] << ", " << mNWire6AbsLREAnalysisInUSProbeFrame[3] << ")");
  LOG_DEBUG("  LRE EUC - mean: " << mNWire6AbsLREAnalysisInUSProbeFrame[4] << ", std: " << mNWire6AbsLREAnalysisInUSProbeFrame[5]);
  LOG_DEBUG("  Validation data confidence level: " << mNWire6AbsLREAnalysisInUSProbeFrame[6]);

	LOG_DEBUG("Point-Line Distance Error");
  LOG_DEBUG("  PLDE mean: " << this->PointLineDistanceErrorAnalysisVector[0]);
  LOG_DEBUG("  PLDE rms: " << this->PointLineDistanceErrorAnalysisVector[1]);
  LOG_DEBUG("  PLDE std: " << this->PointLineDistanceErrorAnalysisVector[2]);
  LOG_DEBUG("  Validation data confidence level: " << mNWire6AbsLREAnalysisInUSProbeFrame[3]);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::constructValidationDataMatrices()
{
	LOG_TRACE("vtkCalibrationController::constructValidationDataMatrices");

	// The total number of the validation data set.
	const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );
	
	// Populate the validation positions into to matrices for processing
	mValidationPositionsInUSImageFrameMatrix4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsInUSProbeFrameMatrix4xN.set_size(4, NumberOfValidationPositions);
	
	mValidationPositionsNWire1InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsNWire3InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsNWire4InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsNWire6InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);

	mValidationPositionsNWire1InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsNWire3InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsNWire4InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);
	mValidationPositionsNWire6InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);

	mValidationPositionsNWireStartInUSProbeFrame3xN.set_size(3, NumberOfValidationPositions);
	mValidationPositionsNWireEndInUSProbeFrame3xN.set_size(3, NumberOfValidationPositions);
	vnl_matrix<double> ValidationPositionsNWireStartInUSProbeFrame4xN(4, NumberOfValidationPositions);
	vnl_matrix<double> ValidationPositionsNWireEndInUSProbeFrame4xN(4, NumberOfValidationPositions);

	for( int i = 0; i < NumberOfValidationPositions; i++ )
	{
		// Validation positions in the US image frame
		mValidationPositionsInUSImageFrameMatrix4xN.set_column( i, mValidationPositionsInUSImageFrame.at(i) );

		// Validation positions in the US probe frame
		mValidationPositionsInUSProbeFrameMatrix4xN.set_column( i, mValidationPositionsInUSProbeFrame.at(i) );

		// Validation points' NWire Start/End positions in US probe frame
		ValidationPositionsNWireStartInUSProbeFrame4xN.set_column( i, mValidationPositionsNWireStartInUSProbeFrame.at(i) );
		ValidationPositionsNWireEndInUSProbeFrame4xN.set_column( i, mValidationPositionsNWireEndInUSProbeFrame.at(i) );

		// Validation positions of the parallel wires in the US image frame
		// Wire N1
		mValidationPositionsNWire1InUSImageFrame4xN.set_column( i, mValidationPositionsNWire1InUSImageFrame.at(i) );
		// Wire N3
		mValidationPositionsNWire3InUSImageFrame4xN.set_column( i, mValidationPositionsNWire3InUSImageFrame.at(i) );
		// Wire N4
		mValidationPositionsNWire4InUSImageFrame4xN.set_column( i, mValidationPositionsNWire4InUSImageFrame.at(i) );
		// Wire N6
		mValidationPositionsNWire6InUSImageFrame4xN.set_column( i, mValidationPositionsNWire6InUSImageFrame.at(i) );

		// Validation positions of the parallel wires in the US probe frame
		// Wire N1
		mValidationPositionsNWire1InUSProbeFrame4xN.set_column( i, mValidationPositionsNWire1InUSProbeFrame.at(i) );
		// Wire N3
		mValidationPositionsNWire3InUSProbeFrame4xN.set_column( i, mValidationPositionsNWire3InUSProbeFrame.at(i) );
		// Wire N4
		mValidationPositionsNWire4InUSProbeFrame4xN.set_column( i, mValidationPositionsNWire4InUSProbeFrame.at(i) );
		// Wire N6
		mValidationPositionsNWire6InUSProbeFrame4xN.set_column( i, mValidationPositionsNWire6InUSProbeFrame.at(i) );
	}
	
	// For PLDE calcuation, we need the first 3 elements for cross products.
	mValidationPositionsNWireStartInUSProbeFrame3xN = ValidationPositionsNWireStartInUSProbeFrame4xN.get_n_rows(0, 3);
	mValidationPositionsNWireEndInUSProbeFrame3xN = ValidationPositionsNWireEndInUSProbeFrame4xN.get_n_rows(0, 3);

	mAreValidationDataMatricesConstructed = true;  //check the return value instead

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkCalibrationController::SaveCalibrationResultsAndErrorReportsToXML()
{
  // Construct the calibration result file name with path and timestamp
  std::string calibrationTimestamp;
  if (this->CalibrationTimestamp == NULL) {
    LOG_ERROR("Calibration timestamp is not set!");
    calibrationTimestamp = "";
  }
  else
  {
    calibrationTimestamp = this->CalibrationTimestamp;
  }
  const std::string calibrationResultFileName = calibrationTimestamp + ".Calibration.results.xml";
	const std::string calibrationResultFileNameWithPath = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/") + calibrationResultFileName;
  this->SetCalibrationResultFileNameWithPath(calibrationResultFileNameWithPath.c_str());

	// <USTemplateCalibrationResult>
	vtkSmartPointer<vtkXMLDataElement> xmlCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	xmlCalibrationResults->SetName("USTemplateCalibrationResult"); 
	xmlCalibrationResults->SetAttribute("version", "1.0"); 

	// <CalibrationFile> 
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationFile = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationFile->SetName("CalibrationFile"); 
	tagCalibrationFile->SetAttribute("Timestamp", calibrationTimestamp.c_str()); 
	tagCalibrationFile->SetAttribute("FileName", calibrationResultFileName.c_str()); 
	vtkstd::string commentCalibrationFile("# Timestamp format: MM/DD/YY HH:MM:SS"); 
	tagCalibrationFile->AddCharacterData(commentCalibrationFile.c_str(), commentCalibrationFile.size()); 

  // <CalibrationResults>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationResults->SetName("CalibrationResults"); 

	double *imageToUserImageMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			imageToUserImageMatrix[i*4+j] = this->TransformImageToUserImage->GetMatrix()->GetElement(i,j); 
		}
	}

	double *userImageToProbeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			userImageToProbeMatrix[i*4+j] = this->TransformUserImageToProbe->GetMatrix()->GetElement(i,j); 
		}
	}

	double *referenceToTemplateHolderHomeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			referenceToTemplateHolderHomeMatrix[i*4+j] = this->TransformReferenceToTemplateHolderHome->GetMatrix()->GetElement(i,j); 
		}
	}

	double *templateHolderToTemplateMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			templateHolderToTemplateMatrix[i*4+j] = this->TransformTemplateHolderToTemplate->GetMatrix()->GetElement(i,j); 
		}
	}

	double *templateHomeToTemplateMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			templateHomeToTemplateMatrix[i*4+j] = this->TransformTemplateHomeToTemplate->GetMatrix()->GetElement(i,j); 
		}
	}

	double *imageToTemplateMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			imageToTemplateMatrix[i*4+j] = this->TransformImageToTemplate->GetMatrix()->GetElement(i,j); 
		}
	}

	// <CalibrationTransform>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationTransform = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationTransform->SetName("CalibrationTransform"); 
	tagCalibrationTransform->SetVectorAttribute("TransformImageToUserImage", 16, imageToUserImageMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformUserImageToProbe", 16, userImageToProbeMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformReferenceToTemplateHolderHome", 16, referenceToTemplateHolderHomeMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformTemplateHolderToTemplate", 16, templateHolderToTemplateMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformTemplateHomeToTemplate", 16, templateHomeToTemplateMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformImageToTemplate", 16, imageToTemplateMatrix); 

	delete[] imageToUserImageMatrix; 
	delete[] userImageToProbeMatrix; 
	delete[] referenceToTemplateHolderHomeMatrix; 
	delete[] templateHolderToTemplateMatrix; 
	delete[] templateHomeToTemplateMatrix; 
	delete[] imageToTemplateMatrix; 

	tagCalibrationResults->AddNestedElement(tagCalibrationTransform); 

	// <ErrorReports>
	vtkSmartPointer<vtkXMLDataElement> tagErrorReports = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagErrorReports->SetName("ErrorReports"); 

	double *preAnalysis = new double[9]; 
	for (int i = 0; i < 9; i++)
	{
		preAnalysis[i] = this->PointReconstructionErrorAnalysisVector[i]; 
	}

	// <PointReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrorAnalysis->SetName("PointReconstructionErrorAnalysis"); 
	tagPointReconstructionErrorAnalysis->SetVectorAttribute("PRE", 9, preAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->PointReconstructionErrorAnalysisVector[9]);  
	vtkstd::string commentPointReconstructionErrorAnalysis("# PRE format: PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std"); 
	tagPointReconstructionErrorAnalysis->AddCharacterData(commentPointReconstructionErrorAnalysis.c_str(), commentPointReconstructionErrorAnalysis.size()); 

  delete[] preAnalysis; 

	double *rawPointReconstructionErrors = new double[this->PointReconstructionErrorMatrix.size()]; 
	for ( int row = 0; row < this->PointReconstructionErrorMatrix.rows(); row++)
	{
		for (int column = 0; column < this->PointReconstructionErrorMatrix.cols(); column++)
		{
			rawPointReconstructionErrors[row * this->PointReconstructionErrorMatrix.cols() + column ] = this->PointReconstructionErrorMatrix.get(row, column); 
		}
	}

	// <PointReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrors->SetName("PointReconstructionErrors"); 
	tagPointReconstructionErrors->SetVectorAttribute("Raw", this->PointReconstructionErrorMatrix.size(), rawPointReconstructionErrors); 
	vtkstd::ostringstream commentPointReconstructionErrors; 
	commentPointReconstructionErrors << "# PointReconstructionErrors format: 4xN matrix, N = "; 
	commentPointReconstructionErrors << this->PointReconstructionErrorMatrix.columns(); 
	commentPointReconstructionErrors << ": the number of validation points"; 
	tagPointReconstructionErrors->AddCharacterData(commentPointReconstructionErrors.str().c_str(), commentPointReconstructionErrors.str().size());

	delete[] rawPointReconstructionErrors; 

	double *pldeAnalysis = new double[3]; 
	for (int i = 0; i < 3; i++)
	{
		pldeAnalysis[i] = this->PointLineDistanceErrorAnalysisVector[i]; 
	}

	// <PointLineDistanceErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrorAnalysis->SetName("PointLineDistanceErrorAnalysis"); 
	tagPointLineDistanceErrorAnalysis->SetVectorAttribute("PLDE", 3, pldeAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointLineDistanceErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->PointLineDistanceErrorAnalysisVector[3]);  
	vtkstd::string commentPointLineDistanceErrorAnalysis("# PLDE format: PLDE_mean, PLDE_rms, PLDE_std"); 
	tagPointLineDistanceErrorAnalysis->AddCharacterData(commentPointLineDistanceErrorAnalysis.c_str(), commentPointLineDistanceErrorAnalysis.size()); 

  delete[] pldeAnalysis; 

	double *rawPointLineDistanceErrors = new double[this->PointLineDistanceErrorVector.size()]; 
	for (int i = 0; i < this->PointLineDistanceErrorVector.size(); i++)
	{
		rawPointLineDistanceErrors[i] = this->PointLineDistanceErrorVector.get(i); 
	}

	double *sortedPointLineDistanceErrors = new double[this->PointLineDistanceErrorVector.size()]; 
	for (int i = 0; i < this->PointLineDistanceErrorVector.size(); i++)
	{
		sortedPointLineDistanceErrors[i] = this->PointLineDistanceErrorVector.get(i); 
	}

	// <PointLineDistanceErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrors->SetName("PointLineDistanceErrors"); 
	tagPointLineDistanceErrors->SetVectorAttribute("Raw", this->PointLineDistanceErrorVector.size(), rawPointLineDistanceErrors); 
	vtkstd::ostringstream commentPointLineDistanceErrors; 
	commentPointLineDistanceErrors << "# PointLineDistanceErrors format: 1xN vector, N = "; 
	commentPointLineDistanceErrors << this->PointLineDistanceErrorVector.size(); 
	commentPointLineDistanceErrors << ": the number of validation points"; 
	tagPointLineDistanceErrors->AddCharacterData(commentPointLineDistanceErrors.str().c_str(), commentPointLineDistanceErrors.str().size()); 

  delete[] rawPointLineDistanceErrors; 
	delete[] sortedPointLineDistanceErrors; 

  std::vector<double> w1LREVector; 
  if ( this->GetLineReconstructionErrorAnalysisVector(1, w1LREVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get LRE for wire #1."); 
  }
  else
  {
    double *w1LREAnalysis = new double[6]; 
    for (int i = 0; i < 6; i++)
    {
      w1LREAnalysis[i] = w1LREVector[i]; 
    }

    // <Wire1LineReconstructionErrorAnalysis>
    vtkSmartPointer<vtkXMLDataElement> tagWire1LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
    tagWire1LineReconstructionErrorAnalysis->SetName("Wire1LineReconstructionErrorAnalysis"); 
    tagWire1LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w1LREAnalysis);  
    // # The percentage of top-ranked validation data used for evaluation
    tagWire1LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", w1LREVector[6]);  
    vtkstd::string commentWire1LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
    tagWire1LineReconstructionErrorAnalysis->AddCharacterData(commentWire1LineReconstructionErrorAnalysis.c_str(), commentWire1LineReconstructionErrorAnalysis.size()); 
    delete[] w1LREAnalysis; 

    tagErrorReports->AddNestedElement(tagWire1LineReconstructionErrorAnalysis);
  }

	const int w1NumOfRows = this->GetLineReconstructionErrorMatrix(1).rows(); 
	const int w1NumOfCols = this->GetLineReconstructionErrorMatrix(1).cols(); 
	double *w1rawXLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawYLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawEUCLineReconstructionErrors = new double[w1NumOfCols]; 

	for (int column = 0; column < w1NumOfCols; column++)
	{
		w1rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(1).get(0, column); 
		w1rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(1).get(1, column); 
		w1rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(1).get(3, column);
	}

	// <Wire1LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW1LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW1LineReconstructionErrors->SetName("Wire1LineReconstructionErrors"); 
	tagW1LineReconstructionErrors->SetVectorAttribute("X", w1NumOfCols, w1rawXLineReconstructionErrors); 
	tagW1LineReconstructionErrors->SetVectorAttribute("Y", w1NumOfCols, w1rawYLineReconstructionErrors); 
	tagW1LineReconstructionErrors->SetVectorAttribute("EUC", w1NumOfCols, w1rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW1LineReconstructionErrors; 
	commentW1LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW1LineReconstructionErrors << w1NumOfCols; 
	commentW1LineReconstructionErrors << ": the number of validation points"; 
	tagW1LineReconstructionErrors->AddCharacterData(commentW1LineReconstructionErrors.str().c_str(), commentW1LineReconstructionErrors.str().size()); 

  delete[] w1rawXLineReconstructionErrors; 
	delete[] w1rawYLineReconstructionErrors; 
	delete[] w1rawEUCLineReconstructionErrors;
	
  std::vector<double> w3LREVector; 
  if ( this->GetLineReconstructionErrorAnalysisVector(3, w3LREVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get LRE for wire #3."); 
  }
  else
  {
    double *w3LREAnalysis = new double[6]; 
    for (int i = 0; i < 6; i++)
    {
      w3LREAnalysis[i] = w3LREVector[i]; 
    }
    // <Wire3LineReconstructionErrorAnalysis>
    vtkSmartPointer<vtkXMLDataElement> tagWire3LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
    tagWire3LineReconstructionErrorAnalysis->SetName("Wire3LineReconstructionErrorAnalysis"); 
    tagWire3LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w3LREAnalysis);  
    // # The percentage of top-ranked validation data used for evaluation
    tagWire3LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", w3LREVector[6]);  
    vtkstd::string commentWire3LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
    tagWire3LineReconstructionErrorAnalysis->AddCharacterData(commentWire3LineReconstructionErrorAnalysis.c_str(), commentWire3LineReconstructionErrorAnalysis.size()); 

    delete[] w3LREAnalysis;

    tagErrorReports->AddNestedElement(tagWire3LineReconstructionErrorAnalysis);
  }

	const int w3NumOfRows = this->GetLineReconstructionErrorMatrix(3).rows(); 
	const int w3NumOfCols = this->GetLineReconstructionErrorMatrix(3).cols(); 
	double *w3rawXLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawYLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawEUCLineReconstructionErrors = new double[w3NumOfCols];

	for (int column = 0; column < w3NumOfCols; column++)
	{
		w3rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(3).get(0, column); 
		w3rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(3).get(1, column); 
		w3rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(3).get(3, column); 
	}

	// <Wire3LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW3LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW3LineReconstructionErrors->SetName("Wire3LineReconstructionErrors"); 
	tagW3LineReconstructionErrors->SetVectorAttribute("X", w3NumOfCols, w3rawXLineReconstructionErrors); 
	tagW3LineReconstructionErrors->SetVectorAttribute("Y", w3NumOfCols, w3rawYLineReconstructionErrors); 
	tagW3LineReconstructionErrors->SetVectorAttribute("EUC", w3NumOfCols, w3rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW3LineReconstructionErrors; 
	commentW3LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW3LineReconstructionErrors << w3NumOfCols; 
	commentW3LineReconstructionErrors << ": the number of validation points"; 
	tagW3LineReconstructionErrors->AddCharacterData(commentW3LineReconstructionErrors.str().c_str(), commentW3LineReconstructionErrors.str().size()); 

  delete[] w3rawXLineReconstructionErrors; 
	delete[] w3rawYLineReconstructionErrors; 
	delete[] w3rawEUCLineReconstructionErrors;

  std::vector<double> w4LREVector; 
  if ( this->GetLineReconstructionErrorAnalysisVector(4, w4LREVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get LRE for wire #4."); 
  }
  else
  {
    double *w4LREAnalysis = new double[6]; 
    for (int i = 0; i < 6; i++)
    {
      w4LREAnalysis[i] = w4LREVector[i]; 
    }

    // <Wire4LineReconstructionErrorAnalysis>
    vtkSmartPointer<vtkXMLDataElement> tagWire4LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
    tagWire4LineReconstructionErrorAnalysis->SetName("Wire4LineReconstructionErrorAnalysis"); 
    tagWire4LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w4LREAnalysis);  
    // # The percentage of top-ranked validation data used for evaluation
    tagWire4LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", w4LREVector[6]);  
    vtkstd::string commentWire4LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
    tagWire4LineReconstructionErrorAnalysis->AddCharacterData(commentWire4LineReconstructionErrorAnalysis.c_str(), commentWire4LineReconstructionErrorAnalysis.size()); 

    delete[] w4LREAnalysis;

    tagErrorReports->AddNestedElement(tagWire4LineReconstructionErrorAnalysis);
  }

	const int w4NumOfRows = this->GetLineReconstructionErrorMatrix(4).rows(); 
	const int w4NumOfCols = this->GetLineReconstructionErrorMatrix(4).cols(); 
	double *w4rawXLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawYLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawEUCLineReconstructionErrors = new double[w4NumOfCols];

	for (int column = 0; column < w4NumOfCols; column++)
	{
		w4rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(4).get(0, column); 
		w4rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(4).get(1, column); 
		w4rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(4).get(3, column); 
	}

	// <Wire4LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW4LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW4LineReconstructionErrors->SetName("Wire4LineReconstructionErrors"); 
	tagW4LineReconstructionErrors->SetVectorAttribute("X", w4NumOfCols, w4rawXLineReconstructionErrors); 
	tagW4LineReconstructionErrors->SetVectorAttribute("Y", w4NumOfCols, w4rawYLineReconstructionErrors); 
	tagW4LineReconstructionErrors->SetVectorAttribute("EUC", w4NumOfCols, w4rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW4LineReconstructionErrors; 
	commentW4LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW4LineReconstructionErrors << w4NumOfCols; 
	commentW4LineReconstructionErrors << ": the number of validation points"; 
	tagW4LineReconstructionErrors->AddCharacterData(commentW4LineReconstructionErrors.str().c_str(), commentW4LineReconstructionErrors.str().size()); 

  delete[] w4rawXLineReconstructionErrors; 
	delete[] w4rawYLineReconstructionErrors; 
	delete[] w4rawEUCLineReconstructionErrors;

  std::vector<double> w6LREVector; 
  if ( this->GetLineReconstructionErrorAnalysisVector(6, w6LREVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get LRE for wire #6."); 
  }
  else
  {
    double *w6LREAnalysis = new double[6];
    for (int i = 0; i < 6; i++)
    {
      w6LREAnalysis[i] = w6LREVector[i]; 
    }

    // <Wire6LineReconstructionErrorAnalysis>
    vtkSmartPointer<vtkXMLDataElement> tagWire6LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
    tagWire6LineReconstructionErrorAnalysis->SetName("Wire6LineReconstructionErrorAnalysis"); 
    tagWire6LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w6LREAnalysis);  
    // # The percentage of top-ranked validation data used for evaluation
    tagWire6LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", w6LREVector[6]);  
    vtkstd::string commentWire6LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
    tagWire6LineReconstructionErrorAnalysis->AddCharacterData(commentWire6LineReconstructionErrorAnalysis.c_str(), commentWire6LineReconstructionErrorAnalysis.size()); 

    delete[] w6LREAnalysis;
  
    tagErrorReports->AddNestedElement(tagWire6LineReconstructionErrorAnalysis);
  }

	const int w6NumOfRows = this->GetLineReconstructionErrorMatrix(6).rows(); 
	const int w6NumOfCols = this->GetLineReconstructionErrorMatrix(6).cols(); 
	double *w6rawXLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawYLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawEUCLineReconstructionErrors = new double[w6NumOfCols];

	for (int column = 0; column < w6NumOfCols; column++)
	{
		w6rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(6).get(0, column); 
		w6rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(6).get(1, column); 
		w6rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(6).get(3, column); 
	}

	// <Wire6LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW6LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW6LineReconstructionErrors->SetName("Wire6LineReconstructionErrors"); 
	tagW6LineReconstructionErrors->SetVectorAttribute("X", w6NumOfCols, w6rawXLineReconstructionErrors); 
	tagW6LineReconstructionErrors->SetVectorAttribute("Y", w6NumOfCols, w6rawYLineReconstructionErrors); 
	tagW6LineReconstructionErrors->SetVectorAttribute("EUC", w6NumOfCols, w6rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW6LineReconstructionErrors; 
	commentW6LineReconstructionErrors << "# LineReconstructionErrors format: 6xN matrix, N = "; 
	commentW6LineReconstructionErrors << w6NumOfCols; 
	commentW6LineReconstructionErrors << ": the number of validation points"; 
	tagW6LineReconstructionErrors->AddCharacterData(commentW6LineReconstructionErrors.str().c_str(), commentW6LineReconstructionErrors.str().size()); 

  delete[] w6rawXLineReconstructionErrors; 
	delete[] w6rawYLineReconstructionErrors;  
	delete[] w6rawEUCLineReconstructionErrors;

	tagErrorReports->AddNestedElement(tagPointReconstructionErrorAnalysis); 
	tagErrorReports->AddNestedElement(tagPointReconstructionErrors);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrorAnalysis);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrors);
	tagErrorReports->AddNestedElement(tagW1LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagW3LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagW4LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagW6LineReconstructionErrors);
	// <ErrorReports>

	xmlCalibrationResults->AddNestedElement(tagCalibrationFile); 
	xmlCalibrationResults->AddNestedElement(tagCalibrationResults); 
	xmlCalibrationResults->AddNestedElement(tagErrorReports); 

  xmlCalibrationResults->PrintXML(calibrationResultFileNameWithPath.c_str()); 
}

//----------------------------------------------------------------------------
// TODO: Read it from XML
PlusStatus vtkCalibrationController::ReadUs3DBeamwidthDataFromFile()
{
	// #1. Read the US 3D Beamwidth Data from a pre-populated file
	std::ifstream USBeamProfileFile( this->US3DBeamProfileDataFileNameAndPath, std::ios::in );
	if( !USBeamProfileFile.is_open() )
	{
		LOG_ERROR("Failed to open the US 3D beam profile data file: " << this->US3DBeamProfileDataFileNameAndPath); 
		return PLUS_FAIL;
	}

	std::string SectionName("");
	std::string ThisConfiguration("");

	// Start from the beginning of the file
	USBeamProfileFile.seekg( 0, ios::beg );

	// Axial Position of the Transducer's Crystal in Original Image Frame
	ThisConfiguration = "AXIAL_POSITION_OF_TRANSDUCER_CRYSTAL_SURFACE_IN_PIXELS]";
	while ( USBeamProfileFile.eof() != true && SectionName != ThisConfiguration )
	{
		USBeamProfileFile.ignore(1024, '[');
		USBeamProfileFile >> SectionName;
	}

	if(  SectionName != ThisConfiguration )
	{	// If the designated configuration is not found, return with error
		LOG_ERROR("Cannot find the config section named: [" << ThisConfiguration << " in the configuration file!");
		USBeamProfileFile.close();
    return PLUS_FAIL;
	}

	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '\n');
	int axialPositionOfCrystalSurfaceInOrigImageFrame(-1); 
	USBeamProfileFile >> axialPositionOfCrystalSurfaceInOrigImageFrame;

	// Minimum US Elevation Beamwidth and its Focal Zone in US Image Frame
	ThisConfiguration = "MINIMUM_ELEVATION_BEAMWIDTH_AND_FOCAL_ZONE]";
	while ( USBeamProfileFile.eof() != true && SectionName != ThisConfiguration )
	{
		USBeamProfileFile.ignore(1024, '[');
		USBeamProfileFile >> SectionName;
	}

	if(  SectionName != ThisConfiguration )
	{	// If the designated configuration is not found, return with error
		LOG_ERROR("Cannot find the config section named: [" << ThisConfiguration << " in the configuration file!");
		USBeamProfileFile.close();
    return PLUS_FAIL;
	}

	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '\n');

	double ElevationFocalZoneAtAxialDistance2TheCrystals(-1);
	double MinElevationBeamwidth(-1);
	USBeamProfileFile >> ElevationFocalZoneAtAxialDistance2TheCrystals >> MinElevationBeamwidth;

	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToUserImage->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4);

	// Convert the crystal surface position to TRUS image frame
	double axialPositionOfCrystalSurfaceInTRUSImageFrame = transformOrigImageFrame2TRUSImageFrameMatrix4x4.get(1,1) * axialPositionOfCrystalSurfaceInOrigImageFrame + transformOrigImageFrame2TRUSImageFrameMatrix4x4.get(1,3);

	this->SetAxialPositionOfCrystalSurfaceInTRUSImageFrame(axialPositionOfCrystalSurfaceInTRUSImageFrame);

	// Then convert the axial distance of the focal-zone into the TRUS Image Frame
	const double ElevationFocalZoneInUSImageFrame = axialPositionOfCrystalSurfaceInTRUSImageFrame + ElevationFocalZoneAtAxialDistance2TheCrystals;

	this->MinElevationBeamwidthAndFocalZoneInUSImageFrame.put(0, ElevationFocalZoneInUSImageFrame );
	this->MinElevationBeamwidthAndFocalZoneInUSImageFrame.put(1,MinElevationBeamwidth );

	// US 3D Beamwidth Profile Data
	ThisConfiguration = "US_3D_BEAM_PROFILE]";
	while ( USBeamProfileFile.eof() != true && SectionName != ThisConfiguration )
	{
		USBeamProfileFile.ignore(1024, '[');
		USBeamProfileFile >> SectionName;
	}

	if(  SectionName != ThisConfiguration )
	{	// If the designated configuration is not found, return with error
		LOG_ERROR("Cannot find the config section named: [" << ThisConfiguration << " in the configuration file!");
		USBeamProfileFile.close();
    return PLUS_FAIL;
	}

	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '\n');

	int numUS3DBeamwidthProfileData; 
	USBeamProfileFile >> numUS3DBeamwidthProfileData;
  this->NumUS3DBeamwidthProfileData = numUS3DBeamwidthProfileData;

	if( this->NumUS3DBeamwidthProfileData <= 0 )
	{	// If the number of data is invalid, return with error
		LOG_ERROR("The number of US 3D beamwidth profile data is invalid! Please double check the data file!");
    return PLUS_FAIL;
	}
	USBeamProfileFile.ignore(1024, '#');
	USBeamProfileFile.ignore(1024, '\n');

	// Here the profile read from file is unsorted (for ascending axial depth), but we will do the sorting in the next step.
	vnl_matrix<double> SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4(this->NumUS3DBeamwidthProfileData, 4);
	USBeamProfileFile >> SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4;

	// Close the file for reading
	USBeamProfileFile.close();

	this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_size(this->NumUS3DBeamwidthProfileData, 4); 
	for ( int r = 0; r < this->NumUS3DBeamwidthProfileData; r++)
	{
		this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_row(r,SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_row(r)); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::LoadUS3DBeamProfileData()
{
	this->SetUS3DBeamwidthDataReady(false); 

	// #1. Read the US 3D Beamwidth Data from a pre-populated file
  if (ReadUs3DBeamwidthDataFromFile() != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading beamwidth data from file failed!");
    return PLUS_FAIL;
  }

	// #2. Sort the beamwidth in an ascending order with respect to the axial depth
	// Sorting the beamwidth profile w.r.t the axial distance to the transducer
	// Sorting algorithm employed: Insertion Sort
	for( int i = 0; i < this->NumUS3DBeamwidthProfileData; i++ )
  {
		for( int j = i; j > 0 && this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get(j-1, 0) > this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get(j, 0); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> SwapVector = this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_row(j-1);

			this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_row( j-1, this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_row(j) );
			this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_row( j, SwapVector ); 
		}
  }

	// #3. US 3D beamwidth profile data in ascending axial depth with weight factors
  // For further details see SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN member variable declaration
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_size(5, this->NumUS3DBeamwidthProfileData);

	// Convert the depth of the beamwidth data into the TRUS Image Frame
	const vnl_vector<double> YPositionsOfUSBeamwidthInTRUSImageFramePixels = this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_column(0) + this->AxialPositionOfCrystalSurfaceInTRUSImageFrame;

	// Reset the axial-depth position and populate the remaining unchanged data
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_row(0, YPositionsOfUSBeamwidthInTRUSImageFramePixels );
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_row(1, this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_column(1));
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_row(2, this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_column(2));
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_row(3, this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_column(3));

	// Calculate the weight factor using: CurrentBeamwidth/MinimumBeamwidth
	// NOTE: For the moment, we are only using the elevation beamwidth (the 4th column) 
	// which has the largest beamwidth among axial, lateral and elevational axes and 
	// plays an dominant role in the error distributions.  However, if axial and lateral
	// beamwidth can also be added into the calculation if they are available.
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_row(4, this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_column(3) / this->MinElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) );

	// #4. Interpolate the beamwidth profile at non-sampled axial depth

	// Initialize the Interpolated Beamwidth list to zero first
	std::vector<vnl_vector<double>> InterpUS3DBeamwidthAndWeightFactorsList;
	InterpUS3DBeamwidthAndWeightFactorsList.resize(0);

	for( int i = 0; i < this->NumUS3DBeamwidthProfileData - 1; i++ )
	{
		// Obtain the starting and ending sampled beamwidth data for interpolation
		const vnl_vector<double> StartBeamWidthDataVector = this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.get_column(i);
		const vnl_vector<double> EndBeamWidthDataVector = this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.get_column(i+1);

		// Obtain the starting and ending axial depth in US Image Frame
		const int StartDepthInUSImageFrame = StartBeamWidthDataVector.get(0);
		const int EndDepthInUSImageFrame = EndBeamWidthDataVector.get(0);

		// Start the interpolation of the beamwidth data
		for( int j = StartDepthInUSImageFrame; j < EndDepthInUSImageFrame; j++ )
		{
			// Perform the linear interpolation
			vnl_vector<double> InterpolatedBeamwidthDataVector = (EndBeamWidthDataVector - StartBeamWidthDataVector) *
        (double(j - StartDepthInUSImageFrame) / double(EndDepthInUSImageFrame - StartDepthInUSImageFrame)) + StartBeamWidthDataVector;

			// Set the axial depth correctly
			InterpolatedBeamwidthDataVector.put(0, j);

			// Populate the interpolation list
			InterpUS3DBeamwidthAndWeightFactorsList.push_back( InterpolatedBeamwidthDataVector );
		}

		if( this->NumUS3DBeamwidthProfileData - 2 == i )
		{
			// Add in the last data in the list (which was not accounted for in the process)
			InterpUS3DBeamwidthAndWeightFactorsList.push_back( EndBeamWidthDataVector );
		}
	}

	// Populate the data to matrix container
	const int TotalNumOfInterpolatedUSBeamwidthData( InterpUS3DBeamwidthAndWeightFactorsList.size() );
	this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(5, TotalNumOfInterpolatedUSBeamwidthData);

	for( int i = 0; i < TotalNumOfInterpolatedUSBeamwidthData; i++ )
	{
		this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_column( i, InterpUS3DBeamwidthAndWeightFactorsList.at(i) );
	}

	// Set the flag to signal the data is now ready
	this->SetUS3DBeamwidthDataReady(true); 

  return PLUS_SUCCESS;
}

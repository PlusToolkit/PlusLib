#include "PlusConfigure.h"

#include "vtkFreehandCalibrationController.h"

#include "vtkObjectFactory.h"

#include "vtkMath.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFreehandCalibrationController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkFreehandCalibrationController);

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::vtkFreehandCalibrationController()
{
	this->ProgressPercent = 0;
	this->CancelRequest = false;
	this->DataCollector = NULL;

  this->USImageFrameOriginXInPixels = 0; 
	this->USImageFrameOriginYInPixels = 0; 

	// Initializing vtkCalibrationController members
	this->InitializedOff(); 
	this->EnableSystemLogOff();
	this->CalibrationResultFileNameWithPath = NULL;
	this->CalibrationResultFileSuffix = NULL;

	this->Calibrator = NULL;

	vtkSmartPointer<vtkTransform> transformImageToProbe = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToProbe = NULL;
	this->SetTransformImageToProbe(transformImageToProbe); 

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::~vtkFreehandCalibrationController()
{
	this->SetTransformImageToProbe(NULL);

	if (this->Calibrator != NULL) {
		delete this->Calibrator;
		this->Calibrator = NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoSpatialCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoSpatialCalibration");

	const int maxNumberOfValidationImages = this->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToAcquire; 
	const int maxNumberOfCalibrationImages = this->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToAcquire; 
	int numberOfAcquiredImages = 0;
	int numberOfFailedSegmentations = 0;

	this->CancelRequestOff();

	// Acquire and add validation and calibration data
	while ((this->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages < maxNumberOfValidationImages)
		|| (this->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages < maxNumberOfCalibrationImages)) {

		bool segmentationSuccessful = false;

		if (this->CancelRequest) {
			// Cancel the job
			return PLUS_FAIL;
		}

		// Get latest tracked frame from data collector
		TrackedFrame trackedFrame; 
		this->DataCollector->GetTrackedFrame(&trackedFrame); 

		if (trackedFrame.Status & (TR_MISSING | TR_OUT_OF_VIEW)) {
			LOG_DEBUG("Tracker out of view"); 
		} else if ( trackedFrame.Status & (TR_REQ_TIMEOUT)) {
			LOG_DEBUG("Tracker request timeout"); 
		} else { // TR_OK
			if (numberOfAcquiredImages < maxNumberOfValidationImages) {
				// Validation data
				if ( this->GetTrackedFrameList(FREEHAND_MOTION_2)->ValidateData(&trackedFrame) ) {
					if (AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_2)) {
						segmentationSuccessful = true;
					} else {
						++numberOfFailedSegmentations;
						LOG_DEBUG("Adding tracked frame " << this->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages << " (for validation) failed!");
					}
				}
			} else {
				// Calibration data
				if ( this->GetTrackedFrameList(FREEHAND_MOTION_1)->ValidateData(&trackedFrame) ) {
					if (AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_1)) {
						segmentationSuccessful = true;
					} else {
						++numberOfFailedSegmentations;
						LOG_DEBUG("Adding tracked frame " << this->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " (for calibration) failed!");
					}
				}
			}
		}

		// Update progress if tracked frame has been successfully added
		if (segmentationSuccessful) {
			++numberOfAcquiredImages;

      int progressPercent = (int)((numberOfAcquiredImages / (double)(maxNumberOfValidationImages + maxNumberOfCalibrationImages)) * 100.0);
			this->SetProgressPercent( progressPercent );
		}

		// Display segmented points (or hide them if unsuccessful)
		//DisplaySegmentedPoints(segmentationSuccessful);
	}

	LOG_INFO("Segmentation success rate: " << numberOfAcquiredImages << " out of " << numberOfAcquiredImages + numberOfFailedSegmentations << " (" << (int)(((double)numberOfAcquiredImages / (double)(numberOfAcquiredImages + numberOfFailedSegmentations)) * 100.0 + 0.49) << " percent)");

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::InitializeCalibration(vtkTransform* aPhantomToPhantomReferenceTransform)
{
	LOG_TRACE("vtkFreehandCalibrationController::InitializeCalibration");

	// Initialize the segmentation component
  if (this->GetPatternRecognition() == NULL) {
    this->PatternRecognition.ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
	}	

	// Initialize the calibration component
	if (this->Calibrator == NULL) {
		this->Calibrator = new BrachyTRUSCalibrator( this->GetPatternRecognition(), this->GetEnableSystemLog() );
	}

	// Set the ultrasound image frame in pixels
	// This defines the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row).
	this->SetUSImageFrameOriginInPixels( this->GetUSImageFrameOriginXInPixels(), this->GetUSImageFrameOriginYInPixels() ); 

	// Apply the US 3D beamwidth data to calibration if desired (pass the US 3D beamwidth data and their predefined weights to the calibration component)
	/*
	if( this->GetUS3DBeamwidthDataReady() )
	{
		this->GetCalibrator()->setUltrasoundBeamwidthAndWeightFactorsTable(
			this->GetIncorporatingUS3DBeamProfile(),
			*this->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM(),
			*this->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN(),
			*this->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() );
	}
	*/

	// Set identity for image frame shifting transform
	vtkSmartPointer<vtkMatrix4x4> identity = vtkSmartPointer<vtkMatrix4x4>::New();
	identity->Identity();
	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	ConvertVtkMatrixToVnlMatrix(identity, transformOrigImageFrame2TRUSImageFrameMatrix4x4); 
	this->GetCalibrator()->setTransformOrigImageToTRUSImageFrame4x4(transformOrigImageFrame2TRUSImageFrameMatrix4x4);

	// Register the phantom geometry to the DRB frame in the "Emulator" mode.
	vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
	this->ConvertVtkMatrixToVnlMatrixInMeter(aPhantomToPhantomReferenceTransform->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode);

	this->Calibrator->registerPhantomGeometryInEmulatorMode(transformMatrixPhantom2DRB4x4InEmulatorMode);

	return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ResetCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::ResetCalibration");

	// Empty tracked frame containers
	this->TrackedFrameListContainer[FREEHAND_MOTION_1]->Clear();
	this->TrackedFrameListContainer[FREEHAND_MOTION_2]->Clear();

  // Reset segmented image counters
  this->ImageDataInfoContainer[FREEHAND_MOTION_1].NumberOfSegmentedImages = 0;
  this->ImageDataInfoContainer[FREEHAND_MOTION_2].NumberOfSegmentedImages = 0;

  // If calibrator is present, delete it so that it can be re-initialized when calibration is started again
	if (this->Calibrator != NULL) {
		delete this->Calibrator;
		this->Calibrator = NULL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::UpdateProgress(int aPercent) {
	LOG_TRACE("vtkFreehandCalibrationController::UpdateProgress(" << aPercent << ")");

	// TODO TEMPORARY CODE UNTIL TRACKED FRAME ACQUISITION GOES INTO THE APPLICATIONS
  LOG_INFO("Calibration progress: " << aPercent << "%");
}

//-----------------------------------------------------------------------------

std::string vtkFreehandCalibrationController::GetResultString()
{
	LOG_TRACE("vtkFreehandCalibrationController::GetResultString");

	std::ostringstream matrixStringStream;
	matrixStringStream << "Image to probe transform:" << std::endl;

	// Print matrix rows
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrixStringStream << std::fixed << std::setprecision(3) << std::setw(6) << std::right << this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j) << " ";
		}

		matrixStringStream << std::endl;
	}

	std::ostringstream errorsStringStream;

	errorsStringStream << "Point-line distance errors" << std::endl << "(mean, rms, std):" << std::endl;
	errorsStringStream << std::fixed << std::setprecision(3) << "  " << GetPointLineDistanceErrorAnalysisVector()[0] * 1000.0 << ", " << GetPointLineDistanceErrorAnalysisVector()[1] * 1000.0 << ", " << GetPointLineDistanceErrorAnalysisVector()[2] * 1000.0 << std::endl;

	std::ostringstream resultStringStream;
	resultStringStream << matrixStringStream.str() << errorsStringStream.str() << std::endl;

	resultStringStream << "Validation data confidence:" << std::endl;
	resultStringStream << "  " << GetPointLineDistanceErrorAnalysisVector()[3];

	return resultStringStream.str();
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoTemporalCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoTemporalCalibration");

	if (GetCalibrationMode() != REALTIME) {
		LOG_ERROR( "Unable to start temporal calibration in offline mode!" );  
		return PLUS_FAIL; 
	}

	this->DataCollector->SetProgressBarUpdateCallbackFunction(UpdateProgress);
  this->DataCollector->Synchronize(vtkPlusConfig::GetInstance()->GetOutputDirectory(), true );

	this->ProgressPercent = 0;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkProbeCalibrationController::AddTrackedFrameData");

	if (Superclass::AddTrackedFrameData(trackedFrame, dataType)) {
		double tProbeToPhantomReference[16];

		if (trackedFrame->GetDefaultFrameTransform(tProbeToPhantomReference)) {
			vtkSmartPointer<vtkMatrix4x4> tProbeToPhantomReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			tProbeToPhantomReferenceMatrix->DeepCopy(tProbeToPhantomReference);
			vnl_matrix<double> transformProbeToPhantomReferenceMatrix4x4(4,4);

			ConvertVtkMatrixToVnlMatrixInMeter(tProbeToPhantomReferenceMatrix, transformProbeToPhantomReferenceMatrix4x4);

			this->PopulateSegmentedFiducialsToDataContainer(transformProbeToPhantomReferenceMatrix4x4, dataType);

			/*
			if ((dataType == FREEHAND_MOTION_1) && (this->GetCalibrator()->areDataPositionsReadyForCalibration())) {
				this->Calibrate(); 
			}
			*/

			return PLUS_SUCCESS;
		}
	}

	return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
void vtkFreehandCalibrationController::Calibrate()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoCalibration"); 

	// Instruct the calibrator to perform the calibration task
	this->GetCalibrator()->calibrate();

	// Instruct the calibrator to validate the calibration accuracy
	this->GetCalibrator()->compute3DPointReconstructionError();
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkFreehandCalibrationController::PopulateSegmentedFiducialsToDataContainer");

	// ========================================================================
	// Populate the segmented N-fiducials to the data container
	// Indices defined in the input std::vector array.
	// This is the order that the segmentation algorithm gives the 
	// segmented positions in each image:
	//
	// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
	// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
	// Each acquired data position is a 4x1 homogenous vector :
	// [ X, Y, 0, 1] all units in pixels
	// ==================================================================

  if (! this->GetPatternRecognition()->GetFidLabeling()->GetDotsFound()) {
		LOG_ERROR("Segmentation failed! Unable to populate segmentation result!"); 
		return PLUS_FAIL; 
	}

	// Top layer:		3, 2, 1 
	// Bottom Layer:	6, 5, 4 
	std::vector<vnl_vector<double>> SegmentedNFiducialsInFixedCorrespondence;
	SegmentedNFiducialsInFixedCorrespondence.resize(0);

	for (int i=0; i<this->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue().size(); i++) {
		vnl_vector<double> NFiducial(4,0);
		NFiducial[0]=this->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue()[i][0];
		NFiducial[1]=this->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue()[i][1];
		NFiducial[2]=0;
		NFiducial[3]=1;

		SegmentedNFiducialsInFixedCorrespondence.push_back(NFiducial);
	}

	// Add the data for calibration or validation
	if (dataType == FREEHAND_MOTION_1) {
		this->GetCalibrator()->addDataPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	} else if (dataType == FREEHAND_MOTION_2) {
		this->GetCalibrator()->addValidationPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoOfflineCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoOfflineCalibration"); 

	try {
		vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
		if ( !this->GetImageDataInfo(FREEHAND_MOTION_2).InputSequenceMetaFileName.empty() ) {
			trackedFrameList->ReadFromSequenceMetafile(this->GetImageDataInfo(FREEHAND_MOTION_2).InputSequenceMetaFileName.c_str()); 
		} else {
			LOG_ERROR("Unable to start OfflineCalibration with validation data: SequenceMetaFileName is empty!"); 
			return PLUS_FAIL; 
		}

		// Validation data
		int validationCounter = 0;
		for (int imgNumber = 0; validationCounter < this->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToAcquire; imgNumber++) {
			if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() ) {
				break; 
			}

			if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), FREEHAND_MOTION_2) ) {
				// The segmentation was successful
				validationCounter++;
			} else {
				LOG_DEBUG("Adding tracked frame " << imgNumber << " (for validation) failed!");
			}

			this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData.GetDisplayableImage());
		}

		LOG_INFO( "A total of " << this->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages << " images have been successfully added for validation.");


		// Calibration data
		vtkSmartPointer<vtkTrackedFrameList> calibrationData = vtkSmartPointer<vtkTrackedFrameList>::New();
		if ( !this->GetImageDataInfo(FREEHAND_MOTION_1).InputSequenceMetaFileName.empty() ) {
			calibrationData->ReadFromSequenceMetafile(this->GetImageDataInfo(FREEHAND_MOTION_1).InputSequenceMetaFileName.c_str()); 
		} else {
			LOG_ERROR("Unable to start OfflineCalibration with calibration data: SequenceMetaFileName is empty!"); 
			return PLUS_FAIL; 
		}

		int calibrationCounter = 0;
		for (int imgNumber = 0; calibrationCounter < this->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToAcquire; imgNumber++) {
			if ( imgNumber >= calibrationData->GetNumberOfTrackedFrames() ) {
				break; 
			}

			if ( this->AddTrackedFrameData(calibrationData->GetTrackedFrame(imgNumber), FREEHAND_MOTION_1) ) {
				// The segmentation was successful
				calibrationCounter++; 
			} else {
				LOG_DEBUG("Adding tracked frame " << imgNumber << " (for calibration) failed!");
			}

			this->SetOfflineImageData(calibrationData->GetTrackedFrame(imgNumber)->ImageData.GetDisplayableImage()); 
		}

		LOG_INFO ("A total of " << this->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " images have been successfully added for calibration.");
	} catch(...) {
		LOG_ERROR("AddAllSavedData: Failed to add saved data!");  
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int* origin) {
	LOG_TRACE("vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels"); 

	this->SetUSImageFrameOriginInPixels(origin[0], origin[1]); 
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int originX, int originY)
{
	LOG_TRACE("vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels: " << originX << "  " << originY);

	this->SetUSImageFrameOriginXInPixels(originX);
	this->SetUSImageFrameOriginYInPixels(originY);

	if (this->GetCalibrator() != NULL) {
		this->GetCalibrator()->setUltrasoundImageFrameOriginInPixels(originX, originY); 
	}
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ReadConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration");

	if (aConfig == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}

  // Have base class read calibration controller configuration
  Superclass::ReadConfiguration(aConfig);

  // Load freehand calibration elements
	this->ReadFreehandCalibrationConfiguration(aConfig);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ReadFreehandCalibrationConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkFreehandCalibrationController::ReadFreehandCalibrationConfiguration"); 

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

	// To enable/disable the system logging
	const char* enableLogFile = probeCalibration->GetAttribute("EnableLogFile"); 
	if (enableLogFile != NULL &&  STRCASECMP( "TRUE", enableLogFile ) == 0) {
		this->EnableSystemLogOn(); 
	} else {
		this->EnableSystemLogOff(); 
	}

	/*
	// To enable/disable the saving of segmented wire positions to file 
	const char* enableSegmentedWirePositionsSaving = probeCalibration->GetAttribute("EnableSegmentedWirePositionsSaving"); 
	if (enableSegmentedWirePositionsSaving != NULL &&  STRCASECMP( "TRUE", enableSegmentedWirePositionsSaving ) == 0) {
		this->EnableSegmentedWirePositionsSavingOn(); 
	} else {
		this->EnableSegmentedWirePositionsSavingOff(); 
	}

	//Transform: from image home position to user defined image home position
	double* imageHomeToUserImageHomeTransform = new double[16]; 
	if ( probeCalibration->GetVectorAttribute("ImageHomeToUserImageHomeTransform", 16, imageHomeToUserImageHomeTransform) )
	{
		this->GetTransformImageHomeToUserImageHome()->SetMatrix(imageHomeToUserImageHomeTransform); 
		this->GetTransformUserImageToImage()->SetMatrix(imageHomeToUserImageHomeTransform); 
		this->GetTransformUserImageToImage()->Inverse(); 
	}
	delete [] imageHomeToUserImageHomeTransform; 

	// Sets the suffix of the data files
	const char* dataFileSuffix = probeCalibration->GetAttribute("DataFileSuffix"); 
	if (dataFileSuffix != NULL) {
		this->SetDataFileSuffix(dataFileSuffix); 
	} else {
		this->SetDataFileSuffix(".data"); 
	}
	*/

	// Sets the suffix of the calibration result file
	const char* calibrationResultFileSuffix = probeCalibration->GetAttribute("CalibrationResultFileSuffix"); 
	if ( calibrationResultFileSuffix != NULL) {
		this->SetCalibrationResultFileSuffix(calibrationResultFileSuffix); 
	} else {
		this->SetCalibrationResultFileSuffix(".Calibration.results"); 
	}

	/*
	// Sets the suffix of the segmentation error log file
	const char* segmentationErrorLogFileNameSuffix = probeCalibration->GetAttribute("SegmentationErrorLogFileNameSuffix"); 
	if ( segmentationErrorLogFileNameSuffix != NULL) {
		this->SetSegmentationErrorLogFileNameSuffix(segmentationErrorLogFileNameSuffix); 
	} else {
		this->SetSegmentationErrorLogFileNameSuffix(".Segmentation.errors"); 
	}

	// Sets the suffix of the segmentation analysis file
	const char* segmentationAnalysisFileNameSuffix = probeCalibration->GetAttribute("SegmentationAnalysisFileNameSuffix"); 
	if ( segmentationAnalysisFileNameSuffix != NULL) {
		this->SetSegmentationAnalysisFileNameSuffix(segmentationAnalysisFileNameSuffix); 
	} else {
		this->SetSegmentationAnalysisFileNameSuffix(".Segmentation.analysis"); 
	}

	// Sets the suffix of the Template2StepperCalibration analysis file
	const char* temp2StepCalibAnalysisFileNameSuffix = probeCalibration->GetAttribute("Temp2StepCalibAnalysisFileNameSuffix"); 
	if ( temp2StepCalibAnalysisFileNameSuffix != NULL) {
		this->SetTemp2StepCalibAnalysisFileNameSuffix(temp2StepCalibAnalysisFileNameSuffix); 
	} else {
		this->SetTemp2StepCalibAnalysisFileNameSuffix(".Template2StepperCalibration.analysis"); 
	}
	*/

	/* TODO
	// US3DBeamwidth specifications
	//********************************************************************
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
	*/
  
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

PlusStatus vtkFreehandCalibrationController::ComputeCalibrationResults()
{
	LOG_TRACE("vtkFreehandCalibrationController::ComputeCalibrationResults"); 

	try {
		// Do final calibration 
		Calibrate();

		// Get the image home to probe home transformation from the calibrator
		vnl_matrix<double> calibResultMatrix = this->GetCalibrator()->getCalibrationResultsInMatrix(); 
		vtkSmartPointer<vtkMatrix4x4> imageToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

		// Convert transform from meter to mm
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 4; j++ ) {
				imageToProbeMatrix->SetElement(i, j, calibResultMatrix.get(i, j) * 1000 ); 
			}
		}
		
		// Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
		double xVector[3] = {imageToProbeMatrix->GetElement(0,0),imageToProbeMatrix->GetElement(1,0),imageToProbeMatrix->GetElement(2,0)};
		double yVector[3] = {imageToProbeMatrix->GetElement(0,1),imageToProbeMatrix->GetElement(1,1),imageToProbeMatrix->GetElement(2,1)};
		double zVector[3] = {0,0,0};

    double dotProduct = vtkMath::Dot(xVector, yVector);
    if (dotProduct > 0.001) {
      LOG_WARNING("Calibration result axes are not orthogonal (dot product of X and Y axes is " << dotProduct << ")");
    }

		vtkMath::Cross(xVector, yVector, zVector);

		// make the z vector have about the same length as x an y,
		// so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
		vtkMath::Normalize(zVector);
		double normZ = (vtkMath::Norm(xVector) + vtkMath::Norm(yVector)) / 2;
		vtkMath::MultiplyScalar(zVector, normZ);
		
		imageToProbeMatrix->SetElement(0, 2, zVector[0]);
		imageToProbeMatrix->SetElement(1, 2, zVector[1]);
		imageToProbeMatrix->SetElement(2, 2, zVector[2]);

		this->TransformImageToProbe->SetMatrix(imageToProbeMatrix);

		// Write transformations to log and output
		std::ostringstream osImageToProbe;
		this->GetTransformImageToProbe()->Print(osImageToProbe);
		LOG_DEBUG("TransformImageToProbe:\n" << osImageToProbe.str().c_str() );

		// Compute the independent point and line reconstruction errors
		LOG_INFO("Compute the independent point and line reconstruction errors");
		this->GetCalibrator()->computeIndependentPointLineReconstructionError();

		// STEP-4. Print the final calibration results and error reports 
		LOG_INFO("Print calibration results and error reports");
		this->PrintCalibrationResultsAndErrorReports();

		// STEP-5. Save the calibration results and error reports into a file 
		LOG_INFO("Save the calibration results and error reports to file");
		this->SaveCalibrationResultsAndErrorReportsToXML();

		/* TODO
		// STEP-6. Save the segmented wire positions into a file 
		if ( this->EnableSegmentedWirePositionsSaving ) {
			LOG_INFO("Save the segmented wire positions to file"); 
			this->CalibrationControllerIO->SaveSegmentedWirePositionsToFile(); 
		}

		if ( this->GetEnableVisualization() ) {
			LOG_INFO("Save PRE3D plot to image"); 
			// save the PRE3D distribution plot to an image file
			this->GetVisualizationComponent()->SavePRE3DplotToImage();
		}
		*/

    if (WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS) {
		  LOG_ERROR("Freehand calibration result could not be saved into session configuration data!");
		  return PLUS_FAIL;
    }

	} catch(...) {
		LOG_ERROR("ComputeCalibrationResults: Failed to compute calibration results!");
		return PLUS_FAIL;
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::WriteConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkFreehandCalibrationController::WriteConfiguration");

  // Save temporal calibration
  double videoTimeOffset = 0.0;
	if ((this->DataCollector != NULL) && (this->DataCollector->GetVideoSource() != NULL) && (this->DataCollector->GetVideoSource()->GetBuffer() != NULL)) {
    videoTimeOffset = this->DataCollector->GetVideoSource()->GetBuffer()->GetLocalTimeOffset();
  }

  vtkSmartPointer<vtkXMLDataElement> imageAcquisition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "USDataCollection", "ImageAcquisition", NULL, NULL);
  imageAcquisition->SetDoubleAttribute("LocalTimeOffset", videoTimeOffset);

	// Save spatial calibration result
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_PROBE, toolType);

  //Find stylus definition element
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
	char imageToProbeTransformChars[256];
  vtkSmartPointer<vtkMatrix4x4> transformMatrix = this->TransformImageToProbe->GetMatrix();
	sprintf_s(imageToProbeTransformChars, 256, "\n\t%.4lf %.4lf %.4lf %.4lf\n\t%.4lf %.4lf %.4lf %.4lf\n\t%.4lf %.4lf %.4lf %.1lf\n\t0 0 0 1", 
		transformMatrix->GetElement(0,0), transformMatrix->GetElement(0,1), transformMatrix->GetElement(0,2), transformMatrix->GetElement(0,3), 
		transformMatrix->GetElement(1,0), transformMatrix->GetElement(1,1), transformMatrix->GetElement(1,2), transformMatrix->GetElement(1,3), 
		transformMatrix->GetElement(2,0), transformMatrix->GetElement(2,1), transformMatrix->GetElement(2,2), transformMatrix->GetElement(2,3));

	calibration->SetAttribute("MatrixValue", imageToProbeTransformChars);

	// Save matrix name, date and error
	calibration->SetAttribute("MatrixName", "ImageToProbe");
	calibration->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());
	calibration->SetDoubleAttribute("Error", GetPointLineDistanceErrorAnalysisVector()[0] * 1000.0); // TODO find the best error number

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::PrintCalibrationResultsAndErrorReports()
{
  LOG_TRACE("vtkProbeCalibrationController::PrintCalibrationResultsAndErrorReports");

	try {
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Calibration result in 4x4 homogeneous transform matrix = ");

		for (int i = 0; i < 4; i++) {
			std::ostringstream matrixRow; 

			for (int j = 0; j < 4; j++) {
				matrixRow << this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j) << "  " ;
			}
			LOG_INFO(matrixRow.str()); 
		}

    LOG_INFO("Transform parameters: " << PlusMath::GetTransformParametersString(this->TransformImageToProbe));

		// Point-Line Distance Error Analysis for Validation Positions in US probe frame
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Point-Line Distance Error (PLDE) Analysis in mm =");
		LOG_INFO("[ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]");
		LOG_INFO("[ vector 3    :  Validation data confidence level ]");
		LOG_INFO(GetPointLineDistanceErrorAnalysisVector()[0] * 1000 << ", " << GetPointLineDistanceErrorAnalysisVector()[1] * 1000 << ", " << GetPointLineDistanceErrorAnalysisVector()[2] * 1000); 
		LOG_INFO(GetPointLineDistanceErrorAnalysisVector()[3]);

	} catch(...) {
		LOG_ERROR("PrintCalibrationResultsAndErrorReports: Failed to retrieve the calibration results!"); 
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::SaveCalibrationResultsAndErrorReportsToXML()
{
	LOG_TRACE("vtkFreehandCalibrationController::SaveCalibrationResultsAndErrorReportsToXML"); 

	// Construct the calibration result file name with path and timestamp
	const std::string calibrationResultFileName = this->GetCalibrator()->getCalibrationTimeStampInString() + this->GetCalibrationResultFileSuffix() + ".xml";
	const std::string calibrationResultFileNameWithPath = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/") + calibrationResultFileName;
	this->SetCalibrationResultFileNameWithPath(calibrationResultFileNameWithPath.c_str()); 

	// <USTemplateCalibrationResult>
	vtkSmartPointer<vtkXMLDataElement> xmlCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	xmlCalibrationResults->SetName("USTemplateCalibrationResult"); 
	xmlCalibrationResults->SetAttribute("version", "1.0"); 

	// <CalibrationFile> 
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationFile = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationFile->SetName("CalibrationFile"); 
	tagCalibrationFile->SetAttribute("Timestamp", this->GetCalibrator()->getCalibrationTimeStampWithFormat().c_str()); 
	tagCalibrationFile->SetAttribute("FileName", calibrationResultFileName.c_str()); 
	vtkstd::string commentCalibrationFile("# Timestamp format: MM/DD/YY HH:MM:SS"); 
	tagCalibrationFile->AddCharacterData(commentCalibrationFile.c_str(), commentCalibrationFile.size()); 
	// </CalibrationFile> 

	// <CalibrationResults>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationResults->SetName("CalibrationResults"); 

	// <UltrasoundImageDimensions>
	vtkSmartPointer<vtkXMLDataElement> tagUltrasoundImageDimensions = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagUltrasoundImageDimensions->SetName("UltrasoundImageDimensions"); 
  tagUltrasoundImageDimensions->SetIntAttribute("Width", this->GetPatternRecognition()->GetFidSegmentation()->GetFrameSize()[0]); 
	tagUltrasoundImageDimensions->SetIntAttribute("Height", this->GetPatternRecognition()->GetFidSegmentation()->GetFrameSize()[1]); 
	vtkstd::string commentUltrasoundImageDimensions("# UltrasoundImageDimensions format: image width and height in pixels."); 
	tagUltrasoundImageDimensions->AddCharacterData(commentUltrasoundImageDimensions.c_str(), commentUltrasoundImageDimensions.size()); 
	// </UltrasoundImageDimensions>

	// <UltrasoundImageOrigin>
	// # FORMAT: X to the right; Y to the bottom; w.r.t the left-upper corner in original image.
	vtkSmartPointer<vtkXMLDataElement> tagUltrasoundImageOrigin = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagUltrasoundImageOrigin->SetName("UltrasoundImageOrigin"); 
	tagUltrasoundImageOrigin->SetIntAttribute("OriginX", this->GetUSImageFrameOriginXInPixels()); 
	tagUltrasoundImageOrigin->SetIntAttribute("OriginY", this->GetUSImageFrameOriginYInPixels()); 
	vtkstd::string commentUltrasoundImageOrigin("# UltrasoundImageOrigin format: X to the right; Y to the bottom; w.r.t the left-upper corner in original image."); 
	tagUltrasoundImageOrigin->AddCharacterData(commentUltrasoundImageOrigin.c_str(), commentUltrasoundImageOrigin.size()); 
	// </UltrasoundImageOrigin>


	double *imageToProbeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			imageToProbeMatrix[i*4+j] = this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j); 
		}
	}

	// <CalibrationTransform>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationTransform = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationTransform->SetName("CalibrationTransform"); 
	tagCalibrationTransform->SetVectorAttribute("TransformImageToProbe", 16, imageToProbeMatrix); 
	// </CalibrationTransform>

	delete[] imageToProbeMatrix; 

	tagCalibrationResults->AddNestedElement(tagUltrasoundImageDimensions); 
	tagCalibrationResults->AddNestedElement(tagUltrasoundImageOrigin); 
	tagCalibrationResults->AddNestedElement(tagCalibrationTransform); 
	// </CalibrationResults>

	// <ErrorReports>
	vtkSmartPointer<vtkXMLDataElement> tagErrorReports = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagErrorReports->SetName("ErrorReports"); 


	double *preAnalysis = new double[9];
	for (int i = 0; i < 9; i++) {
		preAnalysis[i] = this->GetPRE3DVector()[i]; 
	}

	// <PointReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrorAnalysis->SetName("PointReconstructionErrorAnalysis"); 
	tagPointReconstructionErrorAnalysis->SetVectorAttribute("PRE", 9, preAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetPRE3DVector()[9]);  
	vtkstd::string commentPointReconstructionErrorAnalysis("# PRE format: PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std"); 
	tagPointReconstructionErrorAnalysis->AddCharacterData(commentPointReconstructionErrorAnalysis.c_str(), commentPointReconstructionErrorAnalysis.size()); 
	// </PointReconstructionErrorAnalysis>
	delete[] preAnalysis; 


	double *rawPointReconstructionErrors = new double[this->GetPRE3DMatrix().size()]; 
	for (int row = 0; row < this->GetPRE3DMatrix().rows(); row++) {
		for (int column = 0; column < this->GetPRE3DMatrix().cols(); column++) {
			rawPointReconstructionErrors[row*this->GetPRE3DMatrix().cols() + column ] = this->GetPRE3DMatrix().get(row, column); 
		}
	}

	// <PointReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrors->SetName("PointReconstructionErrors"); 
	tagPointReconstructionErrors->SetVectorAttribute("Raw", this->GetPRE3DMatrix().size(), rawPointReconstructionErrors); 
	vtkstd::ostringstream commentPointReconstructionErrors; 
	commentPointReconstructionErrors << "# PointReconstructionErrors format: 4xN matrix, N = "; 
	commentPointReconstructionErrors << this->GetPRE3DMatrix().columns(); 
	commentPointReconstructionErrors << ": the number of validation points"; 
	tagPointReconstructionErrors->AddCharacterData(commentPointReconstructionErrors.str().c_str(), commentPointReconstructionErrors.str().size()); 
	// </PointReconstructionErrors>
	delete[] rawPointReconstructionErrors; 


	double *pldeAnalysis = new double[3]; 
	for (int i = 0; i < 3; i++) {
		pldeAnalysis[i] = this->GetPointLineDistanceErrorAnalysisVector()[i]; 
	}

	// <PointLineDistanceErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrorAnalysis->SetName("PointLineDistanceErrorAnalysis"); 
	tagPointLineDistanceErrorAnalysis->SetVectorAttribute("PLDE", 3, pldeAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointLineDistanceErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetPointLineDistanceErrorAnalysisVector()[3]);  
	vtkstd::string commentPointLineDistanceErrorAnalysis("# PLDE format: PLDE_mean, PLDE_rms, PLDE_std"); 
	tagPointLineDistanceErrorAnalysis->AddCharacterData(commentPointLineDistanceErrorAnalysis.c_str(), commentPointLineDistanceErrorAnalysis.size()); 
	// </PointLineDistanceErrorAnalysis>
	delete[] pldeAnalysis; 


	double *rawPointLineDistanceErrors = new double[this->GetPointLineDistanceErrorVector().size()]; 
	for (int i = 0; i < this->GetPointLineDistanceErrorVector().size(); i++) {
		rawPointLineDistanceErrors[i] = this->GetPointLineDistanceErrorVector().get(i); 
	}

	double *sortedPointLineDistanceErrors = new double[this->GetPointLineDistanceErrorSortedVector().size()]; 
	for (int i = 0; i < this->GetPointLineDistanceErrorSortedVector().size(); i++) {
		sortedPointLineDistanceErrors[i] = this->GetPointLineDistanceErrorSortedVector().get(i); 
	}

	// <PointLineDistanceErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrors->SetName("PointLineDistanceErrors"); 
	tagPointLineDistanceErrors->SetVectorAttribute("Raw", this->GetPointLineDistanceErrorVector().size(), rawPointLineDistanceErrors); 
	//tagPointLineDistanceErrors->SetVectorAttribute("Sorted", this->GetPointLineDistanceErrorSortedVector().size(), sortedPointLineDistanceErrors); 
	vtkstd::ostringstream commentPointLineDistanceErrors; 
	commentPointLineDistanceErrors << "# PointLineDistanceErrors format: 1xN vector, N = "; 
	commentPointLineDistanceErrors << this->GetPointLineDistanceErrorVector().size(); 
	commentPointLineDistanceErrors << ": the number of validation points"; 
	tagPointLineDistanceErrors->AddCharacterData(commentPointLineDistanceErrors.str().c_str(), commentPointLineDistanceErrors.str().size()); 
	// </PointLineDistanceErrors>
	delete[] rawPointLineDistanceErrors; 
	delete[] sortedPointLineDistanceErrors; 


	double *w1LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w1LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(1)[i]; 
	}

	// <Wire1LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire1LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire1LineReconstructionErrorAnalysis->SetName("Wire1LineReconstructionErrorAnalysis"); 
	tagWire1LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w1LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire1LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(1)[6]);  
	vtkstd::string commentWire1LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire1LineReconstructionErrorAnalysis->AddCharacterData(commentWire1LineReconstructionErrorAnalysis.c_str(), commentWire1LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w1LREAnalysis; 


	const int w1NumOfRows = this->GetLineReconstructionErrorMatrix(1).rows(); 
	const int w1NumOfCols = this->GetLineReconstructionErrorMatrix(1).cols(); 
	double *w1rawXLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawYLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawEUCLineReconstructionErrors = new double[w1NumOfCols]; 

	for (int column = 0; column < w1NumOfCols; column++) {
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
	// </Wire1LineReconstructionErrors>
	delete[] w1rawXLineReconstructionErrors; 
	delete[] w1rawYLineReconstructionErrors; 
	delete[] w1rawEUCLineReconstructionErrors;


	double *w3LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w3LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(3)[i]; 
	}

	// <Wire3LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire3LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire3LineReconstructionErrorAnalysis->SetName("Wire3LineReconstructionErrorAnalysis"); 
	tagWire3LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w3LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire3LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(3)[6]);  
	vtkstd::string commentWire3LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire3LineReconstructionErrorAnalysis->AddCharacterData(commentWire3LineReconstructionErrorAnalysis.c_str(), commentWire3LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w3LREAnalysis;


	const int w3NumOfRows = this->GetLineReconstructionErrorMatrix(3).rows(); 
	const int w3NumOfCols = this->GetLineReconstructionErrorMatrix(3).cols(); 
	double *w3rawXLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawYLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawEUCLineReconstructionErrors = new double[w3NumOfCols];

	for (int column = 0; column < w3NumOfCols; column++) {
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
	// </Wire3LineReconstructionErrors>
	delete[] w3rawXLineReconstructionErrors; 
	delete[] w3rawYLineReconstructionErrors; 
	delete[] w3rawEUCLineReconstructionErrors;


	double *w4LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w4LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(4)[i]; 
	}

	// <Wire4LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire4LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire4LineReconstructionErrorAnalysis->SetName("Wire4LineReconstructionErrorAnalysis"); 
	tagWire4LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w4LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire4LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(4)[6]);  
	vtkstd::string commentWire4LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire4LineReconstructionErrorAnalysis->AddCharacterData(commentWire4LineReconstructionErrorAnalysis.c_str(), commentWire4LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w4LREAnalysis;


	const int w4NumOfRows = this->GetLineReconstructionErrorMatrix(4).rows(); 
	const int w4NumOfCols = this->GetLineReconstructionErrorMatrix(4).cols(); 
	double *w4rawXLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawYLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawEUCLineReconstructionErrors = new double[w4NumOfCols];

	for (int column = 0; column < w4NumOfCols; column++) {
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
	// </Wire4LineReconstructionErrors>
	delete[] w4rawXLineReconstructionErrors; 
	delete[] w4rawYLineReconstructionErrors; 
	delete[] w4rawEUCLineReconstructionErrors;


	double *w6LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w6LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(6)[i]; 
	}

	// <Wire6LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire6LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire6LineReconstructionErrorAnalysis->SetName("Wire6LineReconstructionErrorAnalysis"); 
	tagWire6LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w6LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire6LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(6)[6]);  
	vtkstd::string commentWire6LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire6LineReconstructionErrorAnalysis->AddCharacterData(commentWire6LineReconstructionErrorAnalysis.c_str(), commentWire6LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w6LREAnalysis;


	const int w6NumOfRows = this->GetLineReconstructionErrorMatrix(6).rows(); 
	const int w6NumOfCols = this->GetLineReconstructionErrorMatrix(6).cols(); 
	double *w6rawXLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawYLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawEUCLineReconstructionErrors = new double[w6NumOfCols];

	for (int column = 0; column < w6NumOfCols; column++) {
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
	// </Wire6LineReconstructionErrors>
	delete[] w6rawXLineReconstructionErrors; 
	delete[] w6rawYLineReconstructionErrors;  
	delete[] w6rawEUCLineReconstructionErrors;

	tagErrorReports->AddNestedElement(tagPointReconstructionErrorAnalysis); 
	tagErrorReports->AddNestedElement(tagPointReconstructionErrors);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrorAnalysis);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrors);
	tagErrorReports->AddNestedElement(tagWire1LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW1LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire3LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW3LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire4LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW4LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire6LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW6LineReconstructionErrors);
	// <ErrorReports>

	xmlCalibrationResults->AddNestedElement(tagCalibrationFile); 
	xmlCalibrationResults->AddNestedElement(tagCalibrationResults); 
	xmlCalibrationResults->AddNestedElement(tagErrorReports); 
	// </USTemplateCalibrationResult>

	xmlCalibrationResults->PrintXML(this->GetCalibrationResultFileNameWithPath()); 
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SaveCalibrationDataToSequenceMetafile()
{
	LOG_TRACE("vtkFreehandCalibrationController::SaveCalibrationDataToSequenceMetafile"); 

	// Save calibration dataset 
	std::ostringstream calibrationDataFileName; 
	calibrationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetImageDataInfo(FREEHAND_MOTION_1).OutputSequenceMetaFileSuffix; 
	if ( this->SaveTrackedFrameListToMetafile( FREEHAND_MOTION_1, vtkPlusConfig::GetInstance()->GetOutputDirectory(), calibrationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
		return;
	}

	LOG_INFO("Save validation data to sequence metafile"); 
	// TODO add validation file name to config file
	// Save validation dataset
	std::ostringstream validationDataFileName; 
	validationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetImageDataInfo(FREEHAND_MOTION_2).OutputSequenceMetaFileSuffix; 
	if ( this->SaveTrackedFrameListToMetafile( FREEHAND_MOTION_2, vtkPlusConfig::GetInstance()->GetOutputDirectory(), validationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
		return; 
	}
}

//-----------------------------------------------------------------------------

std::vector<double> vtkFreehandCalibrationController::GetLineReconstructionErrorAnalysisVector(int wireNumber)
{
	LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorAnalysisVector (wire #" << wireNumber << ")");

	std::vector<double> absLREAnalysisInUSProbeFrame; 
	switch (wireNumber)
	{
	case 1: { // wire #1
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire1();
		}
		break; 
	case 3: { // wire #3
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire3();
		}
		break; 
	case 4: { // wire #4
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire4();
		}
		break; 
	case 6: { // wire #6
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire6();
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE analysis vector for wire #" << wireNumber ); 
	}

	return absLREAnalysisInUSProbeFrame; 
}

//-----------------------------------------------------------------------------

vnl_matrix<double> vtkFreehandCalibrationController::GetLineReconstructionErrorMatrix(int wireNumber)
{
	//LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorMatrix (wire #" << wireNumber << ")"); 

	vnl_matrix<double> mLREOrigInUSProbeFrameMatrix; 
	switch (wireNumber)
	{
	case 1: { // wire #1
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire1();
		}
		break; 
	case 3: { // wire #3
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire3();
		}
		break; 
	case 4: { // wire #4
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire4();
		}
		break; 
	case 6: { // wire #6
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire6();
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE matrix for wire #" << wireNumber ); 
	}

	return mLREOrigInUSProbeFrameMatrix; 
}

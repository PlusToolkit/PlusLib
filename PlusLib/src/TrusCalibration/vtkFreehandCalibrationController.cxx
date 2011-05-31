#include "vtkFreehandCalibrationController.h"

#include "vtkFreehandController.h"
#include "PhantomRegistrationController.h"
#include "PlusConfigure.h"

#include "vtkObjectFactory.h"
#include "vtkAccurateTimer.h"
#include "vtkMath.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFreehandCalibrationController, "$Revision: 1.0 $");

vtkFreehandCalibrationController *vtkFreehandCalibrationController::Instance = NULL;

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController* vtkFreehandCalibrationController::New()
{
	return vtkFreehandCalibrationController::GetInstance();
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController* vtkFreehandCalibrationController::GetInstance() {
	if(!vtkFreehandCalibrationController::Instance) {
		// Try the factory first
		vtkFreehandCalibrationController::Instance = (vtkFreehandCalibrationController*)vtkObjectFactory::CreateInstance("vtkFreehandCalibrationController");    

		if(!vtkFreehandCalibrationController::Instance) {
			vtkFreehandCalibrationController::Instance = new vtkFreehandCalibrationController();	   
		}
	}
	// return the instance
	return vtkFreehandCalibrationController::Instance;
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::vtkFreehandCalibrationController()
	:AbstractToolboxController()
{
	this->TemporalCalibrationDone = false;
	this->SpatialCalibrationDone = false;
	this->ProgressPercent = 0;

	this->USImageFrameOriginXInPixels = 0; 
	this->USImageFrameOriginYInPixels = 0; 

	// Initializing vtkCalibrationController members
	this->InitializedOff(); 
	this->EnableSystemLogOff();
	this->SetCalibrationMode(REALTIME); 
	this->CanvasImageActor = NULL;
	this->CalibrationResultFileNameWithPath = NULL;
	this->CalibrationResultFileSuffix = NULL;

	this->Calibrator = NULL;

	vtkSmartPointer<vtkTransform> transformProbeToPhantomReference = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformProbeToPhantomReference = NULL;
	this->SetTransformProbeToPhantomReference(transformProbeToPhantomReference); 

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::~vtkFreehandCalibrationController()
{
	this->SetCanvasImageActor(NULL);
	this->SetTransformProbeToPhantomReference(NULL);

	if (this->Calibrator != NULL) {
		delete this->Calibrator;
		this->Calibrator = NULL;
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Initialize()
{
	LOG_DEBUG("Initialize vtkFreehandCalibrationController");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return;
	}

	if (m_Toolbox) {
		m_Toolbox->Initialize();
	}

	// Initialize visualization
	if (controller->GetCanvas() != NULL) {
		if (m_State == ToolboxState_Uninitialized) {
			vtkSmartPointer<vtkImageActor> canvasImageActor = vtkSmartPointer<vtkImageActor>::New();

			if (dataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE) {
				canvasImageActor->VisibilityOn();
				canvasImageActor->SetInput(dataCollector->GetOutput());
			} else {
				LOG_WARNING("Data collector has no output port, canvas image actor initalization failed.");
			}

			this->SetCanvasImageActor(canvasImageActor);
		}

		// Add image actor to the realtime renderer, and add renderer to Canvas
		// If already initialized (it can occur if tab change - and so clear - happened)
		controller->GetCanvasRenderer()->AddActor(this->CanvasImageActor);
		controller->GetCanvasRenderer()->Modified();
	}

	// Initialize vtkCalibrationController
	if ( this->GetSegParameters() == NULL ) {
		this->SegParameters = new SegmentationParameters(); 
	}

	// Initialize the segmenation component
	this->mptrAutomatedSegmentation = new KPhantomSeg( 
		this->GetImageWidthInPixels(), this->GetImageHeightInPixels(), 
		this->GetSearchStartAtX(), this->GetSearchStartAtY(), 
		this->GetSearchDimensionX(), this->GetSearchDimensionY(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

	// Initialize the calibration component
	if ( this->Calibrator == NULL ) {
		this->Calibrator = new BrachyTRUSCalibrator( this->GetEnableSystemLog() );
	}

	// Set the ultrasound image frame in pixels
	// This defines the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row).
	this->SetUSImageFrameOriginInPixels( this->GetUSImageFrameOriginXInPixels(), this->GetUSImageFrameOriginYInPixels() ); 

	// STEP-OPTIONAL. Apply the US 3D beamwidth data to calibration if desired
	// This will pass the US 3D beamwidth data and their predefined weights to the calibration component.
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

	this->DesiredOrientation = "MN";

	// TEMPORARY CODE ////////////
	vtkSmartPointer<vtkMatrix4x4> identity = vtkSmartPointer<vtkMatrix4x4>::New();
	identity->Identity();
	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	ConvertVtkMatrixToVnlMatrix(identity, transformOrigImageFrame2TRUSImageFrameMatrix4x4); 
	this->GetCalibrator()->setTransformOrigImageToTRUSImageFrame4x4(transformOrigImageFrame2TRUSImageFrameMatrix4x4);
	// TEMPORARY CODE ////////////

	// Set state to idle (initialized)
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;

		this->InitializedOn();
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Clear()
{
	LOG_DEBUG("Clear vtkFreehandCalibrationController");

	// Remove actor
	vtkFreehandController::GetInstance()->GetCanvasRenderer()->AddActor(this->CanvasImageActor);

	m_Toolbox->Clear();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::DoAcquisition()
{
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Start()
{
	//TODO
	m_State = ToolboxState_InProgress;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Stop()
{
	//TODO
	m_State = ToolboxState_Done;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::UpdateProgress(int aPercent) {
	vtkFreehandCalibrationController::GetInstance()->SetProgressPercent(aPercent);
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::StartTemporalCalibration()
{
	if (GetCalibrationMode() != REALTIME) {
		LOG_ERROR( "Unable to start temporal calibration in offline mode!" );  
		return; 
	}

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}

	controller->GetDataCollector()->SetProgressBarUpdateCallbackFunction(UpdateProgress);
	controller->GetDataCollector()->Synchronize();

	this->TemporalCalibrationDoneOn();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::StartSpatialCalibration()
{
	if (GetCalibrationMode() != REALTIME) {
		LOG_ERROR( "Unable to start spatial calibration in offline mode!" );  
		return; 
	}

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return;
	}
	if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetTool(dataCollector->GetDefaultToolPortNumber()) < 0)) {
		LOG_ERROR("Tracker is not initialized properly!");
		return;
	}

	// this->CancelRequestOff(); TODO
	int numberOfImagesToUse = 0;

	// Validation images
	numberOfImagesToUse = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToAcquire;
	while ( this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages < numberOfImagesToUse ) {
		if ( false/*this->CancelRequest TODO*/ ) {
			// we should cancel the job...
			return;
		}

		// Acuired tracked frame
		TrackedFrame trackedFrame;
		dataCollector->GetTrackedFrame(&trackedFrame);

		AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_2); 
	}

	// Calibration images
	numberOfImagesToUse = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToAcquire;
	while ( this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages < numberOfImagesToUse ) {
		if ( false/*this->CancelRequest TODO*/ ) {
			// we should cancel the job...
			return;
		}

		// Acuired tracked frame
		TrackedFrame trackedFrame;
		dataCollector->GetTrackedFrame(&trackedFrame);

		AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_1); 
	}

	// Calibrate
	DoCalibration();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int* origin) {
	this->SetUSImageFrameOriginInPixels(origin[0], origin[1]); 
}

//-----------------------------------------------------------------------------

vtkCalibrationController::RealtimeImageDataInfo vtkFreehandCalibrationController::GetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType) {
	return this->RealtimeImageDataInfoContainer[dataType];
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType, RealtimeImageDataInfo realtimeImageDataInfo) {
	this->RealtimeImageDataInfoContainer[dataType] = realtimeImageDataInfo;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int originX, int originY)
{
	LOG_TRACE("vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels: " << originX << "  " << originY); //TODO Copy these traces to the other functions as well

	this->SetUSImageFrameOriginXInPixels(originX);
	this->SetUSImageFrameOriginYInPixels(originY);

	if (this->GetCalibrator() != NULL) {
		this->GetCalibrator()->setUltrasoundImageFrameOriginInPixels(originX, originY); 
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::ReadConfiguration(const char* configFileNameWithPath)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration - " << configFileNameWithPath);

	this->SetConfigurationFileName(configFileNameWithPath);
	
	vtkXMLDataElement *calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName());
	ReadConfiguration(calibrationController);
	calibrationController->Delete();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::ReadConfiguration(vtkXMLDataElement* configData)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration");

	// TEMPORARY CODE //////////// TODO
	{ // Freehand motion calibration data REALTIME
	vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1); 
	int numberOfImagesToUse = 300;
	const char* sequenceMetaFile = ".FreehandMotionData1";
	imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
	imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse;
	this->SetRealtimeImageDataInfo(FREEHAND_MOTION_1, imageDataInfo);
	}

	{ // Freehand motion validation data REALTIME
	vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2); 
	int numberOfImagesToUse = 200;
	const char* sequenceMetaFile = ".FreehandMotionData2";
	imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
	imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse;
	this->SetRealtimeImageDataInfo(FREEHAND_MOTION_2, imageDataInfo);
	}
	//*************************************************************
	{ // Freehand motion calibration data OFFLINE
	vtkCalibrationController::SavedImageDataInfo imageDataInfo = this->GetSavedImageDataInfo(FREEHAND_MOTION_1); 
	int numberOfImagesToUse = 300;
	//const char* sequenceMetaFile = ".FreehandMotionData1";
	//imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
	imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse;
	this->SetSavedImageDataInfo(FREEHAND_MOTION_1, imageDataInfo);
	}

	{ // Freehand motion validation data OFFLINE
	vtkCalibrationController::SavedImageDataInfo imageDataInfo = this->GetSavedImageDataInfo(FREEHAND_MOTION_2); 
	int numberOfImagesToUse = 200;
	imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse;
	this->SetSavedImageDataInfo(FREEHAND_MOTION_2, imageDataInfo);
	}
	//*************************************************************

	this->SetCalibrationResultFileSuffix(".Calibration.results");
	vtkFreehandController::GetInstance()->SetOutputFolder("./Output");
	this->EnableSystemLogOn();

	// TEMPORARY CODE ////////////


	if ( configData == NULL ) {
		LOG_ERROR("Unable to read the main configration file");
		exit(EXIT_FAILURE);
	}

	// Calibration controller specifications
	vtkSmartPointer<vtkXMLDataElement> calibrationController = configData->FindNestedElementWithName("CalibrationController"); 
	this->ReadCalibrationControllerConfiguration(calibrationController);

	/* TODO
	// ProbeCalibration specifications
	vtkSmartPointer<vtkXMLDataElement> probeCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration");
	this->CalibrationControllerIO->ReadProbeCalibrationConfiguration(probeCalibration);
	*/
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::RegisterPhantomGeometry()
{
	LOG_TRACE("vtkFreehandCalibrationController::RegisterPhantomGeometry"); 

	// Load phantom geometry in calibrator
	this->Calibrator->loadGeometry(this->SegParameters);

	// Register the phantom geometry to the DRB frame in the "Emulator" mode.
	vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
	this->ConvertVtkMatrixToVnlMatrixInMeter(PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform()->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode);

	this->Calibrator->registerPhantomGeometryInEmulatorMode(transformMatrixPhantom2DRB4x4InEmulatorMode);
}

//-----------------------------------------------------------------------------

bool vtkFreehandCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkProbeCalibrationController::AddTrackedFrameData");

	if (Superclass::AddTrackedFrameData(trackedFrame, dataType)) {
		double tProbeToPhantomReference[16];

		if (trackedFrame->GetDefaultFrameTransform(tProbeToPhantomReference)) {
			// TODO Is it sure that the transform is not its name inverted?
			vtkSmartPointer<vtkMatrix4x4> tProbeToPhantomReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			tProbeToPhantomReferenceMatrix->DeepCopy(tProbeToPhantomReference);
			vnl_matrix<double> transformProbeToPhantomReferenceMatrix4x4(4,4);

			ConvertVtkMatrixToVnlMatrixInMeter(tProbeToPhantomReferenceMatrix, transformProbeToPhantomReferenceMatrix4x4);

			this->GetTransformProbeToPhantomReference()->SetMatrix(tProbeToPhantomReferenceMatrix); 

			this->PopulateSegmentedFiducialsToDataContainer(transformProbeToPhantomReferenceMatrix4x4, dataType); 

			return true; 
		}
	}

	return false; 
}

//----------------------------------------------------------------------------
void vtkFreehandCalibrationController::DoCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoCalibration"); 

	// Instruct the calibrator to perform the calibration task
	this->GetCalibrator()->calibrate();

	// Instruct the calibrator to validate the calibration accuracy
	this->GetCalibrator()->compute3DPointReconstructionError();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType)
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

	SegmentationResults segResults;
	this->GetSegmenter()->GetSegmentationResults(segResults); 

	if (! segResults.m_DotsFound) {
		LOG_DEBUG("Segmantation failed! Unable to populate segmentation result!"); 
		return; 
	}

	// Top layer:		3, 2, 1 
	// Bottom Layer:	6, 5, 4 
	std::vector<vnl_vector<double>> SegmentedNFiducialsInFixedCorrespondence;
	SegmentedNFiducialsInFixedCorrespondence.resize(0);

	for (int i=0; i<segResults.m_FoundDotsCoordinateValue.size(); i++) {
		vnl_vector<double> NFiducial(4,0);
		NFiducial[0]=segResults.m_FoundDotsCoordinateValue[i][0];
		NFiducial[1]=segResults.m_FoundDotsCoordinateValue[i][1];
		NFiducial[2]=0;
		NFiducial[3]=1;

		SegmentedNFiducialsInFixedCorrespondence.push_back(NFiducial);

		//std::cout << i << ": " << NFiducial[0] << "\t" << NFiducial[1] << "\t"; //TEST
	}

	//std::cout << std::endl; //TEST

	// Add the data for calibration
	if (dataType == FREEHAND_MOTION_1) {
		this->GetCalibrator()->addDataPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	} else if (dataType == FREEHAND_MOTION_2) {
		this->GetCalibrator()->addValidationPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::DoOfflineCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::OfflineUSToTemplateCalibration"); 

	try {
		if (m_State == ToolboxState_Uninitialized) {
			Initialize();
		}

		vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
		if ( !this->GetSavedImageDataInfo(FREEHAND_MOTION_2).SequenceMetaFileName.empty() ) {
			trackedFrameList->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(FREEHAND_MOTION_2).SequenceMetaFileName.c_str()); 
		} else {
			LOG_ERROR("Unable to start OfflineCalibration with validation data: SequenceMetaFileName is empty!"); 
			return; 
		}

		// Validation data
		int validationCounter = 0;
		for (int imgNumber = this->GetSavedImageDataInfo(FREEHAND_MOTION_2).StartingIndex; validationCounter < this->GetSavedImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToUse; imgNumber++) {
			if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() ) {
				break; 
			}

			if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), FREEHAND_MOTION_2) ) {
				// The segmentation was successful
				validationCounter++;
			} else {
				LOG_DEBUG("Adding tracked frame failed!");
			}

			this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData);
		}

		LOG_INFO( "A total of " << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages << " images have been successfully added for validation.");


		// Calibration data
		vtkSmartPointer<vtkTrackedFrameList> calibrationData = vtkSmartPointer<vtkTrackedFrameList>::New();
		if ( !this->GetSavedImageDataInfo(FREEHAND_MOTION_1).SequenceMetaFileName.empty() ) {
			calibrationData->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(FREEHAND_MOTION_1).SequenceMetaFileName.c_str()); 
		} else {
			LOG_ERROR("Unable to start OfflineCalibration with calibration data: SequenceMetaFileName is empty!"); 
			return; 
		}

		int calibrationCounter = 0;
		for (int imgNumber = this->GetSavedImageDataInfo(FREEHAND_MOTION_1).StartingIndex; calibrationCounter < this->GetSavedImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToUse; imgNumber++) {
			if ( imgNumber >= calibrationData->GetNumberOfTrackedFrames() ) {
				break; 
			}

			if ( this->AddTrackedFrameData(calibrationData->GetTrackedFrame(imgNumber), FREEHAND_MOTION_1) ) {
				// The segmentation was successful
				calibrationCounter++; 
			} else {
				LOG_DEBUG("Adding tracked frame failed!");
			}

			this->AddFrameToRenderer(calibrationData->GetTrackedFrame(imgNumber)->ImageData); 
		}

		LOG_INFO ("A total of " << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " images have been successfully added for calibration.");
	} catch(...) {
		LOG_ERROR("AddAllSavedData: Failed to add saved data!");  
		throw;
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::ComputeCalibrationResults()
{
	LOG_TRACE("vtkFreehandCalibrationController::ComputeCalibrationResults"); 

	try {
		if (m_State == ToolboxState_Uninitialized) {
			Initialize();
		}

		// Do final calibration 
		DoCalibration(); 

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

		vtkMath::Cross(xVector, yVector, zVector);

		// make the z vector have about the same length as x an y,
		// so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
		vtkMath::Normalize(zVector);
		double normZ = (vtkMath::Norm(xVector) + vtkMath::Norm(yVector)) / 2;
		vtkMath::MultiplyScalar(zVector, normZ);
		
		imageToProbeMatrix->SetElement(0, 2, zVector[0]);
		imageToProbeMatrix->SetElement(1, 2, zVector[1]);
		imageToProbeMatrix->SetElement(2, 2, zVector[2]);

		// Write transformations to log and output
		std::ostringstream osTransformProbeToPhantomReference; 
		this->GetTransformProbeToPhantomReference()->Print(osTransformProbeToPhantomReference);   
		LOG_DEBUG("TransformProbeToPhantomReference:\n" << osTransformProbeToPhantomReference.str().c_str() );

		// Compute the independent point and line reconstruction errors
		LOG_INFO("Compute the independent point and line reconstruction errors") ;
		this->GetCalibrator()->computeIndependentPointLineReconstructionError();

		// STEP-4. Print the final calibration results and error reports 
		LOG_INFO("Print calibration results and error reports"); ;
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

		this->SpatialCalibrationDoneOn(); 

	} catch(...) {
		LOG_ERROR("ComputeCalibrationResults: Failed to compute calibration results!"); 
		throw;
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::PrintCalibrationResultsAndErrorReports()
{
	LOG_TRACE("vtkProbeCalibrationController::PrintCalibrationResultsAndErrorReports");

	try {
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Calibration result in 4x4 homogeneous transform matrix = ");

		for (int i = 0; i < 4; i++) {
			std::ostringstream matrixRow; 

			for (int j = 0; j < 4; j++) {
				matrixRow << this->GetTransformProbeToPhantomReference()->GetMatrix()->GetElement(i,j) << "  " ;
			}
			LOG_INFO(matrixRow.str()); 
		}

		// Point-Line Distance Error Analysis for Validation Positions in US probe frame
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Point-Line Distance Error (PLDE) Analysis in mm =");
		LOG_INFO("[ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]");
		LOG_INFO("[ vector 3    :  Validation data confidence level ]");
		LOG_INFO(GetPointLineDistanceErrorAnalysisVector()[0] * 1000 << ", " << GetPointLineDistanceErrorAnalysisVector()[1] * 1000 << ", " << GetPointLineDistanceErrorAnalysisVector()[2] * 1000); 
		LOG_INFO(GetPointLineDistanceErrorAnalysisVector()[3]);

	} catch(...) {
		LOG_ERROR("PrintCalibrationResultsAndErrorReports: Failed to retrieve the calibration results!"); 
		throw;
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SaveCalibrationResultsAndErrorReportsToXML()
{
	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}

	// Construct the calibration result file name with path and timestamp
	const std::string calibrationResultFileName = this->GetCalibrator()->getCalibrationTimeStampInString() + this->GetCalibrationResultFileSuffix() + ".xml";
	const std::string calibrationResultFileNameWithPath = controller->GetOutputFolder() + std::string("/") + calibrationResultFileName;
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
	tagUltrasoundImageDimensions->SetIntAttribute("Width", this->GetImageWidthInPixels()); 
	tagUltrasoundImageDimensions->SetIntAttribute("Height", this->GetImageHeightInPixels()); 
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


	double *probeToPhantomReferenceMatrix = new double[16]; 
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			probeToPhantomReferenceMatrix[i*4+j] = this->GetTransformProbeToPhantomReference()->GetMatrix()->GetElement(i,j); 
		}
	}

	// <CalibrationTransform>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationTransform = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationTransform->SetName("CalibrationTransform"); 
	tagCalibrationTransform->SetVectorAttribute("TransformProbeToPhantomReference", 16, probeToPhantomReferenceMatrix); 
	// </CalibrationTransform>

	delete[] probeToPhantomReferenceMatrix; 

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
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SaveCalibrationDataToSequenceMetafile()
{
	LOG_INFO("Save calibration data to sequence metafile"); 
	// Save calibration dataset 
	std::ostringstream calibrationDataFileName; 
	calibrationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).OutputSequenceMetaFileSuffix; 
	this->SaveTrackedFrameListToMetafile( FREEHAND_MOTION_1, this->GetOutputPath(), calibrationDataFileName.str().c_str(), false ); 

	LOG_INFO("Save validation data to sequence metafile"); 
	// TODO add validation file name to config file
	// Save validation dataset
	std::ostringstream validationDataFileName; 
	validationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).OutputSequenceMetaFileSuffix; 
	this->SaveTrackedFrameListToMetafile( FREEHAND_MOTION_2, this->GetOutputPath(), validationDataFileName.str().c_str(), false ); 
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
	LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorMatrix (wire #" << wireNumber << ")"); 
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


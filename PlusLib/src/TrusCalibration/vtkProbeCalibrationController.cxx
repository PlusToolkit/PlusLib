#include "PlusConfigure.h"
#include "vtkProbeCalibrationController.h"

#include "BrachyTRUSCalibrator.h"

// VTK includes
#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include "vtkXMLUtilities.h"
#include "vtkImageAppend.h"
#include "vtkDataSetWriter.h"
#include "vtkImageExtractComponents.h"
#include "vtkDirectory.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include <itkImageDuplicator.h>

vtkCxxRevisionMacro(vtkProbeCalibrationController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkProbeCalibrationController); 

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
vtkProbeCalibrationController::vtkProbeCalibrationController() 
: 
mptrCalibrationPhantom(NULL),
MinElevationBeamwidthAndFocalZoneInUSImageFrame(2,0)
{
	this->EnableSystemLogOff();
	this->EnableSegmentedWirePositionsSavingOff(); 
	this->CalibrationDoneOff(); 

	this->US3DBeamwidthDataReadyOff(); 
	this->InitializedOff(); 

	this->USImageFrameOriginXInPixels = 0; 
	this->USImageFrameOriginYInPixels = 0; 
  this->SetCenterOfRotationPx(0,0);
  this->SetPhantomToProbeDistanceInMm(0.0,0.0); 

	this->SetCurrentPRE3DdistributionID(0); 

	this->SetNumUS3DBeamwidthProfileData(-1);
	this->SetIncorporatingUS3DBeamProfile(0);

	this->SetAxialPositionOfCrystalSurfaceInTRUSImageFrame(-1);

	this->CalibrationConfigFileNameWithPath = NULL; 
	this->CalibrationResultFileNameWithPath = NULL;
	this->US3DBeamProfileDataFileNameAndPath = NULL; 
	this->TemplateModelConfigFileName = NULL; 
	this->SegmentationAnalysisFileNameWithTimeStamp = NULL; 
	this->SegmentationErrorLogFileNameWithTimeStamp = NULL;
	this->DataFileSuffix = NULL;
	this->CalibrationResultFileSuffix = NULL;
	this->SegmentationErrorLogFileNameSuffix = NULL;
	this->SegmentationAnalysisFileNameSuffix = NULL;
	this->Temp2StepCalibAnalysisFileNameSuffix = NULL; 
	this->CalibrationSegWirePosInfoFileName = NULL; 
	this->ValidationSegWirePosInfoFileName = NULL; 

	this->VisualizationComponent = NULL;

	this->CalibrationControllerIO = NULL; 

	vtkSmartPointer<vtkTransform> transformProbeHomeToProbe = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformProbeHomeToProbe = NULL;
	this->SetTransformProbeHomeToProbe(transformProbeHomeToProbe); 

	vtkSmartPointer<vtkTransform> transformProbeHomeToTemplateHolderHome = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformProbeHomeToTemplateHolderHome = NULL;
	this->SetTransformProbeHomeToTemplateHolderHome(transformProbeHomeToTemplateHolderHome); 

	vtkSmartPointer<vtkTransform> transformTemplateHolderHomeToTemplateHome = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHolderHomeToTemplateHome = NULL;
	this->SetTransformTemplateHolderHomeToTemplateHome(transformTemplateHolderHomeToTemplateHome); 

  vtkSmartPointer<vtkTransform> transformTemplateHolderHomeToPhantomHome = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHolderHomeToPhantomHome = NULL;
	this->SetTransformTemplateHolderHomeToPhantomHome(transformTemplateHolderHomeToPhantomHome); 

	vtkSmartPointer<vtkTransform> transformUserImageHomeToProbeHome = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformUserImageHomeToProbeHome = NULL;
	this->SetTransformUserImageHomeToProbeHome(transformUserImageHomeToProbeHome); 

	vtkSmartPointer<vtkTransform> transformImageToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToTemplate = NULL;
	this->SetTransformImageToTemplate(transformImageToTemplate); 

	vtkSmartPointer<vtkTransform> transformImageHomeToUserImageHome = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageHomeToUserImageHome = NULL;
	this->SetTransformImageHomeToUserImageHome(transformImageHomeToUserImageHome); 

	vtkSmartPointer<vtkTransform> transformProbeToUserImage = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformProbeToUserImage = NULL;
	this->SetTransformProbeToUserImage(transformProbeToUserImage); 

	vtkSmartPointer<vtkTransform> transformUserImageToImage = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformUserImageToImage = NULL;
	this->SetTransformUserImageToImage(transformUserImageToImage); 

	vtkSmartPointer<vtkTransform> transformTemplateHolderHomeToTemplateHolder = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHolderHomeToTemplateHolder = NULL;
	this->SetTransformTemplateHolderHomeToTemplateHolder(transformTemplateHolderHomeToTemplateHolder); 

	vtkSmartPointer<vtkTransform> transformTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformTemplateHomeToTemplate = NULL;
	this->SetTransformTemplateHomeToTemplate(transformTemplateHomeToTemplate); 

	// Initialize calibration controller IO
	vtkSmartPointer<vtkProbeCalibrationControllerIO> calibrationControllerIO = vtkSmartPointer<vtkProbeCalibrationControllerIO>::New(); 
	calibrationControllerIO->Initialize( this ); 
	this->SetCalibrationControllerIO( calibrationControllerIO ); 

	// Initialize data containers
	// ===========================
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_size(0,0);
	this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_size(0,0); 
	this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);

}

//----------------------------------------------------------------------------
vtkProbeCalibrationController::~vtkProbeCalibrationController() 
{
	this->SetTransformImageToTemplate(NULL);
	this->SetTransformImageHomeToUserImageHome(NULL);
	this->SetTransformUserImageHomeToProbeHome(NULL);
	this->SetTransformProbeHomeToProbe(NULL);
	this->SetTransformProbeToUserImage(NULL);
	this->SetTransformUserImageToImage(NULL);
	this->SetTransformProbeHomeToTemplateHolderHome(NULL);
	this->SetTransformTemplateHolderHomeToTemplateHome(NULL);
  this->SetTransformTemplateHolderHomeToPhantomHome(NULL); 
	this->SetTransformTemplateHolderHomeToTemplateHolder(NULL);
	this->SetTransformTemplateHomeToTemplate(NULL);
	this->SetCalibrationControllerIO(NULL); 

	// Destroy the calibration phantom
	if( mptrCalibrationPhantom != NULL )
	{
		delete mptrCalibrationPhantom;
		mptrCalibrationPhantom = NULL;
	}

	// Destroy the visualization component
	if (this->VisualizationComponent != NULL) 
	{
		this->VisualizationComponent->Delete(); 
		this->VisualizationComponent = NULL; 
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::Initialize()
{
	LOG_TRACE("vtkProbeCalibrationController::Initialize"); 
	
	if ( this->GetSegParameters() == NULL )
	{
		LOG_ERROR( "Unable to initialize calibration: calibration configuration is not loaded!" );  
		return PLUS_FAIL; 
	}

	// Initialize the segmenation component
	// ====================================
	this->mptrAutomatedSegmentation = new KPhantomSeg( 
		this->GetSegParameters()->GetFrameSize(),this->GetSegParameters()->GetRegionOfInterest(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

	// Initialize the calibration component
	if ( this->GetCalibrator() == NULL ) 
	{
		mptrCalibrationPhantom = new BrachyTRUSCalibrator(this->SegParameters, this->GetEnableSystemLog() );
	}

	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	ConvertVtkMatrixToVnlMatrix(this->GetTransformImageHomeToUserImageHome()->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 
	this->GetCalibrator()->setTransformOrigImageToTRUSImageFrame4x4( transformOrigImageFrame2TRUSImageFrameMatrix4x4 );

	// Initialize the visualization component
	// ====================================
	if ( this->GetEnableVisualization() ) 
	{
		this->InitializeVisualizationComponents();
	}

	// Set the ultrasound image frame in pixels
	// =================================================
	// This defines the US image frame origin in pixels W.R.T. the 
	// left-upper corner of the original image, with X pointing to 
	// the right (column) and Y pointing down to the bottom (row).
	this->SetUSImageFrameOriginInPixels( this->GetUSImageFrameOriginXInPixels(), this->GetUSImageFrameOriginYInPixels() ); 

	// STEP-OPTIONAL. Apply the US 3D beamwidth data to calibration if desired
	// ========================================================================
	// This will pass the US 3D beamwidth data and their predefined
	// weights to the calibration component.
	if( this->GetUS3DBeamwidthDataReady() )
	{
		this->GetCalibrator()->setUltrasoundBeamwidthAndWeightFactorsTable(
			this->GetIncorporatingUS3DBeamProfile(),
			*this->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM(),
			*this->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN(),
			*this->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() );
	}

	this->InitializedOn(); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::InitializeVisualizationComponents()
{
	LOG_TRACE("vtkProbeCalibrationController::InitializeVisualizationComponents"); 
	if ( this->VisualizationComponent == NULL ) 
	{
		this->VisualizationComponent = vtkCalibratorVisualizationComponent::New(); 
		this->VisualizationComponent->SetTemplateModelConfigFileName( this->GetTemplateModelConfigFileName() ); 
		this->VisualizationComponent->Initialize( this ); 
		this->VisualizationComponent->SetOutputPath( this->GetOutputPath() ); 
	}
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::RegisterPhantomGeometry( double phantomToProbeDistanceInMm[2] )
{
  this->SetPhantomToProbeDistanceInMm(phantomToProbeDistanceInMm[0], phantomToProbeDistanceInMm[1]); 
  this->RegisterPhantomGeometry(); 
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::RegisterPhantomGeometry()
{
	LOG_TRACE("vtkProbeCalibrationController::RegisterPhantomGeometry: " << this->GetPhantomToProbeDistanceInMm()[0] << "  " << this->GetPhantomToProbeDistanceInMm()[1]); 
	// Vertical distance from the template mounter hole center
	// to the TRUS Rotation Center
	/*double verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
			this->GetCalibrator()->GetPhantomPoints().WirePositionFrontWall[0].y
            + this->GetPhantomToProbeDistanceInMm()[1]
    - this->GetCalibrator()->GetPhantomPoints().TemplateHolderPosition.y; 
    */

  double verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
			this->GetCalibrator()->GetNWire(1).wires[0].endPointFront[1] // WIRE1 y
            + this->GetPhantomToProbeDistanceInMm()[1]
            - GetTransformTemplateHolderHomeToPhantomHome()->GetPosition()[1]; // :TODO: transform with the whole matrix instead of just using the XY position values

    // Horizontal distance from the template mounter hole center
    // to the TRUS Rotation Center
    /*double horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
        this->GetCalibrator()->GetPhantomPoints().WirePositionFrontWall[2].x
        + phantomToProbeDistanceInMm[0]
    - this->GetCalibrator()->GetPhantomPoints().TemplateHolderPosition.x; */
    double horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
        this->GetCalibrator()->GetNWire(0).wires[2].endPointFront[0] // WIRE3 x
        + this->GetPhantomToProbeDistanceInMm()[0]
    - GetTransformTemplateHolderHomeToPhantomHome()->GetPosition()[0]; // :TODO: transform with the whole matrix instead of just using the XY position values

    double templateHolderPositionX=GetTransformTemplateHolderHomeToPhantomHome()->GetPosition()[0];
    double templateHolderPositionY=GetTransformTemplateHolderHomeToPhantomHome()->GetPosition()[1];
    double templateHolderPositionZ=GetTransformTemplateHolderHomeToPhantomHome()->GetPosition()[2];

    double horizontalTemplateToStepper = horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM + templateHolderPositionX;
    double verticalTemplateToStepper = verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM + templateHolderPositionY;

    vtkSmartPointer<vtkTransform> tTemplateHolderToTemplate = vtkSmartPointer<vtkTransform>::New();
    tTemplateHolderToTemplate->Translate( templateHolderPositionX, templateHolderPositionY, templateHolderPositionZ);
    this->GetTransformTemplateHolderHomeToTemplateHome()->SetMatrix( tTemplateHolderToTemplate->GetMatrix() ); 

    vtkSmartPointer<vtkTransform> tProbeHomeToTemplateHolderHome = vtkSmartPointer<vtkTransform>::New();
    tProbeHomeToTemplateHolderHome->Translate( horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, 0);
    this->GetTransformProbeHomeToTemplateHolderHome()->SetMatrix( tProbeHomeToTemplateHolderHome->GetMatrix() ); 

    vtkSmartPointer<vtkTransform> tTemplateHomeToProbeHome = vtkSmartPointer<vtkTransform>::New();
    tTemplateHomeToProbeHome->PostMultiply(); 
    tTemplateHomeToProbeHome->Concatenate( this->TransformProbeHomeToTemplateHolderHome ); 
    tTemplateHomeToProbeHome->Concatenate( this->TransformTemplateHolderHomeToTemplateHome ); 
    tTemplateHomeToProbeHome->Inverse(); 

    std::ostringstream osTemplateHomeToProbeHome; 
    tTemplateHomeToProbeHome->GetMatrix()->Print(osTemplateHomeToProbeHome);   
    LOG_DEBUG("TemplateHomeToProbeHome:\n" << osTemplateHomeToProbeHome.str().c_str() );

    // Register the phantom geometry to the DRB frame in the "Emulator" mode.
    vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
    ConvertVtkMatrixToVnlMatrixInMeter( tTemplateHomeToProbeHome->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode ); 

    this->GetCalibrator()->registerPhantomGeometryInEmulatorMode( transformMatrixPhantom2DRB4x4InEmulatorMode );
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::OfflineUSToTemplateCalibration()
{
    LOG_TRACE("vtkProbeCalibrationController::OfflineUSToTemplateCalibration"); 
    if ( ! this->GetInitialized() ) 
    {
        this->Initialize(); 
    }

    // Reset calibrator data containers 
    this->GetCalibrator()->resetDataContainers(); 

    // ****************************  Validation data ***********************
    vtkSmartPointer<vtkTrackedFrameList> validationData = vtkSmartPointer<vtkTrackedFrameList>::New();
    const std::string validationDataFileName = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).InputSequenceMetaFileName; 
    if ( !validationDataFileName.empty() )
    {
        if ( validationData->ReadFromSequenceMetafile(validationDataFileName.c_str()) != PLUS_SUCCESS )
        {
            LOG_ERROR("Failed to read tracked frames from sequence metafile from: " << validationDataFileName ); 
            return PLUS_FAIL; 
        }
    }
    else
    {
        LOG_ERROR("Unable to start OfflineUSToTemplateCalibration with validation data: SequenceMetaFileName is empty!"); 
        return PLUS_FAIL; 
    }

    // Reset the counter before we start
    ImageDataInfo validationDataInfo = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2); 
    validationDataInfo.NumberOfSegmentedImages = 0; 
    if ( validationDataInfo.NumberOfImagesToAcquire > validationData->GetNumberOfTrackedFrames() )
    {
      validationDataInfo.NumberOfImagesToAcquire = validationData->GetNumberOfTrackedFrames(); 
    }
    this->SetImageDataInfo(RANDOM_STEPPER_MOTION_2, validationDataInfo); 

    int validationCounter(0); 
    int vImgNumber(0);
    for( vImgNumber = 0; validationCounter < this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfImagesToAcquire; vImgNumber++ )
    {
        if ( vImgNumber >= validationData->GetNumberOfTrackedFrames() )
        {
            break; 
        }

        if ( this->AddTrackedFrameData(validationData->GetTrackedFrame(vImgNumber), RANDOM_STEPPER_MOTION_2) == PLUS_SUCCESS )
        {
            // The segmentation was successful 
            validationCounter++; 
        }

        this->AddFrameToRenderer(validationData->GetTrackedFrame(vImgNumber)->ImageData); 
    }

    int validSegmentationSuccessRate = 100*this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfSegmentedImages / vImgNumber; 
    LOG_INFO ( "A total of " << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfSegmentedImages << " images (" << validSegmentationSuccessRate << "%) have been successfully added for validation.");

    validationData->Clear(); 

    // ****************************  Calibration data ***********************
    vtkSmartPointer<vtkTrackedFrameList> calibrationData = vtkSmartPointer<vtkTrackedFrameList>::New();
    const std::string calibrationDataFileName = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).InputSequenceMetaFileName; 
    if ( !calibrationDataFileName.empty() )
    {
        if ( calibrationData->ReadFromSequenceMetafile(calibrationDataFileName.c_str()) != PLUS_SUCCESS )
        {
            LOG_ERROR("Failed to read tracked frames from sequence metafile from: " << calibrationDataFileName ); 
            return PLUS_FAIL; 
        }
    }
    else
    {
        LOG_ERROR("Unable to start OfflineUSToTemplateCalibration with calibration data: SequenceMetaFileName is empty!"); 
        return PLUS_FAIL; 
    }

    // Reset the counter before we start
    ImageDataInfo calibrationDataInfo = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1); 
    calibrationDataInfo.NumberOfSegmentedImages = 0; 
    if ( calibrationDataInfo.NumberOfImagesToAcquire > calibrationData->GetNumberOfTrackedFrames() )
    {
      calibrationDataInfo.NumberOfImagesToAcquire = calibrationData->GetNumberOfTrackedFrames(); 
    }
    this->SetImageDataInfo(RANDOM_STEPPER_MOTION_1, calibrationDataInfo); 

    int calibrationCounter(0);
    int cImgNumber(0); 
    for( cImgNumber = 0; calibrationCounter < this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfImagesToAcquire; cImgNumber++ )
    {
        if ( cImgNumber >= calibrationData->GetNumberOfTrackedFrames() )
        {
            break; 
        }

        if ( this->AddTrackedFrameData(calibrationData->GetTrackedFrame(cImgNumber), RANDOM_STEPPER_MOTION_1) == PLUS_SUCCESS)
        {
            // The segmentation was successful
            calibrationCounter++; 
        }

        this->AddFrameToRenderer(calibrationData->GetTrackedFrame(cImgNumber)->ImageData); 
    }

    int calibSegmentationSuccessRate = 100*this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages / cImgNumber; 
    LOG_INFO ("A total of " << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages << " images (" << calibSegmentationSuccessRate << "%) have been successfully added for calibration.");

    calibrationData->Clear(); 

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::DoCalibration()
{
	LOG_TRACE("vtkProbeCalibrationController::DoCalibration"); 
	// Instruct the calibrator to perform the calibration task
	this->GetCalibrator()->calibrate();

	// Instruct the calibrator to validate the calibration accuracy
	this->GetCalibrator()->compute3DPointReconstructionError();

	// Update the plot components for this particular iteration
	if ( this->GetEnableVisualization() ) 
	{
		this->GetVisualizationComponent()->UpdatePlotComponents();
		this->GetVisualizationComponent()->PlotPRE3Ddistribution();
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkProbeCalibrationController::AddTrackedFrameData"); 
	if ( Superclass::AddTrackedFrameData(trackedFrame, dataType) )
	{
		double tProbeHomeToProbe[16]; 
		if ( trackedFrame->GetDefaultFrameTransform(tProbeHomeToProbe) )
		{
			vtkSmartPointer<vtkMatrix4x4> tProbeHomeToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
			tProbeHomeToProbeMatrix->DeepCopy(tProbeHomeToProbe); 
			vnl_matrix<double> transformUSProbe2StepperFrameMatrix4x4(4,4);

			ConvertVtkMatrixToVnlMatrixInMeter(tProbeHomeToProbeMatrix, transformUSProbe2StepperFrameMatrix4x4); 

			this->GetTransformProbeHomeToProbe()->SetMatrix(tProbeHomeToProbeMatrix); 

			this->PopulateSegmentedFiducialsToDataContainer(transformUSProbe2StepperFrameMatrix4x4, dataType); 

			if( dataType == RANDOM_STEPPER_MOTION_1  
				&& 
				this->GetCalibrator()->areDataPositionsReadyForCalibration() 
				&& 
				this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages % 10 == 0
				)
			{
				this->DoCalibration(); 
			}
			return PLUS_SUCCESS; 
		}
	}

	return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::ComputeCalibrationResults()
{
	LOG_TRACE("vtkProbeCalibrationController::ComputeCalibrationResults"); 
	try
	{
		if ( ! this->GetInitialized() ) 
		{
			this->Initialize(); 
		}

		// Do final calibration 
		this->DoCalibration(); 

		// Get the image home to probe home transformation from the calibrator
		vnl_matrix<double> calibResultMatrix = this->GetCalibrator()->getCalibrationResultsInMatrix(); 
		vtkSmartPointer<vtkMatrix4x4> userImageHomeToProbeHomeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

		// convert transform from meter to mm
		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				userImageHomeToProbeHomeMatrix->SetElement(i, j, calibResultMatrix.get(i, j) * 1000 ); 
			}
		}
		
		// Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
		double xVector[3] = {userImageHomeToProbeHomeMatrix->GetElement(0,0),userImageHomeToProbeHomeMatrix->GetElement(1,0),userImageHomeToProbeHomeMatrix->GetElement(2,0)}; 
		double yVector[3] = {userImageHomeToProbeHomeMatrix->GetElement(0,1),userImageHomeToProbeHomeMatrix->GetElement(1,1),userImageHomeToProbeHomeMatrix->GetElement(2,1)};  
		double zVector[3] = {0,0,0}; 

    double dotProduct = vtkMath::Dot(xVector, yVector);
    if (dotProduct > 0.001) 
    {
      LOG_WARNING("Calibration result axes are not orthogonal (dot product of X and Y axes is " << dotProduct << ")");
      return PLUS_FAIL; 
    }
		
		vtkMath::Cross(xVector, yVector, zVector); 
						
		// make the z vector have about the same length as x an y,
		// so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
		vtkMath::Normalize(zVector);
		double normZ = (vtkMath::Norm(xVector)+vtkMath::Norm(yVector))/2;  
		vtkMath::MultiplyScalar(zVector, normZ);
		
		userImageHomeToProbeHomeMatrix->SetElement(0, 2, zVector[0]);
		userImageHomeToProbeHomeMatrix->SetElement(1, 2, zVector[1]);
		userImageHomeToProbeHomeMatrix->SetElement(2, 2, zVector[2]);

		std::ostringstream osTransformImageHomeToUserImageHome; 
		this->GetTransformImageHomeToUserImageHome()->Print(osTransformImageHomeToUserImageHome);   
		LOG_DEBUG("TransformImageHomeToUserImageHome:\n" << osTransformImageHomeToUserImageHome.str().c_str() );

		this->GetTransformUserImageHomeToProbeHome()->SetMatrix( userImageHomeToProbeHomeMatrix ); 
		std::ostringstream osTransformUserImageHomeToProbeHome; 
		this->GetTransformUserImageHomeToProbeHome()->Print(osTransformUserImageHomeToProbeHome);   
		LOG_DEBUG("TransformUserImageHomeToProbeHome:\n" << osTransformUserImageHomeToProbeHome.str().c_str() );

		std::ostringstream osTransformProbeHomeToProbe; 
		this->GetTransformProbeHomeToProbe()->Print(osTransformProbeHomeToProbe);   
		LOG_DEBUG("TransformProbeHomeToProbe:\n" << osTransformProbeHomeToProbe.str().c_str() );

		this->GetTransformProbeToUserImage()->SetMatrix( userImageHomeToProbeHomeMatrix ); 
		this->GetTransformProbeToUserImage()->Inverse(); 

		std::ostringstream osTransformProbeHomeToTemplateHolderHome; 
		this->GetTransformProbeHomeToTemplateHolderHome()->Print(osTransformProbeHomeToTemplateHolderHome);  
		LOG_DEBUG("TransformProbeHomeToTemplateHolderHome:\n" << osTransformProbeHomeToTemplateHolderHome.str().c_str() );

		std::ostringstream osTransformTemplateHolderHomeToTemplateHome; 
		this->GetTransformTemplateHolderHomeToTemplateHome()->Print(osTransformTemplateHolderHomeToTemplateHome);  
		LOG_DEBUG("TransformTemplateHolderHomeToTemplateHome:\n" << osTransformTemplateHolderHomeToTemplateHome.str().c_str() );

		std::ostringstream osTransformTemplateHomeToTemplate; 
		this->GetTransformTemplateHomeToTemplate()->Print(osTransformTemplateHomeToTemplate);  
		LOG_DEBUG("TransformTemplateHomeToTemplate:\n" << osTransformTemplateHomeToTemplate.str().c_str() );

		this->TransformImageToTemplate->Identity(); 
		this->TransformImageToTemplate->PostMultiply(); 
		this->TransformImageToTemplate->Concatenate(this->TransformImageHomeToUserImageHome); 
		this->TransformImageToTemplate->Concatenate(this->TransformUserImageHomeToProbeHome); 
		this->TransformImageToTemplate->Concatenate(this->TransformProbeHomeToProbe); 
		this->TransformImageToTemplate->Concatenate(this->TransformProbeHomeToTemplateHolderHome); 
		this->TransformImageToTemplate->Concatenate(this->TransformTemplateHolderHomeToTemplateHome);
		//this->TransformImageToTemplate->Concatenate(this->TransformTemplateHomeToTemplate);
		this->TransformImageToTemplate->Update(); 

		std::ostringstream osTransformImageToTemplate; 
		this->GetTransformImageToTemplate()->Print(osTransformImageToTemplate);  
		LOG_DEBUG("TransformImageToTemplate:\n" << osTransformImageToTemplate.str().c_str() );


		// Compute the independent point and line reconstruction errors
		// ============================================================
		LOG_INFO(">>>>>>>> Compute the independent point and line reconstruction errors ...") ;
		this->GetCalibrator()->computeIndependentPointLineReconstructionError();

		// STEP-4. Print the final calibration results and error reports 
		// =================================================================
		LOG_INFO(">>>>>>>> Print calibration results and error reports ..."); ;
		this->PrintCalibrationResultsAndErrorReports();

		// STEP-5. Save the calibration results and error reports into a file 
		// ===================================================================
		LOG_INFO(">>>>>>>> Save the calibration results and error reports to file...");
		this->CalibrationControllerIO->SaveCalibrationResultsAndErrorReportsToXML();

		// STEP-6. Save the segmented wire positions into a file 
		// ===================================================================
		if ( this->EnableSegmentedWirePositionsSaving )
		{
			LOG_INFO(">>>>>>>> Save the segmented wire positions to file..."); 
			this->CalibrationControllerIO->SaveSegmentedWirePositionsToFile(); 
		}

		if ( this->GetEnableVisualization() ) 
		{
			LOG_INFO(">>>>>>>> Save PRE3D plot to image..."); 
			// save the PRE3D distribution plot to an image file
			this->GetVisualizationComponent()->SavePRE3DplotToImage();
		}

		// save the input images to meta image
		if ( this->GetEnableTrackedSequenceDataSaving() )
		{
			LOG_INFO(">>>>>>>> Save calibration data to sequence metafile..."); 
			// Save calibration dataset 
			std::ostringstream calibrationDataFileName; 
			calibrationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).OutputSequenceMetaFileSuffix; 
			if ( this->SaveTrackedFrameListToMetafile( RANDOM_STEPPER_MOTION_1, this->GetOutputPath(), calibrationDataFileName.str().c_str(), false ) != PLUS_SUCCESS ) 
      {
        LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
        return PLUS_FAIL;
      }

			LOG_INFO(">>>>>>>> Save validation data to sequence metafile..."); 
			// TODO add validation file name to config file
			// Save validation dataset
			std::ostringstream validationDataFileName; 
			validationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).OutputSequenceMetaFileSuffix; 
			if ( this->SaveTrackedFrameListToMetafile( RANDOM_STEPPER_MOTION_2, this->GetOutputPath(), validationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
        return PLUS_FAIL;
      }
		}

		this->CalibrationDoneOn(); 

	}
	catch(...)
	{
		LOG_ERROR("ComputeCalibrationResults: Failed to compute calibration results!"); 
		return PLUS_FAIL; 
	}

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkProbeCalibrationController::PopulateSegmentedFiducialsToDataContainer"); 
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

	if ( !segResults.GetDotsFound() )
	{
		LOG_DEBUG("Segmentation failed! Unable to populate segmentation result!"); 
		return; 
	}

	// Top layer (far from probe): 6, 5, 4 
	// Bottom Layer (close to probe): 3, 2, 1 
	std::vector<vnl_vector_double> SegmentedNFiducialsInFixedCorrespondence;
	SegmentedNFiducialsInFixedCorrespondence.resize(0);
	for (int i=0; i<segResults.GetFoundDotsCoordinateValue().size(); i++)
	{
		vnl_vector<double> NFiducial(4,0);
		NFiducial[0]=segResults.GetFoundDotsCoordinateValue()[i][0];
		NFiducial[1]=segResults.GetFoundDotsCoordinateValue()[i][1];
		NFiducial[2]=0;
		NFiducial[3]=1;
		SegmentedNFiducialsInFixedCorrespondence.push_back(NFiducial);
	}

	if (dataType == RANDOM_STEPPER_MOTION_1)
	{
		// Finally, add the data for calibration
		this->GetCalibrator()->addDataPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
	else if (dataType == RANDOM_STEPPER_MOTION_2)
	{
		this->GetCalibrator()->addValidationPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::SetUSImageFrameOriginInPixels( int originX, int originY)
{
	LOG_TRACE("vtkProbeCalibrationController::SetUSImageFrameOriginInPixels: " << originX << "  " << originY); 
	this->SetUSImageFrameOriginXInPixels(originX); 
	this->SetUSImageFrameOriginYInPixels(originY); 

	if ( this->GetCalibrator() != NULL ) 
	{
		this->GetCalibrator()->setUltrasoundImageFrameOriginInPixels(originX, originY); 
	}
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::SetUSImageFrameOriginInPixels( int* origin )
{
	this->SetUSImageFrameOriginInPixels(origin[0], origin[1]); 
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::GetWirePosInTemplateCoordinate( int wireNum, double* wirePosInTemplate )
{
	LOG_TRACE("vtkProbeCalibrationController::GetWirePosInTemplateCoordinate (wire #" << wireNum << ")"); 

  // wireNum is 1-based (values are between 1..6)
  int nwireIndex=(wireNum-1)/3;
  int wireIndex=(wireNum-1)%3;

	// Wire position on the front wall in template coordinate system
  double p1[3] = {
    this->GetCalibrator()->GetNWire(nwireIndex).wires[wireIndex].endPointFront[0],
    this->GetCalibrator()->GetNWire(nwireIndex).wires[wireIndex].endPointFront[1],
    this->GetCalibrator()->GetNWire(nwireIndex).wires[wireIndex].endPointFront[2]
  }; 

	// Wire position on the back wall in template coordinate system
  double p2[3] = {
    this->GetCalibrator()->GetNWire(nwireIndex).wires[wireIndex].endPointBack[0],
    this->GetCalibrator()->GetNWire(nwireIndex).wires[wireIndex].endPointBack[1],
    this->GetCalibrator()->GetNWire(nwireIndex).wires[wireIndex].endPointBack[2]
  }; 

	vtkSmartPointer<vtkTransform> tTemplateToTemplateHome = vtkSmartPointer<vtkTransform>::New();
	tTemplateToTemplateHome->Concatenate(this->TransformTemplateHomeToTemplate); 
	tTemplateToTemplateHome->Inverse(); 

	vtkSmartPointer<vtkTransform> tImageToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	tImageToTemplate->PostMultiply(); 
	tImageToTemplate->Concatenate(this->TransformImageToTemplate); 
	tImageToTemplate->Concatenate(tTemplateToTemplateHome); 

	// Normal vector
	double n[3] = { 
		tImageToTemplate->GetMatrix()->GetElement(0,2), 
		tImageToTemplate->GetMatrix()->GetElement(1,2), 
		tImageToTemplate->GetMatrix()->GetElement(2,2)
	}; 

	// Origin of the plane
	double p0[3] = {0, 0, 0}; 
	tImageToTemplate->GetPosition(p0); 

	// parametric coordinate along the line
	double t(0); 

	if ( vtkPlane::IntersectWithLine(p1, p2, n, p0, t, wirePosInTemplate ) )
	{
		return PLUS_SUCCESS; 
	}

	// plane and line are parallel
	return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationController::PrintCalibrationResultsAndErrorReports ()
{
	LOG_TRACE("vtkProbeCalibrationController::PrintCalibrationResultsAndErrorReports"); 
	try
	{
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Calibration result in 4x4 homogeneous transform matrix = ");
		for ( int i = 0; i < 4; i++ )
		{
			std::ostringstream matrixRow; 
			for ( int j = 0; j < 4; j++ )
			{
				matrixRow << this->GetTransformUserImageHomeToProbeHome()->GetMatrix()->GetElement(i,j) << "  " ;
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

	}
	catch(...)
	{
		LOG_ERROR("PrintCalibrationResultsAndErrorReports: Failed to retrieve the calibration results!"); 
		throw;
	}
}

//----------------------------------------------------------------------------
std::vector<double> vtkProbeCalibrationController::GetLineReconstructionErrorAnalysisVector(int wireNumber)
{
	LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorAnalysisVector (wire #" << wireNumber << ")"); 
	std::vector<double> absLREAnalysisInUSProbeFrame; 
	switch(wireNumber)
	{
	case 1: // wire #1
		{
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire1();
		}
		break; 
	case 3: // wire #3
		{
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire3();
		}
		break; 
	case 4: // wire #4
		{
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire4();
		}
		break; 
	case 6: // wire #6
		{
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire6();
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE analysis vector for wire #" << wireNumber ); 
	}

	return absLREAnalysisInUSProbeFrame; 
}

//----------------------------------------------------------------------------
vnl_matrix<double> vtkProbeCalibrationController::GetLineReconstructionErrorMatrix(int wireNumber)
{
	//LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorMatrix (wire #" << wireNumber << ")"); 
	vnl_matrix<double> mLREOrigInUSProbeFrameMatrix; 
	switch(wireNumber)
	{
	case 1: // wire #1
		{
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire1();
		}
		break; 
	case 3: // wire #3
		{
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire3();
		}
		break; 
	case 4: // wire #4
		{
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire4();
		}
		break; 
	case 6: // wire #6
		{
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire6();
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE matrix for wire #" << wireNumber ); 
	}

	return mLREOrigInUSProbeFrameMatrix; 
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::ReadConfiguration( const char* configFileNameWithPath )
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration - " << configFileNameWithPath); 
	this->SetConfigurationFileName(configFileNameWithPath); 
	
	vtkSmartPointer<vtkXMLDataElement> calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName()); 
  if (calibrationController==NULL)
  {
    LOG_ERROR("Failed to read calibration controller configuration from " << this->GetConfigurationFileName());
    return PLUS_FAIL;
  }

	PlusStatus status=this->ReadConfiguration(calibrationController); 
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to read configuration"); 
		return PLUS_FAIL;
	}

	// Calibration configuration
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> usCalibration = configData->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL)
  {
    LOG_ERROR("Cannot find USCalibration element in XML tree!");
    return PLUS_FAIL;
	}

  // Calibration controller specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	this->ReadCalibrationControllerConfiguration(calibrationController); 

	// ProbeCalibration specifications
	//*********************************
	vtkSmartPointer<vtkXMLDataElement> probeCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration"); 
	this->CalibrationControllerIO->ReadProbeCalibrationConfiguration(probeCalibration); 

	// Phantom definition
	//*********************************
	this->ReadPhantomDefinition(configData);

	// Custom transforms
	//*********************************
	vtkSmartPointer<vtkXMLDataElement> phantomDefinition = configData->FindNestedElementWithName("PhantomDefinition");
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
    vtkSmartPointer<vtkTransform> transformTemplateHolderHomeToPhantomHome = vtkSmartPointer<vtkTransform>::New(); 
    transformTemplateHolderHomeToPhantomHome->SetMatrix(templateHolderToPhantomTransformVector); 
    SetTransformTemplateHolderHomeToPhantomHome( transformTemplateHolderHomeToPhantomHome  ); 
  }
  else
	{
		LOG_ERROR("Unable to read template origin from template holder from template model file!"); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::WriteConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkProbeCalibrationController::WriteConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to read configuration"); 
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> probeCalibration = configData->LookupElementWithName("ProbeCalibration");
  if ( probeCalibration == NULL )
  {
    LOG_ERROR("Failed to write results to ProbeCalibration XML data element - element not found!"); 
    return PLUS_FAIL; 
  }

  probeCalibration->SetVectorAttribute("CenterOfRotationPx", 2, this->GetCenterOfRotationPx()); 

  probeCalibration->SetVectorAttribute("PhantomToProbeDistanceInMm", 2, this->GetPhantomToProbeDistanceInMm()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkProbeCalibrationController::GenerateProbeCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
	LOG_TRACE("vtkProbeCalibrationController::GenerateProbeCalibrationReport"); 
	if ( htmlReport == NULL || plotter == NULL )
	{
		LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
		return PLUS_FAIL; 
	}

	std::string plotSegmentedWirePositionsHistogramScript = gnuplotScriptsFolder + std::string("/PlotSegmentedWirePositionsHistogram.gnu"); 
	if ( !vtksys::SystemTools::FileExists( plotSegmentedWirePositionsHistogramScript.c_str(), true) )
	{
		LOG_ERROR("Unable to find gnuplot script at: " << plotSegmentedWirePositionsHistogramScript); 
		return PLUS_FAIL; 
	}

	std::string plotSegmentedWirePositionsErrorScript = gnuplotScriptsFolder + std::string("/PlotSegmentedWirePositions.gnu"); 
	if ( !vtksys::SystemTools::FileExists( plotSegmentedWirePositionsErrorScript.c_str(), true) )
	{
		LOG_ERROR("Unable to find gnuplot script at: " << plotSegmentedWirePositionsErrorScript); 
		return PLUS_FAIL; 
	}

	if ( this->CalibrationDone && this->GetEnableSegmentedWirePositionsSaving() )
	{
		const char* reportFile = this->GetCalibrationSegWirePosInfoFileName();
    if ( reportFile == NULL )
    {
      LOG_ERROR("Failed to generate probe calibration report - report file name is NULL!"); 
      return PLUS_FAIL; 
    }

		if ( !vtksys::SystemTools::FileExists( reportFile, true) )
		{
			LOG_ERROR("Unable to find segmented wire positions report file at: " << reportFile); 
			return PLUS_FAIL; 
		}

		htmlReport->AddText("Final Calibration Error", vtkHTMLGenerator::H1); 

		htmlReport->AddText("Error Histogram", vtkHTMLGenerator::H2); 

		const int wires[6] = {1, 2, 3, 4, 5, 6}; 

		for ( int i = 0; i < 6; i++ )
		{
			std::ostringstream wireName; 
			wireName << "Wire #" << wires[i] << std::ends; 
			htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
			plotter->ClearArguments(); 
			plotter->AddArgument("-e");
			std::ostringstream segWirePosError; 
			segWirePosError << "f='" << reportFile << "'; o='SegWirePosHistogram'; w=" << wires[i] << std::ends; 
			plotter->AddArgument(segWirePosError.str().c_str()); 
			plotter->AddArgument(plotSegmentedWirePositionsHistogramScript.c_str());  
			if ( plotter->Execute() != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to run gnuplot executer!"); 
        continue; 
      }
			plotter->ClearArguments(); 

			std::ostringstream imageSource;
			imageSource << "w" << wires[i] << "_SegWirePosHistogram.jpg" << std::ends; 
			std::ostringstream imageAlt; 
			imageAlt << "Final calibration error histogram - wire #" << wires[i] << std::ends; 

			htmlReport->AddImage(imageSource.str().c_str(), imageAlt.str().c_str());  
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		htmlReport->AddText("Error Plot", vtkHTMLGenerator::H2); 

		for ( int i = 0; i < 6; i++ )
		{
			std::ostringstream wireName; 
			wireName << "Wire #" << wires[i] << std::ends; 
			htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
			plotter->ClearArguments(); 
			plotter->AddArgument("-e");
			std::ostringstream segWirePosError; 
			segWirePosError << "f='" << reportFile << "'; o='SegWirePos'; w=" << wires[i] << std::ends; 
			plotter->AddArgument(segWirePosError.str().c_str()); 
			plotter->AddArgument(plotSegmentedWirePositionsErrorScript.c_str());  
			if ( plotter->Execute() != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to run gnuplot executer!"); 
        continue; 
      }

			plotter->ClearArguments(); 

			std::ostringstream imageSourceX, imageAltX, imageSourceY, imageAltY; 

			imageSourceX << "w" << wires[i] << "x_SegWirePos.jpg" << std::ends; 
			imageSourceY << "w" << wires[i] << "y_SegWirePos.jpg" << std::ends; 
			imageAltX << "Final calibration error - wire #" << wires[i] << " X Axis" << std::ends; 
			imageAltY << "Final calibration error - wire #" << wires[i] << " Y Axis" << std::ends; 

			htmlReport->AddImage(imageSourceX.str().c_str(), imageAltX.str().c_str());  
			htmlReport->AddImage(imageSourceY.str().c_str(), imageAltY.str().c_str());  
		}

		htmlReport->AddHorizontalLine(); 
	}

  return PLUS_SUCCESS;
}

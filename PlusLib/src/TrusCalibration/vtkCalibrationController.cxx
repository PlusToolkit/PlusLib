#include "PlusConfigure.h"
#include "vtkCalibrationController.h"

#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkMatrix4x4.h"
#include "vtkDirectory.h"
#include "vtkImageImport.h"
#include "vtkXMLDataElement.h"
#include "vtkFileFinder.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include <itkImageDuplicator.h>
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkTriangle.h"
#include "vtkPlane.h"
#include "vtksys/SystemTools.hxx"

vtkCxxRevisionMacro(vtkCalibrationController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkCalibrationController);

//----------------------------------------------------------------------------
void vtkCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
vtkCalibrationController::vtkCalibrationController() 
: 
mptrAutomatedSegmentation(NULL),
SegmentationProgressCallbackFunction(NULL)
{
	this->EnableTrackedSequenceDataSavingOff();
	this->EnableErroneouslySegmentedDataSavingOff(); 
	this->EnableSegmentationAnalysisOff();
	this->InitializedOff(); 

	// Segmentation parameters
	this->SetImageWidthInPixels(0); 
	this->SetImageHeightInPixels(0); 
	this->SetSearchStartAtX(-1);
	this->SetSearchStartAtY(-1);
	this->SetSearchDimensionX(0);
	this->SetSearchDimensionY(0);

	this->VisualizationComponent = NULL;
	this->OutputPath = NULL; 
	this->ProgramFolderPath = NULL; 
	this->ConfigurationFileName = NULL;
	this->PhantomDefinitionFileName = NULL;
	this->ModelToPhantomTransform = NULL;
	this->PhantomModelFileName = NULL;
	this->SegParameters = NULL; 

	this->SetCalibrationMode(REALTIME); 

	// Set program folder path to current working directory by default
	this->SetProgramFolderPath(vtksys::SystemTools::GetCurrentWorkingDirectory().c_str()); 

	for ( int i = 0; i < NUMBER_OF_IMAGE_DATA_TYPES; i++ )
	{
		vtkTrackedFrameList *trackedFrameList = vtkTrackedFrameList::New(); 
		this->TrackedFrameListContainer.push_back(trackedFrameList); 

		SavedImageDataInfo imageDataInfo; 
		imageDataInfo.NumberOfImagesToUse = 0; 
		imageDataInfo.StartingIndex = 0; 
		imageDataInfo.SequenceMetaFileName.clear(); 
		SavedImageDataInfoContainer.push_back(imageDataInfo); 

		RealtimeImageDataInfo realtimeImageDataInfo; 
		realtimeImageDataInfo.NumberOfImagesToAcquire = 0; 
		realtimeImageDataInfo.NumberOfSegmentedImages = 0; 
		realtimeImageDataInfo.OutputSequenceMetaFileSuffix.clear(); 
		RealtimeImageDataInfoContainer.push_back(realtimeImageDataInfo); 
	}
}

//----------------------------------------------------------------------------
vtkCalibrationController::~vtkCalibrationController() 
{
	// Destroy the segmentation parameters
	if ( this->SegParameters != NULL )
	{
		delete this->SegParameters; 
		this->SegParameters = NULL; 
	}

	// Destroy the segmentation component
	if ( mptrAutomatedSegmentation != NULL )
	{
		delete mptrAutomatedSegmentation;
		mptrAutomatedSegmentation = NULL;
	}

	for ( int i = 0; i < NUMBER_OF_IMAGE_DATA_TYPES; i++ )
	{
		if ( this->TrackedFrameListContainer[i] != NULL )
		{
			this->TrackedFrameListContainer[i]->Delete();  
			this->TrackedFrameListContainer[i] = NULL; 
		}
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::Initialize()
{
	LOG_TRACE("vtkCalibrationController::Initialize"); 
	if ( this->GetSegParameters() == NULL )
	{
		this->SegParameters = new SegmentationParameters(); 
	}

	// Initialize the segmenation component
	// ====================================
	this->mptrAutomatedSegmentation = new KPhantomSeg( 
		this->GetImageWidthInPixels(), this->GetImageHeightInPixels(), 
		this->GetSearchStartAtX(), this->GetSearchStartAtY(), 
		this->GetSearchDimensionX(), this->GetSearchDimensionY(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkTrackedFrameList* vtkCalibrationController::GetTrackedFrameList( IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::GetTrackedFrameList"); 
	return this->TrackedFrameListContainer[dataType]; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::SaveTrackedFrameListToMetafile( IMAGE_DATA_TYPE dataType, const char* outputFolder, const char* sequenceMetafileName, bool useCompression /*= false*/ )
{
  LOG_TRACE("vtkCalibrationController::SaveTrackedFrameListToMetafile"); 
  if ( this->TrackedFrameListContainer[dataType]->SaveToSequenceMetafile(outputFolder, sequenceMetafileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    return PLUS_FAIL;
  }

  this->TrackedFrameListContainer[dataType]->Clear(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::AddVtkImageData(vtkImageData* frame, vtkMatrix4x4* trackingTransform, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - vtkImage"); 
	ImageType::Pointer exportedFrame = ImageType::New();
    if ( UsImageConverterCommon::ConvertVtkImageToItkImage(frame, exportedFrame) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to convert vtk image to itk image!"); 
        return PLUS_FAIL; 
    }
	return this->AddItkImageData(exportedFrame, trackingTransform, dataType); 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::AddItkImageData(const ImageType::Pointer& frame, vtkMatrix4x4* trackingTransform, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - itkImage"); 
	TrackedFrame trackedFrame; 
	this->CreateTrackedFrame(frame, trackingTransform, dataType, trackedFrame); 
	return this->AddTrackedFrameData(&trackedFrame, dataType); 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - TrackedFrame"); 
	try
	{
		// Check frame size before segmentation 
		int * frameSize = trackedFrame->GetFrameSize(); 
		if ( this->ImageWidthInPixels != frameSize[0] || this->ImageHeightInPixels != frameSize[1] )
		{
			LOG_ERROR("Unable to add frame to calibrator! Frame size mismatch: actual (" 
				<< frameSize[0] << "x" << frameSize[1] << ") expected (" 
				<< this->ImageWidthInPixels << "x" << this->ImageHeightInPixels << ")"); 
			return PLUS_FAIL; 
		}

		// Check to see if the segmentation has returned the targets
		SegmentationResults segResults; 
        if ( this->SegmentImage(trackedFrame->ImageData, segResults) != PLUS_SUCCESS )
        {
            LOG_WARNING("Undefined error occured during frame segmentation!"); 
            return PLUS_FAIL; 
        }

		// Add frame to the container 
		int trackedFramePosition(-1); 
		if ( this->EnableErroneouslySegmentedDataSaving )
		{
			// Save the erroneously segmented frames too 
			trackedFramePosition = this->TrackedFrameListContainer[dataType]->AddTrackedFrame(trackedFrame); 
		}
		else if (segResults.GetDotsFound() )
		{
			// Segmentation was successful
			trackedFramePosition = this->TrackedFrameListContainer[dataType]->AddTrackedFrame(trackedFrame); 
		}

		// Draw segmentation results to frame if needed
		if ( segResults.GetDotsFound() && ( this->EnableSegmentationAnalysis || this->CalibrationMode == OFFLINE) )
		{
			// Draw segmentation result to image
			this->GetSegmenter()->drawResults( trackedFrame->ImageData->GetBufferPointer() );
		} 

		if( !segResults.GetDotsFound() )
		{
			LOG_DEBUG("The segmentation cannot locate any meaningful targets, the image was ignored!!!"); 
			return PLUS_FAIL; 
		}

		// Add the segmentation result to the SegmentedFrameContainer
		SegmentedFrame segmentedFrame; 
		segmentedFrame.SegResults = segResults; 
		segmentedFrame.TrackedFrameInfo = new TrackedFrame(*this->TrackedFrameListContainer[dataType]->GetTrackedFrame(trackedFramePosition)); 
		segmentedFrame.DataType = dataType; 
		this->SegmentedFrameContainer.push_back(segmentedFrame); 

		this->RealtimeImageDataInfoContainer[dataType].NumberOfSegmentedImages++; 

		// Notify the caller when a segmentation is done
		if (this->SegmentationProgressCallbackFunction != NULL)
		{
			// Notify the caller 
			(*SegmentationProgressCallbackFunction)(dataType);
		}

		return PLUS_SUCCESS; 
	}
	catch(...)
	{
		LOG_ERROR("AddData: Failed to add tracked data!");  
		return PLUS_FAIL;
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::SegmentImage(vtkImageData * imageData, SegmentationResults& segResult )
{
	LOG_TRACE("vtkCalibrationController::SegmentImage - vtkImage"); 
	ImageType::Pointer frame = ImageType::New();
    if ( UsImageConverterCommon::ConvertVtkImageToItkImage(imageData, frame) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to convert vtk image to itk image!"); 
        return PLUS_FAIL; 

    }
	return this->SegmentImage(frame, segResult);
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::SegmentImage(const ImageType::Pointer& imageData, SegmentationResults& segResult)
{
	LOG_TRACE("vtkCalibrationController::SegmentImage - itkImage"); 
	try
	{
		if ( ! this->GetInitialized() )
		{
			this->Initialize(); 
		}

		// Send the image to the Segmentation component for segmentation
		this->GetSegmenter()->segment( imageData->GetBufferPointer(), *this->GetSegParameters());	
		this->GetSegmenter()->GetSegmentationResults(segResult); 
		return PLUS_SUCCESS;
	}
	catch(...)
	{
		LOG_ERROR("SegmentImage: The segmentation has failed for due to UNKNOWN exception thrown, the image was ignored!!!"); 
		return PLUS_FAIL; 
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::AddFrameToRenderer(const ImageType::Pointer& frame)
{
	LOG_TRACE("vtkCalibrationController::AddFrameToRenderer"); 
	if ( ! this->GetEnableVisualization() ) 
	{
		// We don't want to render anything
		return PLUS_SUCCESS; 
	}

    if ( frame.IsNull() )
    {
        LOG_ERROR("Failed to add frame to the renderer - frame is NULL!"); 
        return PLUS_FAIL; 
    }

	// create an importer to read the data back in
  vtkSmartPointer<vtkImageData> vtkFrame = vtkSmartPointer<vtkImageData>::New(); 
  if ( UsImageConverterCommon::ConvertItkImageToVtkImage(frame, vtkFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to convert itk image to vtk image!"); 
    return PLUS_FAIL;
  }

  return this->AddFrameToRenderer(vtkFrame); 
}


//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::AddFrameToRenderer(vtkImageData* frame)
{
	LOG_TRACE("vtkCalibrationController::AddFrameToRenderer"); 
	if ( ! this->GetEnableVisualization() || this->VisualizationComponent == NULL ) 
	{
		// We don't want to render anything
		return PLUS_SUCCESS; 
	}

	return this->GetVisualizationComponent()->AddFrameToRealtimeRenderer(frame); 
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ConvertVtkMatrixToVnlMatrix(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix )
{
	LOG_TRACE("vtkCalibrationController::ConvertVtkMatrixToVnlMatrix"); 
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			outVnlMatrix.put(row,column, inVtkMatrix->GetElement(row,column)); 
		}
	}
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ConvertVnlMatrixToVtkMatrix(vnl_matrix<double>& inVnlMatrix, vtkMatrix4x4* outVtkMatrix )
{
	LOG_TRACE("vtkCalibrationController::ConvertVnlMatrixToVtkMatrix"); 
	outVtkMatrix->Identity(); 

	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			outVtkMatrix->SetElement(row,column, inVnlMatrix.get(row, column) ); 
		}
	}
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ConvertVtkMatrixToVnlMatrixInMeter(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix )
{
	LOG_TRACE("vtkCalibrationController::ConvertVtkMatrixToVnlMatrixInMeter"); 
	ConvertVtkMatrixToVnlMatrix(inVtkMatrix, outVnlMatrix); 

	// Option: convert the translation to meters
	const double TxInM = 0.001 * outVnlMatrix.get(0,3);
	const double TyInM = 0.001 * outVnlMatrix.get(1,3);
	const double TzInM = 0.001 * outVnlMatrix.get(2,3);

	const double translationInMeters[] = {TxInM, TyInM, TzInM, 1};
	vnl_vector<double> translationInMetersVector(4);
	translationInMetersVector.set( translationInMeters );

	outVnlMatrix.set_column(3, translationInMetersVector);
}

//----------------------------------------------------------------------------
void  vtkCalibrationController::CreateTrackedFrame(const ImageType::Pointer& imageData, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType, TrackedFrame& trackedFrame)
{
	LOG_TRACE("vtkCalibrationController::CreateTrackedFrame - with optical readings"); 
	trackedFrame.Timestamp = 0; // TODO: current time or 0? 
	trackedFrame.DefaultFrameTransformName = "None"; 

	std::ostringstream strProbePosition;
	strProbePosition << probePosition; 
	trackedFrame.SetCustomFrameField("ProbePosition", strProbePosition.str()); 

	std::ostringstream strProbeRotation;
	strProbeRotation << probeRotation; 
	trackedFrame.SetCustomFrameField("ProbeRotation", strProbeRotation.str()); 

	std::ostringstream strTemplatePosition;
	strTemplatePosition << templatePosition; 
	trackedFrame.SetCustomFrameField("TemplatePosition", strTemplatePosition.str()); 

	trackedFrame.ImageData = imageData;
}

//----------------------------------------------------------------------------
void  vtkCalibrationController::CreateTrackedFrame(const ImageType::Pointer& imageData, vtkMatrix4x4* transform, IMAGE_DATA_TYPE dataType, TrackedFrame& trackedFrame)
{
	LOG_TRACE("vtkCalibrationController::CreateTrackedFrame - with transform"); 
	trackedFrame.Timestamp = 0; // TODO: current time or 0?
	trackedFrame.DefaultFrameTransformName = "ToolToTrackerTransform"; 

	std::ostringstream strToolToTracker; 
	double dToolToTracker[ 16 ];
	vtkMatrix4x4::DeepCopy( dToolToTracker, transform );
	for ( int i = 0; i < 16; ++ i )
	{
		strToolToTracker << dToolToTracker[ i ] << " ";
	}

	trackedFrame.SetCustomFrameField(trackedFrame.DefaultFrameTransformName, strToolToTracker.str()); 

	trackedFrame.ImageData = imageData;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadConfiguration( const char* configFileNameWithPath )
{
	LOG_TRACE("vtkCalibrationController::ReadConfiguration"); 
	this->SetConfigurationFileName(configFileNameWithPath); 

	vtkSmartPointer<vtkXMLDataElement> calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName()); 
	this->ReadConfiguration(calibrationController); 

	return PLUS_SUCCESS;
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

	// Calibration controller specifications
	//********************************************************************
	vtkXMLDataElement* calibrationController = configData->FindNestedElementWithName("CalibrationController"); 
	this->ReadCalibrationControllerConfiguration(calibrationController); 

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadCalibrationControllerConfiguration( vtkXMLDataElement* calibrationController )
{
	LOG_TRACE("vtkCalibrationController::ReadCalibrationControllerConfiguration"); 
	if ( calibrationController == NULL) 
	{
		LOG_WARNING("Unable to read the CalibrationController XML data element!"); 
		return PLUS_FAIL; 
	}

	// Set the log level 
	const char* logLevel = calibrationController->GetAttribute("LogLevel"); 
	if ( logLevel!= NULL && STRCASECMP( "ERROR", logLevel ) == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_ERROR); 
	}
	else if ( logLevel!= NULL && STRCASECMP( "WARNING", logLevel ) == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_WARNING); 
	}
	else if ( logLevel!= NULL && STRCASECMP( "INFO", logLevel ) == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_INFO); 
	}
	else if ( logLevel!= NULL && STRCASECMP( "DEBUG", logLevel ) == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_DEBUG); 
	}

	// Path to output calibration results
	const char* outputPath = calibrationController->GetAttribute("OutputPath"); 
	if ( outputPath != NULL) 
	{
		std::string fullOutputPath = vtksys::SystemTools::CollapseFullPath(outputPath, this->ProgramFolderPath); 
		vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
		if ( dir->Open(fullOutputPath.c_str()) == 0 ) 
		{	
			dir->MakeDirectory(fullOutputPath.c_str()); 
		}
		this->SetOutputPath(fullOutputPath.c_str()); 
	}
	else
	{
		// Set to the current working directory
		this->SetOutputPath(vtksys::SystemTools::GetCurrentWorkingDirectory().c_str()); 
	}

	// Enable/disable the tracked sequence data saving to metafile
	const char* enableTrackedSequenceDataSaving = calibrationController->GetAttribute("EnableTrackedSequenceDataSaving"); 
	if ( enableTrackedSequenceDataSaving != NULL &&  STRCASECMP( "TRUE", enableTrackedSequenceDataSaving ) == 0) 
	{
		this->EnableTrackedSequenceDataSavingOn(); 
	}
	else /* FALSE */
	{
		this->EnableTrackedSequenceDataSavingOff(); 
	}

	// Enable/disable the the erroneously segmented data saving to metafile
	const char* enableErroneouslySegmentedDataSaving = calibrationController->GetAttribute("EnableErroneouslySegmentedDataSaving"); 
	if ( enableErroneouslySegmentedDataSaving != NULL &&  STRCASECMP( "TRUE", enableErroneouslySegmentedDataSaving ) == 0) 
	{
		this->EnableErroneouslySegmentedDataSavingOn(); 
	}
	else /* FALSE */
	{
		this->EnableErroneouslySegmentedDataSavingOff(); 
	}

	// To enable/disable the segmentation analysis
	const char* enableSegmentationAnalysis = calibrationController->GetAttribute("EnableSegmentationAnalysis"); 
	if ( enableSegmentationAnalysis != NULL &&  STRCASECMP( "TRUE", enableSegmentationAnalysis ) == 0 ) 
	{
		this->EnableSegmentationAnalysisOn(); 
	}
	else
	{
		this->EnableSegmentationAnalysisOff(); 
	}

	// To enable/disable the visualization component
	const char* enableVisualization = calibrationController->GetAttribute("EnableVisualization"); 
	if ( enableVisualization != NULL &&  STRCASECMP( "TRUE", enableVisualization ) == 0 ) 
	{
		this->EnableVisualizationOn(); 
	}
	else
	{
		this->EnableVisualizationOff(); 
	}

	// Set the calibration mode
	const char* calibrationMode = calibrationController->GetAttribute("CalibrationMode"); 
	if ( calibrationMode != NULL && STRCASECMP( "REALTIME", calibrationMode ) == 0 )
	{
		this->SetCalibrationMode(REALTIME); 
	}
	else /* OFFLINE */
	{
		this->SetCalibrationMode(OFFLINE); 
	}

	// RealtimeCalibration specifications
	//********************************************************************
	vtkXMLDataElement* realtimeCalibration = calibrationController->FindNestedElementWithName("RealtimeCalibration"); 
	this->ReadRealtimeCalibrationConfiguration(realtimeCalibration); 

	// SegmentationParameters specifications
	//********************************************************************
	vtkXMLDataElement* segmentationParameters = calibrationController->FindNestedElementWithName("SegmentationParameters"); 
	this->ReadSegmentationParametersConfiguration(segmentationParameters); 

	// Phantom definition
	//********************************************************************
	this->ReadPhantomDefinition();

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadRealtimeCalibrationConfiguration( vtkXMLDataElement* realtimeCalibration )
{
	LOG_TRACE("vtkCalibrationController::ReadRealtimeCalibrationConfiguration"); 
	if ( realtimeCalibration == NULL) 
	{
		LOG_ERROR("Unable to read the RealtimeCalibration XML data element!"); 
		return PLUS_FAIL; 
	}

	// TemplateTranslationData data set specifications
	//********************************************************************
	vtkXMLDataElement* templateTranslationData = realtimeCalibration->FindNestedElementWithName("TemplateTranslationData"); 
	if ( templateTranslationData != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(TEMPLATE_TRANSLATION); 
		int numberOfImagesToUse = -1;
		if ( templateTranslationData->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = templateTranslationData->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(TEMPLATE_TRANSLATION, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find TemplateTranslationData XML data element, default 100 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(TEMPLATE_TRANSLATION); 
		imageDataInfo.NumberOfImagesToAcquire = 100;
	}

	// ProbeTranslationData data set specifications
	//********************************************************************
	vtkXMLDataElement* probeTranslationData = realtimeCalibration->FindNestedElementWithName("ProbeTranslationData"); 
	if ( probeTranslationData != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(PROBE_TRANSLATION); 
		int numberOfImagesToUse = -1;
		if ( probeTranslationData->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = probeTranslationData->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(PROBE_TRANSLATION, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find ProbeTranslationData XML data element, default 200 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(PROBE_TRANSLATION); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
	}

	// ProbeRotationData data set specifications
	//********************************************************************
	vtkXMLDataElement* probeRotationData = realtimeCalibration->FindNestedElementWithName("ProbeRotationData"); 
	if ( probeRotationData != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(PROBE_ROTATION); 
		int numberOfImagesToUse = -1;
		if ( probeRotationData->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = probeRotationData->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(PROBE_ROTATION, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find ProbeRotationData XML data element, default 500 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(PROBE_ROTATION); 
		imageDataInfo.NumberOfImagesToAcquire = 500;
	}

	// RandomStepperMotionData1 data set specifications
	//********************************************************************
	vtkXMLDataElement* randomStepperMotionData1 = realtimeCalibration->FindNestedElementWithName("RandomStepperMotionData1"); 
	if ( randomStepperMotionData1 != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_1); 
		int numberOfImagesToUse = -1;
		if ( randomStepperMotionData1->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = randomStepperMotionData1->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_1, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find RandomStepperMotionData1 XML data element, default 200 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_1); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
	}

	// RandomStepperMotionData2 data set specifications
	//********************************************************************
	vtkXMLDataElement* randomStepperMotionData2 = realtimeCalibration->FindNestedElementWithName("RandomStepperMotionData2"); 
	if ( randomStepperMotionData2 != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_2); 
		int numberOfImagesToUse = -1;
		if ( randomStepperMotionData2->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = randomStepperMotionData2->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_2, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find RandomStepperMotionData2 XML data element, default 100 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_2); 
		imageDataInfo.NumberOfImagesToAcquire = 100; 
	}

	// FreehandMotionData1 data set specifications
	//********************************************************************
	vtkXMLDataElement* freehandMotionData1 = realtimeCalibration->FindNestedElementWithName("FreehandMotionData1"); 
	if ( freehandMotionData1 != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1); 
		int numberOfImagesToUse = -1;
		if ( freehandMotionData1->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = freehandMotionData1->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(FREEHAND_MOTION_1, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find FreehandMotionData1 XML data element, default 200 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
	}

	// FreehandMotionData2 data set specifications
	//********************************************************************
	vtkXMLDataElement* freehandMotionData2 = realtimeCalibration->FindNestedElementWithName("FreehandMotionData2"); 
	if ( freehandMotionData2 != NULL) 
	{
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2); 
		int numberOfImagesToUse = -1;
		if ( freehandMotionData2->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = freehandMotionData2->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetRealtimeImageDataInfo(FREEHAND_MOTION_2, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find FreehandMotionData2 XML data element, default 100 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2); 
		imageDataInfo.NumberOfImagesToAcquire = 100; 
	}

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadSegmentationParametersConfiguration( vtkXMLDataElement* segmentationParameters )
{
	LOG_TRACE("vtkCalibrationController::ReadSegmentationParametersConfiguration"); 
	if ( segmentationParameters == NULL) 
	{
		LOG_WARNING("Unable to read the SegmentationParameters XML data element!"); 
		return PLUS_FAIL; 
	}

	if ( this->GetSegParameters() == NULL )
	{
		this->SegParameters = new SegmentationParameters(); 
	}

	// The input image dimensions (in pixels)
	int* frameSize = new int[2]; 
	if ( segmentationParameters->GetVectorAttribute("FrameSize", 2, frameSize) ) 
	{
		this->SetImageWidthInPixels(frameSize[0]); 
		this->SetImageHeightInPixels(frameSize[1]);
	}
	delete [] frameSize; 

	double scalingEstimation(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ScalingEstimation", scalingEstimation) )
	{
		this->GetSegParameters()->SetScalingEstimation(scalingEstimation); 
	}

	double morphologicalOpeningCircleRadiusMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MorphologicalOpeningCircleRadiusMm", morphologicalOpeningCircleRadiusMm) )
	{
		this->GetSegParameters()->SetMorphologicalOpeningCircleRadiusMm(morphologicalOpeningCircleRadiusMm); 
	}

	double morphologicalOpeningBarSizeMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MorphologicalOpeningBarSizeMm", morphologicalOpeningBarSizeMm) )
	{
		this->GetSegParameters()->SetMorphologicalOpeningBarSizeMm(morphologicalOpeningBarSizeMm); 
	}

	// Segmentation search region X direction
	int* searchRegionX = new int[2]; 
	if ( segmentationParameters->GetVectorAttribute("SearchRegionX", 2, searchRegionX) )
	{
		this->SetSearchStartAtX(searchRegionX[0]); 
		this->SetSearchDimensionX(searchRegionX[1]);
	}
	delete [] searchRegionX; 

	// Segmentation search region Y direction
	int* searchRegionY = new int[2]; 
	if ( segmentationParameters->GetVectorAttribute("SearchRegionY", 2, searchRegionY) )
	{
		this->SetSearchStartAtY(searchRegionY[0]); 
		this->SetSearchDimensionY(searchRegionY[1]);
	}
	delete [] searchRegionY; 

	double thresholdImageTop(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ThresholdImageTop", thresholdImageTop) )
	{
		this->GetSegParameters()->SetThresholdImageTop(thresholdImageTop); 
	}

	double thresholdImageBottom(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ThresholdImageBottom", thresholdImageBottom) )
	{
		this->GetSegParameters()->SetThresholdImageBottom(thresholdImageBottom); 
	}

	int useOriginalImageIntensityForDotIntensityScore(0); 
	if ( segmentationParameters->GetScalarAttribute("UseOriginalImageIntensityForDotIntensityScore", useOriginalImageIntensityForDotIntensityScore) )
	{
		this->GetSegParameters()->SetUseOriginalImageIntensityForDotIntensityScore((useOriginalImageIntensityForDotIntensityScore?true:false)); 
	}
	
	//if the tolerance parameters are computed automatically
	int computeSegmentationParametersFromPhantomDefinition(0);
	if(segmentationParameters->GetScalarAttribute("ComputeSegmentationParametersFromPhantomDefinition", computeSegmentationParametersFromPhantomDefinition)
		&& computeSegmentationParametersFromPhantomDefinition!=0 )
	{
		double scalingEstimation(0.0);
		if ( segmentationParameters->GetScalarAttribute("ScalingEstimation", scalingEstimation) )
		{
			this->GetSegParameters()->SetScalingEstimation(scalingEstimation); 
		}

		double* imageScalingTolerancePercent = new double[4];
		if ( segmentationParameters->GetVectorAttribute("ImageScalingTolerancePercent", 4, imageScalingTolerancePercent) )
		{
			for( int i = 0; i<4 ; i++)
			{
				this->GetSegParameters()->SetImageScalingTolerancePercent(i, imageScalingTolerancePercent[i]);
			}
		}
		delete [] imageScalingTolerancePercent;

        double* imageNormalVectorInPhantomFrameEstimation = new double[3];
		if ( segmentationParameters->GetVectorAttribute("ImageNormalVectorInPhantomFrameEstimation", 3, imageNormalVectorInPhantomFrameEstimation) )
		{
			this->GetSegParameters()->SetImageNormalVectorInPhantomFrameEstimation(0, imageNormalVectorInPhantomFrameEstimation[0]);
			this->GetSegParameters()->SetImageNormalVectorInPhantomFrameEstimation(1, imageNormalVectorInPhantomFrameEstimation[1]);
			this->GetSegParameters()->SetImageNormalVectorInPhantomFrameEstimation(2, imageNormalVectorInPhantomFrameEstimation[2]);
		}
		delete [] imageNormalVectorInPhantomFrameEstimation;

        double* imageNormalVectorInPhantomFrameMaximumRotationAngleDeg = new double[6];
		if ( segmentationParameters->GetVectorAttribute("ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg", 6, imageNormalVectorInPhantomFrameMaximumRotationAngleDeg) )
		{
			for( int i = 0; i<6 ; i++)
			{
				this->GetSegParameters()->SetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg(i, imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[i]);
			}
		}
		delete [] imageNormalVectorInPhantomFrameMaximumRotationAngleDeg;

		//Compute the tolerances parameters automatically
		this->GetSegParameters()->ComputeParameters();
	}
	else//if the tolerances parameters are given by the configuration file
	{
		double maxLineLengthErrorPercent(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLineLengthErrorPercent", maxLineLengthErrorPercent) )
		{
			this->GetSegParameters()->SetMaxLineLengthErrorPercent(maxLineLengthErrorPercent); 
		}

		double maxLinePairDistanceErrorPercent(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLinePairDistanceErrorPercent", maxLinePairDistanceErrorPercent) )
		{
			this->GetSegParameters()->SetMaxLinePairDistanceErrorPercent(maxLinePairDistanceErrorPercent); 
		}

		double findLines3PtDist(0.0); 
		if ( segmentationParameters->GetScalarAttribute("FindLines3PtDist", findLines3PtDist) )
		{
			this->GetSegParameters()->SetFindLines3PtDist(findLines3PtDist); 
		}

		double maxLineErrorMm(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLineErrorMm", maxLineErrorMm) )
		{
			this->GetSegParameters()->SetMaxLineErrorMm(maxLineErrorMm); 
		}

		double maxAngleDifferenceDegrees(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxAngleDifferenceDegrees", maxAngleDifferenceDegrees) )
		{
			this->GetSegParameters()->SetMaxAngleDiff(maxAngleDifferenceDegrees * M_PI / 180.0); 
		}

		double minThetaDegrees(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MinThetaDegrees", minThetaDegrees) )
		{
			this->GetSegParameters()->SetMinTheta(minThetaDegrees * M_PI / 180.0); 
		}

		double maxThetaDegrees(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxThetaDegrees", maxThetaDegrees) )
		{
			this->GetSegParameters()->SetMaxTheta(maxThetaDegrees * M_PI / 180.0); 
		}
	}

	

	/* Temporarily removed (also from config file) - these are the parameters for the U shaped ablation phantom
	double maxUangleDiffInRad(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxUangleDiffInRad", maxUangleDiffInRad) )
	{
		this->GetSegParameters()->mMaxUangleDiff = maxUangleDiffInRad; 
	}

	double maxUsideLineDiff(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxUsideLineDiff", maxUsideLineDiff) )
	{
		this->GetSegParameters()->mMaxUsideLineDiff = maxUsideLineDiff; 
	}

	double minUsideLineLength(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MinUsideLineLength", minUsideLineLength) )
	{
		this->GetSegParameters()->mMinUsideLineLength = minUsideLineLength; 
	}

	double maxUsideLineLength(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxUsideLineLength", maxUsideLineLength) )
	{
		this->GetSegParameters()->mMaxUsideLineLength = maxUsideLineLength; 
	}
	*/

	// Search for the phantom definition file found in the configuration file
	const char* phantomDefinitionFile =  segmentationParameters->GetAttribute("PhantomDefinition");
	if (phantomDefinitionFile != NULL) {
		std::string searchResult;
		if (STRCASECMP(vtkFileFinder::GetInstance()->GetConfigurationDirectory(), "") == 0) {
			std::string configurationDirectory = vtksys::SystemTools::GetFilenamePath(this->ConfigurationFileName);
			searchResult = vtkFileFinder::GetFirstFileFoundInParentOfDirectory(phantomDefinitionFile, configurationDirectory.c_str());
		} else {
			searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(phantomDefinitionFile);
		}
		if (STRCASECMP("", searchResult.c_str()) == 0) {
			LOG_WARNING("Phantom model file is not found with name: " << phantomDefinitionFile);
		}

		if ( vtksys::SystemTools::FileExists(searchResult.c_str(), true) ) {
			this->SetPhantomDefinitionFileName(searchResult.c_str());
		}
	}

	this->GetSegParameters()->UpdateParameters(); 

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::ReadPhantomDefinition()
{
	LOG_TRACE("vtkCalibrationController::ReadPhantomDefinition");

	if ( this->PhantomDefinitionFileName != NULL )
	{
		std::vector<NWire> tempNWires = this->GetSegParameters()->GetNWires();
		vtkSmartPointer<vtkXMLDataElement> phantomDefinition = vtkXMLUtilities::ReadElementFromFile(this->PhantomDefinitionFileName);

		if (phantomDefinition == NULL) {
			LOG_ERROR("Unable to read the phantom definition file: " << this->PhantomDefinitionFileName); 
			return PLUS_FAIL;
		}

		// Verify XML type
		if (STRCASECMP("PhantomDefinition", phantomDefinition->GetName()) != NULL) {
			LOG_ERROR(this->PhantomDefinitionFileName << " is not a phantom definition file!");
		}

		// Load type
		vtkXMLDataElement* description = phantomDefinition->FindNestedElementWithName("Description"); 
		if (description == NULL) {
			LOG_ERROR("Phantom description not found!");
			return PLUS_FAIL;
		} else {
			const char* type =  description->GetAttribute("Type"); 
			if ( type != NULL ) {
				if (STRCASECMP("Double-N", type) == 0) {
					this->GetSegParameters()->SetFiducialGeometry(SegmentationParameters::CALIBRATION_PHANTOM_6_POINT);
				} else if (STRCASECMP("U-Shaped-N", type) == 0) {
					this->GetSegParameters()->SetFiducialGeometry(SegmentationParameters::TAB2_5_POINT);
				}
			} else {
				LOG_ERROR("Phantom type not found!");
			}
		}

		// Load model information
		vtkXMLDataElement* model = phantomDefinition->FindNestedElementWithName("Model"); 
		if (model == NULL) {
			LOG_WARNING("Phantom model information not found - no model displayed");
		} else {
			const char* file = model->GetAttribute("File");
			if (file) {
				if ((strstr(file, ".stl") != NULL) || ((strstr(file, ".STL") != NULL))) { // If filename contains ".stl" or ".STL" then it is valid, else we do not search for it (and do not return with warning either, because some time we just do not fill that field because we do not have the file)
					std::string searchResult;
					if (STRCASECMP(vtkFileFinder::GetInstance()->GetConfigurationDirectory(), "") == 0) {
						std::string configurationDirectory = vtksys::SystemTools::GetFilenamePath(this->ConfigurationFileName);
						searchResult = vtkFileFinder::GetFirstFileFoundInParentOfDirectory(file, configurationDirectory.c_str());
					} else {
						searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(file);
					}
					if (STRCASECMP("", searchResult.c_str()) != 0) {
						this->SetPhantomModelFileName(searchResult.c_str());
					}
				} else {
					LOG_INFO("'" << file << "' does not appear to be a valid phantom model file name, so it was not searched for");
				}
			}

			// ModelToPhantomTransform - Transforming input model for proper visualization
			double* modelToPhantomTransformVector = new double[16]; 
			if (model->GetVectorAttribute("ModelToPhantomTransform", 16, modelToPhantomTransformVector)) {
				vtkSmartPointer<vtkMatrix4x4> modelToPhantomTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
				modelToPhantomTransformMatrix->Identity();
				modelToPhantomTransformMatrix->DeepCopy(modelToPhantomTransformVector);

				vtkSmartPointer<vtkTransform> modelToPhantomTransform = vtkSmartPointer<vtkTransform>::New();
				modelToPhantomTransform->SetMatrix(modelToPhantomTransformMatrix);
				this->SetModelToPhantomTransform(modelToPhantomTransform);
			}
			delete[] modelToPhantomTransformVector;
		}

		// Load geometry
		vtkXMLDataElement* geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
		if (geometry == NULL) {
			LOG_ERROR("Phantom geometry information not found!");
			return PLUS_FAIL;
		} else {

			tempNWires.clear();

			// Finding of NWires and extracting the endpoints
			int numberOfGeometryChildren = geometry->GetNumberOfNestedElements();
			for (int i=0; i<numberOfGeometryChildren; ++i) {
				vtkSmartPointer<vtkXMLDataElement> nWireElement = geometry->GetNestedElement(i);

				if ((nWireElement == NULL) || (STRCASECMP("NWire", nWireElement->GetName()))) {
					continue;
				}

				NWire nWire;

				int numberOfWires = nWireElement->GetNumberOfNestedElements();

				if (numberOfWires != 3) {
					LOG_WARNING("NWire contains unexpected number of wires - skipped");
					continue;
				}

				for (int j=0; j<numberOfWires; ++j) {
					vtkSmartPointer<vtkXMLDataElement> wireElement = nWireElement->GetNestedElement(j);

					if (wireElement == NULL) {
						LOG_WARNING("Invalid Wire description in NWire - skipped");
						break;
					}

					Wire wire;

					int wireId = -1;
					if ( wireElement->GetScalarAttribute("Id", wireId) ) 
					{
						wire.id = wireId; 
					}
					else
					{
						LOG_WARNING("Wire id not found - skipped");
						continue;
					}

					const char* wireName =  wireElement->GetAttribute("Name"); 
					if ( wireName != NULL )
					{
						strcpy_s(wire.name, 128, wireName);
					}
					if (! wireElement->GetVectorAttribute("EndPointFront", 3, wire.endPointFront)) {
						LOG_WARNING("Wrong wire end point detected - skipped");
						continue;
					}
					if (! wireElement->GetVectorAttribute("EndPointBack", 3, wire.endPointBack)) {
						LOG_WARNING("Wrong wire end point detected - skipped");
						continue;
					}

					nWire.wires[j] = wire;
				}

				tempNWires.push_back(nWire);
			}
		}

		this->GetSegParameters()->SetNWires(tempNWires);
	} else {
		LOG_ERROR("Phantom definition file name is not set!"); 
		return PLUS_FAIL;
	}

	//TODO Load registration

	// Compute error boundaries based on error percents and the NWire definition (supposing that the NWire is regular - parallel sides)
	// Line length of an N-wire: the maximum distance between its wires' front endpoints
	double maxLineLengthSquared = -1.0;
	double minLineLengthSquared = FLT_MAX;
	std::vector<NWire> nWires = this->GetSegParameters()->GetNWires();

	for (std::vector<NWire>::iterator it = nWires.begin(); it != nWires.end(); ++it) {
		Wire wire0 = it->wires[0];
		Wire wire1 = it->wires[1];
		Wire wire2 = it->wires[2];

		double distance01Squared = vtkMath::Distance2BetweenPoints(wire0.endPointFront, wire1.endPointFront);
		double distance02Squared = vtkMath::Distance2BetweenPoints(wire0.endPointFront, wire2.endPointFront);
		double distance12Squared = vtkMath::Distance2BetweenPoints(wire1.endPointFront, wire2.endPointFront);
		double lineLengthSquared = std::max( std::max(distance01Squared, distance02Squared), distance12Squared );

		if (maxLineLengthSquared < lineLengthSquared) {
			maxLineLengthSquared = lineLengthSquared;
		}
		if (minLineLengthSquared > lineLengthSquared) {
			minLineLengthSquared = lineLengthSquared;
		}
	}

	this->GetSegParameters()->SetMaxLineLenMm(sqrt(maxLineLengthSquared) * (1.0 + (this->GetSegParameters()->GetMaxLineLengthErrorPercent() / 100.0)));
	this->GetSegParameters()->SetMinLineLenMm(sqrt(minLineLengthSquared) * (1.0 - (this->GetSegParameters()->GetMaxLineLengthErrorPercent() / 100.0)));
	LOG_DEBUG("Line length - computed min: " << sqrt(minLineLengthSquared) << " , max: " << sqrt(maxLineLengthSquared) << ";  allowed min: " << this->GetSegParameters()->GetMinLineLenMm() << ", max: " << this->GetSegParameters()->GetMaxLineLenMm());

	// Distance between lines (= distance between planes of the N-wires)
	double maxNPlaneDistance = -1.0;
	double minNPlaneDistance = FLT_MAX;
	int numOfNWires = nWires.size();
	double epsilon = 0.001;

	// Compute normal of each NWire and evaluate the other wire endpoints if they are on the computed plane
	std::vector<vtkSmartPointer<vtkPlane>> planes;
	for (int i=0; i<numOfNWires; ++i) {
		double normal[3];
		vtkTriangle::ComputeNormal(nWires.at(i).wires[0].endPointFront, nWires.at(i).wires[0].endPointBack, nWires.at(i).wires[2].endPointFront, normal);

		vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
		plane->SetNormal(normal);
		plane->SetOrigin(nWires.at(i).wires[0].endPointFront);
		planes.push_back(plane);

		double distance1F = plane->DistanceToPlane(nWires.at(i).wires[1].endPointFront);
		double distance1B = plane->DistanceToPlane(nWires.at(i).wires[1].endPointBack);
		double distance2B = plane->DistanceToPlane(nWires.at(i).wires[2].endPointBack);

		if (distance1F > epsilon || distance1B > epsilon || distance2B > epsilon) {
			LOG_ERROR("NWire number " << i << " is invalid: the endpoints are not on the same plane");
		}
	}

	// Compute distances between each NWire pairs and determine the smallest and the largest distance
	for (int i=numOfNWires-1; i>0; --i) {
		for (int j=i-1; j>=0; --j) {
			double distance = planes.at(i)->DistanceToPlane(planes.at(j)->GetOrigin());

			if (maxNPlaneDistance < distance) {
				maxNPlaneDistance = distance;
			}
			if (minNPlaneDistance > distance) {
				minNPlaneDistance = distance;
			}
		}
	}

	this->GetSegParameters()->SetMaxLinePairDistMm(maxNPlaneDistance * (1.0 + (this->GetSegParameters()->GetMaxLinePairDistanceErrorPercent() / 100.0)));
	this->GetSegParameters()->SetMinLinePairDistMm(minNPlaneDistance * (1.0 - (this->GetSegParameters()->GetMaxLinePairDistanceErrorPercent() / 100.0)));
	LOG_DEBUG("Line pair distance - computed min: " << minNPlaneDistance << " , max: " << maxNPlaneDistance << ";  allowed min: " << this->GetSegParameters()->GetMinLinePairDistMm() << ", max: " << this->GetSegParameters()->GetMaxLinePairDistMm());

	return PLUS_SUCCESS;
}

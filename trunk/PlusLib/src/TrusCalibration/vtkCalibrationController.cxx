#include "PlusConfigure.h"
#include "vtkCalibrationController.h"

#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkMatrix4x4.h"
#include "vtkDirectory.h"
#include "vtkImageImport.h"
#include "vtkImageFlip.h"
#include "vtkXMLDataElement.h"

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
	this->EnablePathOverrideOn();
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
	this->DesiredOrientation = NULL;
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
void vtkCalibrationController::Initialize()
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

}

//----------------------------------------------------------------------------
vtkTrackedFrameList* vtkCalibrationController::GetTrackedFrameList( IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::GetTrackedFrameList"); 
	return this->TrackedFrameListContainer[dataType]; 
}

//----------------------------------------------------------------------------
void vtkCalibrationController::SaveTrackedFrameListToMetafile( IMAGE_DATA_TYPE dataType, const char* outputFolder, const char* sequenceMetafileName, bool useCompression /*= false*/ )
{
	LOG_TRACE("vtkCalibrationController::SaveTrackedFrameListToMetafile"); 
	this->TrackedFrameListContainer[dataType]->SaveToSequenceMetafile(outputFolder, sequenceMetafileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression); 
	this->TrackedFrameListContainer[dataType]->Clear(); 
}

//----------------------------------------------------------------------------
bool vtkCalibrationController::AddVtkImageData(vtkImageData* frame, vtkMatrix4x4* trackingTransform, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - vtkImage"); 
	ImageType::Pointer exportedFrame = ImageType::New();
	this->ExportVtkImageData(frame, exportedFrame); 

	return this->AddItkImageData(exportedFrame, trackingTransform, dataType); 
}

//----------------------------------------------------------------------------
bool vtkCalibrationController::AddItkImageData(ImageType* frame, vtkMatrix4x4* trackingTransform, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - itkImage"); 
	TrackedFrame trackedFrame; 
	this->CreateTrackedFrame(frame, trackingTransform, dataType, trackedFrame); 
	return this->AddTrackedFrameData(&trackedFrame, dataType); 
}

//----------------------------------------------------------------------------
bool vtkCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType )
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
			return false; 
		}

		// Check to see if the segmentation has returned the targets
		SegmentationResults segResults;
		if (this->DesiredOrientation != NULL) {
			segResults = this->SegmentImage(trackedFrame->GetOrientedImage(this->DesiredOrientation));
		} else {
			segResults = this->SegmentImage(trackedFrame->ImageData);
		}

		// Add frame to the container 
		int trackedFramePosition(-1); 
		if ( this->EnableErroneouslySegmentedDataSaving )
		{
			// Save the erroneously segmented frames too 
			trackedFramePosition = this->TrackedFrameListContainer[dataType]->AddTrackedFrame(trackedFrame); 
		}
		else if (segResults.m_DotsFound )
		{
			// Segmentation was successful
			trackedFramePosition = this->TrackedFrameListContainer[dataType]->AddTrackedFrame(trackedFrame); 
		}

		// Draw segmentation results to frame if needed
		if ( segResults.m_DotsFound && ( this->EnableSegmentationAnalysis || this->CalibrationMode == OFFLINE) )
		{
			// Draw segmentation result to image
			this->GetSegmenter()->drawResults( trackedFrame->ImageData->GetBufferPointer() );
		} 

		if( !segResults.m_DotsFound )
		{
			LOG_DEBUG("The segmentation cannot locate any meaningful targets, the image was ignored!!!"); 
			return false; 
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

		return true; 
	}
	catch(...)
	{
		LOG_ERROR("AddData: Failed to add tracked data!");  
		throw;
	}
}

//----------------------------------------------------------------------------
SegmentationResults vtkCalibrationController::SegmentImage(vtkImageData * imageData)
{
	LOG_TRACE("vtkCalibrationController::SegmentImage - vtkImage"); 
	ImageType::Pointer frame = ImageType::New();
	this->ExportVtkImageData(imageData, frame); 

	return this->SegmentImage(frame);
}

//----------------------------------------------------------------------------
SegmentationResults vtkCalibrationController::SegmentImage(ImageType* imageData)
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
		SegmentationResults segResults;
		this->GetSegmenter()->GetSegmentationResults(segResults); 
		return segResults;
	}
	catch(...)
	{
		LOG_ERROR("SegmentImage: The segmentation has failed for due to UNKNOWN exception thrown, the image was ignored!!!"); 
		throw;  
	}
}

//----------------------------------------------------------------------------
void vtkCalibrationController::AddFrameToRenderer(ImageType* frame)
{
	LOG_TRACE("vtkCalibrationController::AddFrameToRenderer"); 
	if ( ! this->GetEnableVisualization() ) 
	{
		// We don't want to render anything
		return; 
	}

	// create an importer to read the data back in
	vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
	importer->SetWholeExtent(0,this->GetImageWidthInPixels() - 1,0,this->GetImageHeightInPixels() - 1,0,0);
	importer->SetDataExtentToWholeExtent();
	importer->SetDataScalarTypeToUnsignedChar();
	importer->SetImportVoidPointer(frame->GetBufferPointer());
	importer->SetNumberOfScalarComponents(frame->GetNumberOfComponentsPerPixel()); 
	importer->Update();

	vtkSmartPointer<vtkImageFlip> imageFlipY = vtkSmartPointer<vtkImageFlip>::New(); 
	imageFlipY->SetInput( importer->GetOutput() ); 
	imageFlipY->SetFilteredAxis(1); 
	imageFlipY->Update(); 

	this->AddFrameToRenderer(imageFlipY->GetOutput()); 
}


//----------------------------------------------------------------------------
void vtkCalibrationController::AddFrameToRenderer(vtkImageData* frame)
{
	LOG_TRACE("vtkCalibrationController::AddFrameToRenderer"); 
	if ( ! this->GetEnableVisualization() || this->VisualizationComponent == NULL ) 
	{
		// We don't want to render anything
		return; 
	}
	this->GetVisualizationComponent()->AddFrameToRealtimeRenderer(frame); 
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
void  vtkCalibrationController::CreateTrackedFrame(ImageType* imageData, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType, TrackedFrame& trackedFrame)
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
void  vtkCalibrationController::CreateTrackedFrame(ImageType* imageData, vtkMatrix4x4* transform, IMAGE_DATA_TYPE dataType, TrackedFrame& trackedFrame)
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
void vtkCalibrationController::ExportVtkImageData(vtkImageData* imageData, ImageType* imageDataExported)
{
	LOG_TRACE("vtkCalibrationController::ExportVtkImageData"); 
	vtkSmartPointer<vtkImageFlip> imageFlipy = vtkSmartPointer<vtkImageFlip>::New(); 
	imageFlipy->SetInput(imageData); 
	imageFlipy->SetFilteredAxis(1); 
	imageFlipy->Update(); 

	vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
	imageExport->ImageLowerLeftOff();
	imageExport->SetInput(imageFlipy->GetOutput()); 
	imageExport->Update(); 

	ImageType::SizeType size = {this->GetImageWidthInPixels(), this->GetImageHeightInPixels() };
	ImageType::IndexType start = {0,0};
	ImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	imageDataExported->SetRegions(region);
	imageDataExported->Allocate();

	memcpy(imageDataExported->GetBufferPointer() , imageExport->GetPointerToData(), imageExport->GetDataMemorySize());
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ReadConfiguration( const char* configFileNameWithPath )
{
	LOG_TRACE("vtkCalibrationController::ReadConfiguration"); 
	this->SetConfigurationFileName(configFileNameWithPath); 

	vtkXMLDataElement *calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName()); 
	this->ReadConfiguration(calibrationController); 
	calibrationController->Delete(); 
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkCalibrationController::ReadConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to read configuration"); 
		exit(EXIT_FAILURE); 
	}

	// Calibration controller specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> calibrationController = configData->FindNestedElementWithName("CalibrationController"); 
	this->ReadCalibrationControllerConfiguration(calibrationController); 
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ReadCalibrationControllerConfiguration( vtkXMLDataElement* calibrationController )
{
	LOG_TRACE("vtkCalibrationController::ReadCalibrationControllerConfiguration"); 
	if ( calibrationController == NULL) 
	{
		LOG_WARNING("Unable to read the CalibrationController XML data element!"); 
		return; 
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
	vtkSmartPointer<vtkXMLDataElement> realtimeCalibration = calibrationController->FindNestedElementWithName("RealtimeCalibration"); 
	this->ReadRealtimeCalibrationConfiguration(realtimeCalibration); 

	// SegmentationParameters specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> segmentationParameters = calibrationController->FindNestedElementWithName("SegmentationParameters"); 
	this->ReadSegmentationParametersConfiguration(segmentationParameters); 

	// Phantom definition
	//********************************************************************
	this->ReadPhantomDefinition();
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ReadRealtimeCalibrationConfiguration( vtkXMLDataElement* realtimeCalibration )
{
	LOG_TRACE("vtkCalibrationController::ReadRealtimeCalibrationConfiguration"); 
	if ( realtimeCalibration == NULL) 
	{
		LOG_WARNING("Unable to read the RealtimeCalibration XML data element!"); 
		return; 
	}

	// TemplateTranslationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> templateTranslationData = realtimeCalibration->FindNestedElementWithName("TemplateTranslationData"); 
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
		LOG_WARNING("Unable to find TemplateTranslationData XML data element"); 
	}

	// ProbeTranslationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> probeTranslationData = realtimeCalibration->FindNestedElementWithName("ProbeTranslationData"); 
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
		LOG_WARNING("Unable to find ProbeTranslationData XML data element"); 
	}

	// ProbeRotationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> probeRotationData = realtimeCalibration->FindNestedElementWithName("ProbeRotationData"); 
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
		LOG_WARNING("Unable to find ProbeRotationData XML data element"); 
	}

	// RandomStepperMotionData1 data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData1 = realtimeCalibration->FindNestedElementWithName("RandomStepperMotionData1"); 
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
		LOG_WARNING("Unable to find RandomStepperMotionData1 XML data element"); 
	}

	// RandomStepperMotionData2 data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData2 = realtimeCalibration->FindNestedElementWithName("RandomStepperMotionData2"); 
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
		LOG_WARNING("Unable to find RandomStepperMotionData2 XML data element"); 
	}

	// FreehandMotionData1 data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> freehandMotionData1 = realtimeCalibration->FindNestedElementWithName("FreehandMotionData1"); 
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
		LOG_WARNING("Unable to find FreehandMotionData1 XML data element"); 
	}

	// FreehandMotionData2 data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> freehandMotionData2 = realtimeCalibration->FindNestedElementWithName("FreehandMotionData2"); 
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
		LOG_WARNING("Unable to find FreehandMotionData2 XML data element"); 
	}

}

//----------------------------------------------------------------------------
void vtkCalibrationController::ReadSegmentationParametersConfiguration( vtkXMLDataElement* segmentationParameters )
{
	LOG_TRACE("vtkCalibrationController::ReadSegmentationParametersConfiguration"); 
	if ( segmentationParameters == NULL) 
	{
		LOG_WARNING("Unable to read the SegmentationParameters XML data element!"); 
		return; 
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
		this->GetSegParameters()->mScalingEstimation = scalingEstimation; 
	}

	double morphologicalOpeningCircleRadiusMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MorphologicalOpeningCircleRadiusMm", morphologicalOpeningCircleRadiusMm) )
	{
		this->GetSegParameters()->mMorphologicalOpeningCircleRadiusMm = morphologicalOpeningCircleRadiusMm; 
	}

	double morphologicalOpeningBarSizeMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MorphologicalOpeningBarSizeMm", morphologicalOpeningBarSizeMm) )
	{
		this->GetSegParameters()->mMorphologicalOpeningBarSizeMm = morphologicalOpeningBarSizeMm; 
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
		this->GetSegParameters()->mThresholdImageTop = thresholdImageTop; 
	}

	double thresholdImageBottom(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ThresholdImageBottom", thresholdImageBottom) )
	{
		this->GetSegParameters()->mThresholdImageBottom = thresholdImageBottom; 
	}

	double maxLineLengthErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLineLengthErrorPercent", maxLineLengthErrorPercent) )
	{
		this->GetSegParameters()->mMaxLineLengthErrorPercent = maxLineLengthErrorPercent; 
	}

	double maxLinePairDistanceErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLinePairDistanceErrorPercent", maxLinePairDistanceErrorPercent) )
	{
		this->GetSegParameters()->mMaxLinePairDistanceErrorPercent = maxLinePairDistanceErrorPercent; 
	}

	double findLines3PtDist(0.0); 
	if ( segmentationParameters->GetScalarAttribute("FindLines3PtDist", findLines3PtDist) )
	{
		this->GetSegParameters()->mFindLines3PtDist = findLines3PtDist; 
	}

	double maxLineErrorMm(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLineErrorMm", maxLineErrorMm) )
	{
		this->GetSegParameters()->mMaxLineErrorMm = maxLineErrorMm; 
	}

	double maxAngleDifferenceDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxAngleDifferenceDegrees", maxAngleDifferenceDegrees) )
	{
		this->GetSegParameters()->mMaxAngleDiff = maxAngleDifferenceDegrees * M_PI / 180.0; 
	}

	double minThetaDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MinThetaDegrees", minThetaDegrees) )
	{
		this->GetSegParameters()->mMinTheta = minThetaDegrees * M_PI / 180.0; 
	}

	double maxThetaDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxThetaDegrees", maxThetaDegrees) )
	{
		this->GetSegParameters()->mMaxTheta = maxThetaDegrees * M_PI / 180.0; 
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

	int useOriginalImageIntensityForDotIntensityScore(0); 
	if ( segmentationParameters->GetScalarAttribute("UseOriginalImageIntensityForDotIntensityScore", useOriginalImageIntensityForDotIntensityScore) )
	{
		this->GetSegParameters()->mUseOriginalImageIntensityForDotIntensityScore = (useOriginalImageIntensityForDotIntensityScore?true:false); 
	}

	const char* phantomDefinitionFile =  segmentationParameters->GetAttribute("PhantomDefinition"); 
	if ( (this->GetEnablePathOverride()) && (phantomDefinitionFile != NULL) && (vtksys::SystemTools::FileExists(phantomDefinitionFile, true)) ) {
		this->SetPhantomDefinitionFileName(phantomDefinitionFile);
	}

	this->GetSegParameters()->UpdateParameters(); 
}

//----------------------------------------------------------------------------
void vtkCalibrationController::ReadPhantomDefinition()
{
	LOG_TRACE("vtkCalibrationController::ReadPhantomDefinition");

	if ( this->PhantomDefinitionFileName != NULL )
	{
		vtkSmartPointer<vtkXMLDataElement> phantomDefinition = vtkXMLUtilities::ReadElementFromFile(this->PhantomDefinitionFileName);

		if (phantomDefinition == NULL) {
			LOG_ERROR("Unable to read the phantom definition file: " << this->PhantomDefinitionFileName); 
			return;
		}

		// Load type
		vtkSmartPointer<vtkXMLDataElement> description = phantomDefinition->FindNestedElementWithName("Description"); 
		if (description == NULL) {
			LOG_ERROR("Phantom description not found!");
			return;
		} else {
			const char* type =  description->GetAttribute("Type"); 
			if ( type != NULL ) {
				if (STRCASECMP("Double-N", type) == 0) {
					this->GetSegParameters()->mFiducialGeometry = SegmentationParameters::CALIBRATION_PHANTOM_6_POINT;
				} else if (STRCASECMP("U-Shaped-N", type) == 0) {
					this->GetSegParameters()->mFiducialGeometry = SegmentationParameters::TAB2_5_POINT;
				}
			} else {
				LOG_ERROR("Phantom type not found!");
			}
		}

		// Load geometry
		vtkSmartPointer<vtkXMLDataElement> geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
		if (geometry == NULL) {
			LOG_ERROR("Phantom geometry information not found!");
			return;
		} else {
			this->GetSegParameters()->mNWires.clear();

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
				
				this->GetSegParameters()->mNWires.push_back(nWire);
			}
		}
	} else {
		LOG_ERROR("Phantom definition file path is not set!"); 
		return;
	}

	// Compute error boundaries based on error percents and the NWire definition (supposing that the NWire is regular - parallel sides)
	// Line length of an N-wire: the maximum distance between its wires' front endpoints
	double maxLineLengthSquared = -1.0;
	double minLineLengthSquared = FLT_MAX;
	std::vector<NWire> nWires = this->GetSegParameters()->mNWires;

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

	this->GetSegParameters()->mMaxLineLenMm = sqrt(maxLineLengthSquared) * (1.0 + (this->GetSegParameters()->mMaxLineLengthErrorPercent / 100.0));
	this->GetSegParameters()->mMinLineLenMm = sqrt(minLineLengthSquared) * (1.0 - (this->GetSegParameters()->mMaxLineLengthErrorPercent / 100.0));
	LOG_DEBUG("Line length - computed min: " << sqrt(minLineLengthSquared) << " , max: " << sqrt(maxLineLengthSquared) << ";  allowed min: " << this->GetSegParameters()->mMinLineLenMm << ", max: " << this->GetSegParameters()->mMaxLineLenMm);

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

	this->GetSegParameters()->mMaxLinePairDistMm = maxNPlaneDistance * (1.0 + (this->GetSegParameters()->mMaxLinePairDistanceErrorPercent / 100.0));
	this->GetSegParameters()->mMinLinePairDistMm = minNPlaneDistance * (1.0 - (this->GetSegParameters()->mMaxLinePairDistanceErrorPercent / 100.0));
	LOG_DEBUG("Line pair distance - computed min: " << minNPlaneDistance << " , max: " << maxNPlaneDistance << ";  allowed min: " << this->GetSegParameters()->mMinLinePairDistMm << ", max: " << this->GetSegParameters()->mMaxLinePairDistMm);
}

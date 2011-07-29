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

	this->VisualizationComponent = NULL;
	this->OutputPath = NULL; 
	this->ProgramFolderPath = NULL; 
	this->ConfigurationFileName = NULL;
	this->PhantomDefinitionFileName = NULL;
	this->ModelToPhantomTransform = NULL;
	this->PhantomModelFileName = NULL;
	
	this->SegParameters = new SegmentationParameters(); 

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

	// Initialize the segmenation component
	// ====================================
	this->mptrAutomatedSegmentation = new KPhantomSeg( 
		this->GetSegParameters()->GetFrameSize(),this->GetSegParameters()->GetRegionOfInterest(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

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

		// Check frame size before segmentation 
		int frameSize[2] = {imageData->GetLargestPossibleRegion().GetSize()[0], imageData->GetLargestPossibleRegion().GetSize()[1]}; 
		if ( this->GetSegParameters()->GetFrameSize()[0] != frameSize[0] || this->GetSegParameters()->GetFrameSize()[1] != frameSize[1] )
		{
			LOG_ERROR("Unable to add frame to calibrator! Frame size mismatch: actual (" 
				<< frameSize[0] << "x" << frameSize[1] << ") expected (" 
				<< this->GetSegParameters()->GetFrameSize()[0] << "x" << this->GetSegParameters()->GetFrameSize()[1] << ")"); 
			return PLUS_FAIL; 
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
	return this->ReadConfiguration(calibrationController); 
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
	vtkSmartPointer<vtkXMLDataElement> calibrationController = configData->FindNestedElementWithName("CalibrationController"); 
	return this->ReadCalibrationControllerConfiguration(calibrationController); 
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
	vtkSmartPointer<vtkXMLDataElement> realtimeCalibration = calibrationController->FindNestedElementWithName("RealtimeCalibration"); 
	if ( this->ReadRealtimeCalibrationConfiguration(realtimeCalibration) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to read calibration configuration file!"); 
		return PLUS_FAIL; 
	}

	//Phantom Definition specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> phantomDefinition =  calibrationController->FindNestedElementWithName("PhantomDefinition");
	if ( this->ReadPhantomDefinition(phantomDefinition) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to read phantom definition file!"); 
		return PLUS_FAIL; 
	}

	// SegmentationParameters specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> segmentationParameters = calibrationController->FindNestedElementWithName("SegmentationParameters"); 
	if ( this->GetSegParameters()->ReadSegmentationParametersConfiguration(segmentationParameters) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to read segmentation parameters configuration file!"); 
		return PLUS_FAIL; 
	}

	//Updating parameters
	this->GetSegParameters()->UpdateParameters();

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ReadPhantomDefinition(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkCalibrationController::ReadPhantomDefinition");

	if ( config == NULL )
	{
		LOG_ERROR("Unable to read the phantom definition FileName attribute from configuration file - XML data element is NULL"); 
		return PLUS_FAIL;
	}

	// Search for the phantom definition file found in the configuration file
	const char* phantomDefinitionFile =  config->GetAttribute("FileName");
	if (phantomDefinitionFile != NULL) 
	{
		std::string searchResult;
		if (STRCASECMP(vtkFileFinder::GetInstance()->GetConfigurationDirectory(), "") == 0) 
		{
			std::string configurationDirectory = vtksys::SystemTools::GetFilenamePath(this->ConfigurationFileName);
			searchResult = vtkFileFinder::GetFirstFileFoundInParentOfDirectory(phantomDefinitionFile, configurationDirectory.c_str());
		} 
		else 
		{
			searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(phantomDefinitionFile);
		}

		if (STRCASECMP("", searchResult.c_str()) == 0) 
		{
			LOG_WARNING("Phantom model file is not found with name: " << phantomDefinitionFile);
		}

		if ( vtksys::SystemTools::FileExists(searchResult.c_str(), true) ) 
		{
			this->SetPhantomDefinitionFileName(searchResult.c_str());
		}
	}

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
		vtkSmartPointer<vtkXMLDataElement> description = phantomDefinition->FindNestedElementWithName("Description"); 
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
		vtkSmartPointer<vtkXMLDataElement> model = phantomDefinition->FindNestedElementWithName("Model"); 
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
		vtkSmartPointer<vtkXMLDataElement> geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
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
		LOG_DEBUG("Unable to find TemplateTranslationData XML data element, default 100 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(TEMPLATE_TRANSLATION); 
		imageDataInfo.NumberOfImagesToAcquire = 100;
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
		LOG_DEBUG("Unable to find ProbeTranslationData XML data element, default 200 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(PROBE_TRANSLATION); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
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
		LOG_DEBUG("Unable to find ProbeRotationData XML data element, default 500 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(PROBE_ROTATION); 
		imageDataInfo.NumberOfImagesToAcquire = 500;
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
		LOG_DEBUG("Unable to find RandomStepperMotionData1 XML data element, default 200 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_1); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
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
		LOG_DEBUG("Unable to find RandomStepperMotionData2 XML data element, default 100 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(RANDOM_STEPPER_MOTION_2); 
		imageDataInfo.NumberOfImagesToAcquire = 100; 
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
		LOG_DEBUG("Unable to find FreehandMotionData1 XML data element, default 200 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
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
		LOG_DEBUG("Unable to find FreehandMotionData2 XML data element, default 100 is used"); 
		vtkCalibrationController::RealtimeImageDataInfo imageDataInfo = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2); 
		imageDataInfo.NumberOfImagesToAcquire = 100; 
	}

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

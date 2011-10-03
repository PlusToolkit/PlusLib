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
#include "itkMetaImageSequenceIO.h"
#include <itkImageDuplicator.h>
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkTriangle.h"
#include "vtkPlane.h"
#include "vtkLine.h"
#include "vtksys/SystemTools.hxx"

#include "vnl/vnl_cross.h"

#include "LinearLeastSquares.h" //TODO use PlusMath instead

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
	this->EnableTrackedSequenceDataSavingOff();
	this->EnableErroneouslySegmentedDataSavingOff(); 
	this->EnableSegmentationAnalysisOff();
  this->EnableVisualizationOn(); 
	this->InitializedOff(); 
  this->CalibrationDoneOff(); 

  this->OfflineImageData = NULL;
  
  this->CalibrationDate = NULL; 
  this->CalibrationTimestamp = NULL; 
  
  this->OfflineImageData = vtkImageData::New(); 
  this->OfflineImageData->SetExtent(0,1,0,1,0,0); 
  this->OfflineImageData->SetScalarTypeToUnsignedChar(); 
  this->OfflineImageData->AllocateScalars(); 

	for ( int i = 0; i < NUMBER_OF_IMAGE_DATA_TYPES; i++ )
	{
		vtkTrackedFrameList *trackedFrameList = vtkTrackedFrameList::New(); 
		this->TrackedFrameListContainer.push_back(trackedFrameList); 

		ImageDataInfo imageDataInfo; 
		imageDataInfo.NumberOfImagesToAcquire = 0; 
		imageDataInfo.NumberOfSegmentedImages = 0; 
		imageDataInfo.OutputSequenceMetaFileSuffix.clear(); 
    imageDataInfo.InputSequenceMetaFileName.clear(); 
		ImageDataInfoContainer.push_back(imageDataInfo); 
	}

  // Former ProbeCalibrationController and FreehandCalibraitonController members
	this->EnableSegmentedWirePositionsSavingOff(); 

	this->US3DBeamwidthDataReadyOff(); 

  this->SetCenterOfRotationPx(0,0);

	this->SetCurrentPRE3DdistributionID(0); 

	this->SetNumUS3DBeamwidthProfileData(-1);
	this->SetIncorporatingUS3DBeamProfile(0);

	this->SetAxialPositionOfCrystalSurfaceInTRUSImageFrame(-1);

	this->CalibrationConfigFileNameWithPath = NULL; 
	this->CalibrationResultFileNameWithPath = NULL;
	this->US3DBeamProfileDataFileNameAndPath = NULL; 
	this->SegmentationAnalysisFileNameWithTimeStamp = NULL; 
	this->SegmentationErrorLogFileNameWithTimeStamp = NULL;
	this->DataFileSuffix = NULL;
	this->CalibrationResultFileSuffix = NULL;
	this->SegmentationErrorLogFileNameSuffix = NULL;
	this->SegmentationAnalysisFileNameSuffix = NULL;
	this->Temp2StepCalibAnalysisFileNameSuffix = NULL; 
	this->CalibrationSegWirePosInfoFileName = NULL; 
	this->ValidationSegWirePosInfoFileName = NULL; 

	this->CalibrationControllerIO = NULL; 

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

	// Initialize calibration controller IO
	vtkSmartPointer<vtkProbeCalibrationControllerIO> calibrationControllerIO = vtkSmartPointer<vtkProbeCalibrationControllerIO>::New(); 
	calibrationControllerIO->Initialize( this ); 
	this->SetCalibrationControllerIO( calibrationControllerIO ); 

	// Initialize data containers
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_size(0,0);
	this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_size(0,0); 
	this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);

  // Former Phantom class
	mHasPhantomBeenRegistered = false;
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

  for ( int i = 0; i < this->SegmentedFrameContainer.size(); i++ )
  {
    if ( this->SegmentedFrameContainer[i].TrackedFrameInfo != NULL )
    {
      delete this->SegmentedFrameContainer[i].TrackedFrameInfo; 
      this->SegmentedFrameContainer[i].TrackedFrameInfo = NULL; 
    }
  }

  if ( this->OfflineImageData != NULL )
  {
    this->OfflineImageData->Delete(); 
    this->OfflineImageData = NULL; 
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
	this->SetCalibrationControllerIO(NULL); 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::Initialize()
{
	LOG_TRACE("vtkCalibrationController::Initialize"); 

  this->resetDataContainers(); 

  // Initialize the calibration component
  if (ComputeNWireInstersections() != PLUS_SUCCESS) {
    LOG_ERROR("Computing intersections of NWire wires failed!");
    return PLUS_FAIL;
  }

	// This will pass the US 3D beamwidth data and their predefined
	// weights to the calibration component.
	if( this->US3DBeamwidthDataReady )
	{
		this->FillUltrasoundBeamwidthAndWeightFactorsTable(); //TODO error handling
	}

	this->InitializedOn(); 

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
PlusStatus vtkCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - TrackedFrame"); 
	try
	{
		// Check to see if the segmentation has returned the targets
	  itk::Image<unsigned char, 2>::Pointer image=trackedFrame->GetImageData()->GetImage<unsigned char>();
    if ( image.IsNull())
    {
        LOG_ERROR("vtkCalibrationController::AddTrackedFrameData no image data is available"); 
        return PLUS_FAIL; 
    }

	  if ( ! this->Initialized )
	  {
		  this->Initialize(); 
	  }

	  // Send the image to the Segmentation component for segmentation
    if (this->PatternRecognition.RecognizePattern(trackedFrame, this->PatRecognitionResult) != PLUS_SUCCESS)
    {
      LOG_ERROR("Segmentation encountered errors!");
      return PLUS_FAIL;
    }
 
		// Add frame to the container 
		int trackedFramePosition(-1); 
		if ( this->EnableErroneouslySegmentedDataSaving )
		{
			// Save the erroneously segmented frames too 
			trackedFramePosition = this->TrackedFrameListContainer[dataType]->AddTrackedFrame(trackedFrame); 
		}
		else if (this->GetPatRecognitionResult()->GetDotsFound() )
		{
			// Segmentation was successful
			trackedFramePosition = this->TrackedFrameListContainer[dataType]->AddTrackedFrame(trackedFrame); 
		}

		// Draw segmentation results to frame if needed
    if ( this->GetPatRecognitionResult()->GetDotsFound() && this->EnableSegmentationAnalysis )
		{
			// Draw segmentation result to image
      if (trackedFrame->GetImageData()->GetITKScalarPixelType()==itk::ImageIOBase::UCHAR)
      {
        this->PatternRecognition.DrawResults( static_cast<PixelType*>(trackedFrame->GetImageData()->GetBufferPointer()) ); // :TODO: DrawResults should use an ITK image as input
      }
      else
      {
        LOG_ERROR("Draw results works only on UCHAR images");
      }
		} 

		if( !this->PatRecognitionResult.GetDotsFound() )
		{
			LOG_DEBUG("The segmentation cannot locate any meaningful targets, the image was ignored!"); 
			return PLUS_FAIL; 
		}

		// Add the segmentation result to the SegmentedFrameContainer
		SegmentedFrame segmentedFrame; 
		segmentedFrame.SegResults = this->PatRecognitionResult; 
		segmentedFrame.TrackedFrameInfo = new TrackedFrame(*this->TrackedFrameListContainer[dataType]->GetTrackedFrame(trackedFramePosition)); 
		segmentedFrame.DataType = dataType; 
		this->SegmentedFrameContainer.push_back(segmentedFrame); 

		this->ImageDataInfoContainer[dataType].NumberOfSegmentedImages++; 

		double tProbeToReference[16]; 
		if ( trackedFrame->GetDefaultFrameTransform(tProbeToReference) )
		{
			vtkSmartPointer<vtkMatrix4x4> tProbeToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
			tProbeToReferenceMatrix->DeepCopy(tProbeToReference); 
			vnl_matrix<double> transformProbeToReferenceMatrix4x4(4,4);

      PlusMath::ConvertVtkMatrixToVnlMatrix(tProbeToReferenceMatrix, transformProbeToReferenceMatrix4x4); 

			this->TransformProbeToReference->SetMatrix(tProbeToReferenceMatrix); 

			this->PopulateSegmentedFiducialsToDataContainer(transformProbeToReferenceMatrix4x4, dataType); 
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
void vtkCalibrationController::ClearSegmentedFrameContainer(IMAGE_DATA_TYPE dataType)
{
  for ( SegmentedFrameList::iterator it = this->SegmentedFrameContainer.begin(); it != this->SegmentedFrameContainer.end(); )
  {
    if ( it->DataType == dataType )
    {
      if ( it->TrackedFrameInfo != NULL )
      {
        delete it->TrackedFrameInfo; 
        it->TrackedFrameInfo = NULL; 
      }
      it = this->SegmentedFrameContainer.erase(it); 
    }
    else
    {
      ++it;
    }
  } 
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::SetOfflineImageData(const ImageType::Pointer& frame)
{
	LOG_TRACE("vtkCalibrationController::AddFrameToRenderer"); 
	if ( ! this->EnableVisualization ) 
	{
		// We don't want to render anything
		return PLUS_SUCCESS; 
	}

  if ( frame.IsNull() )
  {
    LOG_ERROR("Failed to add frame to the renderer - frame is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( this->OfflineImageData == NULL )
  {
    this->OfflineImageData = vtkImageData::New(); 
  }

  if ( UsImageConverterCommon::ConvertItkImageToVtkImage(frame, this->OfflineImageData) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to convert itk image to vtk image!"); 
    return PLUS_FAIL;
  }

  this->OfflineImageData->Modified();

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::SetOfflineImageData(vtkImageData* frame)
{
	LOG_TRACE("vtkCalibrationController::AddFrameToRenderer"); 
	if ( ! this->EnableVisualization ) 
	{
		// We don't want to render anything
		return PLUS_SUCCESS; 
	}

  if ( this->OfflineImageData == NULL )
  {
    this->OfflineImageData = vtkImageData::New(); 
  }

  this->OfflineImageData->DeepCopy(frame); 

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

  // Read probe calibration
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_PROBE, toolType);

  vtkSmartPointer<vtkXMLDataElement> probeDefinition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(rootElement, "Tracker", "Tool", "Type", toolType.c_str());
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

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ReadProbeCalibrationConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkCalibrationController::ReadProbeCalibrationConfiguration"); 

  if ( this->CalibrationControllerIO->ReadProbeCalibrationConfiguration(configData) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read probe calibration configuration from file!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::ReadFreehandCalibrationConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkCalibrationController::ReadFreehandCalibrationConfiguration"); 

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
	calibration->SetDoubleAttribute("Error", GetPointLineDistanceErrorAnalysisVector()[0]); // TODO find the best error number

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

//----------------------------------------------------------------------------
void vtkCalibrationController::RegisterPhantomGeometry( vtkTransform* aPhantomToPhantomReferenceTransform )
{
	LOG_TRACE("vtkCalibrationController::RegisterPhantomGeometry"); 

  // Register the phantom geometry to the DRB frame in the "Emulator" mode.
	vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix(aPhantomToPhantomReferenceTransform->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode);

	mTransformMatrixPhantom2DRB4x4 = transformMatrixPhantom2DRB4x4InEmulatorMode;
	mHasPhantomBeenRegistered = true;
}

//----------------------------------------------------------------------------
void vtkCalibrationController::RegisterPhantomGeometry( double phantomToProbeDistanceInMm[2] )
{
	LOG_TRACE("vtkCalibrationController::RegisterPhantomGeometry: " << phantomToProbeDistanceInMm[0] << "  " << phantomToProbeDistanceInMm[1]); 

  // Vertical distance from the template mounter hole center to the TRUS Rotation Center
  double verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[1].wires[0].endPointFront[1] // WIRE1 y
        + phantomToProbeDistanceInMm[1]
        - this->TransformTemplateHolderToPhantom->GetPosition()[1]; // :TODO: transform with the whole matrix instead of just using the XY position values

  // Horizontal distance from the template mounter hole center to the TRUS Rotation Center
  double horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
      this->PatternRecognition.GetFidLineFinder()->GetNWires()[0].wires[2].endPointFront[0] // WIRE3 x
      + phantomToProbeDistanceInMm[0]
      - this->TransformTemplateHolderToPhantom->GetPosition()[0]; // :TODO: transform with the whole matrix instead of just using the XY position values

  double templateHolderPositionX = this->TransformTemplateHolderToPhantom->GetPosition()[0];
  double templateHolderPositionY = this->TransformTemplateHolderToPhantom->GetPosition()[1];
  double templateHolderPositionZ = this->TransformTemplateHolderToPhantom->GetPosition()[2];

  vtkSmartPointer<vtkTransform> tTemplateHolderToTemplate = vtkSmartPointer<vtkTransform>::New();
  tTemplateHolderToTemplate->Translate( templateHolderPositionX, templateHolderPositionY, templateHolderPositionZ);
  this->TransformTemplateHolderToTemplate->SetMatrix( tTemplateHolderToTemplate->GetMatrix() ); 

  vtkSmartPointer<vtkTransform> tReferenceToTemplateHolderHome = vtkSmartPointer<vtkTransform>::New();
  tReferenceToTemplateHolderHome->Translate( horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, 0);
  this->TransformReferenceToTemplateHolderHome->SetMatrix( tReferenceToTemplateHolderHome->GetMatrix() ); 

  vtkSmartPointer<vtkTransform> tTemplateHomeToReference = vtkSmartPointer<vtkTransform>::New();
  tTemplateHomeToReference->PostMultiply(); 
  tTemplateHomeToReference->Concatenate( this->TransformReferenceToTemplateHolderHome ); 
  tTemplateHomeToReference->Concatenate( this->TransformTemplateHolderToTemplate ); 
  tTemplateHomeToReference->Inverse(); 

  std::ostringstream osTemplateHomeToReference; 
  tTemplateHomeToReference->GetMatrix()->Print(osTemplateHomeToReference);   
  LOG_DEBUG("TemplateHomeToProbeHome:\n" << osTemplateHomeToReference.str().c_str() );

  // Register the phantom geometry to the DRB frame in the "Emulator" mode.
  vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
  PlusMath::ConvertVtkMatrixToVnlMatrix( tTemplateHomeToReference->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode );
	mTransformMatrixPhantom2DRB4x4 = transformMatrixPhantom2DRB4x4InEmulatorMode;
	mHasPhantomBeenRegistered = true;
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

  this->resetDataContainers(); 

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::OfflineUSToTemplateCalibration()
{
// TODO it is for TRUS!!!
  LOG_TRACE("vtkCalibrationController::OfflineUSToTemplateCalibration"); 
  if ( ! this->Initialized ) 
  {
      this->Initialize(); 
  }

  // Reset calibrator data containers 
  this->resetDataContainers(); 

  // ****************************  Validation data ***********************
  vtkSmartPointer<vtkTrackedFrameList> validationData = vtkSmartPointer<vtkTrackedFrameList>::New();
  const std::string validationDataFileName = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).InputSequenceMetaFileName; 
  if ( !validationDataFileName.empty() )
  {
      LOG_TRACE("Read tracked frames from sequence metafile: " << validationDataFileName ); 
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

  // ****************************  Calibration data ***********************
  vtkSmartPointer<vtkTrackedFrameList> calibrationData = vtkSmartPointer<vtkTrackedFrameList>::New();
  const std::string calibrationDataFileName = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).InputSequenceMetaFileName; 
  if ( !calibrationDataFileName.empty() )
  {
      LOG_TRACE("Read tracked frames from sequence metafile: " << validationDataFileName ); 
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

  ImageDataInfo calibrationDataInfo = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1); 
  calibrationDataInfo.NumberOfSegmentedImages = 0; 
  if ( calibrationDataInfo.NumberOfImagesToAcquire > calibrationData->GetNumberOfTrackedFrames() )
  {
    calibrationDataInfo.NumberOfImagesToAcquire = calibrationData->GetNumberOfTrackedFrames(); 
  }
  this->SetImageDataInfo(RANDOM_STEPPER_MOTION_1, calibrationDataInfo); 


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

      if ( this->SegmentationProgressCallbackFunction != NULL )
      {
        int numberOfSegmentedImages = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfSegmentedImages + this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages; 
        int percent = 100* numberOfSegmentedImages / (this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfImagesToAcquire + this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfImagesToAcquire); 
        (*SegmentationProgressCallbackFunction)(percent); 
      }

      this->SetOfflineImageData(validationData->GetTrackedFrame(vImgNumber)->GetImageData()->GetDisplayableImage()); 
  }

  int validSegmentationSuccessRate = 100*this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfSegmentedImages / vImgNumber; 
  LOG_INFO ( "A total of " << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfSegmentedImages << " images (" << validSegmentationSuccessRate << "%) have been successfully added for validation.");

  validationData->Clear(); 

  
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

      if ( this->SegmentationProgressCallbackFunction != NULL )
      {
        int numberOfSegmentedImages = this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfSegmentedImages + this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages; 
        int percent = 100* numberOfSegmentedImages / (this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).NumberOfImagesToAcquire + this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfImagesToAcquire); 
        (*SegmentationProgressCallbackFunction)(percent); 
      }

      this->SetOfflineImageData(calibrationData->GetTrackedFrame(cImgNumber)->GetImageData()->GetDisplayableImage()); 
  }

  int calibSegmentationSuccessRate = 100*this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages / cImgNumber; 
  LOG_INFO ("A total of " << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).NumberOfSegmentedImages << " images (" << calibSegmentationSuccessRate << "%) have been successfully added for calibration.");

  calibrationData->Clear(); 

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::DoOfflineCalibration()
{
//TODO it is for Freehand!!!
	LOG_TRACE("vtkCalibrationController::DoOfflineCalibration"); 

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

			this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage());
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

			this->SetOfflineImageData(calibrationData->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage()); 
		}

		LOG_INFO ("A total of " << this->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " images have been successfully added for calibration.");
	} catch(...) {
		LOG_ERROR("AddAllSavedData: Failed to add saved data!");  
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkCalibrationController::DoCalibration() //TODO replace its calls with its content
{
	LOG_TRACE("vtkCalibrationController::DoCalibration"); 
	// Instruct the calibrator to perform the calibration task
	this->calibrate();

	// Instruct the calibrator to validate the calibration accuracy
	this->compute3DPointReconstructionError();
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
	try
	{
		if ( ! this->Initialized ) 
		{
			this->Initialize(); 
		}

    // Set calibration date
    this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 
    this->SetCalibrationTimestamp(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S").c_str()); 

		// Do final calibration 
		this->DoCalibration(); 

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
    IsUserImageToProbeTransformOrthogonal();

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
		this->computeIndependentPointLineReconstructionError();

		// STEP-4. Print the final calibration results and error reports 
    std::ostringstream matrixStream; 
    PlusMath::PrintVtkMatrix(this->TransformUserImageToProbe->GetMatrix(), matrixStream);
    LOG_INFO("Calibration result transform matrix = \n" << matrixStream.str());

	  // Point-Line Distance Error Analysis for Validation Positions in US probe frame
    LOG_INFO("Point-Line Distance Error - mean: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[0] << ", rms: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[1] << ", std: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[2]);
    LOG_INFO("  Validation data confidence level: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[3]);

		// STEP-5. Save the calibration results and error reports into a file 
		this->CalibrationControllerIO->SaveCalibrationResultsAndErrorReportsToXML();

		// STEP-6. Save the segmented wire positions into a file 
		if ( this->EnableSegmentedWirePositionsSaving )
		{
			this->CalibrationControllerIO->SaveSegmentedWirePositionsToFile(); 
		}

    this->ClearSegmentedFrameContainer(RANDOM_STEPPER_MOTION_1); 
		this->ClearSegmentedFrameContainer(RANDOM_STEPPER_MOTION_2);
    this->ClearSegmentedFrameContainer(FREEHAND_MOTION_1); 
		this->ClearSegmentedFrameContainer(FREEHAND_MOTION_2);

    // Save calibration
    if (WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS) {
		  LOG_ERROR("Freehand calibration result could not be saved into session configuration data!");
		  return PLUS_FAIL;
    }

    // save the input images to meta image
    if ( this->EnableTrackedSequenceDataSaving )
    {
      // TODO add validation file name to config file
      // Save validation dataset
      std::ostringstream validationDataFileName; 
      validationDataFileName << this->CalibrationDate << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).OutputSequenceMetaFileSuffix; 
      if ( this->SaveTrackedFrameListToMetafile( RANDOM_STEPPER_MOTION_2, vtkPlusConfig::GetInstance()->GetOutputDirectory(), validationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
      }

      // Save calibration dataset 
      std::ostringstream calibrationDataFileName; 
      calibrationDataFileName << this->CalibrationDate << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).OutputSequenceMetaFileSuffix; 
      if ( this->SaveTrackedFrameListToMetafile( RANDOM_STEPPER_MOTION_1, vtkPlusConfig::GetInstance()->GetOutputDirectory(), calibrationDataFileName.str().c_str(), false ) != PLUS_SUCCESS ) 
      {
        LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
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
void vtkCalibrationController::PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkCalibrationController::PopulateSegmentedFiducialsToDataContainer"); 

  // Populate the segmented N-fiducials to the data container
	// Indices defined in the input std::vector array.
	// This is the order that the segmentation algorithm gives the 
	// segmented positions in each image:
	// Each acquired data position is a 4x1 homogenous vector :
	// [ X, Y, 0, 1] all units in pixels

  if ( !this->GetPatternRecognition()->GetFidLabeling()->GetDotsFound() )
	{
		LOG_DEBUG("Segmentation failed! Unable to populate segmentation result!"); 
		return; 
	}

	// Top layer (close to probe): 3, 2, 1
	// Bottom Layer (far from probe): 6, 5, 4
	std::vector<vnl_vector<double>> SegmentedNFiducialsInFixedCorrespondence;
	SegmentedNFiducialsInFixedCorrespondence.resize(0);

  for (int i=0; i<this->GetPatRecognitionResult()->GetFoundDotsCoordinateValue().size(); i++)
	{
		vnl_vector<double> NFiducial(4,0);
		NFiducial[0]=this->GetPatRecognitionResult()->GetFoundDotsCoordinateValue()[i][0];
		NFiducial[1]=this->GetPatRecognitionResult()->GetFoundDotsCoordinateValue()[i][1];
		NFiducial[2]=0;
		NFiducial[3]=1;
		SegmentedNFiducialsInFixedCorrespondence.push_back(NFiducial);
	}

	if (dataType == RANDOM_STEPPER_MOTION_1 || dataType == FREEHAND_MOTION_1)
	{
		// Finally, add the data for calibration
		this->addDataPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
	else if (dataType == RANDOM_STEPPER_MOTION_2 || dataType == FREEHAND_MOTION_2)
	{
		this->addValidationPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::GetWirePosInTemplateCoordinate( int wireNum, double* wirePosInTemplate )
{
	LOG_TRACE("vtkCalibrationController::GetWirePosInTemplateCoordinate (wire #" << wireNum << ")"); 

  // wireNum is 1-based (values are between 1..6)
  int nwireIndex=(wireNum-1)/3;
  int wireIndex=(wireNum-1)%3;

	// Wire position on the front wall in template coordinate system
  double p1[3] = {
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[nwireIndex].wires[wireIndex].endPointFront[0],
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[nwireIndex].wires[wireIndex].endPointFront[1],
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[nwireIndex].wires[wireIndex].endPointFront[2]
  }; 

	// Wire position on the back wall in template coordinate system
  double p2[3] = {
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[nwireIndex].wires[wireIndex].endPointBack[0],
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[nwireIndex].wires[wireIndex].endPointBack[1],
    this->PatternRecognition.GetFidLineFinder()->GetNWires()[nwireIndex].wires[wireIndex].endPointBack[2]
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
	errorsStringStream << std::fixed << std::setprecision(3) << "  " << GetPointLineDistanceErrorAnalysisVector()[0] << ", " << GetPointLineDistanceErrorAnalysisVector()[1] << ", " << GetPointLineDistanceErrorAnalysisVector()[2] << std::endl;

	std::ostringstream resultStringStream;
	resultStringStream << matrixStringStream.str() << errorsStringStream.str() << std::endl;

	resultStringStream << "Validation data confidence:" << std::endl;
	resultStringStream << "  " << GetPointLineDistanceErrorAnalysisVector()[3];

	return resultStringStream.str();
}

//-----------------------------------------------------------------------------

















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
		if (vtkMath::Norm(cross) != 0) {
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

PlusStatus vtkCalibrationController::addDataPositionsPerImage( 
	std::vector< vnl_vector<double> > SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 )
{
	LOG_TRACE("vtkCalibrationController::addDataPositionsPerImage");

  static int frameIndex=-1;
  frameIndex++;
  if ( ! this->Initialized )
  {
	  this->Initialize(); 
  }

  if( mHasPhantomBeenRegistered != true )
	{
		LOG_ERROR("The phantom is not yet registered to the DRB frame!");
    return PLUS_FAIL;
	}

  if (this->TransformImageToUserImage == NULL)
  {
    LOG_ERROR("Invalid Image to User image transform!");
    return PLUS_FAIL;
  }

  if( SegmentedDataPositionListPerImage.size() != this->PatternRecognition.GetFidLineFinder()->GetNWires().size() * 3 )
	{
		LOG_ERROR("The number of N-wires is NOT "	<< this->PatternRecognition.GetFidLineFinder()->GetNWires().size() * 3 << " in one US image as required!");
    return PLUS_FAIL;
	}

	// Obtain the transform matrix from Template/Stepper Frame to the US probe Frame 
	vnl_matrix_inverse<double> inverseMatrix( TransformMatrixUSProbe2Stepper4x4 );
	vnl_matrix<double> TransformMatrixStepper2USProbe4x4 = inverseMatrix.inverse();
	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	TransformMatrixStepper2USProbe4x4.set_row(3, lastRow);

	// Calculate then store the data positions 
	// ========================================
	// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
	// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
	// Each acquired data position is a 4x1 homogenous vector :
	// [ X, Y, 0, 1] all units in pixels

  // Convert Image to User image transform to VNL
	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToUserImage->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 

  std::vector<NWire> nWires = this->PatternRecognition.GetFidLineFinder()->GetNWires();

  for( int Layer = 0; Layer < 2; Layer++ )
	{
		// The protocol is that the middle point collected in 
		// the set of three points of the N-wire is the data point.
		vnl_vector<double> SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) );
		
		// Convert the segmented image positions from the original 
		// image to the predefined ultrasound image frame.
		vnl_vector<double> SegmentedPositionInUSImageFrame =  
			transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			SegmentedPositionInOriginalImageFrame;
		
		// Add weights to the positions if required (see mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM member description)
		if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
		{
			// Get the axial depth in the US Image Frame for the segmented data point
			// NOTE: the axial depth is in pixels and along the Y-axis of the frame.
			const double ThisAxialDepthInUSImageFrameOriginal = SegmentedPositionInUSImageFrame.get(1);
			// Round the axial depth to one-pixel level (integer)
			const int ThisAxialDepthInUSImageFrameRounded = 
				floor( ThisAxialDepthInUSImageFrameOriginal + 0.5 );

			// Set the weight according to the selected method of incorporation
			double Weight4ThisAxialDepth(-1);
			double USBeamWidthEuclideanMagAtThisAxialDepthInMM(-1);
              if( ThisAxialDepthInUSImageFrameRounded <= 
				mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
			{
				// #1. This is the ultrasound elevation near field which has the
				// the best imaging quality (before the elevation focal zone)

				// We will set the beamwidth at the near field to be the same
				// as that of the elevation focal zone.
				USBeamWidthEuclideanMagAtThisAxialDepthInMM = 
					mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1);

				if( 1 == this->IncorporatingUS3DBeamProfile || 3 == this->IncorporatingUS3DBeamProfile )
				{	
					// Filtering is not necessary in the near field

					// Option: BWVar or BWTHEVar
					// NOTE: The units of the standard deviation of beamwidth
					// needs to be converted to meters to comply to that of
					// the calibration data.
					Weight4ThisAxialDepth = 
						1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
				}
				else 
				{
					// Option = BWRatio
					Weight4ThisAxialDepth = 
						sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,0) );
				}
			}
			else if( ThisAxialDepthInUSImageFrameRounded >= 
				mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
			{
				// #2. Further deep in far field (close to the bottom of the image)
				// Ultrasound diverses quickly in this region and data quality deteriorates

				USBeamWidthEuclideanMagAtThisAxialDepthInMM =
					mUS3DBeamwidthAtFarestAxialDepth.magnitude();

				if( 1 == this->IncorporatingUS3DBeamProfile || 3 == this->IncorporatingUS3DBeamProfile )
				{	
					if( 3 == this->IncorporatingUS3DBeamProfile && 
						USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                          (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) * mNumOfTimesOfMinBeamWidth) )
					{
						// Option: BWTHEVar
						continue;  // Ignore this data, jump to the next iteration of for-loop
					}

					// Option: BWVar or BWTHEVar
					// NOTE: The units of the standard deviation of beamwidth
					// needs to be converted to meters to comply to that of
					// the calibration data.
					Weight4ThisAxialDepth = 
						1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
				}
				else
				{
					// Option = BWRatio
					Weight4ThisAxialDepth = 
						sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
							this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.columns() - 1) );
				}
			}
			else 
			{
				// #3. Ultrasound far field 
				// Here the sound starts to diverse with elevation beamwidth getting
				// larger and larger.  Data quality starts to deteriorate.

				// Populate the beamwidth vector (axial, lateral and elevation elements)
				vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
				US3DBeamwidthAtThisAxialDepth.put(0,
					mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
				US3DBeamwidthAtThisAxialDepth.put(1,
					mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
				US3DBeamwidthAtThisAxialDepth.put(2,
					mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
				USBeamWidthEuclideanMagAtThisAxialDepthInMM =
					US3DBeamwidthAtThisAxialDepth.magnitude();

				if( 1 == this->IncorporatingUS3DBeamProfile || 3 == this->IncorporatingUS3DBeamProfile )
				{	
					if( 3 == this->IncorporatingUS3DBeamProfile && 
						USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                          (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) * mNumOfTimesOfMinBeamWidth) )
					{
						// Option: BWTHEVar
						continue;  // Ignore this data, jump to the next iteration of for-loop
					}

					// Option: BWVar or BWTHEVar
					// NOTE: The units of the standard deviation of beamwidth
					// needs to be converted to meters to comply to that of
					// the calibration data.
					Weight4ThisAxialDepth = 
						1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
				}
				else
				{
					 // Option = BWRatio
					Weight4ThisAxialDepth = 
						sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable) );
				}
			}

			mWeightsForDataPositions.push_back( Weight4ThisAxialDepth );
			mUSBeamWidthEuclideanMagAtDataPositions.push_back( 
				USBeamWidthEuclideanMagAtThisAxialDepthInMM );
		}

		// Calcuate the alpha value
		// alpha = |CiXi|/|CiCi+1|
		vnl_vector<double> VectorCi2Xi = 
			SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
			SegmentedDataPositionListPerImage.at( Layer*3 );
		vnl_vector<double> VectorCi2Cii = 
			SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
			SegmentedDataPositionListPerImage.at( Layer*3 );
		double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
          
		// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
		// where:
		// - Ai and Bi are the N-wire joints in either front or back walls.
		vnl_vector<double> PositionInPhantomFrame(4);
		vnl_vector<double> IntersectPosW12(4);
		vnl_vector<double> IntersectPosW32(4);

		// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
		// in addition to the real-time PRE3D.
		vnl_vector<double> NWireStartinPhantomFrame;
		vnl_vector<double> NWireEndinPhantomFrame;

		for (int i=0; i<3; ++i) {
      IntersectPosW12[i] = nWires[Layer].intersectPosW12[i];
			IntersectPosW32[i] = nWires[Layer].intersectPosW32[i];
		}
		IntersectPosW12[3] = 1.0;
		IntersectPosW32[3] = 1.0;

		PositionInPhantomFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );
    PositionInPhantomFrame[3]=1.0;

		// Finally, calculate the position in the US probe frame
		// X_USProbe = T_Stepper->USProbe * T_Template->Stepper * X_Template
		// NOTE: T_Template->Stepper = mTransformMatrixPhantom2DRB4x4 
		vnl_vector<double> PositionInUSProbeFrame =  
			TransformMatrixStepper2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 *
			PositionInPhantomFrame;

		LOG_DEBUG(" ADD DATA FOR CALIBRATION ("<<frameIndex<<")");
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ));
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ));
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ));
		LOG_DEBUG(" SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame);
		LOG_DEBUG(" SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame);
		LOG_DEBUG(" alpha = " << alpha);
		LOG_DEBUG(" PositionInPhantomFrame = " << PositionInPhantomFrame);
		LOG_DEBUG(" TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4);
    LOG_DEBUG(" mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4);
    LOG_DEBUG(" PositionInUSProbeFrame = " << PositionInUSProbeFrame);

		// Store into the list of positions in the US image frame
		mDataPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

		// Store into the list of positions in the Phantom frame
		mDataPositionsInPhantomFrame.push_back( PositionInPhantomFrame );

		// Store into the list of positions in the US probe frame
		mDataPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
	}

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::addValidationPositionsPerImage( 
	std::vector< vnl_vector<double> > SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 )
{
	LOG_TRACE("vtkCalibrationController::addValidationPositionsPerImage");

  static int frameIndex=-1;
  frameIndex++;

  if ( ! this->Initialized )
  {
	  this->Initialize(); 
  }

  if( mHasPhantomBeenRegistered != true )
	{
		LOG_ERROR("The phantom is not yet registered to the DRB frame!");
    return PLUS_FAIL;
	}

  if (this->TransformImageToUserImage == NULL)
  {
    LOG_ERROR("Invalid Image to User image transform!");
    return PLUS_FAIL;
  }

	if( SegmentedDataPositionListPerImage.size() != this->PatternRecognition.GetFidLineFinder()->GetNWires().size() * 3 )
	{
		LOG_ERROR("The number of N-wires is NOT "	<< this->PatternRecognition.GetFidLineFinder()->GetNWires().size() * 3 << " in one US image as required!");
    return PLUS_FAIL;
	}

  // Convert Image to User image transform to VNL
	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToUserImage->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 

	// Obtain the transform matrix from DRB Frame to the US probe Frame 
	vnl_matrix_inverse<double> inverseMatrix( TransformMatrixUSProbe2Stepper4x4 );
	vnl_matrix<double> TransformMatrixStepper2USProbe4x4 = inverseMatrix.inverse();
	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	TransformMatrixStepper2USProbe4x4.set_row(3, lastRow);

	// Calculate then store the data positions 
	// ========================================
	// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
	// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
	// Each acquired data position is a 4x1 homogenous vector :
	// [ X, Y, 0, 1] all units in pixels

	// Collect the wire locations (the two parallel wires of 
	// each of the N-shape) for independent Line-Reconstruction 
	// Error (LRE) validation.
	// Note: N1, N3, N4, and N6 are the parallel wires here.
          
	vnl_vector<double> N1SegmentedPositionInOriginalImageFrame( 
		SegmentedDataPositionListPerImage.at(0) );
	vnl_vector<double> N3SegmentedPositionInOriginalImageFrame( 
		SegmentedDataPositionListPerImage.at(2) );
	vnl_vector<double> N4SegmentedPositionInOriginalImageFrame( 
		SegmentedDataPositionListPerImage.at(3) );
	vnl_vector<double> N6SegmentedPositionInOriginalImageFrame( 
		SegmentedDataPositionListPerImage.at(5) );

	// Convert the segmented image positions from the original 
	// image to the predefined ultrasound image frame.
	vnl_vector<double> N1SegmentedPositionInUSImageFrame =  
		transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
		N1SegmentedPositionInOriginalImageFrame;
	vnl_vector<double> N3SegmentedPositionInUSImageFrame =  
		transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
		N3SegmentedPositionInOriginalImageFrame;
	vnl_vector<double> N4SegmentedPositionInUSImageFrame =  
		transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
		N4SegmentedPositionInOriginalImageFrame;
	vnl_vector<double> N6SegmentedPositionInUSImageFrame =  
		transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
		N6SegmentedPositionInOriginalImageFrame;

  std::vector<NWire> nWires = this->PatternRecognition.GetFidLineFinder()->GetNWires();

	for( int Layer = 0; Layer < 2; Layer++ )
	{
		// The protocol is that the middle point collected in 
		// the set of three points of the N-wire is the data point.
    vnl_vector<double> SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) );

		// Convert the segmented image positions from the original 
		// image to the predefined ultrasound image frame.
		vnl_vector<double> SegmentedPositionInUSImageFrame =  
			transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			SegmentedPositionInOriginalImageFrame;

		// OPTION: Apply the ultrasound 3D beamwidth to the validation dataset
		// If Option 3 is selected, we will use the ultrasound 3D
		// beamwidth to filter out the validation data that has larger
		// beamwidth which are potentially unreliable than those dataset
		// with a smaller beamwidth.  If Option 1 or 2 is selected, the
		// beamwidth of the validation data will be tracked and recorded 
		// for analysis only (no filtering is performed).
		if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
		{
			// Get the axial depth in the US Image Frame for the segmented data point
			// NOTE: the axial depth is in pixels and along the Y-axis of the frame.
			const double ThisAxialDepthInUSImageFrameOriginal = 
				SegmentedPositionInUSImageFrame.get(1);
			// Round the axial depth to one-pixel level (integer)
			const int ThisAxialDepthInUSImageFrameRounded = 
				floor( ThisAxialDepthInUSImageFrameOriginal + 0.5 );

			double USBeamWidthEuclideanMagAtThisAxialDepthInMM(-1);
			if( ThisAxialDepthInUSImageFrameRounded <= 
				mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
			{
				// #1. This is the ultrasound elevation near field which has the
				// the best imaging quality (before the elevation focal zone)

				// We will set the beamwidth to be the same as the elevation
				// focal zone.
				USBeamWidthEuclideanMagAtThisAxialDepthInMM = 
					mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1);
			}
			else if( ThisAxialDepthInUSImageFrameRounded >= 
				mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
			{
				// #2. Further deep in far field (close to the bottom of the image)
				// Ultrasound diverses quickly in this region and data quality deteriorates
				USBeamWidthEuclideanMagAtThisAxialDepthInMM =
					mUS3DBeamwidthAtFarestAxialDepth.magnitude();
			}
			else 
			{
				// #3. Ultrasound far field 
				// Here the sound starts to diverse with elevation beamwidth getting
				// larger and larger.  Data quality starts to deteriorate.

				// Populate the beamwidth vector (axial, lateral and elevation elements)
				vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
				US3DBeamwidthAtThisAxialDepth.put(0,
					mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
				US3DBeamwidthAtThisAxialDepth.put(1,
					mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
				US3DBeamwidthAtThisAxialDepth.put(2,
					mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,
							ThisAxialDepthInUSImageFrameRounded -
							mTheNearestAxialDepthInUSBeamwidthAndWeightTable));

				USBeamWidthEuclideanMagAtThisAxialDepthInMM =
					US3DBeamwidthAtThisAxialDepth.magnitude();
			}
				
			if( 3 == this->IncorporatingUS3DBeamProfile && USBeamWidthEuclideanMagAtThisAxialDepthInMM >= 
				(mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1) * mNumOfTimesOfMinBeamWidth) )
			{
				continue;  // Ignore this data, jump to the next iteration of the for-loop
			}
			
			mUSBeamWidthEuclideanMagAtValidationPositions.push_back(
				USBeamWidthEuclideanMagAtThisAxialDepthInMM );
		}

		// Calcuate the alpha value
		// alpha = |CiXi|/|CiCi+1|
		vnl_vector<double> VectorCi2Xi = 
			SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
			SegmentedDataPositionListPerImage.at( Layer*3 );
		vnl_vector<double> VectorCi2Cii = 
			SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
			SegmentedDataPositionListPerImage.at( Layer*3 );
		double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
          
		// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
		// where:
		// - Ai and Bi are the N-wire joints in either front or back walls.
		vnl_vector<double> PositionInPhantomFrame(4);
		vnl_vector<double> IntersectPosW12(4);
		vnl_vector<double> IntersectPosW32(4);

		// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
		// in addition to the real-time PRE3D.
		vnl_vector<double> NWireStartinPhantomFrame;
		vnl_vector<double> NWireEndinPhantomFrame;

		for (int i=0; i<3; ++i) {
			IntersectPosW12[i] = nWires[Layer].intersectPosW12[i];
			IntersectPosW32[i] = nWires[Layer].intersectPosW32[i];
		}
		IntersectPosW12[3] = 1.0;
		IntersectPosW32[3] = 1.0;

		PositionInPhantomFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );
    PositionInPhantomFrame[3]=1.0;

		// Finally, calculate the position in the US probe frame
		// X_USProbe = T_Stepper->USProbe * T_Template->Stepper * X_Template
		// NOTE: T_Template->Stepper = mTransformMatrixPhantom2DRB4x4 
		vnl_vector<double> PositionInUSProbeFrame =  
			TransformMatrixStepper2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 *
			PositionInPhantomFrame;

    LOG_DEBUG(" ADD DATA FOR VALIDATION ("<<frameIndex<<")");
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 )); 
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ));  
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ));  
		LOG_DEBUG(" SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame);
		LOG_DEBUG(" SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame);
		LOG_DEBUG(" alpha = " << alpha);
		LOG_DEBUG(" PositionInPhantomFrame = " << PositionInPhantomFrame);
		LOG_DEBUG(" TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4);
    LOG_DEBUG(" mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4);
    LOG_DEBUG(" PositionInUSProbeFrame = " << PositionInUSProbeFrame);

		vnl_vector<double> NWireStartinUSProbeFrame =
			TransformMatrixStepper2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 *
			IntersectPosW12;

		vnl_vector<double> NWireEndinUSProbeFrame =
			TransformMatrixStepper2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 *
			IntersectPosW32;

		// The parallel wires position in US Probe frame 
		// Note: 
		// 1. Parallel wires share the same X, Y coordinates as the N-wire joints
		//    in the phantom (template) frame.
		// 2. The Z-axis of the N-wire joints is not used in the computing.

		// Wire N1 corresponds to mNWireJointTopLayerBackWall 
		vnl_vector<double> NWireJointForN1InUSProbeFrame =
			TransformMatrixStepper2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 *
			IntersectPosW12; //any point of wire 1 of this layer

		// Wire N3 corresponds to mNWireJointTopLayerFrontWall
		vnl_vector<double> NWireJointForN3InUSProbeFrame =
			TransformMatrixStepper2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 *
			IntersectPosW32; //any point of wire 3 of this layer

		// Store into the list of positions in the US image frame
		mValidationPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

		// Store into the list of positions in the US probe frame
		mValidationPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
		mValidationPositionsNWireStartInUSProbeFrame.push_back( NWireStartinUSProbeFrame );
		mValidationPositionsNWireEndInUSProbeFrame.push_back( NWireEndinUSProbeFrame );

    for (int i=0; i<2; i++)
    {
      // all the matrices are expected to have the same length, so we need to add each value 
      // as many times as many layer we have - this really have to be cleaned up
      if (Layer==0)
      {
        mValidationPositionsNWire1InUSImageFrame.push_back( N1SegmentedPositionInUSImageFrame );
        mValidationPositionsNWire3InUSImageFrame.push_back( N3SegmentedPositionInUSImageFrame );
        mValidationPositionsNWire1InUSProbeFrame.push_back( NWireJointForN1InUSProbeFrame );
        mValidationPositionsNWire3InUSProbeFrame.push_back( NWireJointForN3InUSProbeFrame );
      }
      else
      {
        mValidationPositionsNWire4InUSImageFrame.push_back( N4SegmentedPositionInUSImageFrame );
        mValidationPositionsNWire6InUSImageFrame.push_back( N6SegmentedPositionInUSImageFrame );
        mValidationPositionsNWire4InUSProbeFrame.push_back( NWireJointForN1InUSProbeFrame );
        mValidationPositionsNWire6InUSProbeFrame.push_back( NWireJointForN3InUSProbeFrame );
      }
    }

		// Store into the list of positions in the Phantom frame
		mValidationPositionsInPhantomFrame.push_back( PositionInPhantomFrame );
	}

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

std::vector<double> vtkCalibrationController::getPRE3DforRealtimeImage(
    std::vector< vnl_vector<double> > SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 )
{
	LOG_TRACE("vtkCalibrationController::getPRE3DforRealtimeImage");

	std::vector<double> ReturningDataContainer;

  if ( ! this->Initialized )
  {
	  this->Initialize(); 
  }
	
	if( ! this->CalibrationDone )
	{
		LOG_ERROR("This operation is not possible since the calibration is not yet finished!");
    return ReturningDataContainer;
	}

	// STEP-1. Populate the position data

	// Data containers
	std::vector< vnl_vector<double> > DataPositionsInUSImageFrame;
	std::vector< vnl_vector<double> > DataPositionsInPhantomFrame;
	std::vector< vnl_vector<double> > DataPositionsInUSProbeFrame;

	// Obtain the transform matrix from DRB Frame to the US probe Frame 
	vnl_matrix_inverse<double> inverseMatrix( TransformMatrixUSProbe2DRB4x4 );
	vnl_matrix<double> TransformMatrixDRB2USProbe4x4 = inverseMatrix.inverse();
	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	TransformMatrixDRB2USProbe4x4.set_row(3, lastRow);

	// Calculate then store the data positions 
	// ========================================
	// For BrachyTRUSCalibrator, row/col indices are of no interest
	// to us any more, because we only see two layers of N-wires with one
	// N-shape on each layer.  Therefore in each image only two phantom 
	// geometry indices are collected and the program knows their indices
	// in the pre-generated geometry.  This is a good start for automating 
	// the process since we are able to calculate alpha on-the-fly now
	// without requiring to specify the phantom indices one by one.
	//
	// IMPORTANT:
	// Indices defined in the input std::vector array.
	// This is the order that the segmentation algorithm gives the 
	// segmented positions in each image
	//
	// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
	// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
	// Each acquired data position is a 4x1 homogenous vector :
	// [ X, Y, 0, 1] all units in pixels

  // Convert Image to User image transform to VNL
	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToUserImage->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 

  std::vector<NWire> nWires = this->PatternRecognition.GetFidLineFinder()->GetNWires();

  for( int Layer = 0; Layer < 2; Layer++ )
	{
		// The protocol is that the middle point collected in 
		// the set of three points of the N-wire is the data point.
		vnl_vector<double> SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) );

		// Convert the segmented image positions from the original 
		// image to the predefined ultrasound image frame.
		vnl_vector<double> SegmentedPositionInUSImageFrame =  
			transformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			SegmentedPositionInOriginalImageFrame;

		DataPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

		// Calcuate the alpha value
		// alpha = |CiXi|/|CiCi+1|
		vnl_vector<double> VectorCi2Xi = 
			SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
			SegmentedDataPositionListPerImage.at( Layer*3 );
		vnl_vector<double> VectorCi2Cii = 
			SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
			SegmentedDataPositionListPerImage.at( Layer*3 );
		double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
          
		// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
		// where:
		// - Ai and Bi are the N-wire joints in either front or back walls.
		//
		// IMPORTANT:
		// - This is a very crucial point that would easily cause confusions
		//   or mistakes.  There is one and only one fixed correspondence between 
		//   the 6 segmented image positions (N-fiducials) and the wires.
		// - Closely examine the wiring design and set Ai and Bi accordingly.

		vnl_vector<double> PositionInPhantomFrame(4);
		vnl_vector<double> IntersectPosW12(4);
		vnl_vector<double> IntersectPosW32(4);

		// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
		// in addition to the real-time PRE3D.
		vnl_vector<double> NWireStartinPhantomFrame;
		vnl_vector<double> NWireEndinPhantomFrame;

		for (int i=0; i<3; ++i) {
			IntersectPosW12[i] = nWires[Layer].intersectPosW12[i];
			IntersectPosW32[i] = nWires[Layer].intersectPosW32[i];
		}
		IntersectPosW12[3] = 1.0;
		IntersectPosW32[3] = 1.0;

		PositionInPhantomFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );

		// Finally, calculate the position in the US probe frame
		// X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
		vnl_vector<double> PositionInUSProbeFrame =  
			TransformMatrixDRB2USProbe4x4 * 
			mTransformMatrixPhantom2DRB4x4 * 
			PositionInPhantomFrame;

		LOG_DEBUG(" ADD DATA FOR CALIBRATION");
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 )); 
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ));  
		LOG_DEBUG(" SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ));  
		LOG_DEBUG(" SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame);
		LOG_DEBUG(" SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame);
		LOG_DEBUG(" alpha = " << alpha);
		LOG_DEBUG(" PositionInPhantomFrame = " << PositionInPhantomFrame);
		LOG_DEBUG(" TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4);
    LOG_DEBUG(" mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4);
    LOG_DEBUG(" PositionInUSProbeFrame = " << PositionInUSProbeFrame);

		// Store into the list of positions in the Phantom frame
		DataPositionsInPhantomFrame.push_back( PositionInPhantomFrame );

		// Store into the list of positions in the US probe frame
		DataPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
	}

	// STEP-2.  Calculate 3D Point Reconstruction Error (PRE3D)

	std::vector< vnl_vector<double> > PRE3DsInUSProbeFrameABS;
	std::vector< vnl_vector<double> > ProjectedPositionsInUSImageFrame;

	// To get the projected positions using the calibration matrix in
	// the image frame, we need an inverse of the calibration matrix.
	// However, since the 3rd column of the calibration matrix is all 
	// zeros (the US image frame does not have a 3rd z-component) 
	// thus non-singular, we will generate a 3rd vector from cross-
	// product of the 1st and 2nd column to make the matrix singular 
	// just that we could make an inverse out of it.
	// NOTE:
	// - We will normalize the column vectors in the rotation part (3x3) 
	//   of the input matrix to elimate the impact of the scaling factor.  
	// - We don't really care about the scale factor in the 3rd column (or
	//   the Z-axis), since we do not have data to evaluate that. So we 
	//   will use 1 (or normalized) for the sake of calculation.
	// - The translation part (the 4th column) remains untouched.
	vnl_vector<double> Column1st(3,0);
	vnl_vector<double> Column2nd(3,0);
	Column1st.put(0, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(0).get(0));
	Column1st.put(1, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(0).get(1));
	Column1st.put(2, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(0).get(2));
	Column2nd.put(0, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(1).get(0));
	Column2nd.put(1, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(1).get(1));
	Column2nd.put(2, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(1).get(2));
	vnl_vector<double> Column3rdNormalized(4,0);
	Column3rdNormalized.put(0, vnl_cross_3d(Column1st, Column2nd).normalize().get(0));
	Column3rdNormalized.put(1, vnl_cross_3d(Column1st, Column2nd).normalize().get(1));
	Column3rdNormalized.put(2, vnl_cross_3d(Column1st, Column2nd).normalize().get(2));
	Column3rdNormalized.put(3, 0);
	vnl_matrix<double> CreatedTransformMatrixUSProbeFame2USImageFrame4x4 =
		mTransformUSImageFrame2USProbeFrameMatrix4x4;
	CreatedTransformMatrixUSProbeFame2USImageFrame4x4.set_column(2, Column3rdNormalized);
	
	// Apply the calibration result to the data positions
	for( int Layer = 0; Layer < 2; Layer++ )
	{
		// 1. The projected position in the US probe frame
		//    after applying the calibration matrix
		vnl_vector<double> ProjectedPositionInUSProbeFrame = 
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			DataPositionsInUSImageFrame.at(Layer);

		// PRE3D_USProbeFrame = (projected position - true position) in US probe frame.
		vnl_vector<double> PRE3DInUSProbeFrameProjected2True = 			
			ProjectedPositionInUSProbeFrame - DataPositionsInUSProbeFrame.at(Layer);
		// Take the absolute value of PRE3D in each axis
		vnl_vector<double> PRE3DInUSProbeFrameABS(4,0);
		PRE3DInUSProbeFrameABS.put(0, fabs( PRE3DInUSProbeFrameProjected2True.get(0) ));
		PRE3DInUSProbeFrameABS.put(1, fabs( PRE3DInUSProbeFrameProjected2True.get(1) ));
		PRE3DInUSProbeFrameABS.put(2, fabs( PRE3DInUSProbeFrameProjected2True.get(2) ));
		PRE3DInUSProbeFrameABS.put(3, 0 );

		PRE3DsInUSProbeFrameABS.push_back( PRE3DInUSProbeFrameABS );

		// 2. The projected position in the US image frame
		//    after applying the calibration matrix
		vnl_vector<double> ProjectedPositionInUSImageFrame = 
			CreatedTransformMatrixUSProbeFame2USImageFrame4x4 * 
			DataPositionsInUSProbeFrame.at( Layer );
		ProjectedPositionsInUSImageFrame.push_back( ProjectedPositionInUSImageFrame );
	}

	// STEP-3. Generate the returning data container
	// FORMAT: PRE3Ds are averaged for all the data positions in the image and 
	// given in the following format:
	// [vector 0-2: Averaged PRE3D (x, y, z) in the US probe frame in X,Y and Z axis ]
	// [vector 3-6: Back-projected positions in the US image frame, X1, Y1, X2, Y2 ]

	// Averaged PRE3D in the US probe frame (x, y, z, 0)
	vnl_vector<double> AvgPRE3DinUSProbeFrameABS = 0.5 *
		( PRE3DsInUSProbeFrameABS.at(0) + PRE3DsInUSProbeFrameABS.at(1) );
	ReturningDataContainer.push_back( AvgPRE3DinUSProbeFrameABS.get(0) );
	ReturningDataContainer.push_back( AvgPRE3DinUSProbeFrameABS.get(1) );
	ReturningDataContainer.push_back( AvgPRE3DinUSProbeFrameABS.get(2) );

	// Back-projected positions in the US image frame (X1, Y1, X2, Y2)
	ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(0).get(0) );
	ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(0).get(1) );
	ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(1).get(0) );
	ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(1).get(1) );

	return ReturningDataContainer;
}

//-----------------------------------------------------------------------------

void vtkCalibrationController::resetDataContainers()
{
	LOG_TRACE("vtkCalibrationController::resetDataContainers");

  // Initialize flags
  this->CalibrationDoneOff();
  this->SetCalibrationDate(NULL);
  
  // Initialize flags
	mArePRE3DsForValidationPositionsReady = false;
	mAreIndependentPointLineReconErrorsReady = false;
  mHasBeenCalibrated = false;
  mAreValidationDataMatricesConstructed = false;
	mAreOutliersRemoved = false; 

	// Initialize data containers
	mTransformMatrixPhantom2DRB4x4.set_size(4,4);

  mDataPositionsInPhantomFrame.resize(0);
	mDataPositionsInUSProbeFrame.resize(0);
	mDataPositionsInUSImageFrame.resize(0);
	mOutlierDataPositions.resize(0); 
	
	mWeightsForDataPositions.resize(0);
	mUSBeamWidthEuclideanMagAtDataPositions.resize(0);
	mUSBeamWidthEuclideanMagAtValidationPositions.resize(0);
	mValidationPositionsInPhantomFrame.resize(0);
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

	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
	mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_size(0,0);
	mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_size(0,0);
	mPLDEsforValidationPositionsInUSProbeFrame.set_size(0);
	mSortedPLDEsAscendingforValidationInUSProbeFrame.set_size(0);
}

//-----------------------------------------------------------------------------

void vtkCalibrationController::FillUltrasoundBeamwidthAndWeightFactorsTable()
{
	LOG_TRACE("vtkCalibrationController::setUltrasoundBeamwidthAndWeightFactorsTable");

	// Populate the beam-width data accordingly
	int numOfTotalBeamWidthData = this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.columns();

	// Fetch the data at the nearest and farest axial depth for fast access 
	mTheNearestAxialDepthInUSBeamwidthAndWeightTable = 
		ROUND(InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(0,0));
	mTheFarestAxialDepthInUSBeamwidthAndWeightTable = 
		ROUND(InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(0, numOfTotalBeamWidthData-1));

	// Populate the 3D beamwidth elements at the nearest and farest axial depth
	mUS3DBeamwidthAtNearestAxialDepth.put(0,
		InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,0));
	mUS3DBeamwidthAtNearestAxialDepth.put(1,
		InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,0));
	mUS3DBeamwidthAtNearestAxialDepth.put(2,
		InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,0));

	mUS3DBeamwidthAtFarestAxialDepth.put(0,
		InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,numOfTotalBeamWidthData-1));
	mUS3DBeamwidthAtFarestAxialDepth.put(1,
		InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,numOfTotalBeamWidthData-1));
	mUS3DBeamwidthAtFarestAxialDepth.put(2,
		InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,numOfTotalBeamWidthData-1));

	mIsUSBeamwidthAndWeightFactorsTableReady = true;

	// Log the data pipeline if requested.
	LOG_DEBUG(" IncorporatingUS3DBeamProfile = " << this->IncorporatingUS3DBeamProfile 
		<< " (1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding)");
	LOG_DEBUG(" numOfTotalBeamWidthData = " 
		<< numOfTotalBeamWidthData);
	LOG_DEBUG(" MinElevationBeamwidthAndFocalZoneInUSImageFrame [Focal Zone (US Image Frame), Elevation Beamwidth] = " 
		<< this->MinElevationBeamwidthAndFocalZoneInUSImageFrame);
	LOG_DEBUG(" SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN = " 
		<< this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN);
	LOG_DEBUG(" InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM (interpolated) = " 
		<< this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM);
	LOG_DEBUG(" mTheNearestAxialDepthInUSBeamwidthAndWeightTable = " 
		<< mTheNearestAxialDepthInUSBeamwidthAndWeightTable);				
	LOG_DEBUG(" mTheFarestAxialDepthInUSBeamwidthAndWeightTable = " 
		<< mTheFarestAxialDepthInUSBeamwidthAndWeightTable);
}

//-----------------------------------------------------------------------------

void vtkCalibrationController::setOutlierFlags()
{
	LOG_TRACE("vtkCalibrationController::setOutlierFlags");

	const double numOfElements = this->mDataPositionsInUSImageFrame.size(); 
	vnl_vector<double> LREx(numOfElements); 
	vnl_vector<double> LREy(numOfElements); 

	// Get LRE for each calibration data 
	for ( int i = 0; i < numOfElements; i++ )
	{
		vnl_vector<double> LRExy = this->getPointLineReconstructionError(
			this->mDataPositionsInUSImageFrame[i], 
			this->mDataPositionsInUSProbeFrame[i] ); 

		LREx.put(i, LRExy.get(0)); 
		LREy.put(i, LRExy.get(1)); 
	}

	// Compute LRE mean
	double LRExMean = LREx.mean(); 
	double LREyMean = LREy.mean(); 

	// Compute LRE std
	vnl_vector<double> diffFromMeanX = LREx - LRExMean; 
	double LRExStd = sqrt(	diffFromMeanX.squared_magnitude() / numOfElements );

	vnl_vector<double> diffFromMeanY = LREy - LREyMean; 
	double LREyStd = sqrt(	diffFromMeanY.squared_magnitude() / numOfElements );

	// find outliers
	for ( int i = 0; i < numOfElements; i++ )
	{
		if ( abs(LREx.get(i) - LRExMean) > mOutlierDetectionThreshold * LRExStd 
			|| 
			abs(LREy.get(i) - LREyMean) > mOutlierDetectionThreshold * LREyStd )
		{
			// Add position to the outlier list
			this->mOutlierDataPositions.push_back(i); 
		}
	}

	this->mAreOutliersRemoved = true;
}

//-----------------------------------------------------------------------------

void vtkCalibrationController::resetOutlierFlags()
{
	LOG_TRACE("vtkCalibrationController::resetOutlierFlags");

	this->mOutlierDataPositions.clear(); 

	this->mAreOutliersRemoved = false; 
}

//-----------------------------------------------------------------------------

PlusStatus vtkCalibrationController::calibrate()
{
	LOG_TRACE("vtkCalibrationController::calibrate");

	if( true == mHasBeenCalibrated )
	{
		mHasBeenCalibrated = false;
	}

	// STEP-0 Copy original data positions and remove outliers 
	for ( std::vector<int>::reverse_iterator rit = mOutlierDataPositions.rbegin();  rit != mOutlierDataPositions.rend(); ++rit )
	{
		mDataPositionsInUSImageFrame.erase( mDataPositionsInUSImageFrame.begin() + *rit );
		mDataPositionsInUSProbeFrame.erase( mDataPositionsInUSProbeFrame.begin() + *rit );
	}

	// STEP-1. Populate the data positions into matrices
	const int TotalNumberOfDataPositionsForCalibration( mDataPositionsInUSImageFrame.size() );
	vnl_matrix<double> DataPositionsInUSImageFrameMatrix4xN(4, TotalNumberOfDataPositionsForCalibration );
	vnl_matrix<double> DataPositionsInUSProbeFrameMatrix4xN(4, TotalNumberOfDataPositionsForCalibration );

	for( int i = 0; i < TotalNumberOfDataPositionsForCalibration; i++ )
	{
		// Populate the data positions in the US image frame
		DataPositionsInUSImageFrameMatrix4xN.set_column( i, mDataPositionsInUSImageFrame.at(i) );
		
		// Populate the data positions in the US probe frame
		DataPositionsInUSProbeFrameMatrix4xN.set_column( i, mDataPositionsInUSProbeFrame.at(i) );
	}

	// STEP-2. Call the LeastSquare Interface from CommonFramework Component
	// T_USImage2Probe * XinUSImageFrame = XinProbeFrame -> solve T
	// NOTE: The transform obtained is carrying the scaling information
	// for the ultrasound image already.

	LinearLeastSquares LeastSquares(
		DataPositionsInUSImageFrameMatrix4xN,		// dataObserver1
		DataPositionsInUSProbeFrameMatrix4xN);		// dataobserver2

	// If the weights for the data positions are available, apply the weights to the optimization
	vnl_vector<double> WeightsForDataPositionsInVNLvectors(0);
	vnl_vector<double> USBeamWidthEuclideanMagAtDataPositionsInVNLvectors(0);
	if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
	{
		// Copy original data and remove outliers 
		std::vector<double> weightsForDataPositions = mWeightsForDataPositions; 
		std::vector<double> usBeamWidthEuclideanMagAtDataPositions = mUSBeamWidthEuclideanMagAtDataPositions;

		for ( std::vector<int>::reverse_iterator rit = mOutlierDataPositions.rbegin();  rit != mOutlierDataPositions.rend(); ++rit )
		{
			weightsForDataPositions.erase( weightsForDataPositions.begin() + *rit );
			usBeamWidthEuclideanMagAtDataPositions.erase( usBeamWidthEuclideanMagAtDataPositions.begin() + *rit );
		}

		if( weightsForDataPositions.size() != TotalNumberOfDataPositionsForCalibration ||
			usBeamWidthEuclideanMagAtDataPositions.size() != TotalNumberOfDataPositionsForCalibration )
		{
			LOG_ERROR("The number of weights and beamwidth data does NOT match the number of data for calibration!");
			return PLUS_FAIL;
		}

		// Populate the weights to vnl_vector format
		WeightsForDataPositionsInVNLvectors.set_size( TotalNumberOfDataPositionsForCalibration );
		USBeamWidthEuclideanMagAtDataPositionsInVNLvectors.set_size( TotalNumberOfDataPositionsForCalibration );
		for( int i = 0; i < TotalNumberOfDataPositionsForCalibration; i++ )
		{
			WeightsForDataPositionsInVNLvectors.put(i, weightsForDataPositions.at(i));
			USBeamWidthEuclideanMagAtDataPositionsInVNLvectors.put(i, 
				usBeamWidthEuclideanMagAtDataPositions.at(i));
		}

		LeastSquares.setDataWeights( WeightsForDataPositionsInVNLvectors );
	}

  try {
	  LeastSquares.doOptimization();
  } catch (...) {
    LOG_ERROR("Optimiaztion failed!");
    return PLUS_FAIL;
  }

	// Output to the resulting matrix
	mTransformUSImageFrame2USProbeFrameMatrix4x4 = LeastSquares.getTransform();

	if ( mAreOutliersRemoved == true )
	{
    return PLUS_SUCCESS; 
	}

	// eliminate outliers from calibration 
	this->setOutlierFlags(); 

	// do a re-calibration without outliers
	this->calibrate(); 

	// reset the outlier flags 
	this->resetOutlierFlags(); 

	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row(3, lastRow);

	// Reset the calibration flag
	mHasBeenCalibrated = true;

  return PLUS_SUCCESS;
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
	vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix4xN =
		mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
		mValidationPositionsInUSImageFrameMatrix4xN;

	// Obtain the PRE3D matrix:
	// PRE3D_matrix = ( Projected - True ) positions in US probe frame
	mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN = 
		ProjectedPositionsInUSProbeFrameMatrix4xN - 
		mValidationPositionsInUSProbeFrameMatrix4xN;

	// Make sure the last row (4th) in the PRE3D matrix is all zeros
	const vnl_vector<double> AllZerosVector( NumberOfValidationPositions, 0 );
	mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_row(3, AllZerosVector);

	// Sort the PRE3D in an ascending order with respect to the euclidean PRE3D distance
	// =============================================================================

	mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN =
		mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN;

	// First calculate the euclidean PRE3D distance from PRE3D
	// vector and store them in the fourth row of the matrix.
	for( int i = 0; i < NumberOfValidationPositions; i++ )
	{
		mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.put(
			3, i, 
			mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
	}

	// Populate the last row of the raw PRE3D matrix with the euclidean distance as well.
	mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_row( 3, 
		mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_row(3) );

	// Sorting the PRE3D matrix w.r.t the  euclidean PRE3D distance (the 4th row)
	// Sorting algorithm employed: Insertion Sort
	for( int i = 0; i < NumberOfValidationPositions; i++ )
		for( int j = i; j > 0 && 
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get(3, j-1) > 
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> SwapPRE3DColumnVector = 
				mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_column( j-1, 
				mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_column(j) );

			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_column( j, 
				SwapPRE3DColumnVector ); 
		}

	// We are only interested at the top-ranked ascending PRE3D values
	// presented in the sorted validation result matrix.
	// Default percentage: 95%
	const int NumberOfTopRankedCalibrationData = 
		ROUND( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel );

	// We only need absolute values of PRE3D to average
	vnl_matrix<double> AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN = 
		mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_n_columns( 
		0, NumberOfTopRankedCalibrationData ).apply( fabs );

	// Obtain PRE3Ds in X-, Y-, and Z-axis
	vnl_vector<double> AbsPRE3DsinX =  
		AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 0 );
	vnl_vector<double> AbsPRE3DsinY =  
		AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 1 );
	vnl_vector<double> AbsPRE3DsinZ =  
		AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 2 );

	// Perform statistical analysis
	// =============================

	// X-axis PRE3D statistics
	double PRE3D_X_mean = AbsPRE3DsinX.mean();
	double PRE3D_X_rms = AbsPRE3DsinX.rms();
	double PRE3D_X_std = sqrt( 
		1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
		( AbsPRE3DsinX.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_X_mean,2) )
		);

	// Y-axis PRE3D statistics
	double PRE3D_Y_mean = AbsPRE3DsinY.mean();
	double PRE3D_Y_rms = AbsPRE3DsinY.rms();
	double PRE3D_Y_std = sqrt( 
		1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
		( AbsPRE3DsinY.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_Y_mean,2) )
		);

	// Z-axis PRE3D statistics
	double PRE3D_Z_mean = AbsPRE3DsinZ.mean();
	double PRE3D_Z_rms = AbsPRE3DsinZ.rms();
	double PRE3D_Z_std = sqrt( 
		1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
		( AbsPRE3DsinZ.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_Z_mean,2) )
		);
	
	// Consolidate the returning data container
	// =========================================

	// FORMAT: (all positions are in the US probe frame)
	// [ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]
	// [ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]
	// [ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]
	// [ vector 9	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - rms : root mean square;
	// - std : standard deviation.
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_X_mean );
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_X_rms );
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_X_std );

	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Y_mean );
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Y_rms );
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Y_std );

	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Z_mean );
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Z_rms );
	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Z_std );

	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

	// Reset the flag
	mArePRE3DsForValidationPositionsReady = true;

	// Log the data pipeline if requested.
	LOG_DEBUG("3D Point Reconstruction Error (PRE3D)");
  LOG_DEBUG("PRE3D mean: (" << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[3] << ", " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[6] << ")");
  LOG_DEBUG("PRE3D rms: (" << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[1] << ", " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[4] << ", " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[7] << ")");
  LOG_DEBUG("PRE3D std: (" << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[2] << ", " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[5] << ", " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[8] << ")");
	LOG_DEBUG("Validation Data Confidence Level = " << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[9] << " ( " << NumberOfTopRankedCalibrationData << " top-ranked validation data were used out of the total " << NumberOfValidationPositions << " validation data set for the above statistical analysis)");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vnl_vector<double> vtkCalibrationController::getPointLineReconstructionError(vnl_vector<double> NWirePositionInUSImageFrame, 
			vnl_vector<double> NWirePositionInUSProbeFrame)
{
	LOG_TRACE("vtkCalibrationController::getPointLineReconstructionError");

	vnl_vector<double> NWireProjectedPositionsInUSProbeFrame =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * NWirePositionInUSImageFrame;

	vnl_vector<double> NWireLREOrigInUSProbeFrame = 
			NWirePositionInUSProbeFrame - NWireProjectedPositionsInUSProbeFrame;

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
	// ============================================================================

	// Using the calibration matrix, we can obtain the projected positions
	// in the US probe frame after applying the calibration matrix to
	// the validation positions in the US image frame.
	
	// NWire-1 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire1ProjectedPositionsInUSProbeFrameMatrix4xN =
		mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
		mValidationPositionsNWire1InUSImageFrame4xN;

	// NWire-3 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire3ProjectedPositionsInUSProbeFrameMatrix4xN =
		mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
		mValidationPositionsNWire3InUSImageFrame4xN;

	// NWire-4 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire4ProjectedPositionsInUSProbeFrameMatrix4xN =
		mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
		mValidationPositionsNWire4InUSImageFrame4xN;

	// NWire-6 (note only x, y coordinates are of interests to us)
	const vnl_matrix<double> NWire6ProjectedPositionsInUSProbeFrameMatrix4xN =
		mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
		mValidationPositionsNWire6InUSImageFrame4xN;

	// Now we calculate the independent line reconstrution errors (in X and Y axes)
	// for NWires N1, N3, N4, N6.
	// Make sure the 3rd row (z-axis) in the LRE matrix is all zeros
	const vnl_vector<double> AllZerosVector( NumberOfValidationPositions, 0 );

	// NWire-1 LREs
	mNWire1LREOrigInUSProbeFrameMatrix4xN = 
		mValidationPositionsNWire1InUSProbeFrame4xN - 
		NWire1ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire1LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

	// NWire-3 LREs
	mNWire3LREOrigInUSProbeFrameMatrix4xN = 
		mValidationPositionsNWire3InUSProbeFrame4xN - 
		NWire3ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire3LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);
	
	// NWire-4 LREs
	mNWire4LREOrigInUSProbeFrameMatrix4xN = 
		mValidationPositionsNWire4InUSProbeFrame4xN - 
		NWire4ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire4LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

	// NWire-6 LREs
	mNWire6LREOrigInUSProbeFrameMatrix4xN = 
		mValidationPositionsNWire6InUSProbeFrame4xN - 
		NWire6ProjectedPositionsInUSProbeFrameMatrix4xN;
	mNWire6LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

	// Now we calculate the euclidean LRE distance from LRE
	// vector and store them in the fourth row of the matrix.
	for( int i = 0; i < NumberOfValidationPositions; i++ )
	{
		mNWire1LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire1LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

		mNWire3LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire3LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

		mNWire4LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire4LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

		mNWire6LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire6LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
	}

	// Sorting the LRE matrice w.r.t the  euclidean distance (the 4th row)
	// Sorting algorithm employed: Insertion Sort

	mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN = 
		mNWire1LREOrigInUSProbeFrameMatrix4xN;
	mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN = 
		mNWire3LREOrigInUSProbeFrameMatrix4xN;
	mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN = 
		mNWire4LREOrigInUSProbeFrameMatrix4xN;
	mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN = 
		mNWire6LREOrigInUSProbeFrameMatrix4xN;

	// NWire-1
	for( int i = 0; i < NumberOfValidationPositions; i++ )
		for( int j = i; j > 0 && 
			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = 
				mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
				mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
				Swap3DColumnVector ); 
		}
		
	// NWire-3
	for( int i = 0; i < NumberOfValidationPositions; i++ )
		for( int j = i; j > 0 && 
			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = 
				mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
				mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
				Swap3DColumnVector ); 
		}

	// NWire-4
	for( int i = 0; i < NumberOfValidationPositions; i++ )
		for( int j = i; j > 0 && 
			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = 
				mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
				mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
				Swap3DColumnVector ); 
		}

	// NWire-6
	for( int i = 0; i < NumberOfValidationPositions; i++ )
		for( int j = i; j > 0 && 
			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const vnl_vector<double> Swap3DColumnVector = 
				mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
			
			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
				mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
				Swap3DColumnVector ); 
		}			

	// NOTES: 
	// 1. We are only interested in the absolute value of the errors.
	// 2. We will perform analysis on all and the top-ranked ascending LRE values
	//    presented in the sorted validation result matrix.  This will effectively 
	//    get rid of outliers in the validation data set (e.g., caused by erroneous 
	//    segmentation results in the validation data).
	// Default percentage: 95%

	const int NumberOfTopRankedData = 
		floor( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel + 0.5 );

	const vnl_matrix<double> NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN = 
		mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
		0, NumberOfTopRankedData ).apply( fabs );
	const vnl_matrix<double> NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN = 
		mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
		0, NumberOfTopRankedData ).apply( fabs );
	const vnl_matrix<double> NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN = 
		mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
		0, NumberOfTopRankedData ).apply( fabs );
	const vnl_matrix<double> NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN = 
		mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
		0, NumberOfTopRankedData ).apply( fabs );

	// Perform statistical analysis

	// NWire-1: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire1AbsLREDistributionInX = 
		NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire1AbsLREDistributionInY = 
		NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire1AbsLREDistributionInEUC = 
		NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire1LRE_X_mean = NWire1AbsLREDistributionInX.mean();
	const double NWire1LRE_X_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire1AbsLREDistributionInX.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire1LRE_X_mean,2 )) );

	const double NWire1LRE_Y_mean = NWire1AbsLREDistributionInY.mean();
	const double NWire1LRE_Y_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire1AbsLREDistributionInY.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire1LRE_Y_mean,2 )) );

	const double NWire1LRE_EUC_mean = NWire1AbsLREDistributionInEUC.mean();
	const double NWire1LRE_EUC_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire1AbsLREDistributionInEUC.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire1LRE_EUC_mean,2 )) );

	// NWire-3: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire3AbsLREDistributionInX = 
		NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire3AbsLREDistributionInY = 
		NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire3AbsLREDistributionInEUC = 
		NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire3LRE_X_mean = NWire3AbsLREDistributionInX.mean();
	const double NWire3LRE_X_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire3AbsLREDistributionInX.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire3LRE_X_mean,2 )) );

	const double NWire3LRE_Y_mean = NWire3AbsLREDistributionInY.mean();
	const double NWire3LRE_Y_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire3AbsLREDistributionInY.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire3LRE_Y_mean,2 )) );

	const double NWire3LRE_EUC_mean = NWire3AbsLREDistributionInEUC.mean();
	const double NWire3LRE_EUC_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire3AbsLREDistributionInEUC.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire3LRE_EUC_mean,2 )) );						
		
	// NWire-4: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire4AbsLREDistributionInX = 
		NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire4AbsLREDistributionInY = 
		NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire4AbsLREDistributionInEUC = 
		NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire4LRE_X_mean = NWire4AbsLREDistributionInX.mean();
	const double NWire4LRE_X_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire4AbsLREDistributionInX.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire4LRE_X_mean,2 )) );

	const double NWire4LRE_Y_mean = NWire4AbsLREDistributionInY.mean();
	const double NWire4LRE_Y_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire4AbsLREDistributionInY.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire4LRE_Y_mean,2 )) );

	const double NWire4LRE_EUC_mean = NWire4AbsLREDistributionInEUC.mean();
	const double NWire4LRE_EUC_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire4AbsLREDistributionInEUC.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire4LRE_EUC_mean,2 )) );						

	// NWire-6: LRE in X, Y and Euclidean (norm)
	// =========================================
	const vnl_vector<double> NWire6AbsLREDistributionInX = 
		NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
	const vnl_vector<double> NWire6AbsLREDistributionInY = 
		NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
	const vnl_vector<double> NWire6AbsLREDistributionInEUC = 
		NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

	const double NWire6LRE_X_mean = NWire6AbsLREDistributionInX.mean();
	const double NWire6LRE_X_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire6AbsLREDistributionInX.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire6LRE_X_mean,2 )) );

	const double NWire6LRE_Y_mean = NWire6AbsLREDistributionInY.mean();
	const double NWire6LRE_Y_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire6AbsLREDistributionInY.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire6LRE_Y_mean,2 )) );

	const double NWire6LRE_EUC_mean = NWire6AbsLREDistributionInEUC.mean();
	const double NWire6LRE_EUC_std = sqrt( 
		1/(double)( NumberOfTopRankedData - 1 ) *	
		( NWire6AbsLREDistributionInEUC.squared_magnitude() - 
		NumberOfTopRankedData*pow( NWire6LRE_EUC_mean,2 )) );									

	// Consolidate the returning data containers
	// FORMAT: (all positions are in the US probe frame)
	// For NWire N1, N3, N4, N6:
	// [ vector 0 - 1:  LRE_X_mean,   LRE_X_std   ]
	// [ vector 2 - 3:  LRE_Y_mean,   LRE_Y_std   ]
	// [ vector 4 - 5:  LRE_EUC_mean, LRE_EUC_std ]
	// [ vector 6	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - std : standard deviation;
	// - EUC: Euclidean (norm) measurement.

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
	const vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix4xN =
		mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
		mValidationPositionsInUSImageFrameMatrix4xN;
	// We need the 3D coordinates (the first 3 elements) for cross product
	const vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix3xN =
		ProjectedPositionsInUSProbeFrameMatrix4xN.get_n_rows(0,3);

	// Now we calculate the Point-Line Distance Error (PLDE).  		
	// The PLDE was defined as the absolute point-line distance from the projected
	// positions to the N-Wire (the ground truth), both in the US probe frame.  
	// If there was no error, the PLDE would be zero and the projected postions
	// would reside right on the N-Wire.  The physical position of the N-Wire
	// was measured based on the phantom geometry and converted into the US
	// US probe frame by the optical tracking device affixed on the phantom.
	// NOTE: this data may be used for statistical analysis if desired.
	// FORMAT: vector 1xN (with N being the total number of validation positions)
	mPLDEsforValidationPositionsInUSProbeFrame.set_size( NumberOfValidationPositions );

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
		// 
		const double ProjectedPosition2NWireDistance = 
			vnl_cross_3d(NWireVector3D,ProjectedPosition2NWireStartVector3D).magnitude()/
			NWireVector3D.magnitude();

		mPLDEsforValidationPositionsInUSProbeFrame.put(i,
			ProjectedPosition2NWireDistance );

	}

	// Sort the PLDE in an ascending order

	mSortedPLDEsAscendingforValidationInUSProbeFrame = 
		mPLDEsforValidationPositionsInUSProbeFrame;

	// Sorting algorithm employed: Insertion Sort
	for( int i = 0; i < NumberOfValidationPositions; i++ )
		for( int j = i; j > 0 && 
			mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j-1) > 
			mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j); j-- )
		{
			// Swap the positions of j-th and j-1-th elements
			const double SwapPLDEValue = 
				mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j-1);
			
			mSortedPLDEsAscendingforValidationInUSProbeFrame.put( j-1, 
				mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j) );

			mSortedPLDEsAscendingforValidationInUSProbeFrame.put( j, 
				SwapPLDEValue ); 

		}

	// We are only interested at the top-ranked ascending PLDE values
	// presented in the sorted PLDE result vector.
	// Default percentage: 95%
	const int NumberOfTopRankedCalibrationData = 
		ROUND( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel );
	const vnl_vector<double> AscendingTopRankingPLDEsinUSProbeFrame = 
		mSortedPLDEsAscendingforValidationInUSProbeFrame.extract(
			NumberOfTopRankedCalibrationData );

	// Perform statistical analysis
	const double PLDE_mean = AscendingTopRankingPLDEsinUSProbeFrame.mean();
	const double PLDE_rms = AscendingTopRankingPLDEsinUSProbeFrame.rms();
	const double PLDE_std = sqrt( 
		1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
		( AscendingTopRankingPLDEsinUSProbeFrame.squared_magnitude() - 
		NumberOfTopRankedCalibrationData*pow(PLDE_mean,2) ) );

	// Consolidate the returning data container

	// FORMAT: (all positions are in the US probe frame)
	// [ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]
	// [ vector 3	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - rms : root mean square;
	// - std : standard deviation.
	// - validation data confidence level: this is a percentage of 
	//   the independent validation data used to produce the final
	//   validation results.  It serves as an effective way to get 
	//   rid of corrupted data (or outliers) in the validation 
	//   dataset.  Default value: 0.95 (or 95%), meaning the top 
	//   ranked 95% of the ascendingly-ordered results from the 
	//   validation data would be accepted as the valid error values.
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( PLDE_mean );
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( PLDE_rms );
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( PLDE_std );
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

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
  LOG_DEBUG("  PLDE mean: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[0]);
  LOG_DEBUG("  PLDE rms: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[1]);
  LOG_DEBUG("  PLDE std: " << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[2]);
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
		mValidationPositionsInUSImageFrameMatrix4xN.set_column(
			i, mValidationPositionsInUSImageFrame.at(i) );

		// Validation positions in the US probe frame
		mValidationPositionsInUSProbeFrameMatrix4xN.set_column(
			i, mValidationPositionsInUSProbeFrame.at(i) );

		// Validation points' NWire Start/End positions in US probe frame
		ValidationPositionsNWireStartInUSProbeFrame4xN.set_column(
			i, mValidationPositionsNWireStartInUSProbeFrame.at(i) );
		ValidationPositionsNWireEndInUSProbeFrame4xN.set_column(
			i, mValidationPositionsNWireEndInUSProbeFrame.at(i) );

		// Validation positions of the parallel wires in the US image frame
		// Wire N1
		mValidationPositionsNWire1InUSImageFrame4xN.set_column(
			i, mValidationPositionsNWire1InUSImageFrame.at(i) );
		// Wire N3
		mValidationPositionsNWire3InUSImageFrame4xN.set_column(
			i, mValidationPositionsNWire3InUSImageFrame.at(i) );
		// Wire N4
		mValidationPositionsNWire4InUSImageFrame4xN.set_column(
			i, mValidationPositionsNWire4InUSImageFrame.at(i) );
		// Wire N6
		mValidationPositionsNWire6InUSImageFrame4xN.set_column(
			i, mValidationPositionsNWire6InUSImageFrame.at(i) );

		// Validation positions of the parallel wires in the US probe frame
		// Wire N1
		mValidationPositionsNWire1InUSProbeFrame4xN.set_column(
			i, mValidationPositionsNWire1InUSProbeFrame.at(i) );
		// Wire N3
		mValidationPositionsNWire3InUSProbeFrame4xN.set_column(
			i, mValidationPositionsNWire3InUSProbeFrame.at(i) );
		// Wire N4
		mValidationPositionsNWire4InUSProbeFrame4xN.set_column(
			i, mValidationPositionsNWire4InUSProbeFrame.at(i) );
		// Wire N6
		mValidationPositionsNWire6InUSProbeFrame4xN.set_column(
			i, mValidationPositionsNWire6InUSProbeFrame.at(i) );
	}
	
	// For PLDE calcuation, we need the first 3 elements for cross products.
	mValidationPositionsNWireStartInUSProbeFrame3xN =
		ValidationPositionsNWireStartInUSProbeFrame4xN.get_n_rows(0, 3);
	mValidationPositionsNWireEndInUSProbeFrame3xN =
		ValidationPositionsNWireEndInUSProbeFrame4xN.get_n_rows(0, 3);

	// OPTION: populate the beamwidth data if selected (Option-1,2 or 3)
	vnl_vector<double> USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors(0);
	if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
	{
		if( mUSBeamWidthEuclideanMagAtValidationPositions.size() != NumberOfValidationPositions )
		{
			LOG_ERROR("The number of beamwidth data does NOT match the number of data for validation!");
      return PLUS_FAIL;
		}		
		
		// Populate the data to vnl_vector format
		USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors.set_size( NumberOfValidationPositions );
		for( int i = 0; i < NumberOfValidationPositions; i++ )
		{
			USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors.put(i, 
				mUSBeamWidthEuclideanMagAtValidationPositions.at(i));
		}
	}

	mAreValidationDataMatricesConstructed = true;  //check the return value instead

  return PLUS_SUCCESS;
}

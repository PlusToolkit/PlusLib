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
  this->Calibrator = NULL;
	this->EnableSystemLogOff();
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

	//vtkSmartPointer<vtkTransform> transformProbeToUserImage = vtkSmartPointer<vtkTransform>::New(); 
	//this->TransformProbeToUserImage = NULL;
	//this->SetTransformProbeToUserImage(transformProbeToUserImage); 

	//vtkSmartPointer<vtkTransform> transformUserImageToImage = vtkSmartPointer<vtkTransform>::New(); 
	//this->TransformUserImageToImage = NULL;
	//this->SetTransformUserImageToImage(transformUserImageToImage); 

	//vtkSmartPointer<vtkTransform> transformTemplateHolderHomeToTemplateHolder = vtkSmartPointer<vtkTransform>::New(); 
	//this->TransformTemplateHolderHomeToTemplateHolder = NULL;
	//this->SetTransformTemplateHolderHomeToTemplateHolder(transformTemplateHolderHomeToTemplateHolder); 

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
  //this->TransformImageToTemplate->Concatenate(this->TransformTemplateHomeToTemplate);
  this->TransformImageToTemplate->Update(); 

	// Initialize calibration controller IO
	vtkSmartPointer<vtkProbeCalibrationControllerIO> calibrationControllerIO = vtkSmartPointer<vtkProbeCalibrationControllerIO>::New(); 
	calibrationControllerIO->Initialize( this ); 
	this->SetCalibrationControllerIO( calibrationControllerIO ); 

	// Initialize data containers
	this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN.set_size(0,0);
	this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.set_size(0,0); 
	this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);
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
	//this->SetTransformProbeToUserImage(NULL);
	//this->SetTransformUserImageToImage(NULL);
	this->SetTransformReferenceToTemplateHolderHome(NULL);
	this->SetTransformTemplateHolderToTemplate(NULL);
  this->SetTransformTemplateHolderToPhantom(NULL); 
	//this->SetTransformTemplateHolderHomeToTemplateHolder(NULL);
	this->SetTransformTemplateHomeToTemplate(NULL);
	this->SetCalibrationControllerIO(NULL); 

	// Destroy the calibration phantom object
	if( Calibrator != NULL )
	{
		delete Calibrator;
		Calibrator = NULL;
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkCalibrationController::Initialize()
{
	LOG_TRACE("vtkCalibrationController::Initialize"); 

	// Initialize the calibration component
	if ( this->Calibrator == NULL ) 
	{
		this->Calibrator = new BrachyTRUSCalibrator(this->GetPatternRecognition(), this->EnableSystemLog );
	}

	vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
	PlusMath::ConvertVtkMatrixToVnlMatrix(this->TransformImageToUserImage->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 
	this->Calibrator->setTransformOrigImageToTRUSImageFrame4x4( transformOrigImageFrame2TRUSImageFrameMatrix4x4 );

	// This will pass the US 3D beamwidth data and their predefined
	// weights to the calibration component.
	if( this->US3DBeamwidthDataReady )
	{
		this->Calibrator->setUltrasoundBeamwidthAndWeightFactorsTable(
			this->IncorporatingUS3DBeamProfile,
			*this->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM(),
			*this->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN(),
			*this->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() );
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
	  itk::Image<unsigned char, 2>::Pointer image=trackedFrame->ImageData.GetImage<unsigned char>();
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
      if (trackedFrame->ImageData.GetITKScalarPixelType()==itk::ImageIOBase::UCHAR)
      {
        this->PatternRecognition.DrawResults( static_cast<PixelType*>(trackedFrame->ImageData.GetBufferPointer()) ); // :TODO: DrawResults should use an ITK image as input
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

			ConvertVtkMatrixToVnlMatrixInMeter(tProbeToReferenceMatrix, transformProbeToReferenceMatrix4x4); 

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
void vtkCalibrationController::ConvertVtkMatrixToVnlMatrixInMeter(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix )
{
	LOG_TRACE("vtkCalibrationController::ConvertVtkMatrixToVnlMatrixInMeter"); 
  PlusMath::ConvertVtkMatrixToVnlMatrix(inVtkMatrix, outVnlMatrix); 

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
PlusStatus vtkCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkCalibrationController::ReadConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to read configuration"); 
		return PLUS_FAIL; 
	}

  //Setting the fiducial pattern recognition
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

	// To enable/disable the system logging
	const char* enableLogFile = probeCalibration->GetAttribute("EnableLogFile"); 
	if (enableLogFile != NULL &&  STRCASECMP( "TRUE", enableLogFile ) == 0) {
		this->EnableSystemLogOn(); 
	} else {
		this->EnableSystemLogOff(); 
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
	calibration->SetDoubleAttribute("Error", GetPointLineDistanceErrorAnalysisVector()[0] * 1000.0); // TODO find the best error number

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
	this->ConvertVtkMatrixToVnlMatrixInMeter(aPhantomToPhantomReferenceTransform->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode);

	this->Calibrator->registerPhantomGeometryInEmulatorMode(transformMatrixPhantom2DRB4x4InEmulatorMode);
}

//----------------------------------------------------------------------------
void vtkCalibrationController::RegisterPhantomGeometry( double phantomToProbeDistanceInMm[2] )
{
	LOG_TRACE("vtkCalibrationController::RegisterPhantomGeometry: " << phantomToProbeDistanceInMm[0] << "  " << phantomToProbeDistanceInMm[1]); 

  // Vertical distance from the template mounter hole center to the TRUS Rotation Center
  double verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
    this->GetPatternRecognition()->GetFidLineFinder()->GetNWires()[1].wires[0].endPointFront[1] // WIRE1 y
        + phantomToProbeDistanceInMm[1]
        - this->TransformTemplateHolderToPhantom->GetPosition()[1]; // :TODO: transform with the whole matrix instead of just using the XY position values

  // Horizontal distance from the template mounter hole center to the TRUS Rotation Center
  double horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
      this->GetPatternRecognition()->GetFidLineFinder()->GetNWires()[0].wires[2].endPointFront[0] // WIRE3 x
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
  ConvertVtkMatrixToVnlMatrixInMeter( tTemplateHomeToReference->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode ); 

  this->Calibrator->registerPhantomGeometryInEmulatorMode( transformMatrixPhantom2DRB4x4InEmulatorMode );
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

  // If calibrator is present, delete it so that it can be re-initialized when calibration is started again
	if (this->Calibrator != NULL) {
		delete this->Calibrator;
		this->Calibrator = NULL;
	}

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
    this->Calibrator->resetDataContainers(); 

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

        this->SetOfflineImageData(validationData->GetTrackedFrame(vImgNumber)->ImageData.GetDisplayableImage()); 
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

        this->SetOfflineImageData(calibrationData->GetTrackedFrame(cImgNumber)->ImageData.GetDisplayableImage()); 
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

//----------------------------------------------------------------------------
void vtkCalibrationController::DoCalibration()
{
	LOG_TRACE("vtkCalibrationController::DoCalibration"); 
	// Instruct the calibrator to perform the calibration task
	this->Calibrator->calibrate();

	// Instruct the calibrator to validate the calibration accuracy
	this->Calibrator->compute3DPointReconstructionError();
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

		// Do final calibration 
		this->DoCalibration(); 

		// Get the image home to probe home transformation from the calibrator
		vnl_matrix<double> calibResultMatrix = this->Calibrator->getCalibrationResultsInMatrix(); 
		vtkSmartPointer<vtkMatrix4x4> userImageToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

		// convert transform from meter to mm
		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				userImageToProbeMatrix->SetElement(i, j, calibResultMatrix.get(i, j) * 1000 ); 
			}
		}
		
		// Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
		double xVector[3] = {userImageToProbeMatrix->GetElement(0,0),userImageToProbeMatrix->GetElement(1,0),userImageToProbeMatrix->GetElement(2,0)}; 
		double yVector[3] = {userImageToProbeMatrix->GetElement(0,1),userImageToProbeMatrix->GetElement(1,1),userImageToProbeMatrix->GetElement(2,1)};  
		double zVector[3] = {0,0,0}; 

    double dotProduct = vtkMath::Dot(xVector, yVector);
    if (dotProduct > 0.001) 
    {
      LOG_WARNING("Calibration result axes are not orthogonal (dot product of X and Y axes is " << dotProduct << ")");
    }
		
		vtkMath::Cross(xVector, yVector, zVector); 
						
		// make the z vector have about the same length as x an y,
		// so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
		vtkMath::Normalize(zVector);
		double normZ = (vtkMath::Norm(xVector)+vtkMath::Norm(yVector))/2;  
		vtkMath::MultiplyScalar(zVector, normZ);
		
		userImageToProbeMatrix->SetElement(0, 2, zVector[0]);
		userImageToProbeMatrix->SetElement(1, 2, zVector[1]);
		userImageToProbeMatrix->SetElement(2, 2, zVector[2]);

		// Write transformations to log and output
		std::ostringstream osTransformImageToUserImage; 
		this->TransformImageToUserImage->Print(osTransformImageToUserImage);   
		LOG_DEBUG("TransformImageToUserImage:\n" << osTransformImageToUserImage.str().c_str() );

		this->TransformUserImageToProbe->SetMatrix( userImageToProbeMatrix ); 
		std::ostringstream osTransformUserImageToProbe; 
		this->TransformUserImageToProbe->Print(osTransformUserImageToProbe);   
		LOG_DEBUG("TransformUserImageToProbe:\n" << osTransformUserImageToProbe.str().c_str() );

		std::ostringstream osTransformProbeToReference; 
		this->TransformProbeToReference->Print(osTransformProbeToReference);   
		LOG_DEBUG("TransformProbeToReference:\n" << osTransformProbeToReference.str().c_str() );

		//this->GetTransformProbeToUserImage()->SetMatrix( userImageToProbeMatrix ); 
		//this->GetTransformProbeToUserImage()->Inverse(); 

		std::ostringstream osTransformReferenceToTemplateHolderHome; 
		this->TransformReferenceToTemplateHolderHome->Print(osTransformReferenceToTemplateHolderHome);  
		LOG_DEBUG("TransformReferenceToTemplateHolderHome:\n" << osTransformReferenceToTemplateHolderHome.str().c_str() );

		std::ostringstream osTransformTemplateHolderToTemplate; 
		this->TransformTemplateHolderToTemplate->Print(osTransformTemplateHolderToTemplate);  
		LOG_DEBUG("TransformTemplateHolderToTemplate:\n" << osTransformTemplateHolderToTemplate.str().c_str() );

		std::ostringstream osTransformTemplateHomeToTemplate; 
		this->TransformTemplateHomeToTemplate->Print(osTransformTemplateHomeToTemplate);  
		LOG_DEBUG("TransformTemplateHomeToTemplate:\n" << osTransformTemplateHomeToTemplate.str().c_str() );

		std::ostringstream osTransformImageToTemplate; 
		this->TransformImageToTemplate->Print(osTransformImageToTemplate);  
		LOG_DEBUG("TransformImageToTemplate:\n" << osTransformImageToTemplate.str().c_str() );


		// Compute the independent point and line reconstruction errors
		// ============================================================
		LOG_INFO(">>>>>>>> Compute the independent point and line reconstruction errors ...") ;
		this->Calibrator->computeIndependentPointLineReconstructionError();

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

    this->ClearSegmentedFrameContainer(RANDOM_STEPPER_MOTION_1); 
		this->ClearSegmentedFrameContainer(RANDOM_STEPPER_MOTION_2);
    this->ClearSegmentedFrameContainer(FREEHAND_MOTION_1); 
		this->ClearSegmentedFrameContainer(FREEHAND_MOTION_2);

    // Set calibration date
    this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 

    // Save calibration
    if (WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS) {
		  LOG_ERROR("Freehand calibration result could not be saved into session configuration data!");
		  return PLUS_FAIL;
    }

    // save the input images to meta image
    if ( this->EnableTrackedSequenceDataSaving )
    {
      LOG_INFO(">>>>>>>> Save validation data to sequence metafile..."); 
      // TODO add validation file name to config file
      // Save validation dataset
      std::ostringstream validationDataFileName; 
      validationDataFileName << this->Calibrator->getCalibrationTimeStampInString() << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_2).OutputSequenceMetaFileSuffix; 
      if ( this->SaveTrackedFrameListToMetafile( RANDOM_STEPPER_MOTION_2, vtkPlusConfig::GetInstance()->GetOutputDirectory(), validationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
      }

      LOG_INFO(">>>>>>>> Save calibration data to sequence metafile..."); 
      // Save calibration dataset 
      std::ostringstream calibrationDataFileName; 
      calibrationDataFileName << this->Calibrator->getCalibrationTimeStampInString() << this->GetImageDataInfo(RANDOM_STEPPER_MOTION_1).OutputSequenceMetaFileSuffix; 
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
		this->Calibrator->addDataPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
	else if (dataType == RANDOM_STEPPER_MOTION_2 || dataType == FREEHAND_MOTION_2)
	{
		this->Calibrator->addValidationPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
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
    this->Calibrator->GetNWire(nwireIndex).wires[wireIndex].endPointFront[0],
    this->Calibrator->GetNWire(nwireIndex).wires[wireIndex].endPointFront[1],
    this->Calibrator->GetNWire(nwireIndex).wires[wireIndex].endPointFront[2]
  }; 

	// Wire position on the back wall in template coordinate system
  double p2[3] = {
    this->Calibrator->GetNWire(nwireIndex).wires[wireIndex].endPointBack[0],
    this->Calibrator->GetNWire(nwireIndex).wires[wireIndex].endPointBack[1],
    this->Calibrator->GetNWire(nwireIndex).wires[wireIndex].endPointBack[2]
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
void vtkCalibrationController::PrintCalibrationResultsAndErrorReports ()
{
	LOG_TRACE("vtkCalibrationController::PrintCalibrationResultsAndErrorReports"); 
	try
	{
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Calibration result in 4x4 homogeneous transform matrix = ");
		for ( int i = 0; i < 4; i++ )
		{
			std::ostringstream matrixRow; 
			for ( int j = 0; j < 4; j++ )
			{
				matrixRow << this->TransformUserImageToProbe->GetMatrix()->GetElement(i,j) << "  " ;
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
PlusStatus vtkCalibrationController::UpdateLineReconstructionErrorAnalysisVectors()
{
  LOG_TRACE("vtkCalibrationController::UpdateLineReconstructionErrorAnalysisVectors"); 
  this->LineReconstructionErrors.clear(); 
  try 
  {
    // Add wire #1 LRE to map
    this->LineReconstructionErrors[1] = this->Calibrator->getLREAbsAnalysisForNWire1(); 

    // Add wire #3 LRE to map
    this->LineReconstructionErrors[3] = this->Calibrator->getLREAbsAnalysisForNWire3(); 

    // Add wire #4 LRE to map
    this->LineReconstructionErrors[4] = this->Calibrator->getLREAbsAnalysisForNWire4(); 

    // Add wire #6 LRE to map
    this->LineReconstructionErrors[6] = this->Calibrator->getLREAbsAnalysisForNWire6(); 
  }
  catch(...)
  {
    LOG_ERROR("Unable to update line reconstruction error analysis vectors!"); 
    return PLUS_FAIL; 
  }

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
	errorsStringStream << std::fixed << std::setprecision(3) << "  " << GetPointLineDistanceErrorAnalysisVector()[0] * 1000.0 << ", " << GetPointLineDistanceErrorAnalysisVector()[1] * 1000.0 << ", " << GetPointLineDistanceErrorAnalysisVector()[2] * 1000.0 << std::endl;

	std::ostringstream resultStringStream;
	resultStringStream << matrixStringStream.str() << errorsStringStream.str() << std::endl;

	resultStringStream << "Validation data confidence:" << std::endl;
	resultStringStream << "  " << GetPointLineDistanceErrorAnalysisVector()[3];

	return resultStringStream.str();
}

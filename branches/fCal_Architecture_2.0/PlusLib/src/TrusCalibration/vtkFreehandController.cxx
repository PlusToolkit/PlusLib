#include "vtkFreehandController.h"

#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"

#include "vtkTrackerTool.h"
#include "vtkFileFinder.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFCalController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkFCalController);

//----------------------------------------------------------------------------
void vtkFCalController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//-----------------------------------------------------------------------------

vtkFCalController::vtkFCalController()
{
	this->DataCollector = NULL;
	this->RecordingFrameRate = 20;
	this->ConfigurationFileName = NULL;
	this->OutputFolder = NULL;
	this->InitializedOff();
	this->TrackingOnlyOn();

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFCalController::~vtkFCalController()
{
	if (this->DataCollector != NULL) {
		this->DataCollector->Stop();
	}

	this->SetDataCollector(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::Initialize()
{
  LOG_TRACE("vtkFCalController::Initialize"); 

	if (this->Initialized) {
		return PLUS_SUCCESS;
	}

  if (this->LocateDirectories() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to locate directories!");
    return PLUS_FAIL;
  }

	// Create directory for the output
	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ((this->OutputFolder != NULL) && (dir->Open(this->OutputFolder) == 0)) {	
		dir->MakeDirectory(this->OutputFolder);
	}

	this->SetInitialized(true);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkFCalController::GetConfigurationData()
{
	LOG_TRACE("vtkFCalController::GetConfigurationData"); 

	if (this->DataCollector != NULL) {
		return this->DataCollector->GetConfigurationData();
  } else {
    return NULL;
  }
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::InitializeDataCollection()
{
	LOG_TRACE("vtkFCalController::InitializeDataCollection"); 

	// Stop data collection if already started
	if (this->DataCollector != NULL) {
		this->DataCollector->Stop();
	}

	// Initialize data collector and read configuration
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	this->SetDataCollector(dataCollector);

	if (this->DataCollector->ReadConfigurationFromFile(this->ConfigurationFileName) != PLUS_SUCCESS) {
		return PLUS_FAIL;
	}

	if (this->DataCollector->Initialize() != PLUS_SUCCESS) {
		return PLUS_FAIL;
	}

	if (this->DataCollector->Start() != PLUS_SUCCESS) {
		return PLUS_FAIL;
	}

	if ((this->DataCollector->GetTracker() == NULL) || (this->DataCollector->GetTracker()->GetNumberOfTools() < 1)) {
		LOG_WARNING("Unable to initialize Tracker!"); 
	}

	if (! this->DataCollector->GetInitialized()) {
		LOG_ERROR("Unable to initialize DataCollector!"); 
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkFCalController::LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue)
{
  LOG_TRACE("vtkFCalController::LookupElementWithNameContainingChildWithNameAndAttribute(" << aElementName << ", " << aChildName << ", " << (aChildAttributeName==NULL ? "" : aChildAttributeName) << ", " << (aChildAttributeValue==NULL ? "" : aChildAttributeValue) << ")");

  if (aConfig == NULL) {
    aConfig = vtkFCalController::GetInstance()->GetConfigurationData();
  }

	vtkSmartPointer<vtkXMLDataElement> firstElement = aConfig->LookupElementWithName(aElementName);
	if (firstElement == NULL) {
		return NULL;
	} else {
    if (aChildAttributeName && aChildAttributeValue) {
		  return firstElement->FindNestedElementWithNameAndAttribute(aChildName, aChildAttributeName, aChildAttributeValue);
    } else {
      return firstElement->FindNestedElementWithName(aChildName);
    }
	}
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkFCalController::ParseXMLOrFillWithInternalData(const char* aFile)
{
  LOG_TRACE("vtkFCalController::ParseXMLOrFillWithInternalData(" << aFile << ")");

	vtkXMLDataElement* rootElement = NULL;

  if ((aFile != NULL) && (vtksys::SystemTools::FileExists(aFile, true))) {
		rootElement = vtkXMLUtilities::ReadElementFromFile(aFile);

		if (rootElement == NULL) {	
			LOG_ERROR("Unable to get the configuration data from file " << aFile << " !"); 
			return NULL;
		}
	} else {
		LOG_DEBUG("Configuration file " << aFile << " does not exist, using configuration data in vtkFCalController"); 

		rootElement = vtkFCalController::GetInstance()->GetConfigurationData();

		if (rootElement == NULL) {	
			LOG_ERROR("Unable to get the configuration data from neither the file " << aFile << " nor from vtkFCalController"); 
			return NULL;
		}
	}

	return rootElement;
}

//-----------------------------------------------------------------------------

std::string vtkFCalController::GetNewConfigurationFileName()
{
  LOG_TRACE("vtkFCalController::GetNewConfigurationFileName");

  // Construct new file name with date and time
  std::string resultFileName = this->ConfigurationFileName;
  resultFileName = resultFileName.substr(0, resultFileName.find(".xml"));
  resultFileName.append("_");
  resultFileName.append(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S"));
  resultFileName.append(".xml");

  return resultFileName;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::SaveConfigurationToFile(const char* aFile)
{
  LOG_TRACE("vtkFCalController::SaveConfigurationToFile(" << aFile << ")");

  if ( this->DataCollector == NULL ) {
		LOG_ERROR("Data collector is NULL!");
		return PLUS_FAIL;
	}

 this->SetConfigurationFileName(aFile);
  
 return this->DataCollector->SaveConfigurationToFile(aFile); 
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::DumpBuffersToDirectory(const char* aDirectory)
{
  LOG_TRACE("vtkFCalController::DumpBuffersToDirectory(" << aDirectory << ")");

  if ((this->DataCollector == NULL) || (! this->DataCollector->GetInitialized())) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");
  std::string outputVideoBufferSequenceFileName = "BufferDump_Video_";
  outputVideoBufferSequenceFileName.append(dateAndTime);
  std::string outputTrackerBufferSequenceFileName = "BufferDump_Tracker_";
  outputTrackerBufferSequenceFileName.append(dateAndTime);

  // Dump buffers to file 
  if ( this->DataCollector->GetVideoSource() != NULL )  {
    LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
    this->DataCollector->WriteVideoBufferToMetafile( this->DataCollector->GetVideoSource()->GetBuffer(), aDirectory, outputVideoBufferSequenceFileName.c_str(), false); 
  }

  if ( this->DataCollector->GetTracker() != NULL ) {
    LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
    this->DataCollector->WriteTrackerToMetafile( this->DataCollector->GetTracker(), aDirectory, outputTrackerBufferSequenceFileName.c_str(), false); 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::LocateDirectories()
{
  //TODO
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
//TODO return with PlusStatus and the matrix is the parameter instead of position. An accessory function gets the position from the matrix
vtkSmartPointer<vtkMatrix4x4> vtkFCalController::AcquireStylusTrackerPosition(double aPosition[4], bool aReference)
{
	//LOG_TRACE("vtkFCalController::AcquireStylusTrackerPosition");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller == NULL) {
		LOG_ERROR("vtkFreehandController is invalid");
		return NULL;
	}
	if (controller->GetInitialized() == false) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return NULL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return NULL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is invalid!");
		return NULL;
	}

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = NULL; // stylus to reference tool transform
	TrackerStatus status = TR_MISSING;
	double timestamp;
	unsigned int toolNumber;
	if (aReference) {
		toolNumber = dataCollector->GetTracker()->GetReferenceToolNumber();
	} else {
		toolNumber = m_StylusPortNumber;
	}

	if (dataCollector->GetTracker()->GetTool(toolNumber)->GetEnabled()) {
		transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		dataCollector->GetTransformWithTimestamp(transformMatrix, timestamp, status, toolNumber); 
	}

	if (status == TR_MISSING || status == TR_OUT_OF_VIEW ) {
		LOG_DEBUG("Tracker out of view!");
		m_PositionString = std::string("Tracker out of view!");
		return NULL;
	} else if (status == TR_REQ_TIMEOUT ) {
		LOG_WARNING("Tracker request timeout!");
		m_PositionString = std::string("Tracker request timeout!");
		return NULL;
	} else if (aPosition != NULL) { // TR_OK
		// Compute the new position
		double elements[16]; //TODO find other way
		for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = transformMatrix->GetElement(i,j);
		double origin[4] = {0.0, 0.0, 0.0, 1.0};

		vtkMatrix4x4::PointMultiply(elements, origin, aPosition);

		// Assemble position string for toolbox
		char stylusPositionChars[32];

		sprintf_s(stylusPositionChars, 32, "%.1lf X %.1lf X %.1lf", aPosition[0], aPosition[1], aPosition[2]);
		m_PositionString = std::string(stylusPositionChars);
	}

	return transformMatrix;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::DoStylusCalibration()
{
	//LOG_TRACE("vtkFCalController::DoAcquisition");

	if (m_State == ToolboxState_InProgress) {
		LOG_DEBUG("StylusCalibrationAlgo: Acquire positions for stylus calibration"); 

		vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
		if (dataCollector == NULL) {
			LOG_ERROR("Data collector is not initialized");
			return PLUS_FAIL;
		}
		if (dataCollector->GetTracker() == NULL) {
			LOG_ERROR("Tracker is invalid");
			return PLUS_FAIL;
		}

		double stylusPosition[4];

		if (AcquireStylusTrackerPosition(stylusPosition)) {
			vtkPoints* points = m_InputPolyData->GetPoints();
			
			double distance_lowThreshold_mm = 3.0; // TODO review this threshold with the guys
			double distance_highThreshold_mm = 1000.0;
			double distance = -1.0;
			if (m_CurrentPointNumber < 1) {
				distance = (distance_lowThreshold_mm + distance_highThreshold_mm) / 2.0;
			} else {
				double previousPosition[4];
				points->GetPoint(m_CurrentPointNumber-1, previousPosition);
				previousPosition[3] = 1.0;
				// Square distance
				distance = vtkMath::Distance2BetweenPoints(stylusPosition, previousPosition);
			}

			// If current point is close to the previous one, or too far (outlier), we do not insert it
			if (distance < distance_lowThreshold_mm * distance_lowThreshold_mm) {
				LOG_DEBUG("Acquired position is too close to the previous - it is skipped");
			} else if (distance > distance_highThreshold_mm * distance_highThreshold_mm) {
				LOG_DEBUG("Acquired position seems to be an outlier - it is skipped");
			} else {
				// Add the point into the calibration dataset
				dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->InsertNextCalibrationPoint();

				// Add to polydata for rendering and re-compute bounding box
				points->InsertPoint(m_CurrentPointNumber, stylusPosition[0], stylusPosition[1], stylusPosition[2]);
				points->Modified();
				points->ComputeBounds();
				points->GetBounds(m_BoundingBox);

				// Set new current point number
				++m_CurrentPointNumber;

				// Reset the camera once in a while
				if ((m_CurrentPointNumber > 0) && ((m_CurrentPointNumber % 10 == 0) || (m_CurrentPointNumber == 5)) && (vtkFreehandController::GetInstance()->GetCanvas() != NULL)) {
					vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
				}

        // If enough points have been acquired, stop
				if (m_CurrentPointNumber >= m_NumberOfPoints) {
					if (m_Toolbox) {
						m_Toolbox->Stop();
					} else {
						Stop();
					}
					if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
						DisplayStylus();
						vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
					}
				}
			}
		}
	} else if ((m_State == ToolboxState_Done) && (vtkFreehandController::GetInstance()->GetCanvas() != NULL)) {
		DisplayStylus();
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalController::StartStylusCalibration()
{
  LOG_TRACE("vtkFCalController::StartStylusCalibration"); 

	vtkFCalController* controller = m_Visualizer->GetFCalController();
	if (controller == NULL) {
		LOG_ERROR("vtkFCalController is invalid");
		return PLUS_FAIL;
	}
	if (controller->GetInitialized() == false) {
		LOG_ERROR("vtkFCalController is not initialized");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized");
		return PLUS_FAIL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is invalid");
		return PLUS_FAIL;
	}

	m_CurrentPointNumber = 0;

	// Initialize tracker tool
	dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->GetBuffer()->Clear();
	dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->InitializeToolTipCalibration();

	vtkSmartPointer<vtkTransform> initialTransform = vtkSmartPointer<vtkTransform>::New();
	initialTransform->Identity();
	dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->SetCalibrationMatrix(initialTransform->GetMatrix());

	return PLUS_SUCCESS;
}

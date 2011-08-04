#include "PlusConfigure.h"

#include "vtkFreehandController.h"

#include "vtkObjectFactory.h"
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"

//-----------------------------------------------------------------------------

vtkFreehandController *vtkFreehandController::Instance = NULL;

//-----------------------------------------------------------------------------

vtkFreehandController* vtkFreehandController::New()
{
	return vtkFreehandController::GetInstance();
}

//-----------------------------------------------------------------------------

vtkFreehandController* vtkFreehandController::GetInstance()
{
	if(!vtkFreehandController::Instance) {
		// Try the factory first
		vtkFreehandController::Instance = (vtkFreehandController*)vtkObjectFactory::CreateInstance("vtkFreehandController");    

		if(!vtkFreehandController::Instance) {
			vtkFreehandController::Instance = new vtkFreehandController();	   
		}
	}
	// return the instance
	return vtkFreehandController::Instance;
}

//-----------------------------------------------------------------------------

vtkFreehandController::vtkFreehandController()
{
	this->DataCollector = NULL;
	this->RecordingFrameRate = 20;
	this->ConfigurationFileName = NULL;
	this->OutputFolder = NULL;
	this->InitializedOff();
	this->TrackingOnlyOn();
	this->Canvas = NULL;
	this->CanvasRenderer = NULL;

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandController::~vtkFreehandController()
{
	if (this->DataCollector != NULL) {
		this->DataCollector->Stop();
	}

	this->SetDataCollector(NULL);
	this->SetCanvasRenderer(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandController::Initialize()
{
	LOG_TRACE("vtkFreehandController::Initialize"); 

	if (this->Initialized) {
		return PLUS_SUCCESS;
	}

	// Set up canvas renderer
	vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	canvasRenderer->SetBackground(0.6, 0.6, 0.6);
	this->SetCanvasRenderer(canvasRenderer);

	// Create directory for the output
	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ((this->OutputFolder != NULL) && (dir->Open(this->OutputFolder) == 0)) {	
		dir->MakeDirectory(this->OutputFolder);
	}

	this->SetInitialized(true);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandController::SetTrackingOnly(bool aOn)
{
	LOG_TRACE("vtkFreehandController::SetTrackingOnly(" << (aOn ? "true" : "false") << ")");

	this->TrackingOnly = aOn;

	if (this->DataCollector != NULL) {
		this->DataCollector->SetTrackingOnly(aOn);
	}
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkFreehandController::GetConfigurationData()
{
	LOG_TRACE("vtkFreehandController::GetConfigurationData"); 

	if (this->DataCollector != NULL) {
		return this->DataCollector->GetConfigurationData();
  } else {
    return NULL;
  }
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandController::StartDataCollection()
{
	LOG_TRACE("vtkFreehandController::StartDataCollection"); 

	// Stop data collection if already started
	if (this->GetDataCollector() != NULL) {
		this->GetDataCollector()->Stop();
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

vtkXMLDataElement* vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue)
{
  LOG_TRACE("vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(" << aElementName << ", " << aChildName << ", " << (aChildAttributeName==NULL ? "" : aChildAttributeName) << ", " << (aChildAttributeValue==NULL ? "" : aChildAttributeValue) << ")");

  if (aConfig == NULL) {
    aConfig = vtkFreehandController::GetInstance()->GetConfigurationData();
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

vtkXMLDataElement* vtkFreehandController::ParseXMLOrFillWithInternalData(const char* aFile)
{
	vtkXMLDataElement* rootElement = NULL;

  if ((aFile != NULL) && (vtksys::SystemTools::FileExists(aFile, true))) {
		rootElement = vtkXMLUtilities::ReadElementFromFile(aFile);

		if (rootElement == NULL) {	
			LOG_ERROR("Unable to get the configuration data from file " << aFile << " !"); 
			return NULL;
		}
	} else {
		LOG_DEBUG("Configuration file " << aFile << " does not exist, using configuration data in vtkFreehandController"); 

		rootElement = vtkFreehandController::GetInstance()->GetConfigurationData();

		if (rootElement == NULL) {	
			LOG_ERROR("Unable to get the configuration data from neither the file " << aFile << " nor from vtkFreehandController"); 
			return NULL;
		}
	}

	return rootElement;
}

//-----------------------------------------------------------------------------

std::string vtkFreehandController::GetNewConfigurationFileName()
{
  // Construct new file name with date and time
  std::string resultFileName = this->ConfigurationFileName;
  resultFileName = resultFileName.substr(0, resultFileName.find(".xml"));
  resultFileName.append("_");
  resultFileName.append(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S"));
  resultFileName.append(".xml");

  return resultFileName;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandController::SaveConfigurationToFile(std::string aFile)
{
	if ((this->DataCollector == NULL) || (! this->DataCollector->GetInitialized())) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

  this->SetConfigurationFileName(aFile.c_str());

  this->DataCollector->GetConfigurationData()->PrintXML(this->ConfigurationFileName);

  LOG_INFO("Configuration file '" << this->ConfigurationFileName << "' saved");

  return PLUS_SUCCESS;
}

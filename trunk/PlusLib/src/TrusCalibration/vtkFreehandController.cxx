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
	this->ConfigurationData = NULL;
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

	this->ConfigurationData->UnRegister(this);
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

PlusStatus vtkFreehandController::StartDataCollection()
{
	LOG_TRACE("vtkFreehandController::StartDataCollection"); 

	// Stop data collection if already started
	if (this->GetDataCollector() != NULL) {
		this->GetDataCollector()->Stop();
	}

	// Read configuration file
	if ((this->ConfigurationFileName == NULL) || (STRCASECMP(this->ConfigurationFileName, "") == 0)) {
		LOG_ERROR("Cannot start data collection: Invalid configuration file name");
		return PLUS_FAIL;
	}

	this->SetConfigurationData(vtkXMLUtilities::ReadElementFromFile(this->ConfigurationFileName));
	if (this->ConfigurationData == NULL) {	
		LOG_ERROR("Unable to read configuration from file " << this->ConfigurationFileName); 
		return PLUS_FAIL;
	} 

	// Check version
	double version = 0;
	if (this->ConfigurationData->GetScalarAttribute("version", version)) {
		double currentVersion = (double)PLUSLIB_VERSION_MAJOR + ((double)PLUSLIB_VERSION_MINOR / 10.0);
		if (version < currentVersion) {
			LOG_ERROR("This version of configuration file is no longer supported! Please update to version " << std::fixed << currentVersion); 
			return PLUS_FAIL;
		}
	}

	// Initialize data collector and read configuration
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	this->SetDataCollector(dataCollector);

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = this->ConfigurationData->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL) { // Check if it is a separate data collection configuration file
		if (STRCASECMP(this->ConfigurationData->GetName(), "USDataCollection") == 0) {
			dataCollectionConfig = this->ConfigurationData;
		}
	}
	if (dataCollectionConfig != NULL) {
		if (this->DataCollector->ReadConfiguration(dataCollectionConfig) != PLUS_SUCCESS) {
			return PLUS_FAIL;
		}
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

	this->ConfigurationData->Register(this);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue)
{
	LOG_TRACE("vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(" << aElementName << ", " << aChildName << ", " << aChildAttributeName << ", " << aChildAttributeValue << ")");

	vtkXMLDataElement* childElement = NULL;

	vtkSmartPointer<vtkXMLDataElement> firstElement = aConfig->LookupElementWithName(aElementName);
	if (firstElement == NULL) {
		return NULL;
	} else {
		return childElement = firstElement->FindNestedElementWithNameAndAttribute(aChildName, aChildAttributeName, aChildAttributeValue);
	}
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkFreehandController::ParseXMLOrFillWithInternalData(const char* aFile)
{
	vtkXMLDataElement* rootElement = NULL;

	if (vtksys::SystemTools::FileExists(aFile, true)) {
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

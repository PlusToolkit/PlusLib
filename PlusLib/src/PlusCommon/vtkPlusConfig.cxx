#include "vtkPlusConfig.h"

#include "PlusConfigure.h"

#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusConfig, "$Revision: 1.0 $");

vtkPlusConfig *vtkPlusConfig::Instance = NULL;

//-----------------------------------------------------------------------------

vtkPlusConfig* vtkPlusConfig::New()
{
	return vtkPlusConfig::GetInstance();
}

//-----------------------------------------------------------------------------

vtkPlusConfig* vtkPlusConfig::GetInstance() {
	if(!vtkPlusConfig::Instance) {
		if(!vtkPlusConfig::Instance) {
			vtkPlusConfig::Instance = new vtkPlusConfig();	   
		}
	}
	// return the instance
	return vtkPlusConfig::Instance;
}

//-----------------------------------------------------------------------------

vtkPlusConfig::vtkPlusConfig()
{
	this->DeviceSetConfigurationDirectory = NULL;
	this->DeviceSetConfigurationFileName = NULL;
  this->DeviceSetConfigurationData = NULL;
  this->ApplicationConfigurationData = NULL;
  this->ApplicationConfigurationFileName = NULL;
  this->EditorApplicationExecutable = NULL;
  this->OutputDirectory = NULL;
  this->ProgramDirectory = NULL;

	this->SetDeviceSetConfigurationDirectory("");
	this->SetDeviceSetConfigurationFileName("");
  this->SetApplicationConfigurationFileName("PlusConfig.xml");
  this->SetEditorApplicationExecutable("notepad.exe");

  // Set output directory to the cwd/Output folder by default 
  this->SetOutputDirectory(vtksys::SystemTools::CollapseFullPath("./Output").c_str());
}

//-----------------------------------------------------------------------------

vtkPlusConfig::~vtkPlusConfig()
{
  this->SetDeviceSetConfigurationDirectory(NULL);
	this->SetDeviceSetConfigurationFileName(NULL);
  this->SetDeviceSetConfigurationData(NULL);
  this->SetApplicationConfigurationData(NULL);
  this->SetApplicationConfigurationFileName(NULL);
  this->SetEditorApplicationExecutable(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusConfig::SetProgramPath(const char* aProgramDirectory)
{
	LOG_TRACE("vtkPlusConfig::SetProgramPath(" << aProgramDirectory << ")");

  this->SetProgramDirectory(aProgramDirectory);

  return ReadApplicationConfiguration();
}

//------------------------------------------------------------------------------

PlusStatus vtkPlusConfig::WriteApplicationConfiguration()
{
	LOG_TRACE("vtkPlusConfig::WriteApplicationConfiguration");

  vtkSmartPointer<vtkXMLDataElement> applicationConfigurationRoot = this->ApplicationConfigurationData;
	if (applicationConfigurationRoot == NULL) {
    applicationConfigurationRoot = vtkSmartPointer<vtkXMLDataElement>::New();
    applicationConfigurationRoot->SetName("PlusConfig");
	}

  // Verify root element name
  if (STRCASECMP(applicationConfigurationRoot->GetName(), "PlusConfig") != NULL) {
    LOG_ERROR("Invalid application configuration file (root XML element of the file '" << this->ApplicationConfigurationFileName << "' should be 'PlusConfig')");
    return PLUS_FAIL;
  }

  // Save date
	applicationConfigurationRoot->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());

  // Save log level
	applicationConfigurationRoot->SetIntAttribute("LogLevel", vtkPlusLogger::Instance()->GetLogLevel());

  // Save device set directory
  applicationConfigurationRoot->SetAttribute("DeviceSetConfigurationDirectory", this->DeviceSetConfigurationDirectory);

  // Save last device set config file
  applicationConfigurationRoot->SetAttribute("LastDeviceSetConfigurationFileName", this->DeviceSetConfigurationFileName);

  // Save editor application
  applicationConfigurationRoot->SetAttribute("EditorApplicationExecutable", this->EditorApplicationExecutable);

  // Save output path
  applicationConfigurationRoot->SetAttribute("OutputDirectory", this->OutputDirectory);

  // Save configuration
  this->SetApplicationConfigurationData(applicationConfigurationRoot);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusConfig::ReadApplicationConfiguration()
{
	LOG_TRACE("vtkPlusConfig::ReadApplicationConfiguration");

  if (this->ProgramDirectory == NULL) {
    LOG_ERROR("Unable to read configuration - program directory has to be set first!");
    return PLUS_FAIL;
  }

  bool saveNeeded = false;

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> applicationConfigurationRoot = NULL;
  if (vtksys::SystemTools::FileExists(this->ApplicationConfigurationFileName, true)) {
    applicationConfigurationRoot = vtkXMLUtilities::ReadElementFromFile(this->ApplicationConfigurationFileName);
  }
	if (applicationConfigurationRoot == NULL) {
    LOG_INFO("Application configuration file is not found'" << this->ApplicationConfigurationFileName << "' - default values will be used and the file created"); 
    applicationConfigurationRoot = vtkSmartPointer<vtkXMLDataElement>::New();
    applicationConfigurationRoot->SetName("PlusConfig");
    saveNeeded = true;
	}

  this->SetApplicationConfigurationData(applicationConfigurationRoot); 

  // Verify root element name
  if (STRCASECMP(applicationConfigurationRoot->GetName(), "PlusConfig") != NULL) {
    LOG_ERROR("Invalid application configuration file (root XML element of the file '" << this->ApplicationConfigurationFileName << "' should be 'PlusConfig' instead of '" << applicationConfigurationRoot->GetName() << "')");
    return PLUS_FAIL;
  }

  // Read log level
	int logLevel = 0;
	if (applicationConfigurationRoot->GetScalarAttribute("LogLevel", logLevel) ) {
    vtkPlusLogger::Instance()->SetLogLevel(logLevel);
    vtkPlusLogger::Instance()->SetDisplayLogLevel(logLevel); 

	} else {
		LOG_INFO("LogLevel attribute is not found - default 'Info' log level will be used");
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_INFO);
    vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_INFO); 
    saveNeeded = true;
  }

  // Read last device set config file
  const char* lastDeviceSetConfigFile = applicationConfigurationRoot->GetAttribute("LastDeviceSetConfigurationFileName");
  if ((lastDeviceSetConfigFile != NULL) && (STRCASECMP(lastDeviceSetConfigFile, "") != 0)) {
    this->SetDeviceSetConfigurationFileName(lastDeviceSetConfigFile);

  } else {
    LOG_DEBUG("Cannot read last used device set config file until you connect to one first");
  }

  // Read device set configuration directory
  const char* deviceSetDirectory = applicationConfigurationRoot->GetAttribute("DeviceSetConfigurationDirectory");
  if ((deviceSetDirectory != NULL) && (STRCASECMP(deviceSetDirectory, "") != 0)) {
	  this->SetDeviceSetConfigurationDirectory(deviceSetDirectory);

  } else {
    LOG_INFO("Device set configuration directory is not set - default '../Config' will be used");
    std::string parentDirectory = vtksys::SystemTools::GetParentDirectory(this->ProgramDirectory);
    std::string defaultDeviceSetConfigDirectory = vtksys::SystemTools::CollapseFullPath("./Config", parentDirectory.c_str()); 
    this->SetDeviceSetConfigurationDirectory(defaultDeviceSetConfigDirectory.c_str());
    saveNeeded = true;
  }

	// Make device set configuraiton directory
  if (! vtksys::SystemTools::MakeDirectory(this->DeviceSetConfigurationDirectory)) {
    LOG_ERROR("Unable to create device set configuration directory '" << this->DeviceSetConfigurationDirectory << "'");
    return PLUS_FAIL;
  }

  // Read editor application
  const char* editorApplicationExecutable = applicationConfigurationRoot->GetAttribute("EditorApplicationExecutable");
  if ((editorApplicationExecutable != NULL) && (STRCASECMP(editorApplicationExecutable, "") != 0)) {
    this->SetEditorApplicationExecutable(editorApplicationExecutable);

  } else {
    LOG_INFO("Editor application executable is not set - default 'notepad.exe' will be used");
    this->SetEditorApplicationExecutable("notepad.exe");
    saveNeeded = true;
  }

  // Read output directory
  const char* outputDirectory = applicationConfigurationRoot->GetAttribute("OutputDirectory");
  if ((outputDirectory != NULL) && (STRCASECMP(outputDirectory, "") != 0)) {
	  this->SetOutputDirectory(outputDirectory);

  } else {
    LOG_INFO("Output directory is not set - default './Output' will be used");
  	std::string defaultOutputDirectory = vtksys::SystemTools::CollapseFullPath("./Output", this->ProgramDirectory); 
    this->SetOutputDirectory(defaultOutputDirectory.c_str());
    saveNeeded = true;
  }

	// Make output directory
  if (! vtksys::SystemTools::MakeDirectory(this->OutputDirectory)) {
    LOG_ERROR("Unable to create output directory '" << this->OutputDirectory << "'");
    return PLUS_FAIL;
  }

  if (saveNeeded) {
    return SaveApplicationConfigurationToFile();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::GetNewDeviceSetConfigurationFileName()
{
  LOG_TRACE("vtkPlusConfig::GetNewDeviceSetConfigurationFileName");

  std::string resultFileName; 
  if ((this->DeviceSetConfigurationFileName == NULL) || (STRCASECMP(this->DeviceSetConfigurationFileName, "") == 0)) {
    LOG_WARNING("New configuration file name cannot be assembled due to absence of input configuration file name");

    resultFileName = "PlusConfiguration";
  } else {
    resultFileName = this->DeviceSetConfigurationFileName;
    resultFileName = resultFileName.substr(0, resultFileName.find(".xml"));
    resultFileName = resultFileName.substr(resultFileName.find_last_of("/\\") + 1);
  }

  // Detect if date is already in the filename and remove it if it is there
  std::string possibleDate = resultFileName.substr(resultFileName.length() - 15, 15);
  std::string possibleDay = possibleDate.substr(0, 8);
  std::string possibleTime = possibleDate.substr(9, 6);
  if (atoi(possibleDay.c_str()) && atoi(possibleTime.c_str())) {
    resultFileName = resultFileName.substr(0, resultFileName.length() - 16);
  }

  // Construct new file name with date and time
  resultFileName.append("_");
  resultFileName.append(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S"));
  resultFileName.append(".xml");

  return resultFileName;
}

//------------------------------------------------------------------------------

PlusStatus vtkPlusConfig::SaveApplicationConfigurationToFile()
{
  LOG_TRACE("vtkPlusConfig::SaveApplicationConfigurationToFile");

  if (WriteApplicationConfiguration() != PLUS_SUCCESS) {
    LOG_ERROR("Failed to save application configuration to XML data!"); 
    return PLUS_FAIL; 
  }

  this->ApplicationConfigurationData->PrintXML( this->ApplicationConfigurationFileName );

  LOG_INFO("Application configuration file '" << this->ApplicationConfigurationFileName << "' saved");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vtkXMLDataElement* vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue)
{
  LOG_TRACE("vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(" << aElementName << ", " << aChildName << ", " << (aChildAttributeName==NULL ? "" : aChildAttributeName) << ", " << (aChildAttributeValue==NULL ? "" : aChildAttributeValue) << ")");

  if (aConfig == NULL) {
    LOG_ERROR("No input XML data element is specified!");
    return NULL;
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

std::string vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(const char* aFileName)
{
	LOG_TRACE("vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(" << aFileName << ")");

  if ((vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory() == NULL) || (STRCASECMP(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), "") == 0)) {
		std::string configurationDirectory = vtksys::SystemTools::GetFilenamePath(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName());
		return vtkPlusConfig::GetFirstFileFoundInParentOfDirectory(aFileName, configurationDirectory.c_str());

  } else {
  	return GetFirstFileFoundInParentOfDirectory(aFileName, vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
  }
};

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::GetFirstFileFoundInParentOfDirectory(const char* aFileName, const char* aDirectory)
{
	LOG_TRACE("vtkPlusConfig::GetFirstFileFoundInParentOfDirectory(" << aFileName << ", " << aDirectory << ")"); 

	std::string parentDirectory = vtksys::SystemTools::GetParentDirectory(aDirectory);

	return GetFirstFileFoundInDirectory(aFileName, parentDirectory.c_str());
};

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::GetFirstFileFoundInDirectory(const char* aFileName, const char* aDirectory)
{
	LOG_TRACE("vtkPlusConfig::GetFirstFileFoundInDirectory(" << aFileName << ", " << aDirectory << ")"); 

	std::string result = FindFileRecursivelyInDirectory(aFileName, aDirectory);
	if (STRCASECMP("", result.c_str()) == 0) {
		LOG_WARNING("File " << aFileName << " was not found in directory " << aDirectory);
	}

	return result;
};

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::FindFileRecursivelyInDirectory(const char* aFileName, const char* aDirectory)
{
	LOG_TRACE("vtkPlusConfig::FindFileRecursivelyInDirectory(" << aFileName << ", " << aDirectory << ")"); 

	std::vector<std::string> directoryList;
	directoryList.push_back(aDirectory);

	// Search for the file in the input directory first (no system paths allowed)
	std::string result = vtksys::SystemTools::FindFile(aFileName, directoryList, true);
	if (STRCASECMP("", result.c_str())) {
		return result;
	// If not found then call this function recursively for the subdirectories of the input directory
	} else {
		vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
		if (dir->Open(aDirectory)) {				
			for (int i=0; i<dir->GetNumberOfFiles(); ++i) {
				const char* fileOrDirectory = dir->GetFile(i);
				if ((! dir->FileIsDirectory(fileOrDirectory)) || (STRCASECMP(".", fileOrDirectory) == 0) || (STRCASECMP("..", fileOrDirectory) == 0)) {
					continue;
				}

				std::string fullPath(aDirectory);
				fullPath.append("/");
				fullPath.append(fileOrDirectory);

				result = FindFileRecursivelyInDirectory(aFileName, fullPath.c_str());
				if (STRCASECMP("", result.c_str())) {
					return result;
				}
			}
		} else {
			LOG_DEBUG("Directory " << aDirectory << " cannot be opened");
			return "";
		}
	}

	return "";
};

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
	this->ConfigurationDirectory = NULL;
	this->ConfigurationFileName = NULL;
  this->ConfigurationData = NULL;
  this->ApplicationConfigurationData = NULL;
  this->ApplicationConfigurationFileName = NULL;

	this->SetConfigurationDirectory("");
	this->SetConfigurationFileName("");

  this->SetApplicationConfigurationFileName("Config.xml");
}

//-----------------------------------------------------------------------------

vtkPlusConfig::~vtkPlusConfig()
{
  this->SetConfigurationDirectory(NULL);
	this->SetConfigurationFileName(NULL);
  this->SetConfigurationData(NULL);
  this->SetApplicationConfigurationData(NULL);
  this->SetApplicationConfigurationFileName(NULL);
}

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(const char* aFileName)
{
	LOG_TRACE("vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(" << aFileName << ")"); 

	return GetFirstFileFoundInParentOfDirectory(aFileName, vtkPlusConfig::GetInstance()->GetConfigurationDirectory());
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

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::GetNewConfigurationFileName()
{
  LOG_TRACE("vtkPlusConfig::GetNewConfigurationFileName");

  std::string resultFileName = "";
  if ((this->ConfigurationFileName == NULL) || (STRCASECMP(this->ConfigurationFileName, "") == 0)) {
    LOG_WARNING("New configuration file name cannot be assembled due to absence of input configuration file name");

    resultFileName = "PlusConfiguration";
  } else {
    resultFileName = this->ConfigurationFileName;
    resultFileName = resultFileName.substr(0, resultFileName.find(".xml"));
    resultFileName = resultFileName.substr(resultFileName.find_last_of("/\\") + 1);
  }

  // Construct new file name with date and time
  resultFileName.append("_");
  resultFileName.append(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S"));
  resultFileName.append(".xml");

  return resultFileName;
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

//------------------------------------------------------------------------------

PlusStatus vtkPlusConfig::SaveConfigurationToFile(const char* aFile)
{
  LOG_TRACE("vtkPlusConfig::SaveConfigurationToFile(" << aFile << ")");

  if ( aFile == NULL ) {
    LOG_ERROR("Failed to save configuration to file - file name is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( this->ConfigurationData == NULL )
  {
    LOG_ERROR("Failed to save configuration data to file - configuration data is NULL!"); 
    return PLUS_FAIL; 
  }

  this->ConfigurationData->PrintXML( aFile );

  LOG_INFO("Configuration file '" << aFile << "' saved");

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------

PlusStatus vtkPlusConfig::SaveApplicationConfigurationToFile()
{
  LOG_TRACE("vtkPlusConfig::SaveApplicationConfigurationToFile");

  if ( this->ApplicationConfigurationData == NULL )
  {
    LOG_ERROR("Failed to save application configuration data to file - application configuration data is NULL!"); 
    return PLUS_FAIL; 
  }

  this->ApplicationConfigurationData->PrintXML( this->ApplicationConfigurationFileName );

  LOG_INFO("Application configuration file '" << this->ApplicationConfigurationFileName << "' saved");

  return PLUS_SUCCESS;
}

#include "vtkFileFinder.h"

#include "PlusConfigure.h"

#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFileFinder, "$Revision: 1.0 $");

vtkFileFinder *vtkFileFinder::Instance = NULL;

//-----------------------------------------------------------------------------

vtkFileFinder* vtkFileFinder::New()
{
	return vtkFileFinder::GetInstance();
}

//-----------------------------------------------------------------------------

vtkFileFinder* vtkFileFinder::GetInstance() {
	if(!vtkFileFinder::Instance) {
		if(!vtkFileFinder::Instance) {
			vtkFileFinder::Instance = new vtkFileFinder();	   
		}
	}
	// return the instance
	return vtkFileFinder::Instance;
}

//-----------------------------------------------------------------------------

vtkFileFinder::vtkFileFinder()
{
	this->ConfigurationDirectory = NULL;
	this->ConfigurationFileName = NULL;

	this->SetConfigurationDirectory("");
	this->SetConfigurationFileName("");
}

//-----------------------------------------------------------------------------

vtkFileFinder::~vtkFileFinder()
{
}

//-----------------------------------------------------------------------------

std::string vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(const char* aFileName)
{
	LOG_TRACE("vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(" << aFileName << ")"); 

	return GetFirstFileFoundInParentOfDirectory(aFileName, vtkFileFinder::GetInstance()->GetConfigurationDirectory());
};

//-----------------------------------------------------------------------------

std::string vtkFileFinder::GetFirstFileFoundInParentOfDirectory(const char* aFileName, const char* aDirectory)
{
	LOG_TRACE("vtkFileFinder::GetFirstFileFoundInParentOfDirectory(" << aFileName << ", " << aDirectory << ")"); 

	std::string parentDirectory = vtksys::SystemTools::GetParentDirectory(aDirectory);

	return GetFirstFileFoundInDirectory(aFileName, parentDirectory.c_str());
};

//-----------------------------------------------------------------------------

std::string vtkFileFinder::GetFirstFileFoundInDirectory(const char* aFileName, const char* aDirectory)
{
	LOG_TRACE("vtkFileFinder::GetFirstFileFoundInDirectory(" << aFileName << ", " << aDirectory << ")"); 

	std::string result = FindFileRecursivelyInDirectory(aFileName, aDirectory);
	if (STRCASECMP("", result.c_str()) == 0) {
		LOG_WARNING("File " << aFileName << " was not found in directory " << aDirectory);
	}

	return result;
};

//-----------------------------------------------------------------------------

std::string vtkFileFinder::FindFileRecursivelyInDirectory(const char* aFileName, const char* aDirectory)
{
	LOG_TRACE("vtkFileFinder::FindFileRecursivelyInDirectory(" << aFileName << ", " << aDirectory << ")"); 

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

std::string vtkFileFinder::GetNewConfigurationFileName()
{
  LOG_TRACE("vtkFileFinder::GetNewConfigurationFileName");

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

vtkXMLDataElement* vtkFileFinder::LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue)
{
  LOG_TRACE("vtkFileFinder::LookupElementWithNameContainingChildWithNameAndAttribute(" << aElementName << ", " << aChildName << ", " << (aChildAttributeName==NULL ? "" : aChildAttributeName) << ", " << (aChildAttributeValue==NULL ? "" : aChildAttributeValue) << ")");

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

#include "vtkFileFinder.h"

#include "PlusConfigure.h"

#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"

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

	this->SetConfigurationDirectory("");
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

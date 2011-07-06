// .NAME vtkFileFinder - class for finding files in a directory and its subdirectorys
// .SECTION Description
// This class is used for finding files in a directory and its subdirectorys

#ifndef __VTKFILEFINDER_H
#define __VTKFILEFINDER_H

#include "vtkObject.h"

#include "PlusConfigure.h"

#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"

//-----------------------------------------------------------------------------

class VTK_EXPORT vtkFileFinder : public vtkObject
{
public:
	static std::string GetFirstFileFoundInParentOfDirectory(const char* aFileName, const char* aDirectory)
	{
		std::string parentDirectory = vtksys::SystemTools::GetParentDirectory(aDirectory);

		return GetFirstFileFoundInDirectory(aFileName, parentDirectory.c_str());
	};

	static std::string GetFirstFileFoundInDirectory(const char* aFileName, const char* aDirectory)
	{
		std::string result = FindFileRecursivelyInDirectory(aFileName, aDirectory);
		if (STRCASECMP("", result.c_str()) == 0) {
			LOG_WARNING("File " << aFileName << " was not found in directory " << aDirectory);
		}

		return result;
	};

protected:
	static std::string FindFileRecursivelyInDirectory(const char* aFileName, const char* aDirectory)
	{
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

}; 

#endif

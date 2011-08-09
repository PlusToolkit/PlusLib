// .NAME vtkFileFinder - class for finding files in a directory and its subdirectorys
// .SECTION Description
// This class is used for finding files in a directory and its subdirectorys

#ifndef __VTKFILEFINDER_H
#define __VTKFILEFINDER_H

#include "vtkObject.h"

//-----------------------------------------------------------------------------

class VTK_EXPORT vtkFileFinder : public vtkObject
{
public:
	vtkTypeRevisionMacro(vtkFileFinder, vtkObject);

	/*!
	* \brief New
	*/
	static vtkFileFinder *New();

	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static vtkFileFinder* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~vtkFileFinder();

	/*!
	* \brief Search recursively for a file in the configuration directory
	* \param aFileName Name of the file to be searched
	* \return The first file that is found (with full path)
	*/
	static std::string GetFirstFileFoundInConfigurationDirectory(const char* aFileName);

	/*!
	* \brief Search recursively for a file in the parent of a specified directory
	* \param aFileName Name of the file to be searched
	* \param aDirectory Directory in whose parent the search is performed
	* \return The first file that is found (with full path)
	*/
	static std::string GetFirstFileFoundInParentOfDirectory(const char* aFileName, const char* aDirectory);

	/*!
	* \brief Search recursively for a file in a specified directory
	* \param aFileName Name of the file to be searched
	* \param aDirectory Directory in which search is performed
	* \return The first file that is found (with full path)
	*/
	static std::string GetFirstFileFoundInDirectory(const char* aFileName, const char* aDirectory);

public:
	//! Get macro for configuration directory
	vtkGetStringMacro(ConfigurationDirectory);
	//! Set macro for configuration directory
	vtkSetStringMacro(ConfigurationDirectory);

protected:
	/*!
	* \brief Search recursively for a file in a specified directory (core, protected function)
	* \param aFileName Name of the file to be searched
	* \param aDirectory Directory in which search is performed
	* \return The first file that is found with full path
	*/
	static std::string FindFileRecursivelyInDirectory(const char* aFileName, const char* aDirectory);

protected:
	/*!
	* \brief Constructor
	*/
	vtkFileFinder();

protected:
	//! Configuration directory path
	char *ConfigurationDirectory;

private:
	//! Instance of the singleton
	static vtkFileFinder*	Instance;
}; 


#endif

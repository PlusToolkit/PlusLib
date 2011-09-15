// .NAME vtkPlusConfig - class for finding files in a directory and its subdirectorys
// .SECTION Description
// This class is used for finding files in a directory and its subdirectorys, handling configuration (directory, file name, XML tree)

#ifndef __vtkPlusConfig_h
#define __vtkPlusConfig_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

/*!
* \brief Singleton class providing tools needed for handling the configuration - finding files, assembling file names, holding and saving the configuration data
*/
class VTK_EXPORT vtkPlusConfig : public vtkObject
{
public:
	vtkTypeRevisionMacro(vtkPlusConfig, vtkObject);

	/*!
	* \brief New
	*/
	static vtkPlusConfig *New();

	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static vtkPlusConfig* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~vtkPlusConfig();

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

	/*!
	 * \brief Assembles a filename that is the same as the input file name, only with the current date and time in the end (for saving to a new file)
	 * \return New configuration file nname
	 */
  std::string GetNewConfigurationFileName();

	/*!
	 * \brief Searches a data element in an XML tree: the child of aElementName that has the name aChildName and has an attribute aChildAttributeName with the value aChildAttributeValue
   * \param aConfig Root XML element in that the search is conducted
   * \param aElementName Name of the parent of the searched element
   * \param aChildName Name of the searched element
   * \param aChildAttributeName Name of the attribute based on which we want the element to be found
   * \param aChildAttributeValue Value of the attribute based on which we want the element to be found
	 * \return Found XML data element
	 */
	static vtkXMLDataElement* LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue);

	/*!
	* \brief Saves current configuration data to specified file
	* \param aFile Name of the file to be saved
	* \return Success flag
	*/
  PlusStatus SaveConfigurationToFile(const char* aFile);

	/*!
	* \brief Saves application configuration data to file whose name is stored in ApplicationConfigurationFileName
	* \return Success flag
	*/
  PlusStatus SaveApplicationConfigurationToFile();

public:
	//! Get/Set macro for configuration directory
	vtkGetStringMacro(ConfigurationDirectory);
	vtkSetStringMacro(ConfigurationDirectory);

	//! Get/Set macro for configuration file name
	vtkGetStringMacro(ConfigurationFileName);
	vtkSetStringMacro(ConfigurationFileName);

	//! Get/Set macro for configuration data
  vtkGetObjectMacro(ConfigurationData, vtkXMLDataElement); 
  vtkSetObjectMacro(ConfigurationData, vtkXMLDataElement); 

	//! Get/Set macro for application configuration file name
	vtkGetStringMacro(ApplicationConfigurationFileName);
	vtkSetStringMacro(ApplicationConfigurationFileName);

	//! Get/Set macro for application configuration data
  vtkGetObjectMacro(ApplicationConfigurationData, vtkXMLDataElement); 
  vtkSetObjectMacro(ApplicationConfigurationData, vtkXMLDataElement); 

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
	vtkPlusConfig();

protected:
	//! Configuration directory path
	char*               ConfigurationDirectory;

  //! Used configuration file name (for assembling the new file name with the date)
	char*               ConfigurationFileName;

  //! Session configuration data
  vtkXMLDataElement*  ConfigurationData;

  //! Application configuration file name (./Config.xml by default)
	char*               ApplicationConfigurationFileName;

  //! Application configuration data
  vtkXMLDataElement*  ApplicationConfigurationData;

private:
	//! Instance of the singleton
	static vtkPlusConfig*	Instance;
}; 


#endif

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
	* \brief Saves application configuration data to file with the name that is stored in ApplicationConfigurationFileName
	* \return Success flag
	*/
  PlusStatus SaveApplicationConfigurationToFile();

	/*!
	 * \brief Assembles a filename for the new device set configuration that is the same as the input file name, only with the current date and time in the end (for saving to a new file)
	 * \return New device set configuration file nname
	 */
  std::string GetNewDeviceSetConfigurationFileName();

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
	* \brief Set program path and read application configuration
	* \return Success flag
	*/
  PlusStatus SetProgramPath(const char* aProgramDirectory);

public:
	//! Get/Set macros
	vtkGetStringMacro(DeviceSetConfigurationDirectory);
	vtkSetStringMacro(DeviceSetConfigurationDirectory);

	vtkGetStringMacro(DeviceSetConfigurationFileName);
	vtkSetStringMacro(DeviceSetConfigurationFileName);

  vtkGetObjectMacro(DeviceSetConfigurationData, vtkXMLDataElement); 
  vtkSetObjectMacro(DeviceSetConfigurationData, vtkXMLDataElement); 

	vtkGetStringMacro(ApplicationConfigurationFileName);
	vtkSetStringMacro(ApplicationConfigurationFileName);

	vtkGetStringMacro(EditorApplicationExecutable);
	vtkSetStringMacro(EditorApplicationExecutable);

	vtkGetStringMacro(OutputDirectory);
	vtkSetStringMacro(OutputDirectory);

	vtkGetStringMacro(ProgramDirectory);

protected:
	vtkSetStringMacro(ProgramDirectory);

  vtkGetObjectMacro(ApplicationConfigurationData, vtkXMLDataElement); 
  vtkSetObjectMacro(ApplicationConfigurationData, vtkXMLDataElement); 

protected:
  /*!
	* \brief Save application configuration file
  * \return Success flag
	*/
  PlusStatus WriteApplicationConfiguration();

	/*!
	* \brief Read application configuration from file
  * \return Success flag
	*/
  PlusStatus ReadApplicationConfiguration();

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

	/*!
	* \brief Destructor
	*/
	virtual	~vtkPlusConfig();

protected:
	//! Device set configuration directory path
	char*               DeviceSetConfigurationDirectory;

  //! Used device set configuration file name (for assembling the new file name with the date)
	char*               DeviceSetConfigurationFileName;

  //! Session device set configuration data
  vtkXMLDataElement*  DeviceSetConfigurationData;

  //! Application configuration file name (./Config.xml by default)
	char*               ApplicationConfigurationFileName;

  //! Application configuration data
  vtkXMLDataElement*  ApplicationConfigurationData;

  //! Path and filename of the editor application executable to be used
  char*               EditorApplicationExecutable;

  //! Output directory path
  char*               OutputDirectory;

  //! Program path
  char*               ProgramDirectory;

private:
	//! Instance of the singleton
	static vtkPlusConfig*	Instance;
}; 


#endif

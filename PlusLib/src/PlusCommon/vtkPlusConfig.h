/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusConfig_h
#define __vtkPlusConfig_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

/*!
  \class vtkPlusConfig 
  \brief Singleton class providing tools needed for handling the configuration - finding files, assembling file names, holding and saving the configuration data
  \ingroup PlusLibCommon
*/ 
class VTK_EXPORT vtkPlusConfig : public vtkObject
{
public:
	vtkTypeRevisionMacro(vtkPlusConfig, vtkObject);

	/*!
	* New
	*/
	static vtkPlusConfig *New();

	/*!
	* Instance getter for the singleton class
	* \return Instance object
	*/
	static vtkPlusConfig* GetInstance();

  /*! Allows cleanup of the singleton at application exit */
  static void SetInstance(vtkPlusConfig* instance);

	/*!
	* Saves application configuration data to file with the name that is stored in ApplicationConfigurationFileName
	* \return Success flag
	*/
  PlusStatus SaveApplicationConfigurationToFile();

	/*!
	 * Assembles a filename for the new device set configuration that is the same as the input file name, only with the current date and time in the end (for saving to a new file)
	 * \return New device set configuration file name
	 */
  std::string GetNewDeviceSetConfigurationFileName();

	/*!
	 * Replaces an element with a name on the level under the top level in device set configuration with the element in the parameter root XML data element
   * \param Element name to replace
   * \param Root element containing the element to replace with
	 * \return Success flag
	 */
  static PlusStatus ReplaceElementInDeviceSetConfiguration(const char* aElementName, vtkXMLDataElement* aNewRootElement);

  /*!
	 * Searches a data element in an XML tree: the child of aElementName that has the name aChildName and has an attribute aChildAttributeName with the value aChildAttributeValue
   * \param aConfig Root XML element in that the search is conducted
   * \param aElementName Name of the parent of the searched element
   * \param aChildName Name of the searched element
   * \param aChildAttributeName Name of the attribute based on which we want the element to be found
   * \param aChildAttributeValue Value of the attribute based on which we want the element to be found
	 * \return Found XML data element
	 */
	static vtkXMLDataElement* LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue);

  /*!
	* Search recursively for a file in the configuration directory
	* \param aFileName Name of the file to be searched
	* \return The first file that is found (with full path)
	*/
	static std::string GetFirstFileFoundInConfigurationDirectory(const char* aFileName);

	/*!
	* Search recursively for a file in the parent of a specified directory
	* \param aFileName Name of the file to be searched
	* \param aDirectory Directory in whose parent the search is performed
	* \return The first file that is found (with full path)
	*/
	static std::string GetFirstFileFoundInParentOfDirectory(const char* aFileName, const char* aDirectory);

	/*!
	* Search recursively for a file in a specified directory
	* \param aFileName Name of the file to be searched
	* \param aDirectory Directory in which search is performed
	* \return The first file that is found (with full path)
	*/
	static std::string GetFirstFileFoundInDirectory(const char* aFileName, const char* aDirectory);

	/*!
	* Set program path and read application configuration
	* \return Success flag
	*/
  PlusStatus SetProgramPath(const char* aProgramDirectory);

  /*!
	* Set output directory and change vtkPlusLog file name
	*/
  void SetOutputDirectory(const char* outputDir);

	/*!
	* Get absolute image path from input image path
	* \param aImagePath Image path read from configuration file
	* \return Absolute image path
	*/
  static std::string GetAbsoluteImagePath(const char* aImagePath);

public:
	/*! Get device set configuration directory path */
	vtkGetStringMacro(DeviceSetConfigurationDirectory);
	/*! Set device set configuration directory path */
	vtkSetStringMacro(DeviceSetConfigurationDirectory);

	/*! Get device set configuration file name */
	vtkGetStringMacro(DeviceSetConfigurationFileName);
	/*! Set device set configuration file name */
	vtkSetStringMacro(DeviceSetConfigurationFileName);

	/*! Get device set configuration data */
  vtkGetObjectMacro(DeviceSetConfigurationData, vtkXMLDataElement); 
	/*! Set device set configuration data */
  vtkSetObjectMacro(DeviceSetConfigurationData, vtkXMLDataElement); 

	/*! Get application configuration file name */
	vtkGetStringMacro(ApplicationConfigurationFileName);
	/*! Set application configuration file name */
	vtkSetStringMacro(ApplicationConfigurationFileName);

	/*! Get editor application executable path and file name */
	vtkGetStringMacro(EditorApplicationExecutable);
	/*! Set editor application executable path and file name */
	vtkSetStringMacro(EditorApplicationExecutable);

	/*! Get output directory path */
	vtkGetStringMacro(OutputDirectory);
	
	/*! Get program directory path */
	vtkGetStringMacro(ProgramDirectory);

	/*! Get image directory path */
	vtkGetStringMacro(ImageDirectory);
	/*! Set image directory path */
	vtkSetStringMacro(ImageDirectory);

	/*! Get gnuplot directory path */
	vtkGetStringMacro(GnuplotDirectory);
	/*! Set gnuplot directory path */
	vtkSetStringMacro(GnuplotDirectory);

    /*! Get scripts directory path */
	vtkGetStringMacro(ScriptsDirectory);
	/*! Set scripts directory path */
	vtkSetStringMacro(ScriptsDirectory);

protected:
	/*! Set program directory path */
	vtkSetStringMacro(ProgramDirectory);

	/*! Get application configuration data */
  vtkGetObjectMacro(ApplicationConfigurationData, vtkXMLDataElement); 
	/*! Set application configuration data */
  vtkSetObjectMacro(ApplicationConfigurationData, vtkXMLDataElement); 

protected:
  /*!
	* Save application configuration file
  * \return Success flag
	*/
  PlusStatus WriteApplicationConfiguration();

	/*!
	* Read application configuration from file
  * \return Success flag
	*/
  PlusStatus ReadApplicationConfiguration();

  /*!
	* Search recursively for a file in a specified directory (core, protected function)
	* \param aFileName Name of the file to be searched
	* \param aDirectory Directory in which search is performed
	* \return The first file that is found with full path
	*/
	static std::string FindFileRecursivelyInDirectory(const char* aFileName, const char* aDirectory);

protected:
	/*!
	* Constructor
	*/
	vtkPlusConfig();

	/*!
	* Destructor
	*/
	virtual	~vtkPlusConfig();

protected:
	/*! Device set configuration directory path */
	char*               DeviceSetConfigurationDirectory;

  /*! Used device set configuration file name (for assembling the new file name with the date) */
	char*               DeviceSetConfigurationFileName;

  /*! Session device set configuration data */
  vtkXMLDataElement*  DeviceSetConfigurationData;

  /*! Application configuration file name (./Config.xml by default) */
	char*               ApplicationConfigurationFileName;

  /*! Application configuration data */
  vtkXMLDataElement*  ApplicationConfigurationData;

  /*! Path and filename of the editor application executable to be used */
  char*               EditorApplicationExecutable;

  /*! Output directory path */
  char*               OutputDirectory;

  /*! Program path */
  char*               ProgramDirectory;

  /*! Image directory path. It is used as home directory for images when the image path for a saved dataset is not an absolute path */
  char*               ImageDirectory;

  /*! Gnuplot binary directory path */
  char*               GnuplotDirectory;

  /*! Scripts directory path */
  char*               ScriptsDirectory;

private:
	/*! Instance of the singleton */
	static vtkPlusConfig*	Instance;
}; 


#endif

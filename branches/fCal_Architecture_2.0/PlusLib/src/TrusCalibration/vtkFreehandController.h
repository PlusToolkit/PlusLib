#ifndef __vtkFCalController_h
#define __vtkFCalController_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkDataCollector.h"

class vtkTrackedFrameList;
class QVTKWidget;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Controller of the freehand calibration application
*/
class vtkFCalController : public vtkObject
{
public:
	/*!
	* \brief New
	*/
	static vtkFCalController *New();

	vtkTypeRevisionMacro(vtkFCalController, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

public:
	/*!
   * \brief Initialize
	 */
	PlusStatus Initialize();

	/*!
	 * \brief Read configuration file and start data collection
	 * \return Success flag
	 */
	PlusStatus InitializeDataCollection();

  /*!
	 * \brief Assembles a filename that is the same as the input file name, only with the current date and time in the end (for saving to a new file)
	 * \return New configuration file nname
	 */
  std::string GetNewConfigurationFileName();

	/*!
	 * \brief Saves session configuratin data into file
   * \param aFile File path and name
	 * \return Success flag
	 */
  PlusStatus SaveConfigurationToFile(const char* aFile);

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
	 * \brief Try to parse the input file and return its content. If it fails (eg the file does not exist) then return the session configuration data
   * \param aConfigFile File path and name
	 * \return XML data element of either the input file or the session configuration data
	 */
	static vtkXMLDataElement* vtkFreehandController::ParseXMLOrFillWithInternalData(const char* aConfigFile);

	/*!
	 * \brief Dump buffers to a given directory
   * \param aDirectory Directory path
	 * \return Success flag
	 */
  PlusStatus DumpBuffersToDirectory(const char* aDirectory);

	/*!
	* \brief Locates and sets directory paths to freehand controller
	 * \return Success flag
	*/
  PlusStatus LocateDirectories();

  //TODO
  PlusStatus StartStylusCalibration();

public:
  // Get function for vtkDataCollector's ConfigurationData
  vtkXMLDataElement* GetConfigurationData();

	// Set/Get macros for member variables
	vtkSetMacro(Initialized, bool); 
	vtkGetMacro(Initialized, bool); 
	vtkBooleanMacro(Initialized, bool); 

	vtkSetMacro(RecordingFrameRate, int); 
	vtkGetMacro(RecordingFrameRate, int); 

	vtkSetStringMacro(ConfigurationFileName); 
	vtkGetStringMacro(ConfigurationFileName); 

	vtkSetStringMacro(OutputFolder); 
	vtkGetStringMacro(OutputFolder); 

	vtkGetObjectMacro(DataCollector, vtkDataCollector); 
	vtkSetObjectMacro(DataCollector, vtkDataCollector); 

protected:
	/*!
	* \brief Constructor
	*/
	vtkFreehandController();

	/*!
	* \brief Destructor
	*/
	virtual ~vtkFreehandController();	

protected:
	//! Tracker object
	vtkDataCollector*	DataCollector;

	//! Initialization flag
	bool							        Initialized;

	//! Desired frame rate of synchronized recording
	int							          RecordingFrameRate;

	//! Used configuration file name
	char*						          ConfigurationFileName;

	//! Output folder
	char*						          OutputFolder;

  //TODO
  StylusCalibrationAlgo*    StylusCalibrationAlgo;
  PhantomRegistrationAlgo*  PhantomRegistrationAlgo;
  vtkFreehandCalibrationController*    FreehandCalibrationController; //TODO make this an algo (along with the other calibration controllers) (and the base class thing)

};

#endif

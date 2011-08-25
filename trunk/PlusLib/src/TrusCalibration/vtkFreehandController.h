#ifndef __vtkFreehandController_h
#define __vtkFreehandController_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkDataCollector.h"
#include "vtkRenderer.h"

class vtkTrackedFrameList;
class QVTKWidget;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Controller of the freehand calibration application
*/
class vtkFreehandController : public vtkObject
{
public:
	/*!
	* \brief New
	*/
	static vtkFreehandController *New();

	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static vtkFreehandController* GetInstance();

public:
	/*!
	* \brief Initialize object, connect to devices, load configuration
	*/
	PlusStatus Initialize();

	/*!
	 * \brief Sets tracking only flag and forwards the request to vtkDataCollector
	 * \return Tracking only flag
	 */
	void SetTrackingOnly(bool);

	/*!
	 * \brief Read configuration file and start data collection
	 * \return Success flag
	 */
	PlusStatus StartDataCollection();

	/*!
	 * \brief Saves session configuratin data into file
   * \param aFile File path and name
	 * \return Success flag
	 */
  PlusStatus SaveConfigurationToFile(const char* aFile);

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

public:
	// Set/Get functions for canvas
	QVTKWidget* GetCanvas() { return this->Canvas; };
	void SetCanvas(QVTKWidget* aCanvas) { this->Canvas = aCanvas; };

  // Get function for vtkDataCollector's ConfigurationData
  vtkXMLDataElement* GetConfigurationData();

	// Set/Get macros for member variables
	vtkSetMacro(Initialized, bool); 
	vtkGetMacro(Initialized, bool); 
	vtkBooleanMacro(Initialized, bool); 

	vtkGetMacro(TrackingOnly, bool); 
	vtkBooleanMacro(TrackingOnly, bool); 

	vtkSetMacro(RecordingFrameRate, int); 
	vtkGetMacro(RecordingFrameRate, int); 

	vtkSetStringMacro(OutputFolder); 
	vtkGetStringMacro(OutputFolder); 

	vtkGetObjectMacro(DataCollector, vtkDataCollector); 
	vtkSetObjectMacro(DataCollector, vtkDataCollector); 

	vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
	vtkSetObjectMacro(CanvasRenderer, vtkRenderer);

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
	bool							Initialized;

	//! Flag determining if there is image recording beside tracker recording
	bool							TrackingOnly;

	//! Desired frame rate of synchronized recording
	int								RecordingFrameRate;

	//! Output folder
	char*							OutputFolder;

	//! Canvas object for real-time 3D visualization
	QVTKWidget*				Canvas;

	//! Renderer for the canvas
	vtkRenderer*			CanvasRenderer; 

private:
	//! Instance of the singleton
	static vtkFreehandController*	Instance;
};

#endif

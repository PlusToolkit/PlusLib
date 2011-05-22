#ifndef __vtkFreehandController_h
#define __vtkFreehandController_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkDataCollector.h"
#include "vtkRenderer.h"

class vtkTrackedFrameList;
class QVTKWidget;

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
	void Initialize();

	/*!
	 * \brief Get index of next active tool
	 * \return Tool index
	 */ //TODO torolni
	//int GetNextActiveToolNumber();

	/*!
	 * \brief Sets tracking only flag and forwards the request to vtkDataCollector
	 * \return Tracking only flag
	 */
	void SetTrackingOnly(bool); 

	/*!
	 * \brief Gets number of recorded frames
	 * \return Number of recorded frames
	 */ // TODO torolni
	//int GetNumberOfRecordedFrames(); 

	/*!
	 * \brief Adds tracked frame to contained
	 * \param aImageData Image
	 * \param aToolTransforms Transform matrices of the tools
	 * \param aToolTransformNames Names of the tool transforms
	 * \param aFlags Flags of tracking
	 * \param aTimestamp Timestamp of the tracked frame
	 */ // TODO torolni
	//void AddTrackedFrame(vtkImageData* aImageData, std::vector<vtkMatrix4x4*> aToolTransforms, std::vector<std::string> aToolTransformNames, std::vector<long> aFlags, double aTimestamp);

	/*!
	* \brief Empty tracked frame container for next operation
	*/ // TODO torolni
	//void ClearTrackedFrameContainer();

public:
	// Set/Get functions for canvas
	QVTKWidget* GetCanvas() { return this->Canvas; };
	void SetCanvas(QVTKWidget* aCanvas) { this->Canvas = aCanvas; };

	// Set/Get macros for member variables
	vtkSetMacro(Initialized, bool); 
	vtkGetMacro(Initialized, bool); 
	vtkBooleanMacro(Initialized, bool); 

	vtkGetMacro(TrackingOnly, bool); 
	vtkBooleanMacro(TrackingOnly, bool); 

	vtkSetMacro(RecordingFrameRate, int); 
	vtkGetMacro(RecordingFrameRate, int); 

	vtkSetStringMacro(InputConfigFileName); 
	vtkGetStringMacro(InputConfigFileName); 
	
	vtkSetStringMacro(OutputFolder); 
	vtkGetStringMacro(OutputFolder); 

	vtkGetObjectMacro(DataCollector, vtkDataCollector); 
	vtkSetObjectMacro(DataCollector, vtkDataCollector); 

	vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
	vtkSetObjectMacro(CanvasRenderer, vtkRenderer);

	vtkSetStringMacro(ProgramPath); 
	vtkGetStringMacro(ProgramPath); 

	vtkSetStringMacro(ConfigDirectory); 
	vtkGetStringMacro(ConfigDirectory); 

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
	vtkDataCollector*				DataCollector;

	//! Container of recorded data
	//vtkTrackedFrameList*			TrackedFrameContainer; //TODO

	//! Initialization flag
	bool							Initialized;

	//! Flag determining if there is image recording beside tracker recording
	bool							TrackingOnly;

	//! Desired frame rate of synchronized recording
	int								RecordingFrameRate;

	//! Input configuration file name
	char*							InputConfigFileName;

	//! Output folder
	char*							OutputFolder;

	//! Canvas object for real-time 3D visualization
	QVTKWidget*						Canvas;

	//! Renderer for the canvas
	vtkRenderer*					CanvasRenderer; 

	//! Program path
	char*							ProgramPath;

	//! Config directory path
	char*							ConfigDirectory;

private:
	//! Instance of the singleton
	static vtkFreehandController*	Instance;
};

#endif

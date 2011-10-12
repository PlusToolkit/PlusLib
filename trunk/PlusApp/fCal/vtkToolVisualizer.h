#ifndef __vtkToolVisualizer_h
#define __vtkToolVisualizer_h

#include "AbstractToolbox.h"

#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkDataCollector.h"

#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

#include <QObject>

class vtkMatrix4x4;

class QTimer;

class vtkSTLReader;

//-----------------------------------------------------------------------------

/*!
* \brief Class that encapsulates the objects needed for visualizing a tool - the tool object, the actor, a flag indicating whether it is displayable
*/
class vtkDisplayableTool : public vtkObject
{
public:
	static vtkDisplayableTool *New();

  bool IsDisplayable()
  {
    return ((this->Actor->GetMapper() != NULL) && this->Displayable);
  };

public:
	vtkGetObjectMacro(Tool, vtkTrackerTool);
	vtkSetObjectMacro(Tool, vtkTrackerTool);

  vtkGetObjectMacro(Actor, vtkActor);

	vtkBooleanMacro(Displayable, bool);
	vtkGetMacro(Displayable, bool);
	vtkSetMacro(Displayable, bool);

protected:
 	vtkSetObjectMacro(Actor, vtkActor);

protected:
  vtkDisplayableTool()
  {
	  this->Tool = NULL;
    this->Actor = NULL;

    vtkSmartPointer<vtkActor> toolActor = vtkSmartPointer<vtkActor>::New();
	  this->SetActor(toolActor);

    Displayable = true;
  };

  virtual ~vtkDisplayableTool()
  {
    this->SetActor(NULL);
  };

protected:
  vtkTrackerTool* Tool;
  vtkActor*       Actor;
  bool            Displayable;
};

//-----------------------------------------------------------------------------

/*!
* \brief Class that is responsible for visualizing the tools (and so getting acquired tracked data)
*/
class vtkToolVisualizer : public QObject, public vtkObject
{
	Q_OBJECT

public:
	/*!
	* \brief New
	*/
	static vtkToolVisualizer *New();

public:
	/*!
	* \brief Initialize object, connect to devices, load configuration
	*/
	PlusStatus Initialize();

	/*!
	* \brief Initializes device visualization - loads models, transforms, assembles visualization pipeline
	*/
  PlusStatus InitializeDeviceVisualization();

	/*!
	 * \brief Read configuration file and start data collection
	 * \return Success flag
	 */
	PlusStatus StartDataCollection();

	/*!
	 * \brief Get displayable tool object
	 * \param aType Tool type identifier
	 * \return Displayable tool object
	 */
  vtkDisplayableTool* GetDisplayableTool(TRACKER_TOOL_TYPE aType);

	/*!
	* \brief Acquires new position from stylus tool of the tracker
	* \param aType Tool type identifier
  * \param aCalibrated Flag whether to return the calibrated or the unbalibrated matrix
	* \return Acquired transform if successful, else NULL
	*/
	TrackerStatus AcquireTrackerPositionForToolByType(TRACKER_TOOL_TYPE aType, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool aCalibrated = false);

	/*!
	 * \brief Get tool position in string format
	 * \param aType Tool type identifier
	 * \param aCalibrated Flag whether to return the calibrated or the unbalibrated matrix
	 * \return Tool position string
	 */
  std::string GetToolPositionString(TRACKER_TOOL_TYPE aType, bool aCalibrated);

	/*!
	 * \brief Set phantom registration transform and enables displaying of the phantom
   * \param aTransform Phantom to phantom reference transform
	 */
  void SetPhantomToPhantomReferenceTransform(vtkTransform* aTransform);

	/*!
	 * \brief Hide all tools from main canvas
	 * \return Success flag
	 */
  PlusStatus HideAll();

	/*!
	 * \brief Show or hide a tool
   * \param aType Tool type identifier
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowTool(TRACKER_TOOL_TYPE aType, bool aOn);

	/*!
	 * \brief Show or hide input points
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowInput(bool aOn);

	/*!
	 * \brief Show or hide result points
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowResult(bool aOn);

	/*!
	 * \brief Enable/disable image mode
	 * \param aOn Image mode flag - true: show only the image and interactions are off - false: show all toola and the image and interactions are on
	 * \return Success flag
	 */
  PlusStatus EnableImageMode(bool aOn);

	/*!
	 * \brief Enable/disable camera movements (mouse interactions on rendering window)
	 * \param aEnabled Trackball interactions if true, no interactions if false
	 * \return Success flag
	 */
  PlusStatus EnableCameraMovements(bool aEnabled);

	/*!
	 * \brief Calculate and set camera parameters so that image fits canvas in image mode
	 * \return Success flag
	 */
	PlusStatus CalculateImageCameraParameters();

	/*!
	 * \brief Dump video and tracker buffers to a given directory
   * \param aDirectory Destination directory
	 * \return Success flag
	 */
  PlusStatus DumpBuffersToDirectory(const char* aDirectory);

	/*!
	 * \brief Load phantom model to an STL reader object (parse up model file and load it to the STL reader for use in the main canvas or elsewhere)
   * \param aSTLReader STL reader object
	 * \return Success flag
	 */
  PlusStatus LoadPhantomModel(vtkSTLReader* aSTLReader);

	/*!
	 * \brief Return acquisition timer (to be able to connect actions to it)
	 * \return Acquisition timer object
	 */
  QTimer* GetAcquisitionTimer() { return this->AcquisitionTimer; };

protected:
	/*!
	* \brief Initialize 3D visualization
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

	/*!
	* \brief Initialize phantom visualization (registration, model to phantom transform, phantom model)
	* \return Success flag
	*/
  PlusStatus InitializePhantomVisualization();

	/*!
	* \brief Assemble and set default stylus model for stylus tool actor
	* \param aActor Actor to add the model to
	* \return Success flag
	*/
	PlusStatus SetDefaultStylusModel(vtkActor* aActor);

protected slots:
	/*!
	* \brief Displays the devices if not in image mode
	* \return Success flag
	*/
  PlusStatus DisplayDevices();

public:
	// Set/Get macros for member variables
  vtkSetMacro(Initialized, bool); 
	vtkGetMacro(Initialized, bool); 
	vtkBooleanMacro(Initialized, bool); 

	vtkGetMacro(ImageMode, bool);

	vtkGetObjectMacro(ImageActor, vtkImageActor);

  PlusStatus SetAcquisitionFrameRate(int aFrameRate); 
	vtkGetMacro(AcquisitionFrameRate, int); 

	vtkGetObjectMacro(DataCollector, vtkDataCollector); 
	vtkSetObjectMacro(DataCollector, vtkDataCollector); 

	vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
	vtkSetObjectMacro(CanvasRenderer, vtkRenderer);

	vtkGetObjectMacro(InputPolyData, vtkPolyData);
	vtkGetObjectMacro(ResultPolyData, vtkPolyData);

	vtkSetObjectMacro(ImageToProbeTransform, vtkTransform);

protected:
	vtkSetObjectMacro(ImageActor, vtkImageActor);
	vtkSetObjectMacro(InputActor, vtkActor);
	vtkSetObjectMacro(InputPolyData, vtkPolyData);
	vtkSetObjectMacro(ResultActor, vtkActor);
	vtkSetObjectMacro(ResultPolyData, vtkPolyData);
	vtkSetObjectMacro(ImageCamera, vtkCamera);

	vtkSetMacro(ImageMode, bool); 
	vtkBooleanMacro(ImageMode, bool); 

protected:
	/*!
	* \brief Constructor
	*/
	vtkToolVisualizer();

	/*!
	* \brief Destructor
	*/
	virtual ~vtkToolVisualizer();	

protected:
	//! Data collector object
	vtkDataCollector*	                DataCollector;

  //! List of displayable tools (structures holding the tool objects and actors)
  std::vector<vtkDisplayableTool*>  DisplayableToolVector;

  //! Timer for acquisition
	QTimer*	                          AcquisitionTimer;

  //! Initialization flag
	bool							                Initialized;

  //! Flag indicating if the cisualization is in image mode (show only the image and interactions are off) or device display mode (show all tools and the image and interactions are on)
  bool                              ImageMode;

	//! Desired frame rate of synchronized recording
	int								                AcquisitionFrameRate;

	//! Renderer for the canvas
	vtkRenderer*			                CanvasRenderer; 

  //! Canvas image actor
  vtkImageActor*                    ImageActor;

	//! Polydata holding the input points
	vtkPolyData*			                InputPolyData;

	//! Actor for displaying the input points in 3D
	vtkActor*					                InputActor;

	//! Polydata holding the result points (eg. stylus tip, segmented points)
	vtkPolyData*			                ResultPolyData;

	//! Actor for displaying the result points (eg. stylus tip, segmented points)
	vtkActor*					                ResultActor;

	//! Camera of the scene
	vtkCamera*		                    ImageCamera;

  //! Image to probe transform (probe calibration result)
  vtkTransform*                     ImageToProbeTransform;

};

#endif

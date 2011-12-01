/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkToolVisualizer_h
#define __vtkToolVisualizer_h

#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkDataCollector.h"

#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkTransformRepository.h"

#include <QObject>

class vtkMatrix4x4;

class QTimer;

class vtkSTLReader;

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableTool 
 * \brief Class that encapsulates the objects needed for visualizing a tool - the tool object, the actor, a flag indicating whether it is displayable
 * \ingroup PlusAppFCal
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
  /*! Pointer to the tool object */
  vtkTrackerTool* Tool;

  /*! Actor displaying the tool model */
  vtkActor*       Actor;

  /*! Flag that can disable displaying of this tool */
  bool            Displayable;
};

//-----------------------------------------------------------------------------

/*! \class vtkToolVisualizer 
 * \brief Class that is responsible for visualizing the tools (and so getting acquired tracked data)
 * \ingroup PlusAppFCal
 */
class vtkToolVisualizer : public QObject, public vtkObject
{
	Q_OBJECT

public:
	/*!
	* New
	*/
	static vtkToolVisualizer *New();

public:
	/*!
	* Initialize object, connect to devices, load configuration
	*/
	PlusStatus Initialize();

	/*!
	* Initializes device visualization - loads models, transforms, assembles visualization pipeline
	*/
  PlusStatus InitializeDeviceVisualization();

	/*!
	 * Read configuration file and start data collection
	 * \return Success flag
	 */
	PlusStatus StartDataCollection();

	/*!
	 * Get displayable tool object
	 * \param aToolName Name of the tool
	 * \param aDisplayableTool Displayable tool object out parameter
	 * \return Success flag
	 */
  PlusStatus GetDisplayableTool(const char* aToolName, vtkDisplayableTool* &aDisplayableTool);

	/*!
	* Acquires new position from a given tool of the tracker
	* \param aName Tool name
  * \param aCalibrated Flag whether to return the calibrated or the unbalibrated matrix
	* \return Acquired transform if successful, else NULL
	*/
  TrackerStatus AcquireTrackerPositionForToolByName(const char* aName, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool aCalibrated = false);

	/*!
	 * Get tool position in string format
	 * \param aToolName Name of the tool
	 * \param aCalibrated Flag whether to return the calibrated or the unbalibrated matrix
	 * \return Tool position string
	 */
  std::string GetToolPositionString(const char* aToolName, bool aCalibrated);

	/*!
	 * Set phantom registration transform and enables displaying of the phantom
   * \param aTransform Phantom to phantom reference transform
	 */
  void SetPhantomToReferenceTransform(vtkTransform* aTransform);

	/*!
	 * Hide all tools, other models and the image from main canvas
	 * \return Success flag
	 */
  PlusStatus HideAll();

	/*!
	 * Show or hide all tools
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowAllTools(bool aOn);

	/*!
	 * Show or hide a tool
   * \param aToolName Tool name
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowTool(const char* aToolName, bool aOn);

	/*!
	 * Show or hide input points
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowInput(bool aOn);

	/*!
	 * Show or hide result points
   * \param aOn Show if true, else hide
	 * \return Success flag
	 */
  PlusStatus ShowResult(bool aOn);

	/*!
	 * Enable/disable image mode
	 * \param aOn Image mode flag - true: show only the image and interactions are off - false: show all toola and the image and interactions are on
	 * \return Success flag
	 */
  PlusStatus EnableImageMode(bool aOn);

	/*!
	 * Enable/disable camera movements (mouse interactions on rendering window)
	 * \param aEnabled Trackball interactions if true, no interactions if false
	 * \return Success flag
	 */
  PlusStatus EnableCameraMovements(bool aEnabled);

	/*!
	 * Calculate and set camera parameters so that image fits canvas in image mode
	 * \return Success flag
	 */
	PlusStatus CalculateImageCameraParameters();

	/*!
	 * Dump video and tracker buffers to a given directory
   * \param aDirectory Destination directory
	 * \return Success flag
	 */
  PlusStatus DumpBuffersToDirectory(const char* aDirectory);

	/*!
	 * Load phantom model to an STL reader object (parse up model file and load it to the STL reader for use in the main canvas or elsewhere)
   * \param aSTLReader STL reader object
	 * \return Success flag
	 */
  PlusStatus LoadPhantomModel(vtkSTLReader* aSTLReader);

	/*!
	 * Return acquisition timer (to be able to connect actions to it)
	 * \return Acquisition timer object
	 */
  QTimer* GetAcquisitionTimer() { return this->AcquisitionTimer; };

protected:
	/*!
	* Initialize 3D visualization
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

	/*!
	* Initialize phantom visualization (registration, model to phantom transform, phantom model)
	* \return Success flag
	*/
  PlusStatus InitializePhantomVisualization();

	/*!
	* Assemble and set default stylus model for stylus tool actor
	* \param aActor Actor to add the model to
	* \return Success flag
	*/
	PlusStatus SetDefaultStylusModel(vtkActor* aActor);

protected slots:
	/*!
	* Displays the devices if not in image mode
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

	vtkGetObjectMacro(TransformRepository, vtkTransformRepository);

	vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
	vtkSetObjectMacro(CanvasRenderer, vtkRenderer);

	vtkGetObjectMacro(InputPolyData, vtkPolyData);
	vtkGetObjectMacro(ResultPolyData, vtkPolyData);

	vtkGetObjectMacro(VolumeActor, vtkActor);

  vtkSetObjectMacro(ImageToProbeTransform, vtkTransform);

  vtkGetStringMacro(ProbeToolName);
  vtkSetStringMacro(ProbeToolName);

  vtkGetStringMacro(ReferenceToolName);
  vtkSetStringMacro(ReferenceToolName);

protected:
	vtkSetObjectMacro(ImageActor, vtkImageActor);
	vtkSetObjectMacro(InputActor, vtkActor);
	vtkSetObjectMacro(InputPolyData, vtkPolyData);
	vtkSetObjectMacro(ResultActor, vtkActor);
	vtkSetObjectMacro(ResultPolyData, vtkPolyData);
	vtkSetObjectMacro(VolumeActor, vtkActor);
	vtkSetObjectMacro(ImageCamera, vtkCamera);
	vtkSetObjectMacro(TransformRepository, vtkTransformRepository);

	vtkSetMacro(ImageMode, bool); 
	vtkBooleanMacro(ImageMode, bool); 

protected:
	/*!
	* Constructor
	*/
	vtkToolVisualizer();

	/*!
	* Destructor
	*/
	virtual ~vtkToolVisualizer();	

protected:
	/*! Data collector object */
	vtkDataCollector*	DataCollector;

  /*! List of displayable tools (structures holding the tool objects and actors) */
  std::map<std::string, vtkDisplayableTool*> DisplayableTools;

  /*! Transform repository to store and handle all transforms */
  vtkTransformRepository* TransformRepository;

  /*! Timer for acquisition */
	QTimer*	AcquisitionTimer;

  /*! Initialization flag */
	bool Initialized;

  /*! Flag indicating if the cisualization is in image mode (show only the image and interactions are off) or device display mode (show all tools and the image and interactions are on) */
  bool ImageMode;

	/*! Desired frame rate of synchronized recording */
	int AcquisitionFrameRate;

  /*! Name of the tool (which must be a probe) that acquires the images to display */
  char* ProbeToolName;

  /*! Name of the reference tool */
  char* ReferenceToolName;

  /*! Renderer for the canvas */
	vtkRenderer* CanvasRenderer; 

  /*! Canvas image actor */
  vtkImageActor* ImageActor;

	/*! Polydata holding the input points */
	vtkPolyData* InputPolyData;

	/*! Actor for displaying the input points in 3D */
	vtkActor* InputActor;

	/*! Polydata holding the result points (eg. stylus tip, segmented points) */
	vtkPolyData* ResultPolyData;

	/*! Actor for displaying the result points (eg. stylus tip, segmented points) */
	vtkActor* ResultActor;

	/*! Actor for displaying a volume */
	vtkActor* VolumeActor;

  /*! Camera of the scene */
	vtkCamera* ImageCamera;

  /*! Image to probe transform (probe calibration result) */
  vtkTransform* ImageToProbeTransform;
};

#endif

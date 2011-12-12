/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkToolVisualizer_h
#define __vtkToolVisualizer_h

#include "vtkDataCollector.h"
#include "TrackedFrame.h"
#include "vtkDisplayableObject.h"

#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkTransformRepository.h"

#include <QObject>

class vtkMatrix4x4;
class vtkTransform;

class QTimer;

class vtkSTLReader;

//-----------------------------------------------------------------------------

/*! \class vtkToolVisualizer 
 \brief Class that is responsible for visualizing the tools (and so getting acquired tracked data)

 Usage: Instantiate, call Initialize function and add its canvas renderer to the vtkRenderWindow (get it by calling QVTKWidget::GetRenderWindow() function)
   that can be reached by calling its GetCanvasRenderer() function. Updating the visualization is done by calling the update() function of the application's
   QVTKWidget object. Before calling this, force the data collector to provide new data by calling GetDataCollector()->Modified() function.

   It has two  modes, ImageMode and DeviceMode. In image mode it shows only the video input in the whole window (you'd better call CalculateImageCameraParameters()
   function in the resize event of the application window because of this). In DeviceMode, all the devices and the image is visible (that are defined in the device
   set configuration file's Rendering element). Devices and objects can be shown and hidden (HideAll(), ShowAllObjects(), ShowObject(), ShowInput(), ShowResult())

 \ingroup PlusAppCommonWidgets
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
	* Initialize object
	*/
	PlusStatus Initialize();

  /*!
  * Read rendering configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
	* Initializes device visualization - loads models, transforms, assembles visualization pipeline
	*/
  PlusStatus InitializeObjectVisualization();

	/*!
	 * Read configuration file and start data collection
	 */
	PlusStatus StartDataCollection();

	/*!
	 * Get displayable tool object
	 * \param aObjectCoordinateFrame Object coordinate frame name
	 * \param aDisplayableTool Displayable object out parameter
	 */
  PlusStatus GetDisplayableObject(const char* aObjectCoordinateFrame, vtkDisplayableObject* &aDisplayableTool);

  /*! Clear displayable object vector */
  PlusStatus ClearDisplayableObjects();

	/*!
	 * Hide all tools, other models and the image from main canvas
	 */
  PlusStatus HideAll();

	/*!
	 * Show or hide all displayable objects
   * \param aOn Show if true, else hide
	 */
  PlusStatus ShowAllObjects(bool aOn);

	/*!
	 * Show or hide a displayable object
   * \param aObjectCoordinateFrame Object coordinate frame name
   * \param aOn Show if true, else hide
	 */
  PlusStatus ShowObject(const char* aObjectCoordinateFrame, bool aOn);

	/*!
	 * Show or hide input points
   * \param aOn Show if true, else hide
	 */
  PlusStatus ShowInput(bool aOn);

	/*!
	 * Show or hide result points
   * \param aOn Show if true, else hide
	 */
  PlusStatus ShowResult(bool aOn);

	/*!
	 * Enable/disable image mode
	 * \param aOn Image mode flag - true: show only the image and interactions are off - false: show all toola and the image and interactions are on
	 */
  PlusStatus EnableImageMode(bool aOn);

	/*!
	 * Enable/disable camera movements (mouse interactions on rendering window)
	 * \param aEnabled Trackball interactions if true, no interactions if false
	 */
  PlusStatus EnableCameraMovements(bool aEnabled);

	/*!
	 * Calculate and set camera parameters so that image fits canvas in image mode
	 */
	PlusStatus CalculateImageCameraParameters();

	/*!
	 * Dump video and tracker buffers to a given directory
   * \param aDirectory Destination directory
	 */
  PlusStatus DumpBuffersToDirectory(const char* aDirectory);

	/*!
	 * Return acquisition timer (to be able to connect actions to it)
	 * \return Acquisition timer object
	 */
  QTimer* GetAcquisitionTimer() { return this->AcquisitionTimer; };

  /*!
    Check if a transform exists in transform repository
    /param aUseLatestTrackedFrame Get latest tracked frame and set its transforms to transform repository before checking
  */
  PlusStatus IsExistingTransform(const char* aTransformFrom, const char* aTransformTo, bool aUseLatestTrackedFrame = true);

  /*!
    Acquire transform matrix from tracking and provide string containing the translation part
    /param aTransformTranslationString Out parameter for the position string
    /param aValid True if the transform is valid, false otherwise (optional parameter)
  */
  PlusStatus GetTransformTranslationString(const char* aTransformFrom, const char* aTransformTo, std::string &aTransformTranslationString, bool* aValid = NULL);
  /*!
    Acquire transform matrix from tracking and provide string containing the translation part
    /param aTransformTranslationString Out parameter for the position string
    /param aValid True if the transform is valid, false otherwise (optional parameter)
  */
  PlusStatus GetTransformTranslationString(PlusTransformName aTransform, std::string &aTransformTranslationString, bool* aValid = NULL);

  /*!
    Acquire transform matrix from tracking
    /param aOutputMatrix Out parameter for the transform matrix
    /param aValid True if the transform is valid, false otherwise (optional parameter)
  */
  PlusStatus GetTransformMatrix(const char* aTransformFrom, const char* aTransformTo, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid = NULL);
  /*!
    Acquire transform matrix from tracking
    /param aOutputMatrix Out parameter for the transform matrix
    /param aValid True if the transform is valid, false otherwise (optional parameter)
  */
  PlusStatus GetTransformMatrix(PlusTransformName aTransform, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid = NULL);

protected:
	/*!
	* Initialize 3D visualization
	*/
	PlusStatus InitializeBasicVisualization();

protected slots:
	/*!
	* Displays the displayable objects if not in image mode
	*/
  PlusStatus UpdateObjectVisualization();

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

  vtkGetStringMacro(WorldCoordinateFrame);
  vtkSetStringMacro(WorldCoordinateFrame);

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

  /*! List of displayable objects */
  std::map<std::string, vtkDisplayableObject*> DisplayableObjects;

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

  /*! Name of the rendering world coordinate frame */
  char* WorldCoordinateFrame;

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
};

#endif

#ifndef __vtkFCalVisualizer_h
#define __vtkFCalVisualizer_h

#include "PlusConfigure.h"

#include "vtkFreehandController.h"

#include "vtkObject.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"

class QVTKWidget;
class QTimer;
class AbstractToolbox;

//-----------------------------------------------------------------------------

// Tracker tool types
enum FCAL_TOOLBOX_TYPE
{
	FCAL_TOOLBOX_UNDEFINED = -1,
	FCAL_TOOLBOX_CONFIGURATION,
  FCAL_TOOLBOX_STYLUS_CALIBRATION,
  FCAL_TOOLBOX_PHANTOM_REGISTRATION,
  FCAL_TOOLBOX_FREEHAND_CALIBRATION,
  FCAL_TOOLBOX_VOLUME_RECONSTRUCTION
}; 


//-----------------------------------------------------------------------------

/*!
* \brief Controller of the freehand calibration application
*/
class vtkFCalVisualizer : public vtkObject
{
public:
	/*!
	* \brief New
	*/
	static vtkFCalVisualizer *New();

	vtkTypeRevisionMacro(vtkFCalVisualizer, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

public:
	/*!
   * \brief Initialize
	 */
	PlusStatus Initialize();

	/*!
   * \brief Initialize visualization
	 */
	PlusStatus InitializeVisualization();

	/*!
   * \brief Initialize tool (device) visualization
	 * \return Success flag
	 */
  PlusStatus InitializeToolVisualization();

	/*!
   * \brief Acquire new tracker positions and displays tools on canvas
	 * \return Success flag
	 */
	PlusStatus DisplayTools();

	/*!
	 * \brief Gets active toolbox
	 * \return Pointer to abstract toolbox of the active toolbox
	 */
  AbstractToolbox* GetActiveToolbox();

	/*!
	 * \brief Sets active toolbox and changes visualization accordingly
   * \param aToolboxType Active toolbox type
	 * \return Success flag
	 */
	PlusStatus SetActiveToolbox(FCAL_TOOLBOX_TYPE aToolboxType);

  //TODO
  PlusStatus Start(FCAL_TOOLBOX_TYPE aToolboxType);
  PlusStatus Stop(FCAL_TOOLBOX_TYPE aToolboxType);
  PlusStatus Clear(FCAL_TOOLBOX_TYPE aToolboxType);

public:
	// Set/Get functions for canvas
	QVTKWidget* GetCanvas() { return this->Canvas; };
	void SetCanvas(QVTKWidget* aCanvas) { this->Canvas = aCanvas; };

	// Set/Get macros for member variables
	vtkSetMacro(Initialized, bool); 
	vtkGetMacro(Initialized, bool); 
	vtkBooleanMacro(Initialized, bool); 

	vtkGetObjectMacro(CanvasRenderer, vtkRenderer);
	vtkSetObjectMacro(CanvasRenderer, vtkRenderer);

	vtkGetObjectMacro(CanvasImageActor, vtkImageActor);
	vtkSetObjectMacro(CanvasImageActor, vtkImageActor);

	vtkGetObjectMacro(PhantomBodyActor, vtkActor);
	vtkSetObjectMacro(PhantomBodyActor, vtkActor);

	vtkGetObjectMacro(ProbeActor, vtkActor);
	vtkSetObjectMacro(ProbeActor, vtkActor);

	vtkGetObjectMacro(StylusActor, vtkActor);
	vtkSetObjectMacro(StylusActor, vtkActor);

	vtkGetObjectMacro(NeedleActor, vtkActor);
	vtkSetObjectMacro(NeedleActor, vtkActor);
	
	vtkGetObjectMacro(SegmentedPointsActor, vtkActor);
	vtkSetObjectMacro(SegmentedPointsActor, vtkActor);

	vtkGetObjectMacro(SegmentedPointsPolyData, vtkPolyData);
	vtkSetObjectMacro(SegmentedPointsPolyData, vtkPolyData);

	vtkGetObjectMacro(InputPointsActor, vtkActor);
	vtkSetObjectMacro(InputPointsActor, vtkActor);

	vtkGetObjectMacro(StylusTipActor, vtkActor);
	vtkSetObjectMacro(StylusTipActor, vtkActor);

	vtkGetObjectMacro(ImageCamera, vtkCamera);
	vtkSetObjectMacro(ImageCamera, vtkCamera);

	vtkGetObjectMacro(FCalController, vtkFCalController);
	vtkSetObjectMacro(FCalController, vtkFCalController);

  vtkGetMacro(ActiveToolbox, FCAL_TOOLBOX_TYPE);

protected:
	/*!
	* \brief Constructor
	*/
	vtkFCalVisualizer();

	/*!
	* \brief Destructor
	*/
	virtual ~vtkFCalVisualizer();	

protected:
	//! Initialization flag
	bool							      Initialized;

	//! Canvas object for real-time 3D visualization
	QVTKWidget*			        Canvas;

	//! Renderer for the canvas
	vtkRenderer*		        CanvasRenderer; 

	//! Actor displaying the image
	vtkImageActor*	        CanvasImageActor;

	//! Camera of the scene
	vtkCamera*		          ImageCamera;

	//! Actor for displaying the phantom body
	vtkActor*               PhantomBodyActor;

	//! Actor for displaying the probe
	vtkActor*               ProbeActor;

	//! Actor for displaying the stylus
	vtkActor*               StylusActor;

	//! Actor for displaying the needle
	vtkActor*               NeedleActor;

	//! Actor for displaying segmented points
	vtkActor*               SegmentedPointsActor;

  //! TODO
  vtkActor*               InputPointsActor;

  //! TODO
  vtkActor*               StylusTipActor;

	//! Poly data for holding the segmented points
	vtkPolyData*            SegmentedPointsPolyData;

	//! Active toolbox identifier
	FCAL_TOOLBOX_TYPE       ActiveToolbox;

  //! TODO
  vtkFCalController*      FCalController;

	//! Timer for acquisition
	QTimer*                 AcquisitionTimer;
};

#endif

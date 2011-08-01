#include "PlusConfigure.h"

#include "vtkFreehandCalibrationController.h"

#include "vtkFreehandController.h"
#include "PhantomRegistrationController.h"
#include "StylusCalibrationController.h"

#include "vtkObjectFactory.h"
#include "vtkAccurateTimer.h"
#include "vtkTrackerTool.h"
#include "vtkFileFinder.h"

#include "vtkMath.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSTLReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkTransformPolyDataFilter.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFreehandCalibrationController, "$Revision: 1.0 $");

vtkFreehandCalibrationController *vtkFreehandCalibrationController::Instance = NULL;

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController* vtkFreehandCalibrationController::New()
{
	return vtkFreehandCalibrationController::GetInstance();
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController* vtkFreehandCalibrationController::GetInstance() {
	if(!vtkFreehandCalibrationController::Instance) {
		// Try the factory first
		vtkFreehandCalibrationController::Instance = (vtkFreehandCalibrationController*)vtkObjectFactory::CreateInstance("vtkFreehandCalibrationController");    

		if(!vtkFreehandCalibrationController::Instance) {
			vtkFreehandCalibrationController::Instance = new vtkFreehandCalibrationController();	   
		}
	}
	// return the instance
	return vtkFreehandCalibrationController::Instance;
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::vtkFreehandCalibrationController()
	: AbstractToolboxController()
{
	this->TemporalCalibrationDone = false;
	this->SpatialCalibrationDone = false;
	this->ProgressPercent = 0;
	this->CancelRequest = false;
	this->ShowDevices = false;

	this->USImageFrameOriginXInPixels = 0; 
	this->USImageFrameOriginYInPixels = 0; 

	// Initializing vtkCalibrationController members
	this->InitializedOff(); 
	this->EnableSystemLogOff();
	this->SetCalibrationMode(REALTIME); 
	this->CanvasImageActor = NULL;
	this->PhantomBodyActor = NULL;
	this->ProbeActor = NULL;
	this->StylusActor = NULL;
	this->NeedleActor = NULL;
	this->SegmentedPointsActor = NULL;
	this->SegmentedPointsPolyData = NULL;
	this->ImageCamera = NULL;
	this->CalibrationResultFileNameWithPath = NULL;
	this->CalibrationResultFileSuffix = NULL;

	this->Calibrator = NULL;

	vtkSmartPointer<vtkTransform> transformImageToProbe = vtkSmartPointer<vtkTransform>::New(); 
	this->TransformImageToProbe = NULL;
	this->SetTransformImageToProbe(transformImageToProbe); 

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::~vtkFreehandCalibrationController()
{
	this->SetCanvasImageActor(NULL);
	this->SetPhantomBodyActor(NULL);
	this->SetProbeActor(NULL);
	this->SetStylusActor(NULL);
	this->SetNeedleActor(NULL);
	this->SetSegmentedPointsActor(NULL);
	this->SetSegmentedPointsPolyData(NULL);
	this->SetTransformImageToProbe(NULL);

	if (this->Calibrator != NULL) {
		delete this->Calibrator;
		this->Calibrator = NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::Initialize()
{
	LOG_TRACE("vtkFreehandCalibrationController::Initialize");

	if (m_Toolbox) {
		m_Toolbox->Initialize();
	}

	// Initialize visualization
	if (! InitializeVisualization()) {
		LOG_ERROR("Initializing freehand calibration visualization failed!");
		return PLUS_FAIL;
	}

	// Set state to idle (initialized)
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;

		this->InitializedOn();
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::InitializeVisualization()
{
	LOG_TRACE("vtkFreehandCalibrationController::InitializeVisualization");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

	if (m_State == ToolboxState_Uninitialized) {
		if (controller->GetCanvas() != NULL) {
			// Initialize canvas image actor
			if (m_State == ToolboxState_Uninitialized) {
				vtkSmartPointer<vtkImageActor> canvasImageActor = vtkSmartPointer<vtkImageActor>::New();

				if (dataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE) {
					canvasImageActor->VisibilityOn();
					canvasImageActor->SetInput(dataCollector->GetOutput());
				} else {
					LOG_WARNING("Data collector has no output port, canvas image actor initalization failed.");
				}

				this->SetCanvasImageActor(canvasImageActor);
			}

			// Create segmented points actor
			vtkSmartPointer<vtkActor> segmentedPointsActor = vtkSmartPointer<vtkActor>::New();

			vtkSmartPointer<vtkPolyData> segmentedPointsPolyData = vtkSmartPointer<vtkPolyData>::New();
			segmentedPointsPolyData->Initialize();
			this->SetSegmentedPointsPolyData(segmentedPointsPolyData);

			vtkSmartPointer<vtkPolyDataMapper> segmentedPointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkGlyph3D> segmentedPointGlyph = vtkSmartPointer<vtkGlyph3D>::New();
			vtkSmartPointer<vtkSphereSource> segmentedPointSphereSource = vtkSmartPointer<vtkSphereSource>::New();
			segmentedPointSphereSource->SetRadius(4.0);

			segmentedPointGlyph->SetInputConnection(this->SegmentedPointsPolyData->GetProducerPort());
			segmentedPointGlyph->SetSourceConnection(segmentedPointSphereSource->GetOutputPort());
			segmentedPointMapper->SetInputConnection(segmentedPointGlyph->GetOutputPort());

			segmentedPointsActor->SetMapper(segmentedPointMapper);
			segmentedPointsActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
			segmentedPointsActor->VisibilityOff();

			this->SetSegmentedPointsActor(segmentedPointsActor);
			
			// Add actors to the realtime renderer, and add renderer to Canvas
			// If already initialized (it can occur if tab change - and so clear - happened)
			controller->GetCanvasRenderer()->AddActor(this->CanvasImageActor);
			controller->GetCanvasRenderer()->AddActor(this->SegmentedPointsActor);
			controller->GetCanvasRenderer()->SetBackground(0.2, 0.2, 0.2);
			controller->GetCanvasRenderer()->InteractiveOff(); // TODO it doesn't work - find a way to disable interactions (also re-enable on Clear)
			//controller->GetCanvasRenderer()->GetRenderWindow()->GetInteractor()->Disable();
			controller->GetCanvasRenderer()->Modified();

			// Compute image camera parameters and set it to display live image
			CalculateImageCameraParameters();
		}
	} else if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {  // If already initialized (it can occur if tab change - and so clear - happened)
		// Add all actors to the renderer again - state must be "Done", because tab cannot be changed if "In progress"
		vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();

		renderer->AddActor(this->CanvasImageActor);
		renderer->AddActor(this->SegmentedPointsActor);

		if (this->ShowDevices) {
			renderer->AddActor(this->PhantomBodyActor);
			renderer->AddActor(this->ProbeActor);
			renderer->AddActor(this->StylusActor);
			renderer->AddActor(this->NeedleActor);
		}

		renderer->Modified();

		// Compute image camera parameters and set it to display live image
		CalculateImageCameraParameters();
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::InitializeDeviceVisualization()
{
	LOG_TRACE("vtkFreehandCalibrationController::InitializeDeviceVisualization");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller == NULL) {
		LOG_ERROR("vtkFreehandController is invalid");
		return PLUS_FAIL;
	}
	if (controller->GetInitialized() == false) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is not initialized!");
		return PLUS_FAIL;
	}

	vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();

	if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
		// Load phantom model and create phantom body actor
		if ((this->ModelToPhantomTransform != NULL) && (PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform() != NULL)) {
			// Initialize phantom model visualization
			if (! vtksys::SystemTools::FileExists(this->PhantomModelFileName)) {
				LOG_WARNING("Phantom model file is not found in the specified path: " << this->PhantomModelFileName);
			} else {
				vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
				stlReader->SetFileName(this->PhantomModelFileName);
				
				vtkSmartPointer<vtkTransform> phantomModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
				phantomModelToPhantomReferenceTransform->Identity();
				phantomModelToPhantomReferenceTransform->Concatenate(PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform());
				phantomModelToPhantomReferenceTransform->Concatenate(this->ModelToPhantomTransform);
				phantomModelToPhantomReferenceTransform->Modified();

				vtkSmartPointer<vtkTransformPolyDataFilter> phantomModelToPhantomReferenceTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				phantomModelToPhantomReferenceTransformFilter->AddInputConnection(stlReader->GetOutputPort());
				phantomModelToPhantomReferenceTransformFilter->SetTransform(phantomModelToPhantomReferenceTransform);

				vtkSmartPointer<vtkPolyDataMapper> phantomMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				phantomMapper->SetInputConnection(phantomModelToPhantomReferenceTransformFilter->GetOutputPort());
				vtkSmartPointer<vtkActor> phantomBodyActor = vtkSmartPointer<vtkActor>::New();
				phantomBodyActor->SetMapper(phantomMapper);
				this->SetPhantomBodyActor(phantomBodyActor);
			}
		} else {
			LOG_WARNING("Phantom definiton file or its related transforms are not specified, phantom will not be displayed");
		}

		// Load device models and create actors
		for (int i=0; i<dataCollector->GetTracker()->GetNumberOfTools(); ++i) {
			vtkTrackerTool *tool = dataCollector->GetTracker()->GetTool(i);
			if ((tool == NULL) || (!tool->GetEnabled())) {
				continue;
			}

			vtkSmartPointer<vtkActor> deviceActor = vtkSmartPointer<vtkActor>::New();

			// Set proper members
			if (STRCASECMP(tool->GetToolName(), "Stylus") == 0) {
				StylusCalibrationController::GetInstance()->LoadStylusModel(deviceActor);

				this->SetStylusActor(deviceActor);

			} else {
				// Load model if file name exists and file can be found
				if (STRCASECMP(tool->GetTool3DModelFileName(), "") != 0) {
					std::string searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(tool->GetTool3DModelFileName());

					if (STRCASECMP("", searchResult.c_str()) == 0) {
						LOG_WARNING("Tool (" << tool->GetToolName() << ") model file is not found with name: " << tool->GetTool3DModelFileName());
					} else {
						vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
						stlReader->SetFileName(searchResult.c_str());

						// TODO Try to use this filter instead of always setting all these transforms on every acquisition
						//vtkSmartPointer<vtkTransform> toolModelToToolReferenceTransform = vtkSmartPointer<vtkTransform>::New();
						//toolModelToToolReferenceTransform->Identity();
						//toolModelToToolReferenceTransform->Concatenate(tool->GetToolToToolReferenceTransform());
						//toolModelToToolReferenceTransform->Concatenate(tool->GetModelToToolTransform());
						//toolModelToToolReferenceTransform->Modified();

						//vtkSmartPointer<vtkTransformPolyDataFilter> toolModelToToolReferenceTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
						//toolModelToToolReferenceTransformFilter->AddInputConnection(stlReader->GetOutputPort());
						//toolModelToToolReferenceTransformFilter->SetTransform(toolModelToToolReferenceTransform);

						vtkSmartPointer<vtkPolyDataMapper> toolMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
						//toolMapper->SetInputConnection(toolModelToToolReferenceTransformFilter->GetOutputPort());
						toolMapper->SetInputConnection(stlReader->GetOutputPort());

						deviceActor->SetMapper(toolMapper);

						if (STRCASECMP(tool->GetToolName(), "Probe") == 0) {
							this->SetProbeActor(deviceActor);

						} else if (STRCASECMP(tool->GetToolName(), "Needle") == 0) {
							this->SetNeedleActor(deviceActor);
						}
					}
				}
			}
		} // for each tool

		if (this->ProbeActor == NULL) {
			LOG_ERROR("Initializing probe visualization failed!");
			return PLUS_FAIL;
		}
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::ToggleDeviceVisualization(bool aOn)
{
	LOG_TRACE("vtkFreehandCalibrationController::ToggleDeviceVisualization(" << (aOn?"true":"false") << ")");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}
	vtkRenderer* renderer = controller->GetCanvasRenderer();

	if (aOn == false) {
		// Reset image canvas position
		vtkSmartPointer<vtkTransform> identity = vtkSmartPointer<vtkTransform>::New();
		identity->Identity();
		this->CanvasImageActor->SetUserTransform(identity);

		// Remove device actors
		renderer->RemoveActor(this->PhantomBodyActor);
		renderer->RemoveActor(this->ProbeActor);
		renderer->RemoveActor(this->StylusActor);
		renderer->RemoveActor(this->NeedleActor);

		// Calculate camera to show only the image
		CalculateImageCameraParameters();
	} else if (aOn == true) {
		// Add device actors
		renderer->AddActor(this->PhantomBodyActor);
		renderer->AddActor(this->ProbeActor);
		renderer->AddActor(this->StylusActor);
		renderer->AddActor(this->NeedleActor);

		// Reset camera to show all devices and the image
		vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
		imageCamera->SetViewUp(0, -1, 0);
		imageCamera->ParallelProjectionOff();
		this->SetImageCamera(imageCamera);
		renderer->SetActiveCamera(this->ImageCamera);
		renderer->ResetCamera();
	}

	this->SetShowDevices(aOn);
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::CalculateImageCameraParameters()
{
	LOG_TRACE("vtkFreehandCalibrationController::CalculateImageCameraParameters");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if (dataCollector->GetVideoSource() == NULL) {
		LOG_WARNING("Data collector has no video source!");
		return PLUS_FAIL;
	}

	// Calculate image center
	double imageCenterX = 0;
	double imageCenterY = 0;
	if ((this->GetSegParameters()->GetFrameSize()[0] <= 0) || (this->GetSegParameters()->GetFrameSize()[1] <= 0)) {
		int dimensions[3];
		dataCollector->GetVideoSource()->GetFrameSize(dimensions);
		imageCenterX = dimensions[0] / 2.0;
		imageCenterY = dimensions[1] / 2.0;
	} else {
		imageCenterX = this->GetSegParameters()->GetFrameSize()[0] / 2.0; 
		imageCenterY = this->GetSegParameters()->GetFrameSize()[1] / 2.0; 
	}

	// Set up camera
	vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
	imageCamera->SetFocalPoint(imageCenterX, imageCenterY, 0);
	imageCamera->SetViewUp(0, -1, 0);
	imageCamera->SetClippingRange(0.1, 2000.0);
	imageCamera->ParallelProjectionOn();

	// Calculate distance of camera from the plane
	int *size = controller->GetCanvasRenderer()->GetSize();
	if ((double)size[0] / (double)size[1] > imageCenterX / imageCenterY) {
		// If canvas aspect ratio is more elongenated in the X position then compute the distance according to the Y axis
		imageCamera->SetParallelScale(imageCenterY);
	} else {
		imageCamera->SetParallelScale(imageCenterY * sqrt((double)size[0] / (double)size[1]));
	}

	imageCamera->SetPosition(imageCenterX, imageCenterY, -200.0);

	// Set camera
	this->SetImageCamera(imageCamera);

	controller->GetCanvasRenderer()->SetActiveCamera(this->ImageCamera);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::Clear()
{
	LOG_TRACE("vtkFreehandCalibrationController::Clear");

	vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();

	// Remove image actor and reset background color
	renderer->RemoveActor(this->CanvasImageActor);
	renderer->RemoveActor(this->SegmentedPointsActor);
	renderer->SetBackground(0.6, 0.6, 0.6);
	renderer->InteractiveOn();

	// If device visualization is on, remove those actors too
	if (this->ShowDevices) {
		renderer->RemoveActor(this->PhantomBodyActor);
		renderer->RemoveActor(this->ProbeActor);
		renderer->RemoveActor(this->StylusActor);
		renderer->RemoveActor(this->NeedleActor);
	}

	m_Toolbox->Clear();  

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoSpatialCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoSpatialCalibration");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}

	// Make segmented point actor visible
	this->SegmentedPointsActor->VisibilityOn();

	const int maxNumberOfValidationImages = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToAcquire; 
	const int maxNumberOfCalibrationImages = this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToAcquire; 
	int numberOfAcquiredImages = 0;
	int numberOfFailedSegmentations = 0;

	this->CancelRequestOff();

	// Acquire and add validation and calibration data
	while ((this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages < maxNumberOfValidationImages)
		|| (this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages < maxNumberOfCalibrationImages)) {

		bool segmentationSuccessful = false;

		if (this->CancelRequest) {
			// Cancel the job
			return PLUS_FAIL;
		}

		// Get latest tracked frame from data collector
		TrackedFrame trackedFrame; 
		controller->GetDataCollector()->GetTrackedFrame(&trackedFrame); 

		if (trackedFrame.Status & (TR_MISSING | TR_OUT_OF_VIEW)) {
			LOG_DEBUG("Tracker out of view"); 
		} else if ( trackedFrame.Status & (TR_REQ_TIMEOUT)) {
			LOG_DEBUG("Tracker request timeout"); 
		} else { // TR_OK
			if (numberOfAcquiredImages < maxNumberOfValidationImages) {
				// Validation data
				if ( this->GetTrackedFrameList(FREEHAND_MOTION_2)->ValidateData(&trackedFrame) ) {
					if (AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_2)) {
						segmentationSuccessful = true;
					} else {
						++numberOfFailedSegmentations;
						LOG_DEBUG("Adding tracked frame " << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages << " (for validation) failed!");
					}
				}
			} else {
				// Calibration data
				if ( this->GetTrackedFrameList(FREEHAND_MOTION_1)->ValidateData(&trackedFrame) ) {
					if (AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_1)) {
						segmentationSuccessful = true;
					} else {
						++numberOfFailedSegmentations;
						LOG_DEBUG("Adding tracked frame " << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " (for calibration) failed!");
					}
				}
			}
		}

		// Update progress if tracked frame has been successfully added
		if (segmentationSuccessful) {
			++numberOfAcquiredImages;
			this->SetProgressPercent( (int)((numberOfAcquiredImages / (double)(maxNumberOfValidationImages + maxNumberOfCalibrationImages)) * 100.0) );
		}

		// Display segmented points (or hide them if unsuccessful)
		DisplaySegmentedPoints(segmentationSuccessful);

		// It makes the GUI update while acquiring the data
		if (m_Toolbox) {
			m_Toolbox->RefreshToolboxContent();
		}
	}

	LOG_INFO("Segmentation success rate: " << numberOfAcquiredImages << " out of " << numberOfAcquiredImages + numberOfFailedSegmentations << " (" << (int)(((double)numberOfAcquiredImages / (double)(numberOfAcquiredImages + numberOfFailedSegmentations)) * 100.0 + 0.49) << " percent)");
  
	// Make the segmented results disappear
	this->SegmentedPointsActor->VisibilityOff();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoAcquisition()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoAcquisition");

	// This function can only be called if show devices is on
	if (! this->ShowDevices) {
		LOG_WARNING("DoAcquisition function should only be called if show devices is on");
		return PLUS_FAIL;
	}

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is not initialized properly!");
		return PLUS_FAIL;
	}
	int firstActiveToolNumber = -1; 
	if (dataCollector->GetTracker()->GetFirstActiveTool(firstActiveToolNumber) != PLUS_SUCCESS)
	{
		LOG_ERROR("There are no active tracker tools!"); 
		return PLUS_FAIL; 
	}

	int referenceToolNumber = dataCollector->GetTracker()->GetReferenceToolNumber();
	bool resetCameraNeeded = false;

	for (int toolNumber=0; toolNumber<dataCollector->GetTracker()->GetNumberOfTools(); ++toolNumber) {
		// If reference then no need for setting transform (the phantom is fixed to the reference)
		if (toolNumber == referenceToolNumber) {
			continue;
		}

		vtkTrackerTool *tool = dataCollector->GetTracker()->GetTool(toolNumber);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			continue;
		}

		TrackerStatus status = TR_MISSING;
		double timestamp;

		vtkSmartPointer<vtkMatrix4x4> toolToReferenceTransformMatrix = NULL;

		// Acquire position from tracker
		toolToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		dataCollector->GetTransformWithTimestamp(toolToReferenceTransformMatrix, timestamp, status, toolNumber); 

		// Compute and set transforms for actors
		if (status == TR_OK) {
			// If other tool, set the transform according to the tool name
			if ((this->ProbeActor != NULL) && (STRCASECMP(tool->GetToolName(), "Probe") == 0)) {
				if (this->ProbeActor->GetProperty()->GetOpacity() < 1.0) {
					resetCameraNeeded = true;
				}
				this->ProbeActor->GetProperty()->SetOpacity(1.0);
				this->ProbeActor->SetUserMatrix(toolToReferenceTransformMatrix);

				// Image canvas transform
				vtkSmartPointer<vtkTransform> imageToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
				imageToPhantomReferenceTransform->Identity();
				imageToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
				imageToPhantomReferenceTransform->Concatenate(tool->GetCalibrationMatrix());
				imageToPhantomReferenceTransform->Concatenate(this->TransformImageToProbe);
				
				this->CanvasImageActor->SetUserTransform(imageToPhantomReferenceTransform);

			} else {

				// Create and apply actor transform
				vtkSmartPointer<vtkTransform> toolModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
				toolModelToPhantomReferenceTransform->Identity();
				toolModelToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
				toolModelToPhantomReferenceTransform->Concatenate(tool->GetCalibrationMatrix());
				toolModelToPhantomReferenceTransform->Concatenate(tool->GetModelToToolTransform());
				toolModelToPhantomReferenceTransform->Modified();

				if ((this->StylusActor != NULL) && (STRCASECMP(tool->GetToolName(), "Stylus") == 0)) {
					this->StylusActor->GetProperty()->SetOpacity(1.0);
					this->StylusActor->SetUserTransform(toolModelToPhantomReferenceTransform);

				} else if ((this->NeedleActor != NULL) && (STRCASECMP(tool->GetToolName(), "Needle") == 0)) {
					this->NeedleActor->GetProperty()->SetOpacity(1.0);
					this->NeedleActor->SetUserTransform(toolModelToPhantomReferenceTransform);
				}
			}
		} else {
			if ((this->ProbeActor != NULL) && (STRCASECMP(tool->GetToolName(), "Probe") == 0)) {
				this->ProbeActor->GetProperty()->SetOpacity(0.3);

			} else if ((this->StylusActor != NULL) && (STRCASECMP(tool->GetToolName(), "Stylus") == 0)) {
				this->StylusActor->GetProperty()->SetOpacity(0.3);

			} else if ((this->NeedleActor != NULL) && (STRCASECMP(tool->GetToolName(), "Needle") == 0)) {
				this->NeedleActor->GetProperty()->SetOpacity(0.3);
			}
		}
	} // for each tool

	if (resetCameraNeeded) {
		controller->GetCanvasRenderer()->ResetCamera();
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::Start()
{
	LOG_TRACE("vtkFreehandCalibrationController::Start");

	if (this->TemporalCalibrationDone) {
		// Initialize vtkCalibrationController
		if (this->GetSegParameters() == NULL) {
			LOG_ERROR( "Unable to start spatial calibration: calibration configuration is not loaded!" );  
			return PLUS_FAIL; 
		}

		// Initialize the segmentation component
		if (this->mptrAutomatedSegmentation == NULL) {
			this->mptrAutomatedSegmentation = new KPhantomSeg( 
				this->GetSegParameters()->GetFrameSize(), this->GetSegParameters()->GetRegionOfInterest(), this->GetEnableSegmentationAnalysis(), "frame.jpg");
		}	

		// Initialize the calibration component
		if (this->Calibrator == NULL) {
			this->Calibrator = new BrachyTRUSCalibrator( this->SegParameters, this->GetEnableSystemLog() );
		}

		// Set the ultrasound image frame in pixels
		// This defines the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row).
		this->SetUSImageFrameOriginInPixels( this->GetUSImageFrameOriginXInPixels(), this->GetUSImageFrameOriginYInPixels() ); 

		// Apply the US 3D beamwidth data to calibration if desired (pass the US 3D beamwidth data and their predefined weights to the calibration component)
		/*
		if( this->GetUS3DBeamwidthDataReady() )
		{
			this->GetCalibrator()->setUltrasoundBeamwidthAndWeightFactorsTable(
				this->GetIncorporatingUS3DBeamProfile(),
				*this->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM(),
				*this->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN(),
				*this->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() );
		}
		*/

		// Set identity for image frame shifting transform
		vtkSmartPointer<vtkMatrix4x4> identity = vtkSmartPointer<vtkMatrix4x4>::New();
		identity->Identity();
		vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
		ConvertVtkMatrixToVnlMatrix(identity, transformOrigImageFrame2TRUSImageFrameMatrix4x4); 
		this->GetCalibrator()->setTransformOrigImageToTRUSImageFrame4x4(transformOrigImageFrame2TRUSImageFrameMatrix4x4);

		// Register the phantom geometry to the DRB frame in the "Emulator" mode.
		vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
		this->ConvertVtkMatrixToVnlMatrixInMeter(PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform()->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode);

		this->Calibrator->registerPhantomGeometryInEmulatorMode(transformMatrixPhantom2DRB4x4InEmulatorMode);
	}

	m_State = ToolboxState_InProgress;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::Stop()
{
	LOG_TRACE("vtkFreehandCalibrationController::Stop");

	// Load models and initialize device actors (entering this function means that the calibration is successful)
	if (this->SpatialCalibrationDone) {
		InitializeDeviceVisualization();

		m_State = ToolboxState_Done;
	} else {
		m_State = ToolboxState_Idle;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::Reset()
{
	LOG_TRACE("vtkFreehandCalibrationController::Reset");

	// If spatial calibration is done
	if (this->SpatialCalibrationDone == true) {
		this->SpatialCalibrationDoneOff();

		// Empty tracked frame containers
		this->TrackedFrameListContainer[FREEHAND_MOTION_1]->Clear();
		this->TrackedFrameListContainer[FREEHAND_MOTION_2]->Clear();

    // Reset segmented image counters
    this->RealtimeImageDataInfoContainer[FREEHAND_MOTION_1].NumberOfSegmentedImages = 0;
    this->RealtimeImageDataInfoContainer[FREEHAND_MOTION_2].NumberOfSegmentedImages = 0;

  // If temporal calibration is done but spatial has not been started
	} else if ((this->TemporalCalibrationDone == true) && (m_State != ToolboxState_InProgress)) {
    this->TemporalCalibrationDoneOff();

	// If spatial calibration is in progress
	} else if ((this->TemporalCalibrationDone == true) && (m_State == ToolboxState_InProgress)) {
		// Turn on cancel flag
		this->CancelRequestOn();

		if (this->Calibrator != NULL) {
			delete this->Calibrator;
			this->Calibrator = NULL;
		}

		// Empty tracked frame containers
		this->TrackedFrameListContainer[FREEHAND_MOTION_1]->Clear();
		this->TrackedFrameListContainer[FREEHAND_MOTION_2]->Clear();
	}

	// If temporal calibration still in progress
	else if ((this->TemporalCalibrationDone == false) && (m_State == ToolboxState_InProgress)) {
		vtkFreehandController* controller = vtkFreehandController::GetInstance();
		if ((controller == NULL) || (controller->GetInitialized() == false)) {
			LOG_ERROR("vtkFreehandController is not initialized!");
			return PLUS_FAIL;
		}
		vtkDataCollector* dataCollector = controller->GetDataCollector();
		if (dataCollector == NULL) {
			LOG_ERROR("Data collector is not initialized!");
			return PLUS_FAIL;
		}

		// Cancel synchronization (temporal calibration) in data collector
		dataCollector->CancelSyncRequestOn();
  } else {
    LOG_ERROR("Reset freehand calibration was called in an unexpected state!");
  }

	// Reset progress
	this->SetProgressPercent(0);

	// Set state back to idle
	m_State = ToolboxState_Idle;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool vtkFreehandCalibrationController::IsReadyToStartSpatialCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::IsReadyToStartSpatialCalibration");

	if ((m_State == ToolboxState_Uninitialized)
		|| (! this->TemporalCalibrationDone)
		|| (PhantomRegistrationController::GetInstance() == NULL)
		|| (PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform() == NULL)
		|| (this->ConfigurationFileName == NULL))
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::UpdateProgress(int aPercent) {
	LOG_TRACE("vtkFreehandCalibrationController::UpdateProgress(" << aPercent << ")");

	// Calls this way because it is a static function
	vtkFreehandCalibrationController::GetInstance()->SetProgressPercent(aPercent);

	// It makes the GUI update while data collector is acquiring the data
	if (vtkFreehandCalibrationController::GetInstance()->GetToolbox()) {
		vtkFreehandCalibrationController::GetInstance()->GetToolbox()->RefreshToolboxContent();
	}
}

//-----------------------------------------------------------------------------

double vtkFreehandCalibrationController::GetVideoTimeOffset()
{
	LOG_TRACE("vtkFreehandCalibrationController::GetVideoTimeOffset");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

	if ((dataCollector->GetVideoSource() != NULL) && (dataCollector->GetVideoSource()->GetBuffer() != NULL)) {
		return dataCollector->GetVideoSource()->GetBuffer()->GetLocalTimeOffset();
	} else {
		return 0.0;
	}
}

//-----------------------------------------------------------------------------

std::string vtkFreehandCalibrationController::GetResultString()
{
	LOG_TRACE("vtkFreehandCalibrationController::GetResultString");

	std::ostringstream matrixStringStream;
	matrixStringStream << "Image to probe transform:" << std::endl;

	// Print matrix rows
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrixStringStream << std::fixed << std::setprecision(3) << std::setw(6) << std::right << this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j) << " ";
		}

		matrixStringStream << std::endl;
	}

	std::ostringstream errorsStringStream;

	errorsStringStream << "Point-line distance errors" << std::endl << "(mean, rms, std):" << std::endl;
	errorsStringStream << std::fixed << std::setprecision(3) << "  " << GetPointLineDistanceErrorAnalysisVector()[0] * 1000.0 << ", " << GetPointLineDistanceErrorAnalysisVector()[1] * 1000.0 << ", " << GetPointLineDistanceErrorAnalysisVector()[2] * 1000.0 << std::endl;

	std::ostringstream resultStringStream;
	resultStringStream << matrixStringStream.str() << errorsStringStream.str() << std::endl;

	resultStringStream << "Validation data confidence:" << std::endl;
	resultStringStream << "  " << GetPointLineDistanceErrorAnalysisVector()[3];

	return resultStringStream.str();
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoTemporalCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoTemporalCalibration");

	if (GetCalibrationMode() != REALTIME) {
		LOG_ERROR( "Unable to start temporal calibration in offline mode!" );  
		return PLUS_FAIL; 
	}

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

	dataCollector->SetProgressBarUpdateCallbackFunction(UpdateProgress);
	dataCollector->Synchronize(controller->GetOutputFolder(), true );

	this->TemporalCalibrationDoneOn();
	this->ProgressPercent = 0;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::AddTrackedFrameData(TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkProbeCalibrationController::AddTrackedFrameData");

	if (Superclass::AddTrackedFrameData(trackedFrame, dataType)) {
		double tProbeToPhantomReference[16];

		if (trackedFrame->GetDefaultFrameTransform(tProbeToPhantomReference)) {
			vtkSmartPointer<vtkMatrix4x4> tProbeToPhantomReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			tProbeToPhantomReferenceMatrix->DeepCopy(tProbeToPhantomReference);
			vnl_matrix<double> transformProbeToPhantomReferenceMatrix4x4(4,4);

			ConvertVtkMatrixToVnlMatrixInMeter(tProbeToPhantomReferenceMatrix, transformProbeToPhantomReferenceMatrix4x4);

			this->PopulateSegmentedFiducialsToDataContainer(transformProbeToPhantomReferenceMatrix4x4, dataType);

			/*
			if ((dataType == FREEHAND_MOTION_1) && (this->GetCalibrator()->areDataPositionsReadyForCalibration())) {
				this->Calibrate(); 
			}
			*/

			return PLUS_SUCCESS;
		}
	}

	return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
void vtkFreehandCalibrationController::Calibrate()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoCalibration"); 

	// Instruct the calibrator to perform the calibration task
	this->GetCalibrator()->calibrate();

	// Instruct the calibrator to validate the calibration accuracy
	this->GetCalibrator()->compute3DPointReconstructionError();
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkFreehandCalibrationController::PopulateSegmentedFiducialsToDataContainer");

	// ========================================================================
	// Populate the segmented N-fiducials to the data container
	// Indices defined in the input std::vector array.
	// This is the order that the segmentation algorithm gives the 
	// segmented positions in each image:
	//
	// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
	// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
	// Each acquired data position is a 4x1 homogenous vector :
	// [ X, Y, 0, 1] all units in pixels
	// ==================================================================

	SegmentationResults segResults;
	this->GetSegmenter()->GetSegmentationResults(segResults); 

	if (! segResults.GetDotsFound()) {
		LOG_ERROR("Segmentation failed! Unable to populate segmentation result!"); 
		return PLUS_FAIL; 
	}

	// Top layer:		3, 2, 1 
	// Bottom Layer:	6, 5, 4 
	std::vector<vnl_vector<double>> SegmentedNFiducialsInFixedCorrespondence;
	SegmentedNFiducialsInFixedCorrespondence.resize(0);

	for (int i=0; i<segResults.GetFoundDotsCoordinateValue().size(); i++) {
		vnl_vector<double> NFiducial(4,0);
		NFiducial[0]=segResults.GetFoundDotsCoordinateValue()[i][0];
		NFiducial[1]=segResults.GetFoundDotsCoordinateValue()[i][1];
		NFiducial[2]=0;
		NFiducial[3]=1;

		SegmentedNFiducialsInFixedCorrespondence.push_back(NFiducial);
	}

	// Add the data for calibration or validation
	if (dataType == FREEHAND_MOTION_1) {
		this->GetCalibrator()->addDataPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	} else if (dataType == FREEHAND_MOTION_2) {
		this->GetCalibrator()->addValidationPositionsPerImage( SegmentedNFiducialsInFixedCorrespondence, transformUSProbe2StepperFrameMatrix4x4 );
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DoOfflineCalibration()
{
	LOG_TRACE("vtkFreehandCalibrationController::DoOfflineCalibration"); 

	try {
		if (m_State == ToolboxState_Uninitialized) {
			Initialize();
		}

		vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
		if ( !this->GetSavedImageDataInfo(FREEHAND_MOTION_2).SequenceMetaFileName.empty() ) {
			trackedFrameList->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(FREEHAND_MOTION_2).SequenceMetaFileName.c_str()); 
		} else {
			LOG_ERROR("Unable to start OfflineCalibration with validation data: SequenceMetaFileName is empty!"); 
			return PLUS_FAIL; 
		}

		// Validation data
		int validationCounter = 0;
		for (int imgNumber = this->GetSavedImageDataInfo(FREEHAND_MOTION_2).StartingIndex; validationCounter < this->GetSavedImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToUse; imgNumber++) {
			if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() ) {
				break; 
			}

			if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), FREEHAND_MOTION_2) ) {
				// The segmentation was successful
				validationCounter++;
			} else {
				LOG_DEBUG("Adding tracked frame " << imgNumber << " (for validation) failed!");
			}

			this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData);
		}

		LOG_INFO( "A total of " << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages << " images have been successfully added for validation.");


		// Calibration data
		vtkSmartPointer<vtkTrackedFrameList> calibrationData = vtkSmartPointer<vtkTrackedFrameList>::New();
		if ( !this->GetSavedImageDataInfo(FREEHAND_MOTION_1).SequenceMetaFileName.empty() ) {
			calibrationData->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(FREEHAND_MOTION_1).SequenceMetaFileName.c_str()); 
		} else {
			LOG_ERROR("Unable to start OfflineCalibration with calibration data: SequenceMetaFileName is empty!"); 
			return PLUS_FAIL; 
		}

		int calibrationCounter = 0;
		for (int imgNumber = this->GetSavedImageDataInfo(FREEHAND_MOTION_1).StartingIndex; calibrationCounter < this->GetSavedImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToUse; imgNumber++) {
			if ( imgNumber >= calibrationData->GetNumberOfTrackedFrames() ) {
				break; 
			}

			if ( this->AddTrackedFrameData(calibrationData->GetTrackedFrame(imgNumber), FREEHAND_MOTION_1) ) {
				// The segmentation was successful
				calibrationCounter++; 
			} else {
				LOG_DEBUG("Adding tracked frame " << imgNumber << " (for calibration) failed!");
			}

			this->AddFrameToRenderer(calibrationData->GetTrackedFrame(imgNumber)->ImageData); 
		}

		LOG_INFO ("A total of " << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " images have been successfully added for calibration.");
	} catch(...) {
		LOG_ERROR("AddAllSavedData: Failed to add saved data!");  
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::DisplaySegmentedPoints(bool aSuccess)
{
	LOG_TRACE("vtkFreehandCalibrationController::DisplaySegmentedPoints(" << (aSuccess?"true":"false") << ")");

	if (! aSuccess) {
		this->SegmentedPointsActor->VisibilityOff();
		//this->SegmentedPointsActor->GetProperty()->SetColor(0.5, 0.5, 0.5);

		return PLUS_SUCCESS;
	}

	// Get last results and feed the points into vtkPolyData for displaying
	SegmentedFrame lastSegmentedFrame = this->SegmentedFrameContainer.at(this->SegmentedFrameContainer.size() - 1);
	SegmentationResults results = lastSegmentedFrame.SegResults;
	int height = lastSegmentedFrame.TrackedFrameInfo->FrameSize[1];

	vtkSmartPointer<vtkPoints> inputPoints = vtkSmartPointer<vtkPoints>::New();
	inputPoints->SetNumberOfPoints(results.GetFoundDotsCoordinateValue().size());

	std::vector<std::vector<double>> dots = results.GetFoundDotsCoordinateValue();
	for (int i=0; i<dots.size(); ++i) {
		inputPoints->InsertPoint(i, dots[i][0], height - dots[i][1], 0.0);
	}
	inputPoints->Modified();

	this->SegmentedPointsPolyData->Initialize();
	this->SegmentedPointsPolyData->SetPoints(inputPoints);

	this->SegmentedPointsActor->VisibilityOn();
	//this->SegmentedPointsActor->GetProperty()->SetColor(0.0, 0.8, 0.0);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int* origin) {
	LOG_TRACE("vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels"); 

	this->SetUSImageFrameOriginInPixels(origin[0], origin[1]); 
}

//-----------------------------------------------------------------------------

vtkCalibrationController::RealtimeImageDataInfo vtkFreehandCalibrationController::GetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkFreehandCalibrationController::GetRealtimeImageDataInfo"); 

	return this->RealtimeImageDataInfoContainer[dataType];
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType, RealtimeImageDataInfo realtimeImageDataInfo)
{
	this->RealtimeImageDataInfoContainer[dataType] = realtimeImageDataInfo;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int originX, int originY)
{
	LOG_TRACE("vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels: " << originX << "  " << originY); //TODO Copy these traces to the other functions as well

	this->SetUSImageFrameOriginXInPixels(originX);
	this->SetUSImageFrameOriginYInPixels(originY);

	if (this->GetCalibrator() != NULL) {
		this->GetCalibrator()->setUltrasoundImageFrameOriginInPixels(originX, originY); 
	}
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ReadConfiguration(const char* configFileNameWithPath)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration - " << configFileNameWithPath);

	this->SetConfigurationFileName(configFileNameWithPath);
	
	vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName());
	if (rootElement == NULL) {
		LOG_ERROR("Failed to read configuration from file: " << this->GetConfigurationFileName());
		return PLUS_FAIL;
	}

	return ReadConfiguration(rootElement);
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ReadConfiguration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration");

	if (aConfig == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}

	// Find and load calibration configuration
	vtkSmartPointer<vtkXMLDataElement> usCalibration = aConfig->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
		LOG_ERROR("No calibration configuration is found in the XML tree!");
		return PLUS_FAIL;
	}
	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	if (calibrationController == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}
	this->ReadCalibrationControllerConfiguration(calibrationController);
	vtkFreehandController::GetInstance()->SetOutputFolder(this->GetOutputPath());

	// Freehand Calibration specifications (from ProbeCalibration section of the config file)
	vtkSmartPointer<vtkXMLDataElement> freehandCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration");
	if (freehandCalibration == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}
	this->ReadFreehandCalibrationConfiguration(freehandCalibration);

	// Find and load phantom definition
	this->ReadPhantomDefinition(aConfig);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ReadFreehandCalibrationConfiguration(vtkXMLDataElement* probeCalibration)
{
	LOG_TRACE("vtkFreehandCalibrationController::ReadFreehandCalibrationConfiguration"); 

	if (probeCalibration == NULL) {	
		LOG_WARNING("Unable to read ProbeCalibration XML data element!"); 
		return PLUS_FAIL; 
	} 

	// To enable/disable the system logging
	const char* enableLogFile = probeCalibration->GetAttribute("EnableLogFile"); 
	if (enableLogFile != NULL &&  STRCASECMP( "TRUE", enableLogFile ) == 0) {
		this->EnableSystemLogOn(); 
	} else {
		this->EnableSystemLogOff(); 
	}

	/*
	// To enable/disable the saving of segmented wire positions to file 
	const char* enableSegmentedWirePositionsSaving = probeCalibration->GetAttribute("EnableSegmentedWirePositionsSaving"); 
	if (enableSegmentedWirePositionsSaving != NULL &&  STRCASECMP( "TRUE", enableSegmentedWirePositionsSaving ) == 0) {
		this->EnableSegmentedWirePositionsSavingOn(); 
	} else {
		this->EnableSegmentedWirePositionsSavingOff(); 
	}

	//Transform: from image home position to user defined image home position
	double* imageHomeToUserImageHomeTransform = new double[16]; 
	if ( probeCalibration->GetVectorAttribute("ImageHomeToUserImageHomeTransform", 16, imageHomeToUserImageHomeTransform) )
	{
		this->GetTransformImageHomeToUserImageHome()->SetMatrix(imageHomeToUserImageHomeTransform); 
		this->GetTransformUserImageToImage()->SetMatrix(imageHomeToUserImageHomeTransform); 
		this->GetTransformUserImageToImage()->Inverse(); 
	}
	delete [] imageHomeToUserImageHomeTransform; 

	// Sets the suffix of the data files
	const char* dataFileSuffix = probeCalibration->GetAttribute("DataFileSuffix"); 
	if (dataFileSuffix != NULL) {
		this->SetDataFileSuffix(dataFileSuffix); 
	} else {
		this->SetDataFileSuffix(".data"); 
	}
	*/

	// Sets the suffix of the calibration result file
	const char* calibrationResultFileSuffix = probeCalibration->GetAttribute("CalibrationResultFileSuffix"); 
	if ( calibrationResultFileSuffix != NULL) {
		this->SetCalibrationResultFileSuffix(calibrationResultFileSuffix); 
	} else {
		this->SetCalibrationResultFileSuffix(".Calibration.results"); 
	}

	/*
	// Sets the suffix of the segmentation error log file
	const char* segmentationErrorLogFileNameSuffix = probeCalibration->GetAttribute("SegmentationErrorLogFileNameSuffix"); 
	if ( segmentationErrorLogFileNameSuffix != NULL) {
		this->SetSegmentationErrorLogFileNameSuffix(segmentationErrorLogFileNameSuffix); 
	} else {
		this->SetSegmentationErrorLogFileNameSuffix(".Segmentation.errors"); 
	}

	// Sets the suffix of the segmentation analysis file
	const char* segmentationAnalysisFileNameSuffix = probeCalibration->GetAttribute("SegmentationAnalysisFileNameSuffix"); 
	if ( segmentationAnalysisFileNameSuffix != NULL) {
		this->SetSegmentationAnalysisFileNameSuffix(segmentationAnalysisFileNameSuffix); 
	} else {
		this->SetSegmentationAnalysisFileNameSuffix(".Segmentation.analysis"); 
	}

	// Sets the suffix of the Template2StepperCalibration analysis file
	const char* temp2StepCalibAnalysisFileNameSuffix = probeCalibration->GetAttribute("Temp2StepCalibAnalysisFileNameSuffix"); 
	if ( temp2StepCalibAnalysisFileNameSuffix != NULL) {
		this->SetTemp2StepCalibAnalysisFileNameSuffix(temp2StepCalibAnalysisFileNameSuffix); 
	} else {
		this->SetTemp2StepCalibAnalysisFileNameSuffix(".Template2StepperCalibration.analysis"); 
	}
	*/

	// RandomStepperMotionData2 data set specifications
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData_2 = probeCalibration->FindNestedElementWithName("RandomStepperMotionData2"); 
	if (randomStepperMotionData_2 != NULL) {
		SavedImageDataInfo imageDataInfo; 
		int numberOfImagesToUse = -1;
		if (randomStepperMotionData_2->GetScalarAttribute("NumberOfImagesToUse", numberOfImagesToUse)) {
			imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse;
		}

		int startingIndex = 0;
		if (randomStepperMotionData_2->GetScalarAttribute("StartingIndex", startingIndex)) {
			imageDataInfo.StartingIndex = startingIndex; 
		}

		// Path to validation input sequence metafile
		const char* sequenceMetaFile = randomStepperMotionData_2->GetAttribute("SequenceMetaFile"); 
		if (sequenceMetaFile != NULL) {
			//imageDataInfo.SequenceMetaFileName.assign(sequenceMetaFile); TODO do we need this tag at all? sequence metafile path comes from data collection config
		}

		this->SetSavedImageDataInfo(FREEHAND_MOTION_2, imageDataInfo); 
	} else {
		LOG_WARNING("Unable to find RandomStepperMotionData2 XML data element"); 
	}

	// RandomStepperMotionData_1 data set specifications
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData_1 = probeCalibration->FindNestedElementWithName("RandomStepperMotionData1"); 
	if (randomStepperMotionData_1 != NULL) {
		SavedImageDataInfo imageDataInfo; 
		int numberOfImagesToUse = -1;
		if (randomStepperMotionData_1->GetScalarAttribute("NumberOfImagesToUse", numberOfImagesToUse)) {
			imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse; 
		}

		int startingIndex = 0;
		if (randomStepperMotionData_1->GetScalarAttribute("StartingIndex", startingIndex)) {
			imageDataInfo.StartingIndex = startingIndex; 
		}

		// Path to calibration input sequence metafile
		const char* sequenceMetaFile = randomStepperMotionData_1->GetAttribute("SequenceMetaFile"); 
		if (sequenceMetaFile != NULL) {
			//imageDataInfo.SequenceMetaFileName.assign(sequenceMetaFile); 
		}

		this->SetSavedImageDataInfo(FREEHAND_MOTION_1, imageDataInfo); 
	} else {
		LOG_WARNING("Unable to find RandomStepperMotionData1 XML data element"); 
	}

	/* TODO
	// US3DBeamwidth specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> us3DBeamProfile = probeCalibration->FindNestedElementWithName("US3DBeamProfile"); 
	if ( us3DBeamProfile != NULL) 
	{
		// To incorporate the ultrasound beam profile (3D beam width)
		int incorporatingUS3DBeamProfile = 0;
		if ( us3DBeamProfile->GetScalarAttribute("IncorporatingUS3DBeamProfile", incorporatingUS3DBeamProfile) ) 
		{
			this->SetIncorporatingUS3DBeamProfile(incorporatingUS3DBeamProfile); 
		}

		// The US-3D-beam-profile data file name and path (if choose to incorporate the beam profile)
		const char* configFile = us3DBeamProfile->GetAttribute("ConfigFile"); 
		if ( configFile != NULL) 
		{
			this->SetUS3DBeamProfileDataFileNameAndPath(configFile); 
		}

		if( this->GetIncorporatingUS3DBeamProfile() > 0 && this->GetIncorporatingUS3DBeamProfile() < 4 )
		{
			this->LoadUS3DBeamProfileData();
		}
	}
	else
	{
		LOG_WARNING("Unable to find US3DBeamProfile XML data element"); 
	}
	*/
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::ComputeCalibrationResults()
{
	LOG_TRACE("vtkFreehandCalibrationController::ComputeCalibrationResults"); 

	try {
		if (m_State == ToolboxState_Uninitialized) {
			Initialize();
		}

		// Do final calibration 
		Calibrate();

		// Get the image home to probe home transformation from the calibrator
		vnl_matrix<double> calibResultMatrix = this->GetCalibrator()->getCalibrationResultsInMatrix(); 
		vtkSmartPointer<vtkMatrix4x4> imageToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

		// Convert transform from meter to mm
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 4; j++ ) {
				imageToProbeMatrix->SetElement(i, j, calibResultMatrix.get(i, j) * 1000 ); 
			}
		}
		
		// Complete the transformation matrix from a projection matrix to a 3D-3D transformation matrix (so that it can be inverted or can be used to transform 3D widgets to the image plane)
		double xVector[3] = {imageToProbeMatrix->GetElement(0,0),imageToProbeMatrix->GetElement(1,0),imageToProbeMatrix->GetElement(2,0)};
		double yVector[3] = {imageToProbeMatrix->GetElement(0,1),imageToProbeMatrix->GetElement(1,1),imageToProbeMatrix->GetElement(2,1)};
		double zVector[3] = {0,0,0};

		vtkMath::Cross(xVector, yVector, zVector);

		// make the z vector have about the same length as x an y,
		// so that when a 3D widget is transformed using this transform, the aspect ratio is maintained
		vtkMath::Normalize(zVector);
		double normZ = (vtkMath::Norm(xVector) + vtkMath::Norm(yVector)) / 2;
		vtkMath::MultiplyScalar(zVector, normZ);
		
		imageToProbeMatrix->SetElement(0, 2, zVector[0]);
		imageToProbeMatrix->SetElement(1, 2, zVector[1]);
		imageToProbeMatrix->SetElement(2, 2, zVector[2]);

		this->GetTransformImageToProbe()->SetMatrix(imageToProbeMatrix);

		// Write transformations to log and output
		std::ostringstream osImageToProbe;
		this->GetTransformImageToProbe()->Print(osImageToProbe);
		LOG_DEBUG("TransformImageToProbe:\n" << osImageToProbe.str().c_str() );

		// Compute the independent point and line reconstruction errors
		LOG_INFO("Compute the independent point and line reconstruction errors");
		this->GetCalibrator()->computeIndependentPointLineReconstructionError();

		// STEP-4. Print the final calibration results and error reports 
		LOG_INFO("Print calibration results and error reports");
		this->PrintCalibrationResultsAndErrorReports();

		// STEP-5. Save the calibration results and error reports into a file 
		LOG_INFO("Save the calibration results and error reports to file");
		this->SaveCalibrationResultsAndErrorReportsToXML();

		/* TODO
		// STEP-6. Save the segmented wire positions into a file 
		if ( this->EnableSegmentedWirePositionsSaving ) {
			LOG_INFO("Save the segmented wire positions to file"); 
			this->CalibrationControllerIO->SaveSegmentedWirePositionsToFile(); 
		}

		if ( this->GetEnableVisualization() ) {
			LOG_INFO("Save PRE3D plot to image"); 
			// save the PRE3D distribution plot to an image file
			this->GetVisualizationComponent()->SavePRE3DplotToImage();
		}
		*/

		this->SpatialCalibrationDoneOn();

	} catch(...) {
		LOG_ERROR("ComputeCalibrationResults: Failed to compute calibration results!");
		return PLUS_FAIL;
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::SaveCalibrationResults()
{
	LOG_TRACE("vtkFreehandCalibrationController::SaveCalibrationResults");

	// Find stylus definition element
	vtkSmartPointer<vtkXMLDataElement> probeDefinition = vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(NULL, "Tracker", "Tool", "Type", "Probe");
	if (probeDefinition == NULL) {
		LOG_ERROR("No probe definition is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = probeDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in probe definition!");
		return PLUS_FAIL;
	}

	// Assemble and save transform
	char imageToProbeTransformChars[256];
  vtkSmartPointer<vtkMatrix4x4> transformMatrix = this->TransformImageToProbe->GetMatrix();
	sprintf_s(imageToProbeTransformChars, 256, "\n\t%.4lf %.4lf %.4lf %.4lf\n\t%.4lf %.4lf %.4lf %.4lf\n\t%.4lf %.4lf %.4lf %.1lf\n\t0 0 0 1", 
		transformMatrix->GetElement(0,0), transformMatrix->GetElement(0,1), transformMatrix->GetElement(0,2), transformMatrix->GetElement(0,3), 
		transformMatrix->GetElement(1,0), transformMatrix->GetElement(1,1), transformMatrix->GetElement(1,2), transformMatrix->GetElement(1,3), 
		transformMatrix->GetElement(2,0), transformMatrix->GetElement(2,1), transformMatrix->GetElement(2,2), transformMatrix->GetElement(2,3));

	calibration->SetAttribute("MatrixValue", imageToProbeTransformChars);

	// Save matrix name, date and error
	calibration->SetAttribute("MatrixName", "ImageToProbe");
	calibration->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());
	calibration->SetDoubleAttribute("Error", GetPointLineDistanceErrorAnalysisVector()[0] * 1000.0); // TODO find the best error number

  vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

  dataCollector->GetConfigurationData()->PrintXML(vtkFreehandController::GetInstance()->GetConfigurationFileName());

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::PrintCalibrationResultsAndErrorReports()
{
  LOG_TRACE("vtkProbeCalibrationController::PrintCalibrationResultsAndErrorReports");

	try {
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Calibration result in 4x4 homogeneous transform matrix = ");

		for (int i = 0; i < 4; i++) {
			std::ostringstream matrixRow; 

			for (int j = 0; j < 4; j++) {
				matrixRow << this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j) << "  " ;
			}
			LOG_INFO(matrixRow.str()); 
		}

		// Point-Line Distance Error Analysis for Validation Positions in US probe frame
		LOG_INFO("---------------------------------------------------------------");
		LOG_INFO("Point-Line Distance Error (PLDE) Analysis in mm =");
		LOG_INFO("[ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]");
		LOG_INFO("[ vector 3    :  Validation data confidence level ]");
		LOG_INFO(GetPointLineDistanceErrorAnalysisVector()[0] * 1000 << ", " << GetPointLineDistanceErrorAnalysisVector()[1] * 1000 << ", " << GetPointLineDistanceErrorAnalysisVector()[2] * 1000); 
		LOG_INFO(GetPointLineDistanceErrorAnalysisVector()[3]);

	} catch(...) {
		LOG_ERROR("PrintCalibrationResultsAndErrorReports: Failed to retrieve the calibration results!"); 
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandCalibrationController::SaveCalibrationResultsAndErrorReportsToXML()
{
	LOG_TRACE("vtkFreehandCalibrationController::SaveCalibrationResultsAndErrorReportsToXML"); 

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return PLUS_FAIL;
	}

	// Construct the calibration result file name with path and timestamp
	const std::string calibrationResultFileName = this->GetCalibrator()->getCalibrationTimeStampInString() + this->GetCalibrationResultFileSuffix() + ".xml";
	const std::string calibrationResultFileNameWithPath = controller->GetOutputFolder() + std::string("/") + calibrationResultFileName;
	this->SetCalibrationResultFileNameWithPath(calibrationResultFileNameWithPath.c_str()); 

	// <USTemplateCalibrationResult>
	vtkSmartPointer<vtkXMLDataElement> xmlCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	xmlCalibrationResults->SetName("USTemplateCalibrationResult"); 
	xmlCalibrationResults->SetAttribute("version", "1.0"); 

	// <CalibrationFile> 
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationFile = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationFile->SetName("CalibrationFile"); 
	tagCalibrationFile->SetAttribute("Timestamp", this->GetCalibrator()->getCalibrationTimeStampWithFormat().c_str()); 
	tagCalibrationFile->SetAttribute("FileName", calibrationResultFileName.c_str()); 
	vtkstd::string commentCalibrationFile("# Timestamp format: MM/DD/YY HH:MM:SS"); 
	tagCalibrationFile->AddCharacterData(commentCalibrationFile.c_str(), commentCalibrationFile.size()); 
	// </CalibrationFile> 

	// <CalibrationResults>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationResults->SetName("CalibrationResults"); 

	// <UltrasoundImageDimensions>
	vtkSmartPointer<vtkXMLDataElement> tagUltrasoundImageDimensions = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagUltrasoundImageDimensions->SetName("UltrasoundImageDimensions"); 
	tagUltrasoundImageDimensions->SetIntAttribute("Width", this->GetSegParameters()->GetFrameSize()[0]); 
	tagUltrasoundImageDimensions->SetIntAttribute("Height", this->GetSegParameters()->GetFrameSize()[1]); 
	vtkstd::string commentUltrasoundImageDimensions("# UltrasoundImageDimensions format: image width and height in pixels."); 
	tagUltrasoundImageDimensions->AddCharacterData(commentUltrasoundImageDimensions.c_str(), commentUltrasoundImageDimensions.size()); 
	// </UltrasoundImageDimensions>

	// <UltrasoundImageOrigin>
	// # FORMAT: X to the right; Y to the bottom; w.r.t the left-upper corner in original image.
	vtkSmartPointer<vtkXMLDataElement> tagUltrasoundImageOrigin = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagUltrasoundImageOrigin->SetName("UltrasoundImageOrigin"); 
	tagUltrasoundImageOrigin->SetIntAttribute("OriginX", this->GetUSImageFrameOriginXInPixels()); 
	tagUltrasoundImageOrigin->SetIntAttribute("OriginY", this->GetUSImageFrameOriginYInPixels()); 
	vtkstd::string commentUltrasoundImageOrigin("# UltrasoundImageOrigin format: X to the right; Y to the bottom; w.r.t the left-upper corner in original image."); 
	tagUltrasoundImageOrigin->AddCharacterData(commentUltrasoundImageOrigin.c_str(), commentUltrasoundImageOrigin.size()); 
	// </UltrasoundImageOrigin>


	double *imageToProbeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			imageToProbeMatrix[i*4+j] = this->GetTransformImageToProbe()->GetMatrix()->GetElement(i,j); 
		}
	}

	// <CalibrationTransform>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationTransform = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationTransform->SetName("CalibrationTransform"); 
	tagCalibrationTransform->SetVectorAttribute("TransformImageToProbe", 16, imageToProbeMatrix); 
	// </CalibrationTransform>

	delete[] imageToProbeMatrix; 

	tagCalibrationResults->AddNestedElement(tagUltrasoundImageDimensions); 
	tagCalibrationResults->AddNestedElement(tagUltrasoundImageOrigin); 
	tagCalibrationResults->AddNestedElement(tagCalibrationTransform); 
	// </CalibrationResults>

	// <ErrorReports>
	vtkSmartPointer<vtkXMLDataElement> tagErrorReports = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagErrorReports->SetName("ErrorReports"); 


	double *preAnalysis = new double[9];
	for (int i = 0; i < 9; i++) {
		preAnalysis[i] = this->GetPRE3DVector()[i]; 
	}

	// <PointReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrorAnalysis->SetName("PointReconstructionErrorAnalysis"); 
	tagPointReconstructionErrorAnalysis->SetVectorAttribute("PRE", 9, preAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetPRE3DVector()[9]);  
	vtkstd::string commentPointReconstructionErrorAnalysis("# PRE format: PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std"); 
	tagPointReconstructionErrorAnalysis->AddCharacterData(commentPointReconstructionErrorAnalysis.c_str(), commentPointReconstructionErrorAnalysis.size()); 
	// </PointReconstructionErrorAnalysis>
	delete[] preAnalysis; 


	double *rawPointReconstructionErrors = new double[this->GetPRE3DMatrix().size()]; 
	for (int row = 0; row < this->GetPRE3DMatrix().rows(); row++) {
		for (int column = 0; column < this->GetPRE3DMatrix().cols(); column++) {
			rawPointReconstructionErrors[row*this->GetPRE3DMatrix().cols() + column ] = this->GetPRE3DMatrix().get(row, column); 
		}
	}

	// <PointReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrors->SetName("PointReconstructionErrors"); 
	tagPointReconstructionErrors->SetVectorAttribute("Raw", this->GetPRE3DMatrix().size(), rawPointReconstructionErrors); 
	vtkstd::ostringstream commentPointReconstructionErrors; 
	commentPointReconstructionErrors << "# PointReconstructionErrors format: 4xN matrix, N = "; 
	commentPointReconstructionErrors << this->GetPRE3DMatrix().columns(); 
	commentPointReconstructionErrors << ": the number of validation points"; 
	tagPointReconstructionErrors->AddCharacterData(commentPointReconstructionErrors.str().c_str(), commentPointReconstructionErrors.str().size()); 
	// </PointReconstructionErrors>
	delete[] rawPointReconstructionErrors; 


	double *pldeAnalysis = new double[3]; 
	for (int i = 0; i < 3; i++) {
		pldeAnalysis[i] = this->GetPointLineDistanceErrorAnalysisVector()[i]; 
	}

	// <PointLineDistanceErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrorAnalysis->SetName("PointLineDistanceErrorAnalysis"); 
	tagPointLineDistanceErrorAnalysis->SetVectorAttribute("PLDE", 3, pldeAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointLineDistanceErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetPointLineDistanceErrorAnalysisVector()[3]);  
	vtkstd::string commentPointLineDistanceErrorAnalysis("# PLDE format: PLDE_mean, PLDE_rms, PLDE_std"); 
	tagPointLineDistanceErrorAnalysis->AddCharacterData(commentPointLineDistanceErrorAnalysis.c_str(), commentPointLineDistanceErrorAnalysis.size()); 
	// </PointLineDistanceErrorAnalysis>
	delete[] pldeAnalysis; 


	double *rawPointLineDistanceErrors = new double[this->GetPointLineDistanceErrorVector().size()]; 
	for (int i = 0; i < this->GetPointLineDistanceErrorVector().size(); i++) {
		rawPointLineDistanceErrors[i] = this->GetPointLineDistanceErrorVector().get(i); 
	}

	double *sortedPointLineDistanceErrors = new double[this->GetPointLineDistanceErrorSortedVector().size()]; 
	for (int i = 0; i < this->GetPointLineDistanceErrorSortedVector().size(); i++) {
		sortedPointLineDistanceErrors[i] = this->GetPointLineDistanceErrorSortedVector().get(i); 
	}

	// <PointLineDistanceErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrors->SetName("PointLineDistanceErrors"); 
	tagPointLineDistanceErrors->SetVectorAttribute("Raw", this->GetPointLineDistanceErrorVector().size(), rawPointLineDistanceErrors); 
	//tagPointLineDistanceErrors->SetVectorAttribute("Sorted", this->GetPointLineDistanceErrorSortedVector().size(), sortedPointLineDistanceErrors); 
	vtkstd::ostringstream commentPointLineDistanceErrors; 
	commentPointLineDistanceErrors << "# PointLineDistanceErrors format: 1xN vector, N = "; 
	commentPointLineDistanceErrors << this->GetPointLineDistanceErrorVector().size(); 
	commentPointLineDistanceErrors << ": the number of validation points"; 
	tagPointLineDistanceErrors->AddCharacterData(commentPointLineDistanceErrors.str().c_str(), commentPointLineDistanceErrors.str().size()); 
	// </PointLineDistanceErrors>
	delete[] rawPointLineDistanceErrors; 
	delete[] sortedPointLineDistanceErrors; 


	double *w1LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w1LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(1)[i]; 
	}

	// <Wire1LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire1LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire1LineReconstructionErrorAnalysis->SetName("Wire1LineReconstructionErrorAnalysis"); 
	tagWire1LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w1LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire1LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(1)[6]);  
	vtkstd::string commentWire1LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire1LineReconstructionErrorAnalysis->AddCharacterData(commentWire1LineReconstructionErrorAnalysis.c_str(), commentWire1LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w1LREAnalysis; 


	const int w1NumOfRows = this->GetLineReconstructionErrorMatrix(1).rows(); 
	const int w1NumOfCols = this->GetLineReconstructionErrorMatrix(1).cols(); 
	double *w1rawXLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawYLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawEUCLineReconstructionErrors = new double[w1NumOfCols]; 

	for (int column = 0; column < w1NumOfCols; column++) {
		w1rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(1).get(0, column);
		w1rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(1).get(1, column);
		w1rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(1).get(3, column);
	}

	// <Wire1LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW1LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW1LineReconstructionErrors->SetName("Wire1LineReconstructionErrors"); 
	tagW1LineReconstructionErrors->SetVectorAttribute("X", w1NumOfCols, w1rawXLineReconstructionErrors); 
	tagW1LineReconstructionErrors->SetVectorAttribute("Y", w1NumOfCols, w1rawYLineReconstructionErrors); 
	tagW1LineReconstructionErrors->SetVectorAttribute("EUC", w1NumOfCols, w1rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW1LineReconstructionErrors; 
	commentW1LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW1LineReconstructionErrors << w1NumOfCols; 
	commentW1LineReconstructionErrors << ": the number of validation points"; 
	tagW1LineReconstructionErrors->AddCharacterData(commentW1LineReconstructionErrors.str().c_str(), commentW1LineReconstructionErrors.str().size()); 
	// </Wire1LineReconstructionErrors>
	delete[] w1rawXLineReconstructionErrors; 
	delete[] w1rawYLineReconstructionErrors; 
	delete[] w1rawEUCLineReconstructionErrors;


	double *w3LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w3LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(3)[i]; 
	}

	// <Wire3LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire3LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire3LineReconstructionErrorAnalysis->SetName("Wire3LineReconstructionErrorAnalysis"); 
	tagWire3LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w3LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire3LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(3)[6]);  
	vtkstd::string commentWire3LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire3LineReconstructionErrorAnalysis->AddCharacterData(commentWire3LineReconstructionErrorAnalysis.c_str(), commentWire3LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w3LREAnalysis;


	const int w3NumOfRows = this->GetLineReconstructionErrorMatrix(3).rows(); 
	const int w3NumOfCols = this->GetLineReconstructionErrorMatrix(3).cols(); 
	double *w3rawXLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawYLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawEUCLineReconstructionErrors = new double[w3NumOfCols];

	for (int column = 0; column < w3NumOfCols; column++) {
		w3rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(3).get(0, column); 
		w3rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(3).get(1, column); 
		w3rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(3).get(3, column); 
	}

	// <Wire3LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW3LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW3LineReconstructionErrors->SetName("Wire3LineReconstructionErrors"); 
	tagW3LineReconstructionErrors->SetVectorAttribute("X", w3NumOfCols, w3rawXLineReconstructionErrors); 
	tagW3LineReconstructionErrors->SetVectorAttribute("Y", w3NumOfCols, w3rawYLineReconstructionErrors); 
	tagW3LineReconstructionErrors->SetVectorAttribute("EUC", w3NumOfCols, w3rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW3LineReconstructionErrors; 
	commentW3LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW3LineReconstructionErrors << w3NumOfCols; 
	commentW3LineReconstructionErrors << ": the number of validation points"; 
	tagW3LineReconstructionErrors->AddCharacterData(commentW3LineReconstructionErrors.str().c_str(), commentW3LineReconstructionErrors.str().size()); 
	// </Wire3LineReconstructionErrors>
	delete[] w3rawXLineReconstructionErrors; 
	delete[] w3rawYLineReconstructionErrors; 
	delete[] w3rawEUCLineReconstructionErrors;


	double *w4LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w4LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(4)[i]; 
	}

	// <Wire4LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire4LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire4LineReconstructionErrorAnalysis->SetName("Wire4LineReconstructionErrorAnalysis"); 
	tagWire4LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w4LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire4LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(4)[6]);  
	vtkstd::string commentWire4LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire4LineReconstructionErrorAnalysis->AddCharacterData(commentWire4LineReconstructionErrorAnalysis.c_str(), commentWire4LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w4LREAnalysis;


	const int w4NumOfRows = this->GetLineReconstructionErrorMatrix(4).rows(); 
	const int w4NumOfCols = this->GetLineReconstructionErrorMatrix(4).cols(); 
	double *w4rawXLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawYLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawEUCLineReconstructionErrors = new double[w4NumOfCols];

	for (int column = 0; column < w4NumOfCols; column++) {
		w4rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(4).get(0, column); 
		w4rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(4).get(1, column); 
		w4rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(4).get(3, column); 
	}

	// <Wire4LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW4LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW4LineReconstructionErrors->SetName("Wire4LineReconstructionErrors"); 
	tagW4LineReconstructionErrors->SetVectorAttribute("X", w4NumOfCols, w4rawXLineReconstructionErrors); 
	tagW4LineReconstructionErrors->SetVectorAttribute("Y", w4NumOfCols, w4rawYLineReconstructionErrors); 
	tagW4LineReconstructionErrors->SetVectorAttribute("EUC", w4NumOfCols, w4rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW4LineReconstructionErrors; 
	commentW4LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW4LineReconstructionErrors << w4NumOfCols; 
	commentW4LineReconstructionErrors << ": the number of validation points"; 
	tagW4LineReconstructionErrors->AddCharacterData(commentW4LineReconstructionErrors.str().c_str(), commentW4LineReconstructionErrors.str().size()); 
	// </Wire4LineReconstructionErrors>
	delete[] w4rawXLineReconstructionErrors; 
	delete[] w4rawYLineReconstructionErrors; 
	delete[] w4rawEUCLineReconstructionErrors;


	double *w6LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++) {
		w6LREAnalysis[i] = this->GetLineReconstructionErrorAnalysisVector(6)[i]; 
	}

	// <Wire6LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire6LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire6LineReconstructionErrorAnalysis->SetName("Wire6LineReconstructionErrorAnalysis"); 
	tagWire6LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w6LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire6LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->GetLineReconstructionErrorAnalysisVector(6)[6]);  
	vtkstd::string commentWire6LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire6LineReconstructionErrorAnalysis->AddCharacterData(commentWire6LineReconstructionErrorAnalysis.c_str(), commentWire6LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w6LREAnalysis;


	const int w6NumOfRows = this->GetLineReconstructionErrorMatrix(6).rows(); 
	const int w6NumOfCols = this->GetLineReconstructionErrorMatrix(6).cols(); 
	double *w6rawXLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawYLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawEUCLineReconstructionErrors = new double[w6NumOfCols];

	for (int column = 0; column < w6NumOfCols; column++) {
		w6rawXLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(6).get(0, column); 
		w6rawYLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(6).get(1, column); 
		w6rawEUCLineReconstructionErrors[column] = this->GetLineReconstructionErrorMatrix(6).get(3, column); 
	}

	// <Wire6LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW6LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW6LineReconstructionErrors->SetName("Wire6LineReconstructionErrors"); 
	tagW6LineReconstructionErrors->SetVectorAttribute("X", w6NumOfCols, w6rawXLineReconstructionErrors); 
	tagW6LineReconstructionErrors->SetVectorAttribute("Y", w6NumOfCols, w6rawYLineReconstructionErrors); 
	tagW6LineReconstructionErrors->SetVectorAttribute("EUC", w6NumOfCols, w6rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW6LineReconstructionErrors; 
	commentW6LineReconstructionErrors << "# LineReconstructionErrors format: 6xN matrix, N = "; 
	commentW6LineReconstructionErrors << w6NumOfCols; 
	commentW6LineReconstructionErrors << ": the number of validation points"; 
	tagW6LineReconstructionErrors->AddCharacterData(commentW6LineReconstructionErrors.str().c_str(), commentW6LineReconstructionErrors.str().size()); 
	// </Wire6LineReconstructionErrors>
	delete[] w6rawXLineReconstructionErrors; 
	delete[] w6rawYLineReconstructionErrors;  
	delete[] w6rawEUCLineReconstructionErrors;

	tagErrorReports->AddNestedElement(tagPointReconstructionErrorAnalysis); 
	tagErrorReports->AddNestedElement(tagPointReconstructionErrors);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrorAnalysis);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrors);
	tagErrorReports->AddNestedElement(tagWire1LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW1LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire3LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW3LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire4LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW4LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire6LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW6LineReconstructionErrors);
	// <ErrorReports>

	xmlCalibrationResults->AddNestedElement(tagCalibrationFile); 
	xmlCalibrationResults->AddNestedElement(tagCalibrationResults); 
	xmlCalibrationResults->AddNestedElement(tagErrorReports); 
	// </USTemplateCalibrationResult>

	xmlCalibrationResults->PrintXML(this->GetCalibrationResultFileNameWithPath()); 
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SaveCalibrationDataToSequenceMetafile()
{
	LOG_TRACE("vtkFreehandCalibrationController::SaveCalibrationDataToSequenceMetafile"); 

	// Save calibration dataset 
	std::ostringstream calibrationDataFileName; 
	calibrationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_1).OutputSequenceMetaFileSuffix; 
	if ( this->SaveTrackedFrameListToMetafile( FREEHAND_MOTION_1, this->GetOutputPath(), calibrationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
		return;
	}

	LOG_INFO("Save validation data to sequence metafile"); 
	// TODO add validation file name to config file
	// Save validation dataset
	std::ostringstream validationDataFileName; 
	validationDataFileName << this->GetCalibrator()->getCalibrationTimeStampInString() << this->GetRealtimeImageDataInfo(FREEHAND_MOTION_2).OutputSequenceMetaFileSuffix; 
	if ( this->SaveTrackedFrameListToMetafile( FREEHAND_MOTION_2, this->GetOutputPath(), validationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
	{
		LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
		return; 
	}
}

//-----------------------------------------------------------------------------

std::vector<double> vtkFreehandCalibrationController::GetLineReconstructionErrorAnalysisVector(int wireNumber)
{
	LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorAnalysisVector (wire #" << wireNumber << ")");

	std::vector<double> absLREAnalysisInUSProbeFrame; 
	switch (wireNumber)
	{
	case 1: { // wire #1
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire1();
		}
		break; 
	case 3: { // wire #3
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire3();
		}
		break; 
	case 4: { // wire #4
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire4();
		}
		break; 
	case 6: { // wire #6
			absLREAnalysisInUSProbeFrame = this->GetCalibrator()->getLREAbsAnalysisForNWire6();
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE analysis vector for wire #" << wireNumber ); 
	}

	return absLREAnalysisInUSProbeFrame; 
}

//-----------------------------------------------------------------------------

vnl_matrix<double> vtkFreehandCalibrationController::GetLineReconstructionErrorMatrix(int wireNumber)
{
	//LOG_TRACE("vtkProbeCalibrationController::GetLineReconstructionErrorMatrix (wire #" << wireNumber << ")"); 

	vnl_matrix<double> mLREOrigInUSProbeFrameMatrix; 
	switch (wireNumber)
	{
	case 1: { // wire #1
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire1();
		}
		break; 
	case 3: { // wire #3
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire3();
		}
		break; 
	case 4: { // wire #4
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire4();
		}
		break; 
	case 6: { // wire #6
			mLREOrigInUSProbeFrameMatrix = this->GetCalibrator()->getLREOrigMatrix4xNForNWire6();
		}
		break; 
	default: 
		LOG_ERROR("Unable to get LRE matrix for wire #" << wireNumber ); 
	}

	return mLREOrigInUSProbeFrameMatrix; 
}


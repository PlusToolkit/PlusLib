#include "PlusConfigure.h"

#include "StylusCalibrationController.h"
#include "vtkFreehandController.h"

#include "vtkTrackerTool.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkXMLUtilities.h"

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkCylinderSource.h"
#include "vtkConeSource.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMath.h"
#include "vtkAxesActor.h"

//-----------------------------------------------------------------------------

StylusCalibrationController *StylusCalibrationController::m_Instance = NULL;

//-----------------------------------------------------------------------------

StylusCalibrationController* StylusCalibrationController::GetInstance() { // macro-val TODO
	if (! m_Instance) {
		m_Instance = new StylusCalibrationController();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

StylusCalibrationController::StylusCalibrationController()
	:AbstractToolboxController()
	,m_NumberOfPoints(200)
	,m_CurrentPointNumber(0)
	,m_StartingFrame(-1)
	,m_StylusPortNumber(-1)
	,m_StylusToStylustipTransform(NULL)
	,m_Precision(-1.0)
	,m_InputPolyData(NULL)
	,m_InputActor(NULL)
	,m_StylusTipPolyData(NULL)
	,m_StylusTipActor(NULL)
	,m_StylusActor(NULL)
	,m_PositionString("")
{
	m_BoundingBox[0] = m_BoundingBox[1] = m_BoundingBox[2] = m_BoundingBox[3] = m_BoundingBox[4] = m_BoundingBox[5] = 0.0;
}

//-----------------------------------------------------------------------------

StylusCalibrationController::~StylusCalibrationController()
{
	if (m_StylusToStylustipTransform != NULL) {
		m_StylusToStylustipTransform->Delete();
		m_StylusToStylustipTransform = NULL;
	}

	if (m_InputActor != NULL) {
		m_InputActor->Delete();
		m_InputActor = NULL;
	}

	if (m_InputPolyData != NULL) {
		m_InputPolyData->Delete();
		m_InputPolyData = NULL;
	}

	if (m_StylusTipPolyData != NULL) {
		m_StylusTipPolyData->Delete();
		m_StylusTipPolyData = NULL;
	}
	
	if (m_StylusTipActor != NULL) {
		m_StylusTipActor->Delete();
		m_StylusTipActor = NULL;
	}
	
	if (m_StylusActor != NULL) {
		m_StylusActor->Delete();
		m_StylusActor = NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::Initialize()
{
	LOG_DEBUG("Initialize StylusCalibrationController");

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

	// Get stylus tool port number
	unsigned int toolNumber = dataCollector->GetTracker()->GetToolPortByName("Stylus");
	if (toolNumber != -1) {
		m_StylusPortNumber = toolNumber;
	} else {
		LOG_WARNING("Stylus port number not found in configuration file, default tool used!");
		m_StylusPortNumber = dataCollector->GetTracker()->GetDefaultTool();
	}

	// Initialize visualization
	InitializeVisualization();

	// Set state to idle
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void StylusCalibrationController::InitializeVisualization()
{
	if (m_State == ToolboxState_Uninitialized) {
		// Initialize input poly data
		m_InputPolyData = vtkPolyData::New();
		m_InputPolyData->Initialize();
		vtkSmartPointer<vtkPoints> inputPoints = vtkSmartPointer<vtkPoints>::New();
		inputPoints->SetNumberOfPoints(m_NumberOfPoints);
		m_InputPolyData->SetPoints(inputPoints);

		// Initialize stylus tip poly data
		m_StylusTipPolyData = vtkPolyData::New();
		m_StylusTipPolyData->Initialize();
		vtkSmartPointer<vtkPoints> stylusTipPoint = vtkSmartPointer<vtkPoints>::New();
		//stylusTipPoint->SetNumberOfPoints(1); // Makes the input actor disappear!
		m_StylusTipPolyData->SetPoints(stylusTipPoint);

		if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
			vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();

			// Initialize input visualization
			m_InputActor = vtkActor::New();
			vtkSmartPointer<vtkPolyDataMapper> inputMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkGlyph3D> inputGlyph = vtkSmartPointer<vtkGlyph3D>::New();
			vtkSmartPointer<vtkSphereSource> inputSphereSource = vtkSmartPointer<vtkSphereSource>::New();
			inputSphereSource->SetRadius(1.5); // mm

			inputGlyph->SetInputConnection(m_InputPolyData->GetProducerPort());
			inputGlyph->SetSourceConnection(inputSphereSource->GetOutputPort());
			inputMapper->SetInputConnection(inputGlyph->GetOutputPort());
			m_InputActor->SetMapper(inputMapper);
			m_InputActor->GetProperty()->SetColor(0.0, 0.0, 1.0);

			// Initialize stylustip visualization
			m_StylusTipActor = vtkActor::New();
			vtkSmartPointer<vtkPolyDataMapper> stylusTipMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkGlyph3D> stylusTipGlyph = vtkSmartPointer<vtkGlyph3D>::New();
			vtkSmartPointer<vtkSphereSource> stylusTipSphereSource = vtkSmartPointer<vtkSphereSource>::New();
			stylusTipSphereSource->SetRadius(3.0); // mm

			stylusTipGlyph->SetInputConnection(m_StylusTipPolyData->GetProducerPort());
			stylusTipGlyph->SetSourceConnection(stylusTipSphereSource->GetOutputPort());
			stylusTipMapper->SetInputConnection(stylusTipGlyph->GetOutputPort());
			m_StylusTipActor->SetMapper(stylusTipMapper);
			m_StylusTipActor->GetProperty()->SetColor(0.0, 0.5, 0.0);

			// Initialize stylus visualization - in ReferenceTool coordinate system
			m_StylusActor = vtkActor::New();
			vtkSmartPointer<vtkPolyDataMapper> stylusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkCylinderSource> stylusBigCylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
			stylusBigCylinderSource->SetRadius(3.0); // mm
			stylusBigCylinderSource->SetHeight(120.0); // mm
			stylusBigCylinderSource->SetCenter(0.0, 150.0, 0.0);
			vtkSmartPointer<vtkCylinderSource> stylusSmallCylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
			stylusSmallCylinderSource->SetRadius(1.5); // mm
			stylusSmallCylinderSource->SetHeight(80.0); // mm
			stylusSmallCylinderSource->SetCenter(0.0, 50.0, 0.0);
			vtkSmartPointer<vtkConeSource> stylusTipConeSource = vtkSmartPointer<vtkConeSource>::New();
			stylusTipConeSource->SetRadius(1.5); // mm
			stylusTipConeSource->SetHeight(10.0); //mm
			vtkSmartPointer<vtkTransform> coneTransform = vtkSmartPointer<vtkTransform>::New();
			coneTransform->Identity();
			coneTransform->RotateZ(-90.0);
			coneTransform->Translate(-5.0, 0.0, 0.0);
			vtkSmartPointer<vtkTransformPolyDataFilter> coneTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			coneTransformFilter->AddInputConnection(stylusTipConeSource->GetOutputPort());
			coneTransformFilter->SetTransform(coneTransform);

			vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
			appendFilter->AddInputConnection(stylusBigCylinderSource->GetOutputPort());
			appendFilter->AddInputConnection(stylusSmallCylinderSource->GetOutputPort());
			appendFilter->AddInputConnection(coneTransformFilter->GetOutputPort());
			vtkSmartPointer<vtkTransform> stylusTransform = vtkSmartPointer<vtkTransform>::New();
			stylusTransform->Identity();
			stylusTransform->RotateZ(90.0);
			vtkSmartPointer<vtkTransformPolyDataFilter> stylusTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			stylusTransformFilter->AddInputConnection(appendFilter->GetOutputPort());
			stylusTransformFilter->SetTransform(stylusTransform);

			stylusMapper->SetInputConnection(stylusTransformFilter->GetOutputPort());
			m_StylusActor->SetMapper(stylusMapper);
			m_StylusActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

			// Axes actor
			/* TODO reference-re rakni
			vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
			axesActor->SetShaftTypeToCylinder();
			axesActor->SetXAxisLabelText("X");
			axesActor->SetYAxisLabelText("Y");
			axesActor->SetZAxisLabelText("Z");
			axesActor->SetAxisLabels(0);
			axesActor->SetTotalLength(50, 50, 50);
			axesActor->SetPosition(1400, 1200, 100);
			renderer->AddActor(axesActor);
			*/

			// Add actors
			renderer->AddActor(m_InputActor);
		}
	} else if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {  // If already initialized (it can occur if tab change - and so clear - happened)
		// Add all actors to the renderer again - state must be "Done", because tab cannot be changed if "In progress"
		vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
		renderer->AddActor(m_InputActor);
		renderer->AddActor(m_StylusActor);
		renderer->AddActor(m_StylusTipActor);
		renderer->Modified();
	}
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::Clear()
{
	LOG_DEBUG("Clear StylusCalibrationController");

	// Remove all actors from the renderer
	vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
	renderer->RemoveActor(m_InputActor);
	renderer->RemoveActor(m_StylusActor);
	renderer->RemoveActor(m_StylusTipActor);
	renderer->Modified();

	m_Toolbox->Clear();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

int StylusCalibrationController::GetNumberOfPoints() {
	return m_NumberOfPoints;
}

//-----------------------------------------------------------------------------

void StylusCalibrationController::SetNumberOfPoints(int aNumOfPoints) {
	m_NumberOfPoints = aNumOfPoints;
}

//-----------------------------------------------------------------------------

int StylusCalibrationController::GetCurrentPointNumber() {
	return m_CurrentPointNumber;
}

//-----------------------------------------------------------------------------

unsigned int StylusCalibrationController::GetStylusPortNumber() {
	return m_StylusPortNumber;
}

//-----------------------------------------------------------------------------

double StylusCalibrationController::GetPrecision() {
	return m_Precision;
}

//-----------------------------------------------------------------------------

std::string StylusCalibrationController::GetBoundingBoxString()
{
	char boundingBoxChars[32];
	sprintf_s(boundingBoxChars, 32, "%.1lf X %.1lf X %.1lf", m_BoundingBox[1]-m_BoundingBox[0], m_BoundingBox[3]-m_BoundingBox[2], m_BoundingBox[5]-m_BoundingBox[4]);
	std::string boundingBoxString(boundingBoxChars);

	return boundingBoxString;
}

//-----------------------------------------------------------------------------

vtkTransform* StylusCalibrationController::GetStylusToStylustipTransform()
{
	return m_StylusToStylustipTransform;
}

//-----------------------------------------------------------------------------

std::string StylusCalibrationController::GetStylusToStylustipTransformString()
{
	vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
	m_StylusToStylustipTransform->GetMatrix(transform);

	char stylusToStylustipTransformChars[32];
	sprintf_s(stylusToStylustipTransformChars, 32, "%.1lf X %.1lf X %.1lf", transform->GetElement(0,3), transform->GetElement(1,3), transform->GetElement(2,3));
	std::string stylusToStylustipTransformString(stylusToStylustipTransformChars);

	return stylusToStylustipTransformString;
}

//-----------------------------------------------------------------------------

std::string StylusCalibrationController::GetPositionString()
{
	return m_PositionString;
}

//-----------------------------------------------------------------------------

vtkMatrix4x4* StylusCalibrationController::AcquireStylusTrackerPosition(double aPosition[4], bool aReference)
{
	vtkFreehandController* controller = vtkFreehandController::GetInstance();
  if (controller == NULL) {
		LOG_ERROR("vtkFreehandController is invalid");
		return NULL;
	}
  if (controller->GetInitialized() == false) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return NULL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return NULL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is invalid!");
		return NULL;
	}
  if ((dataCollector->GetTracker()->GetTool(dataCollector->GetDefaultToolPortNumber()) < 0)) {
		LOG_ERROR("Tracker is not initialized properly!");
		return NULL;
	}
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = NULL; // stylus to reference tool transform
	long flags = -1;
	double timestamp;
	unsigned int toolNumber;
	if (aReference) {
		toolNumber = dataCollector->GetTracker()->GetReferenceTool();
	} else {
		toolNumber = m_StylusPortNumber;
	}

	if (dataCollector->GetTracker()->GetTool(toolNumber)->GetEnabled()) {
		transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		dataCollector->GetTransformWithTimestamp(transformMatrix, timestamp, flags, toolNumber); 

		transformMatrix->Register(NULL);
	}

	if (flags & (TR_MISSING | TR_OUT_OF_VIEW) ) {
		LOG_DEBUG("Tracker out of view!");
		m_PositionString = std::string("Tracker out of view!");
		return NULL;
	} else if (flags & TR_REQ_TIMEOUT ) {
		LOG_WARNING("Tracker request timeout!");
		m_PositionString = std::string("Tracker request timeout!");
		return NULL;
	} else if (aPosition != NULL) { // TR_OK
		// Compute the new position
		double elements[16]; //TODO find other way
		for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = transformMatrix->GetElement(i,j);
		double origin[4] = {0.0, 0.0, 0.0, 1.0};

		vtkMatrix4x4::PointMultiply(elements, origin, aPosition);

		// Assemble position string for toolbox
		char stylusPositionChars[32];

		sprintf_s(stylusPositionChars, 32, "%.1lf X %.1lf X %.1lf", aPosition[0], aPosition[1], aPosition[2]);
		m_PositionString = std::string(stylusPositionChars);
	}

	return transformMatrix;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::DoAcquisition()
{
	if (m_State == ToolboxState_InProgress) {
		LOG_DEBUG("StylusCalibrationController: Acquire positions for stylus calibration"); 

		vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
		if (dataCollector == NULL) {
			LOG_ERROR("Data collector is not initialized");
			return PLUS_FAIL;
		}
		if (dataCollector->GetTracker() == NULL) {
			LOG_ERROR("Tracker is invalid");
			return PLUS_FAIL;
		}
    if (dataCollector->GetTracker()->GetTool(dataCollector->GetDefaultToolPortNumber()) < 0) {
			LOG_ERROR("Tracker is not initialized properly");
			return PLUS_FAIL;
		}

		double stylusPosition[4];

		if (AcquireStylusTrackerPosition(stylusPosition)) {
			vtkPoints* points = m_InputPolyData->GetPoints();
			
			double distance_lowThreshold_mm = 3.0; // TODO review this threshold with the guys
			double distance_highThreshold_mm = 1000.0;
			double distance = -1.0;
			if (m_CurrentPointNumber < 1) {
				distance = (distance_lowThreshold_mm + distance_highThreshold_mm) / 2.0;
			} else {
				double previousPosition[4];
				points->GetPoint(m_CurrentPointNumber-1, previousPosition);
				previousPosition[3] = 1.0;
				// Square distance
				distance = vtkMath::Distance2BetweenPoints(stylusPosition, previousPosition);
			}

			// If current point is close to the previous one, or too far (outlier), we do not insert it
			if (distance < distance_lowThreshold_mm * distance_lowThreshold_mm) {
				LOG_DEBUG("Acquired position is too close to the previous - it is skipped");
			} else if (distance > distance_highThreshold_mm * distance_highThreshold_mm) {
				LOG_DEBUG("Acquired position seems to be an outlier - it is skipped");
			} else {
				// Add the point into the calibration dataset
				dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->InsertNextCalibrationPoint();

				// Add to polydata for rendering and re-compute bounding box
				points->InsertPoint(m_CurrentPointNumber, stylusPosition[0], stylusPosition[1], stylusPosition[2]);
				points->Modified();
				points->ComputeBounds();
				points->GetBounds(m_BoundingBox);

				// Set new current point number
				++m_CurrentPointNumber;

				// Reset the camera once in a while
				if ((m_CurrentPointNumber > 0) && ((m_CurrentPointNumber % 10 == 0) || (m_CurrentPointNumber == 5)) && (vtkFreehandController::GetInstance()->GetCanvas() != NULL)) {
					vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
				}

				if (m_CurrentPointNumber >= m_NumberOfPoints) {
					if (m_Toolbox) {
						m_Toolbox->Stop();
					} else {
						Stop();
					}
					if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
						DisplayStylus();
						vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
					}
				}
			}
		}
	} else if ((m_State == ToolboxState_Done) && (vtkFreehandController::GetInstance()->GetCanvas() != NULL)) {
		DisplayStylus();
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void StylusCalibrationController::DisplayStylus()
{
	LOG_DEBUG("StylusCalibrationController: Display stylustip position"); 

	double stylusPosition[4];
	vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTransformMatrix = NULL;

	if (referenceToolToStylusTransformMatrix = AcquireStylusTrackerPosition(stylusPosition)) {
		m_StylusActor->GetProperty()->SetOpacity(1.0);
		m_StylusActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

		// Apply calibration
		vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTipTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		referenceToolToStylusTipTransformMatrix->Identity();
		vtkMatrix4x4::Multiply4x4(referenceToolToStylusTransformMatrix, m_StylusToStylustipTransform->GetMatrix(), referenceToolToStylusTipTransformMatrix);

		m_StylusActor->SetUserMatrix(referenceToolToStylusTipTransformMatrix);
	} else {
		m_StylusActor->GetProperty()->SetOpacity(0.3);
		m_StylusActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
	}
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::Start()
{
	LOG_INFO("Start stylus calibration"); 

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller == NULL) {
		LOG_ERROR("vtkFreehandController is invalid");
		return PLUS_FAIL;
	}
	if (controller->GetInitialized() == false) {
		LOG_ERROR("vtkFreehandController is not initialized");
		return PLUS_FAIL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized");
		return PLUS_FAIL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is invalid");
		return PLUS_FAIL;
	}
	if (dataCollector->GetTracker()->GetTool(dataCollector->GetDefaultToolPortNumber()) < 0) {
		LOG_ERROR("Tracker is not initialized properly");
		return PLUS_FAIL;
	}

	m_CurrentPointNumber = 0;

	// Initialize tracker tool
	dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->GetBuffer()->Clear();
	dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->InitializeToolTipCalibration();

	vtkSmartPointer<vtkTransform> initialTransform = vtkSmartPointer<vtkTransform>::New();
	initialTransform->Identity();
	dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->SetCalibrationMatrix(initialTransform->GetMatrix());

	// Reset polydatas (make it look like empty)
	m_InputPolyData->GetPoints()->Reset();
	m_StylusTipPolyData->GetPoints()->Reset();

	// Remove stylus tip actor
	controller->GetCanvasRenderer()->RemoveActor(m_StylusTipActor);
	controller->GetCanvasRenderer()->RemoveActor(m_StylusActor);

	// Set state to in progress
	m_State = ToolboxState_InProgress;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::Stop()
{
	bool success = CalibrateStylus();

	if (success) {
		LOG_INFO("Stylus calibration successful");
		m_State = ToolboxState_Done;
	} else {
		LOG_ERROR("Stylus calibration failed!");
		m_CurrentPointNumber = 0;
		m_State = ToolboxState_Error;
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::LoadStylusCalibrationFromDataCollectionConfigFile()
{
	//TODO

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::SaveStylusCalibrationToDataCollectionConfigFile()
{
	//TODO

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::LoadStylusCalibrationFromFile(std::string aFile)
{
	vtkSmartPointer<vtkXMLDataElement> stylusCalibration = vtkXMLUtilities::ReadElementFromFile(aFile.c_str());

	if (stylusCalibration == NULL) {	
		LOG_ERROR("Unable to read the stylus calibration file: " << aFile); 
		return PLUS_FAIL;
	}

	// Load stylus calibration transform
	vtkXMLDataElement* stylusToStylusTipTransform = stylusCalibration->FindNestedElementWithName("StylusToStylusTipTransform"); 
	if (stylusToStylusTipTransform == NULL) {
		LOG_ERROR("Stylus calibration transform not found!");
		return PLUS_FAIL;
	} else {
		double* transform = new double[16]; 
		if (stylusToStylusTipTransform->GetVectorAttribute("Transform", 16, transform)) {
			// Create matrix and set it to controller member variable
			vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			transformMatrix->Identity();
			transformMatrix->DeepCopy(transform);

			if (m_StylusToStylustipTransform == NULL) {
				m_StylusToStylustipTransform = vtkTransform::New();
			}
			m_StylusToStylustipTransform->Identity();
			m_StylusToStylustipTransform->SetMatrix(transformMatrix);

			// Set calibration matrix to stylus tool
			vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
			if (dataCollector != NULL) {
				dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->SetCalibrationMatrix(transformMatrix);
			} else {
				LOG_WARNING("Data collector is not initialized!");
			}
		}
		delete[] transform; 
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void StylusCalibrationController::SaveStylusCalibrationToFile(std::string aFile)
{
	vtkSmartPointer<vtkXMLDataElement> stylusCalibration = vtkSmartPointer<vtkXMLDataElement>::New();
	stylusCalibration->SetName("StylusCalibration");
	stylusCalibration->SetAttribute("version", "1.0");

	vtkSmartPointer<vtkXMLDataElement> stylusToStylusTipTransform = vtkSmartPointer<vtkXMLDataElement>::New();

	char stylusToStylustipTransformChars[256];
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = m_StylusToStylustipTransform->GetMatrix();
	sprintf_s(stylusToStylustipTransformChars, 256, "\n\t1 0 0 %.4lf\n\t0 1 0 %.4lf\n\t0 0 1 %.4lf\n\t0 0 0 1", transformMatrix->GetElement(0,3), transformMatrix->GetElement(1,3), transformMatrix->GetElement(2,3));

	stylusToStylusTipTransform->SetName("StylusToStylusTipTransform");

	stylusToStylusTipTransform->SetAttribute("Transform", stylusToStylustipTransformChars);

	stylusCalibration->AddNestedElement(stylusToStylusTipTransform);

	stylusCalibration->PrintXML(aFile.c_str());
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationController::CalibrateStylus()
{
	LOG_INFO("Calculate calibration"); 

	vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetTool(m_StylusPortNumber) < 0)) {
		LOG_ERROR("Tracker is not initialized properly!");
		return PLUS_FAIL;
	}

	// Do the calibration
	m_Precision = dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->DoToolTipCalibration();

	// Get the calibration result
	if (m_StylusToStylustipTransform == NULL) {
		m_StylusToStylustipTransform = vtkTransform::New();
	}
	m_StylusToStylustipTransform->Identity();
	vtkMatrix4x4* transform = dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->GetCalibrationMatrix();
	m_StylusToStylustipTransform->SetMatrix(transform);

	// Display calibration result stylus tip (by acquiring a new point and using it) //TODO do it an other way, without getting a new point
	if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
		double stylusTipPosition[4];
		vtkMatrix4x4* referenceToolToStylusTransformMatrix = NULL;

		if (referenceToolToStylusTransformMatrix = AcquireStylusTrackerPosition(stylusTipPosition)) {
			// Apply calibration
			vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTipTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			referenceToolToStylusTipTransformMatrix->Identity();
			vtkMatrix4x4::Multiply4x4(referenceToolToStylusTransformMatrix, m_StylusToStylustipTransform->GetMatrix(), referenceToolToStylusTipTransformMatrix);

			double elements[16]; //TODO find other way
			for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = referenceToolToStylusTipTransformMatrix->GetElement(i,j);
			double origin[4] = {0.0, 0.0, 0.0, 1.0};
			vtkMatrix4x4::PointMultiply(elements, origin, stylusTipPosition);

			m_StylusTipPolyData->GetPoints()->InsertPoint(0, stylusTipPosition[0], stylusTipPosition[1], stylusTipPosition[2]);
			m_StylusTipPolyData->GetPoints()->Modified();
		}

		// Add stylus tip actor
		if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
			vtkFreehandController::GetInstance()->GetCanvasRenderer()->AddActor(m_StylusTipActor);
			vtkFreehandController::GetInstance()->GetCanvasRenderer()->AddActor(m_StylusActor);
		}
	}

	return PLUS_SUCCESS;
}

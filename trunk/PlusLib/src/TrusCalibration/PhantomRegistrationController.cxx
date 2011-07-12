#include "PlusConfigure.h"

#include "PhantomRegistrationController.h"

#include "vtkFreehandController.h"
#include "StylusCalibrationController.h"

#include "vtkFakeTracker.h"
#include "vtkAccurateTimer.h"
#include "vtkTrackerTool.h"
#include "vtkPolyData.h"
#include "vtkSTLReader.h"
#include "vtkXMLUtilities.h"
#include "vtkFileFinder.h"

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

#include "itkLandmarkBasedTransformInitializer.h"
#include "itkSimilarity3DTransform.h"

//-----------------------------------------------------------------------------

PhantomRegistrationController *PhantomRegistrationController::m_Instance = NULL;

//-----------------------------------------------------------------------------

PhantomRegistrationController* PhantomRegistrationController::GetInstance() {
	if (! m_Instance) {
		m_Instance = new PhantomRegistrationController();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

PhantomRegistrationController::PhantomRegistrationController()
	:AbstractToolboxController()
	,m_PhantomBodyActor(NULL)
	,m_RegisteredPhantomBodyActor(NULL)
	,m_StylusActor(NULL)
	,m_LandmarksActor(NULL)
	,m_RequestedLandmarksActor(NULL)
	,m_AxesActor(NULL)
	,m_LandmarksPolyData(NULL)
	,m_RequestedLandmarkPolyData(NULL)
	,m_DefinedLandmarks(NULL)
	,m_PhantomToPhantomReferenceTransform(NULL)
	,m_ModelToPhantomOriginTransform(NULL)
	,m_PositionString("")
	,m_PhantomDefinitionFileName("")
	,m_CurrentLandmarkIndex(-1)
	,m_RecordRequested(false)
{
	m_LandmarkNames.clear();

	m_PhantomRenderer = vtkRenderer::New();
}

//-----------------------------------------------------------------------------

PhantomRegistrationController::~PhantomRegistrationController()
{
	//TODO use vtkSmartPointer when creating these
	if (m_PhantomRenderer != NULL) {
		m_PhantomRenderer->Delete();
		m_PhantomRenderer = NULL;
	}

	if (m_PhantomBodyActor != NULL) {
		m_PhantomBodyActor->Delete();
		m_PhantomBodyActor = NULL;
	}

	if (m_RegisteredPhantomBodyActor != NULL) {
		m_RegisteredPhantomBodyActor->Delete();
		m_RegisteredPhantomBodyActor = NULL;
	}

	if (m_StylusActor != NULL) {
		m_StylusActor->Delete();
		m_StylusActor = NULL;
	}

	if (m_LandmarksActor != NULL) {
		m_LandmarksActor->Delete();
		m_LandmarksActor = NULL;
	}
	
	if (m_RequestedLandmarksActor != NULL) {
		m_RequestedLandmarksActor->Delete();
		m_RequestedLandmarksActor = NULL;
	}
	
	if (m_AxesActor != NULL) {
		m_AxesActor->Delete();
		m_AxesActor = NULL;
	}

	if (m_LandmarksPolyData != NULL) {
		m_LandmarksPolyData->Delete();
		m_LandmarksPolyData = NULL;
	}
	
	if (m_RequestedLandmarkPolyData != NULL) {
		m_RequestedLandmarkPolyData->Delete();
		m_RequestedLandmarkPolyData = NULL;
	}
	
	if (m_PhantomToPhantomReferenceTransform != NULL) {
		m_PhantomToPhantomReferenceTransform->Delete();
		m_PhantomToPhantomReferenceTransform = NULL;
	}
	
	if (m_ModelToPhantomOriginTransform != NULL) {
		m_ModelToPhantomOriginTransform->Delete();
		m_ModelToPhantomOriginTransform = NULL;
	}

	if (m_DefinedLandmarks != NULL) {
		m_DefinedLandmarks->Delete();
		m_DefinedLandmarks = NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::Initialize()
{
	LOG_DEBUG("Initialize PhantomRegistrationController");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller == NULL) {
		LOG_ERROR("vtkFreehandController is invalid!");
		return PLUS_FAIL;
	}
  if (controller->GetInitialized() == false) {
    LOG_ERROR("vtkFreehandController is not initialized!");
    return PLUS_FAIL;
  }

	if (m_Toolbox) {
		m_Toolbox->Initialize();
	}

	InitializeVisualization();

	// Set state to idle
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;
	}

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void PhantomRegistrationController::InitializeVisualization()
{
	if (m_State == ToolboxState_Uninitialized) {
		//Initialize polydatas
		m_LandmarksPolyData = vtkPolyData::New();
		m_LandmarksPolyData->Initialize();
		vtkSmartPointer<vtkPoints> landmarkPoints = vtkSmartPointer<vtkPoints>::New();
		m_LandmarksPolyData->SetPoints(landmarkPoints);

		m_RequestedLandmarkPolyData = vtkPolyData::New();
		m_RequestedLandmarkPolyData->Initialize();
		vtkSmartPointer<vtkPoints> requestedLandmarkPoints = vtkSmartPointer<vtkPoints>::New();
		m_RequestedLandmarkPolyData->SetPoints(requestedLandmarkPoints);

		if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
			// Get renderer from freehand controller
			vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();

			// Initialize toolbox renderer
			m_PhantomRenderer->SetBackground(0.6, 0.6, 0.6);
			
			// Initialize landmark actors
			m_LandmarksActor = vtkActor::New();
			vtkSmartPointer<vtkPolyDataMapper> landmarksMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkGlyph3D> landmarksGlyph = vtkSmartPointer<vtkGlyph3D>::New();
			vtkSmartPointer<vtkSphereSource> landmarksSphereSource = vtkSmartPointer<vtkSphereSource>::New();
			landmarksSphereSource->SetRadius(1.5); // mm

			landmarksGlyph->SetInputConnection(m_LandmarksPolyData->GetProducerPort());
			landmarksGlyph->SetSourceConnection(landmarksSphereSource->GetOutputPort());
			landmarksMapper->SetInputConnection(landmarksGlyph->GetOutputPort());
			m_LandmarksActor->SetMapper(landmarksMapper);
			m_LandmarksActor->GetProperty()->SetColor(0.0, 0.0, 1.0);

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

			// Initialize registered phantom body actor
			m_RegisteredPhantomBodyActor = vtkActor::New();
			m_RegisteredPhantomBodyActor->VisibilityOff();

			// Add actors
			renderer->AddActor(m_StylusActor);
			renderer->AddActor(m_LandmarksActor);
			renderer->AddActor(m_RegisteredPhantomBodyActor);

			renderer->ResetCamera();

			// Phantom body
			m_PhantomBodyActor = vtkActor::New();

			// Initialize defined landmark visualization
			m_RequestedLandmarksActor = vtkActor::New();
			vtkSmartPointer<vtkPolyDataMapper> requestedLandmarksMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkGlyph3D> requestedLandmarksGlyph = vtkSmartPointer<vtkGlyph3D>::New();
			vtkSmartPointer<vtkSphereSource> requestedLandmarksSphereSource = vtkSmartPointer<vtkSphereSource>::New();
			requestedLandmarksSphereSource->SetRadius(1.5); // mm

			requestedLandmarksGlyph->SetInputConnection(m_RequestedLandmarkPolyData->GetProducerPort());
			requestedLandmarksGlyph->SetSourceConnection(requestedLandmarksSphereSource->GetOutputPort());
			requestedLandmarksMapper->SetInputConnection(requestedLandmarksGlyph->GetOutputPort());
			m_RequestedLandmarksActor->SetMapper(requestedLandmarksMapper);
			m_RequestedLandmarksActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

			// Add actors
			m_PhantomRenderer->AddActor(m_PhantomBodyActor);
			m_PhantomRenderer->AddActor(m_RequestedLandmarksActor);

			// Axes actor
			/*
			m_AxesActor = vtkAxesActor::New();
			m_AxesActor->SetShaftTypeToCylinder();
			m_AxesActor->SetXAxisLabelText("X");
			m_AxesActor->SetYAxisLabelText("Y");
			m_AxesActor->SetZAxisLabelText("Z");
			m_AxesActor->SetAxisLabels(0);
			m_AxesActor->SetTotalLength(50, 50, 50);
			//m_AxesActor->SetPosition(1400, 1200, 100);
			//m_AxesActor->SetVisibility(0);
			//renderer->AddActor(m_AxesActor);
			//m_PhantomRenderer->AddActor(m_AxesActor);
			*/
		}
	} else if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) { // If already initialized (it can occur if tab change - and so clear - happened)
		// Add all actors to the renderers again
		vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
		renderer->AddActor(m_StylusActor);
		renderer->AddActor(m_LandmarksActor);
		renderer->AddActor(m_RegisteredPhantomBodyActor);
		renderer->Modified();

		m_PhantomRenderer->AddActor(m_PhantomBodyActor);
		m_PhantomRenderer->AddActor(m_RequestedLandmarksActor);
		m_PhantomRenderer->Modified();
	}
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::Clear()
{
	LOG_DEBUG("Clear PhantomRegistrationController");

	// Remove all actors from the renderers
	vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
	renderer->RemoveActor(m_StylusActor);
	renderer->RemoveActor(m_LandmarksActor);
	renderer->RemoveActor(m_RegisteredPhantomBodyActor);
	renderer->Modified();

	m_PhantomRenderer->RemoveActor(m_PhantomBodyActor);
	m_PhantomRenderer->RemoveActor(m_RequestedLandmarksActor);
	m_PhantomRenderer->Modified();

	m_Toolbox->Clear();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

std::string PhantomRegistrationController::GetPositionString() {
	return m_PositionString;
}

//-----------------------------------------------------------------------------

std::string PhantomRegistrationController::GetPhantomDefinitionFileName() {
	return m_PhantomDefinitionFileName;
}

//-----------------------------------------------------------------------------

int PhantomRegistrationController::GetNumberOfLandmarks()
{
	if (m_DefinedLandmarks == NULL) {
		return 0;
	} else {
		return m_DefinedLandmarks->GetNumberOfPoints();
	}
}

//-----------------------------------------------------------------------------

int PhantomRegistrationController::GetCurrentLandmarkIndex()
{
	return m_CurrentLandmarkIndex;
}

//-----------------------------------------------------------------------------

vtkTransform* PhantomRegistrationController::GetPhantomToPhantomReferenceTransform()
{
	return m_PhantomToPhantomReferenceTransform;
}

//-----------------------------------------------------------------------------

std::string PhantomRegistrationController::GetCurrentLandmarkName()
{
	return m_LandmarkNames[m_CurrentLandmarkIndex];
}

//-----------------------------------------------------------------------------

vtkRenderer* PhantomRegistrationController::GetPhantomRenderer()
{
	return m_PhantomRenderer;
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::Start()
{
	if ( (m_DefinedLandmarks != NULL) && (m_DefinedLandmarks->GetNumberOfPoints() >= 4)
		&& (StylusCalibrationController::GetInstance() != NULL)
		&& (StylusCalibrationController::GetInstance()->GetStylusToStylustipTransform() != NULL))
	{
		m_CurrentLandmarkIndex = 0;

		m_State = ToolboxState_InProgress;
	}
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::Stop()
{
	m_RequestedLandmarkPolyData->GetPoints()->GetData()->RemoveTuple(0);
	m_RequestedLandmarkPolyData->GetPoints()->Modified();

	m_State = ToolboxState_Done;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void PhantomRegistrationController::Undo()
{
	if (m_State == ToolboxState_Done) {
		m_State = ToolboxState_InProgress;
	}

	if (m_CurrentLandmarkIndex > 0) {
		// Decrease current landmark index
		--m_CurrentLandmarkIndex;

		// Reset result transform (if Undo was pressed when the registration was ready we have to make it null)
		if (m_PhantomToPhantomReferenceTransform != NULL) {
			m_PhantomToPhantomReferenceTransform->Delete();
			m_PhantomToPhantomReferenceTransform = NULL;
		}

		// Delete previously acquired landmark
		m_LandmarksPolyData->GetPoints()->GetData()->RemoveTuple(m_CurrentLandmarkIndex);
		m_LandmarksPolyData->Modified();

		// Highlight previous landmark
		m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_DefinedLandmarks->GetPoint(m_CurrentLandmarkIndex));
		m_RequestedLandmarkPolyData->GetPoints()->Modified();
	}

	// If tracker is FakeTracker then set counter
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(vtkFreehandController::GetInstance()->GetDataCollector()->GetTracker());
	if (fakeTracker != NULL) {
		fakeTracker->SetCounter(m_CurrentLandmarkIndex);
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationController::Reset()
{
	if (m_State == ToolboxState_Done) {
		m_State = ToolboxState_InProgress;
	}

	// Delete acquired landmarks
	vtkSmartPointer<vtkPoints> landmarkPoints = vtkSmartPointer<vtkPoints>::New();
	m_LandmarksPolyData->SetPoints(landmarkPoints);
	m_LandmarksPolyData->Modified();

	// Reset current landmark index
	m_CurrentLandmarkIndex = 0;

	// Reset result transform (if Reset was pressed when the registration was ready we have to make it null)
	if (m_PhantomToPhantomReferenceTransform != NULL) {
		m_PhantomToPhantomReferenceTransform->Delete();
		m_PhantomToPhantomReferenceTransform = NULL;
	}

	// Highlight first landmark
	m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_DefinedLandmarks->GetPoint(0));
	m_RequestedLandmarkPolyData->GetPoints()->Modified();

	// If tracker is FakeTracker then reset counter
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(vtkFreehandController::GetInstance()->GetDataCollector()->GetTracker());
	if (fakeTracker != NULL) {
		fakeTracker->SetCounter(m_CurrentLandmarkIndex);
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationController::Register()
{
	// Get recorded landmark point list
	vtkPoints* recordedLandmarks;
	if (m_LandmarksPolyData->GetPoints() != NULL) {
		recordedLandmarks = m_LandmarksPolyData->GetPoints();
	} else {
		LOG_ERROR("Data sets not inialized properly!");
		m_State = ToolboxState_Error;
		return;
	}
	if (m_DefinedLandmarks->GetNumberOfPoints() != recordedLandmarks->GetNumberOfPoints()) {
		LOG_ERROR("Landmark counts do not match! Registration not possible");
		m_State = ToolboxState_Error;
		return;
	}

	// Create input point vectors
	int numberOfLandmarks = m_DefinedLandmarks->GetNumberOfPoints();
	std::vector<itk::Point<double,3>> fixedPoints;
	std::vector<itk::Point<double,3>> movingPoints;

	for (int i=0; i<numberOfLandmarks; ++i) {
		// Defined landmarks from xml are in the phantom coordinate system
		double* fixedPointArray = m_DefinedLandmarks->GetPoint(i);
		itk::Point<double,3> fixedPoint(fixedPointArray);

		// Recorded landmarks are in the tracker coordinate system
		double* movingPointArray = recordedLandmarks->GetPoint(i);
		itk::Point<double,3> movingPoint(movingPointArray);

		fixedPoints.push_back(fixedPoint);
		movingPoints.push_back(movingPoint);
	}

	for (int i=0; i<numberOfLandmarks; ++i) {
		LOG_DEBUG("Phantom point " << i << ": Defined: " << fixedPoints[i] << "  Recorded: " << movingPoints[i]);
	}

	// Initialize ITK transform
	itk::Similarity3DTransform<double>::Pointer transform = itk::Similarity3DTransform<double>::New();
	transform->SetIdentity();

	itk::LandmarkBasedTransformInitializer<itk::Similarity3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::Pointer initializer = itk::LandmarkBasedTransformInitializer<itk::Similarity3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::New();
	initializer->SetTransform(transform);
	initializer->SetFixedLandmarks(fixedPoints);
	initializer->SetMovingLandmarks(movingPoints);
	initializer->InitializeTransform();

	// Get result (do the registration)
	vtkSmartPointer<vtkMatrix4x4> phantomToPhantomReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	phantomToPhantomReferenceTransformMatrix->Identity();

	itk::Matrix<double,3,3> transformMatrix = transform->GetMatrix();
	for (int i=0; i<transformMatrix.RowDimensions; ++i) {
		for (int j=0; j<transformMatrix.ColumnDimensions; ++j) {
			phantomToPhantomReferenceTransformMatrix->SetElement(i, j, transformMatrix[i][j]);
		}
	}
	itk::Vector<double,3> transformOffset = transform->GetOffset();
	for (int j=0; j<transformOffset.GetNumberOfComponents(); ++j) {
		phantomToPhantomReferenceTransformMatrix->SetElement(j, 3, transformOffset[j]);
	}

	m_PhantomToPhantomReferenceTransform = vtkTransform::New();
	m_PhantomToPhantomReferenceTransform->SetMatrix(phantomToPhantomReferenceTransformMatrix);

	std::ostringstream osPhantomToPhantomReferenceTransform;
	m_PhantomToPhantomReferenceTransform->GetMatrix()->Print(osPhantomToPhantomReferenceTransform);

	LOG_DEBUG("PhantomToPhantomReferenceTransform:\n" << osPhantomToPhantomReferenceTransform.str().c_str() );

	// Display phantom model in the main canvas
	if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
		vtkSmartPointer<vtkTransform> modelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
		modelToPhantomReferenceTransform->Identity();
		modelToPhantomReferenceTransform->Concatenate(m_ModelToPhantomOriginTransform);
		modelToPhantomReferenceTransform->Concatenate(m_PhantomToPhantomReferenceTransform);
		modelToPhantomReferenceTransform->Modified();

		m_RegisteredPhantomBodyActor->SetUserTransform(modelToPhantomReferenceTransform);
		m_RegisteredPhantomBodyActor->VisibilityOn();
		m_RegisteredPhantomBodyActor->Modified();

		vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationController::RequestRecording()
{
	// Turn on record request flag
	m_RecordRequested = true;

	// If tracker is FakeTracker then set counter (trigger position change)
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(vtkFreehandController::GetInstance()->GetDataCollector()->GetTracker());
	if (fakeTracker != NULL) {
		fakeTracker->SetCounter(m_CurrentLandmarkIndex);
	}
}

//-----------------------------------------------------------------------------

vtkMatrix4x4* PhantomRegistrationController::AcquireStylusTipTrackerPosition(double aPosition[4], bool aReference)
{
	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return NULL;
	}
	vtkDataCollector* dataCollector = controller->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return NULL;
	}
	if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetTool(dataCollector->GetDefaultToolPortNumber()) < 0)) {
		LOG_ERROR("Tracker is not initialized properly!");
		return NULL;
	}

	vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTransformMatrix = NULL; // stylus to reference tool transform
	vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTipTransformMatrix = NULL;

	long flags = -1;
	double timestamp;
	unsigned int toolNumber;
	if (aReference) {
		toolNumber = dataCollector->GetTracker()->GetReferenceTool();
	} else {
		toolNumber = StylusCalibrationController::GetInstance()->GetStylusPortNumber();
	}

	// If tracker is FakeTracker and recording has been requested then wait to ensure new position is set by tracker thread
	if (m_RecordRequested) { // Save the time of dynamic cast is record has not been requested
		vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(dataCollector->GetTracker());
		if (fakeTracker != NULL) {
			vtkAccurateTimer::Delay(1.0 / fakeTracker->GetFrequency());
		}
	}

	// Acquire position from tracker
	if (dataCollector->GetTracker()->GetTool(toolNumber)->GetEnabled()) {
		referenceToolToStylusTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		dataCollector->GetTransformWithTimestamp(referenceToolToStylusTransformMatrix, timestamp, flags, toolNumber); 
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
		// Apply calibration if stylus was requested
		referenceToolToStylusTipTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		referenceToolToStylusTipTransformMatrix->Identity();
		if (! aReference) {
			vtkMatrix4x4::Multiply4x4(referenceToolToStylusTransformMatrix, StylusCalibrationController::GetInstance()->GetStylusToStylustipTransform()->GetMatrix(), referenceToolToStylusTipTransformMatrix);
		} else {
			referenceToolToStylusTipTransformMatrix->DeepCopy(referenceToolToStylusTransformMatrix);
		}
		referenceToolToStylusTipTransformMatrix->Register(NULL);

		// Compute the new position
		double elements[16]; //TODO find other way
		for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = referenceToolToStylusTipTransformMatrix->GetElement(i,j);
		double origin[4] = {0.0, 0.0, 0.0, 1.0};

		vtkMatrix4x4::PointMultiply(elements, origin, aPosition);

		// Assemble position string for toolbox
		char stylusPositionChars[32];

		sprintf_s(stylusPositionChars, 32, "%.1lf X %.1lf X %.1lf", aPosition[0], aPosition[1], aPosition[2]);
		m_PositionString = std::string(stylusPositionChars);
	}

	return referenceToolToStylusTipTransformMatrix;
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::DoAcquisition()
{
	if ((m_State == ToolboxState_InProgress) || (m_State == ToolboxState_Done)) {
		// Display stylus
		double stylusTipPosition[4];
		vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTipTransformMatrix;
		
		if (referenceToolToStylusTipTransformMatrix = AcquireStylusTipTrackerPosition(stylusTipPosition)) {
			// If first acquisition, then reset camera and determine FOV
			bool firstAcquisition = false;
			if ((m_StylusActor != NULL) && (m_StylusActor->GetUserMatrix() == NULL)) {
				firstAcquisition = true;
			}

			// Display stylus
			if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
				// If acquisition was successful
				if (referenceToolToStylusTipTransformMatrix != NULL) {
					m_StylusActor->GetProperty()->SetOpacity(1.0);
					m_StylusActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

					m_StylusActor->SetUserMatrix(referenceToolToStylusTipTransformMatrix);
				} else {
					m_StylusActor->GetProperty()->SetOpacity(0.3);
					m_StylusActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
				}
			}

			// Record landmark if requested
			if ((m_RecordRequested) && (m_State == ToolboxState_InProgress)) {
				// Compute calibrated position
				double elements[16]; //TODO find other way
				for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = referenceToolToStylusTipTransformMatrix->GetElement(i,j);
				double origin[4] = {0.0, 0.0, 0.0, 1.0};
				vtkMatrix4x4::PointMultiply(elements, origin, stylusTipPosition);

				// Add recorded point
				vtkPoints* points = m_LandmarksPolyData->GetPoints();
				points->InsertPoint(m_CurrentLandmarkIndex, stylusTipPosition[0], stylusTipPosition[1], stylusTipPosition[2]);
				points->Modified();

				// Set new current landmark number and reset request flag
				++m_CurrentLandmarkIndex;

				// If it was the last landmark then set status to done and reset landmark counter
				if (m_CurrentLandmarkIndex == GetNumberOfLandmarks()) {
					if (m_Toolbox) {
						m_Toolbox->Stop();
					} else {
						Stop();
					}
				} else {
					// Highlight next landmark
					m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_DefinedLandmarks->GetPoint(m_CurrentLandmarkIndex));
					m_RequestedLandmarkPolyData->GetPoints()->Modified();
				}

				m_RecordRequested = false;
			}

			if ((firstAcquisition) && (vtkFreehandController::GetInstance()->GetCanvas() != NULL)) {
				vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
				vtkFreehandController::GetInstance()->GetCanvasRenderer()->GetActiveCamera()->Zoom(0.5);
			}
		}
	}
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::LoadPhantomDefinitionFromFile(std::string aFile)
{
	vtkSmartPointer<vtkXMLDataElement> phantomDefinition = vtkXMLUtilities::ReadElementFromFile(aFile.c_str());

	if (phantomDefinition == NULL) {	
		LOG_ERROR("Unable to read the phantom definition file: " << aFile); 
		return PLUS_FAIL;
	} else if (STRCASECMP("PhantomDefinition", phantomDefinition->GetName()) != NULL) {
		LOG_INFO(aFile << " is not a phantom definition file! The registration data cannot be used by calibration.");
	} else {
		m_PhantomDefinitionFileName = aFile;
	}

	// Load model information
	vtkXMLDataElement* model = phantomDefinition->FindNestedElementWithName("Model"); 
	if (model == NULL) {
		LOG_WARNING("Phantom model information not found - no model displayed");
	} else {
		const char* file = model->GetAttribute("File");

		if (file) {
			// Initialize phantom model visualization
			if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
				vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
				
				std::string searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(file);
				if (STRCASECMP("", searchResult.c_str()) == 0) {
					LOG_ERROR("Phantom model file is not found with name: " << file);
				} else {
					stlReader->SetFileName(searchResult.c_str());
					vtkSmartPointer<vtkPolyDataMapper> stlMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
					stlMapper->SetInputConnection(stlReader->GetOutputPort());
					m_PhantomBodyActor->SetMapper(stlMapper);

					vtkSmartPointer<vtkPolyDataMapper> stlMapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
					stlMapper2->SetInputConnection(stlReader->GetOutputPort());
					m_RegisteredPhantomBodyActor->SetMapper(stlMapper2);
				}
			}

			// ModelToPhantomOriginTransform - Transforming input model for proper visualization
			double* modelToPhantomOriginTransformVector = new double[16]; 
			if (model->GetVectorAttribute("ModelToPhantomOriginTransform", 16, modelToPhantomOriginTransformVector)) {
				vtkSmartPointer<vtkMatrix4x4> modelToPhantomOriginTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
				modelToPhantomOriginTransformMatrix->Identity();
				modelToPhantomOriginTransformMatrix->DeepCopy(modelToPhantomOriginTransformVector);

				if (m_ModelToPhantomOriginTransform != NULL) {
					m_ModelToPhantomOriginTransform->Delete();
				}
				m_ModelToPhantomOriginTransform = vtkTransform::New();
				m_ModelToPhantomOriginTransform->SetMatrix(modelToPhantomOriginTransformMatrix);

				if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
					m_PhantomBodyActor->SetUserTransform(m_ModelToPhantomOriginTransform);
				}
			}
			delete[] modelToPhantomOriginTransformVector;

			if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
				m_PhantomRenderer->ResetCamera();
			}
		}
	}

	// Initialize geometry objects
	m_LandmarkNames.clear();

	if (m_DefinedLandmarks != NULL) {
		m_DefinedLandmarks->Delete();
	}
	m_DefinedLandmarks = vtkPoints::New();
	m_DefinedLandmarks->Initialize();

	m_LandmarksPolyData->GetPoints()->Initialize();
	m_LandmarksPolyData->Modified();

	// Load geometry
	vtkXMLDataElement* geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
	if (geometry == NULL) {
		LOG_ERROR("Phantom geometry information not found!");
		if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
			m_PhantomRenderer->RemoveActor(m_PhantomBodyActor);
		}
		return PLUS_FAIL;
	} else {
		// Read landmarks (NWires are not interesting at this point, it is only parsed if segmentation is needed)
		vtkSmartPointer<vtkXMLDataElement> landmarks = geometry->FindNestedElementWithName("Landmarks"); 
		if (landmarks == NULL) {
			LOG_ERROR("Landmarks not found, registration is not possible!");
			return PLUS_FAIL;
		} else {
			int numberOfLandmarks = landmarks->GetNumberOfNestedElements();
			m_LandmarkNames.resize(numberOfLandmarks);

			for (int i=0; i<numberOfLandmarks; ++i) {
				vtkSmartPointer<vtkXMLDataElement> landmark = landmarks->GetNestedElement(i);

				if ((landmark == NULL) || (STRCASECMP("Landmark", landmark->GetName()))) {
					LOG_WARNING("Invalid landmark definition found!");
					continue;
				}

				const char* landmarkName = landmark->GetAttribute("Name");
				std::string landmarkNameString(landmarkName);
				double landmarkPosition[3];

				if (! landmark->GetVectorAttribute("Position", 3, landmarkPosition)) {
					LOG_WARNING("Invalid landmark position!");
					continue;
				}

				m_DefinedLandmarks->InsertPoint(i, landmarkPosition);
				m_LandmarkNames[i] = landmarkNameString;
			}

			if (m_DefinedLandmarks->GetNumberOfPoints() != numberOfLandmarks) {
				LOG_WARNING("Some invalid landmarks were found!");
			}
		}

		if (m_DefinedLandmarks->GetNumberOfPoints() == 0) {
			LOG_ERROR("No valid landmarks were found!");
			return PLUS_FAIL;
		}

		// Highlight first landmark
		m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_DefinedLandmarks->GetPoint(0));
		m_RequestedLandmarkPolyData->GetPoints()->Modified();
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationController::LoadPhantomRegistrationFromFile(std::string aFile)
{
	vtkSmartPointer<vtkXMLDataElement> phantomRegistration = vtkXMLUtilities::ReadElementFromFile(aFile.c_str());

	if (phantomRegistration == NULL) {	
		LOG_ERROR("Unable to read the phantom registration file: " << aFile); 
		return PLUS_FAIL;
	}

	// Load stylus calibration transform
	vtkXMLDataElement* phantomRegistrationTransform = phantomRegistration->FindNestedElementWithName("PhantomToPhantomReferenceTransform"); 
	if (phantomRegistrationTransform == NULL) {
		LOG_ERROR("Phantom registration transform not found!");
		return PLUS_FAIL;
	} else {
		double* transform = new double[16]; 
		if (phantomRegistrationTransform->GetVectorAttribute("Transform", 16, transform)) {
			if (m_PhantomToPhantomReferenceTransform == NULL) {
				m_PhantomToPhantomReferenceTransform = vtkTransform::New();
			}

			m_PhantomToPhantomReferenceTransform->SetMatrix(transform);
		}
		delete[] transform; 
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void PhantomRegistrationController::SavePhantomRegistrationToFile(std::string aFile)
{
	vtkSmartPointer<vtkXMLDataElement> phantomRegistration = vtkSmartPointer<vtkXMLDataElement>::New();
	phantomRegistration->SetName("PhantomRegistration");
	phantomRegistration->SetAttribute("version", "1.0");

	vtkSmartPointer<vtkXMLDataElement> phantomRegistrationTransform = vtkSmartPointer<vtkXMLDataElement>::New();

	char phantomRegistrationChars[256];
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = m_PhantomToPhantomReferenceTransform->GetMatrix();
	sprintf_s(phantomRegistrationChars, 256, "\n\t%.4lf %.4lf %.4lf %.4lf\n\t%.4lf %.4lf %.4lf %.4lf\n\t%.4lf %.4lf %.4lf %.1lf\n\t0 0 0 1", 
		transformMatrix->GetElement(0,0), transformMatrix->GetElement(0,1), transformMatrix->GetElement(0,2), transformMatrix->GetElement(0,3), 
		transformMatrix->GetElement(1,0), transformMatrix->GetElement(1,1), transformMatrix->GetElement(1,2), transformMatrix->GetElement(1,3), 
		transformMatrix->GetElement(2,0), transformMatrix->GetElement(2,1), transformMatrix->GetElement(2,2), transformMatrix->GetElement(2,3));

	phantomRegistrationTransform->SetName("PhantomToPhantomReferenceTransform");

	phantomRegistrationTransform->SetAttribute("Transform", phantomRegistrationChars);

	phantomRegistration->AddNestedElement(phantomRegistrationTransform);

	phantomRegistration->PrintXML(aFile.c_str());
}

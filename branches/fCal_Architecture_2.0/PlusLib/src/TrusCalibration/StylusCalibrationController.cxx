#include "PlusConfigure.h"

#include "StylusCalibrationAlgo.h"
#include "vtkFreehandController.h"

#include "vtkTrackerTool.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
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
#include "vtkSTLReader.h"

//-----------------------------------------------------------------------------

StylusCalibrationAlgo *StylusCalibrationAlgo::m_Instance = NULL;

//-----------------------------------------------------------------------------

StylusCalibrationAlgo* StylusCalibrationAlgo::GetInstance() { // macro-val TODO
	if (! m_Instance) {
		m_Instance = new StylusCalibrationAlgo();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

StylusCalibrationAlgo::StylusCalibrationAlgo()
	:AbstractToolboxController()
	,m_NumberOfPoints(200)
	,m_CurrentPointNumber(0)
	,m_StylusPortNumber(-1)
	,m_StylustipToStylusTransform(NULL)
	,m_CalibrationError(-1.0)
	,m_InputPolyData(NULL)
	,m_StylusTipPolyData(NULL)
	,m_PositionString("")
{
	m_BoundingBox[0] = m_BoundingBox[1] = m_BoundingBox[2] = m_BoundingBox[3] = m_BoundingBox[4] = m_BoundingBox[5] = 0.0;

	// Determine stylus tool port number
  if (DetermineStylusPortNumber() != PLUS_SUCCESS) {
    LOG_ERROR("Determining stylus port number failed!");
    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------

StylusCalibrationAlgo::~StylusCalibrationAlgo()
{
	if (m_StylustipToStylusTransform != NULL) {
		m_StylustipToStylusTransform->Delete();
		m_StylustipToStylusTransform = NULL;
	}

	if (m_InputPolyData != NULL) {
		m_InputPolyData->Delete();
		m_InputPolyData = NULL;
	}

	if (m_StylusTipPolyData != NULL) {
		m_StylusTipPolyData->Delete();
		m_StylusTipPolyData = NULL;
	}
}

//-----------------------------------------------------------------------------
/*
PlusStatus StylusCalibrationAlgo::DetermineStylusPortNumber()
{
	LOG_TRACE("StylusCalibrationAlgo::DetermineStylusPortNumber");

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
	int toolNumber = dataCollector->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_STYLUS);
	if (toolNumber != -1) {
		m_StylusPortNumber = toolNumber;
	} else {
		LOG_ERROR("Stylus port number not found in configuration file!");
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::InitializeVisualization()
{
	LOG_TRACE("StylusCalibrationAlgo::InitializeVisualization");

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

			// Load stylus model
			m_StylusActor = vtkActor::New();

      if (LoadStylusModel() != PLUS_SUCCESS) {
        LOG_ERROR("Loading stylus model failed!");
        return PLUS_FAIL;
      }

			// Axes actor
			//vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
			//axesActor->SetShaftTypeToCylinder();
			//axesActor->SetXAxisLabelText("X");
			//axesActor->SetYAxisLabelText("Y");
			//axesActor->SetZAxisLabelText("Z");
			//axesActor->SetAxisLabels(0);
			//axesActor->SetTotalLength(50, 50, 50);
			//renderer->AddActor(axesActor);

			// Add actors
			renderer->AddActor(m_InputActor);
      renderer->SetGradientBackground(true);

      m_InputActor->VisibilityOff();
		}
	} else if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {  // If already initialized (it can occur if tab change - and so clear - happened)
		// Add all actors to the renderer again - state must be "Done", because tab cannot be changed if "In progress"
		vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
		renderer->AddActor(m_InputActor);
    m_InputActor->VisibilityOff();
		renderer->AddActor(m_StylusActor);
		renderer->AddActor(m_StylusTipActor);
    renderer->SetGradientBackground(true);
		renderer->Modified();
	}

  return PLUS_SUCCESS;
}
*/
//-----------------------------------------------------------------------------

int StylusCalibrationAlgo::GetNumberOfPoints() {
	LOG_TRACE("StylusCalibrationAlgo::GetNumberOfPoints");

	return m_NumberOfPoints;
}

//-----------------------------------------------------------------------------

void StylusCalibrationAlgo::SetNumberOfPoints(int aNumOfPoints) {
	LOG_TRACE("StylusCalibrationAlgo::SetNumberOfPoints(" << aNumOfPoints << ")");

	m_NumberOfPoints = aNumOfPoints;
}

//-----------------------------------------------------------------------------

int StylusCalibrationAlgo::GetCurrentPointNumber() {
	LOG_TRACE("StylusCalibrationAlgo::GetCurrentPointNumber");

	return m_CurrentPointNumber;
}


//-----------------------------------------------------------------------------

double StylusCalibrationAlgo::GetPrecision() {
	LOG_TRACE("StylusCalibrationAlgo::GetPrecision");

	return m_CalibrationError;
}

//-----------------------------------------------------------------------------

std::string StylusCalibrationAlgo::GetBoundingBoxString()
{
	//LOG_TRACE("StylusCalibrationAlgo::GetBoundingBoxString");

	char boundingBoxChars[32];
	sprintf_s(boundingBoxChars, 32, "%.1lf X %.1lf X %.1lf", m_BoundingBox[1]-m_BoundingBox[0], m_BoundingBox[3]-m_BoundingBox[2], m_BoundingBox[5]-m_BoundingBox[4]);
	std::string boundingBoxString(boundingBoxChars);

	return boundingBoxString;
}

//-----------------------------------------------------------------------------

vtkTransform* StylusCalibrationAlgo::GetStylustipToStylusTransform()
{
	LOG_TRACE("StylusCalibrationAlgo::GetStylustipToStylusTransform");

	return m_StylustipToStylusTransform;
}

//-----------------------------------------------------------------------------

std::string StylusCalibrationAlgo::GetStylustipToStylusTransformString()
{
	//LOG_TRACE("StylusCalibrationAlgo::GetStylustipToStylusTransformString");

	vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
	m_StylustipToStylusTransform->GetMatrix(transform);

	char stylustipToStylusTransformChars[32];
	sprintf_s(stylustipToStylusTransformChars, 32, "%.2lf X %.2lf X %.2lf", transform->GetElement(0,3), transform->GetElement(1,3), transform->GetElement(2,3));
	std::string stylustipToStylusTransformString(stylustipToStylusTransformChars);

	return stylustipToStylusTransformString;
}

//-----------------------------------------------------------------------------

std::string StylusCalibrationAlgo::GetPositionString()
{
	//LOG_TRACE("StylusCalibrationAlgo::GetPositionString");

	return m_PositionString;
}

//-----------------------------------------------------------------------------
/*
void StylusCalibrationAlgo::DisplayStylus()
{
	LOG_TRACE("StylusCalibrationAlgo::DisplayStylus");

	vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is not initialized properly!");
		return;
	}
	vtkTrackerTool *tool = dataCollector->GetTracker()->GetTool(m_StylusPortNumber);
	if ((tool == NULL) || (!tool->GetEnabled())) {
		return;
	}

	double stylusPosition[4];
	vtkSmartPointer<vtkMatrix4x4> stylusToReferenceTransformMatrix = NULL;

	if (stylusToReferenceTransformMatrix = AcquireStylusTrackerPosition(stylusPosition)) {
		m_StylusActor->GetProperty()->SetOpacity(1.0);
		m_StylusActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

		vtkSmartPointer<vtkTransform> referenceToolToStylusTipTransform = vtkSmartPointer<vtkTransform>::New();
		referenceToolToStylusTipTransform->Identity();
		referenceToolToStylusTipTransform->Concatenate(stylusToReferenceTransformMatrix);
		referenceToolToStylusTipTransform->Concatenate(m_StylustipToStylusTransform);
		referenceToolToStylusTipTransform->Concatenate(tool->GetModelToToolTransform());
		referenceToolToStylusTipTransform->Modified();

		m_StylusActor->SetUserTransform(referenceToolToStylusTipTransform);
	} else {
		m_StylusActor->GetProperty()->SetOpacity(0.3);
		m_StylusActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
	}
}
*/
//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::LoadStylusCalibrationFromFile(std::string aFile)
{
	LOG_TRACE("StylusCalibrationAlgo::LoadStylusCalibrationFromFile(" << aFile << ")");

	vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(aFile.c_str());
	if (rootElement == NULL) {	
		LOG_ERROR("Unable to read the configuration file: " << aFile); 
		return PLUS_FAIL;
	}

	return LoadStylusCalibration(rootElement);
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::LoadStylusCalibration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("StylusCalibrationAlgo::LoadStylusCalibration");

	// Find stylus definition element
	char* toolType = NULL;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_STYLUS, toolType);
  vtkSmartPointer<vtkXMLDataElement> stylusDefinition = vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType);
	if (stylusDefinition == NULL) {
		LOG_ERROR("No stylus definition is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = stylusDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in stylus definition!");
		return PLUS_FAIL;
	}

  // Check date - if it is empty, the calibration is considered as invalid (as the calibration transforms in the installed config files are identity matrices with empty dates)
  const char* date = calibration->GetAttribute("Date");
  if ((date == NULL) || (STRCASECMP(date, "") == 0)) {
    LOG_WARNING("Transform cannot be loaded with no date entered!");
    return PLUS_FAIL;
  }

	// Get transform
	double* stylustipToStylusTransformVector = new double[16]; 
	if (calibration->GetVectorAttribute("MatrixValue", 16, stylustipToStylusTransformVector)) {
		// Create matrix and set it to controller member variable
		vtkSmartPointer<vtkMatrix4x4> stylustipToStylusTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		stylustipToStylusTransformMatrix->Identity();
		stylustipToStylusTransformMatrix->DeepCopy(stylustipToStylusTransformVector);

		if (m_StylustipToStylusTransform == NULL) {
			m_StylustipToStylusTransform = vtkTransform::New();
		}
		m_StylustipToStylusTransform->Identity();
		m_StylustipToStylusTransform->SetMatrix(stylustipToStylusTransformMatrix);

		// Set calibration matrix to stylus tool
		vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
		if (dataCollector != NULL) {
			// If one jumped to Phantom registration toolbox right away
			if (m_State == ToolboxState_Uninitialized) {
        if (DetermineStylusPortNumber() != PLUS_SUCCESS) {
          LOG_ERROR("Unable to determine stylus port number!");
          return PLUS_FAIL;
        }
			}

			dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->SetCalibrationMatrix(stylustipToStylusTransformMatrix);

		} else {
			LOG_WARNING("Data collector is not initialized!");
		}
	}
	delete[] stylustipToStylusTransformVector; 

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::SaveStylusCalibrationToFile(std::string aFile)
{
	LOG_TRACE("StylusCalibrationAlgo::SaveStylusCalibrationToFile(" << aFile << ")");

	vtkSmartPointer<vtkXMLDataElement> rootElement = NULL;
	if ((rootElement = vtkFreehandController::ParseXMLOrFillWithInternalData(aFile.c_str())) == NULL) {
		LOG_ERROR("Neither input file not internal configuration data is valid!");
		return PLUS_FAIL;
	}

	if (SaveStylusCalibration(rootElement) != PLUS_SUCCESS) {
		LOG_ERROR("Stylus calibration result could not be saved!");
		return PLUS_FAIL;
	}

	rootElement->PrintXML(aFile.c_str());

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::SaveStylusCalibration(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("StylusCalibrationAlgo::SaveStylusCalibration");

	// Find stylus definition element
	char* toolType = NULL;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_STYLUS, toolType);
	vtkSmartPointer<vtkXMLDataElement> stylusDefinition = vtkFreehandController::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType);
	if (stylusDefinition == NULL) {
		LOG_ERROR("No stylus definition is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = stylusDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in stylus definition!");
		return PLUS_FAIL;
	}

	// Assemble and save transform
	char stylustipToStylusTransformChars[256];
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = m_StylustipToStylusTransform->GetMatrix();
	sprintf_s(stylustipToStylusTransformChars, 256, "\n\t1 0 0 %.4lf\n\t0 1 0 %.4lf\n\t0 0 1 %.4lf\n\t0 0 0 1", transformMatrix->GetElement(0,3), transformMatrix->GetElement(1,3), transformMatrix->GetElement(2,3));

	calibration->SetAttribute("MatrixValue", stylustipToStylusTransformChars);

	// Save matrix name, date and error
	calibration->SetAttribute("MatrixName", "StylusTipToStylus");
	calibration->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());
	calibration->SetDoubleAttribute("Error", m_CalibrationError);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::CalibrateStylus()
{
	LOG_TRACE("StylusCalibrationAlgo::CalibrateStylus");

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
	m_CalibrationError = dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->DoToolTipCalibration();

	// Get the calibration result
	if (m_StylustipToStylusTransform == NULL) {
		m_StylustipToStylusTransform = vtkTransform::New();
	}
	m_StylustipToStylusTransform->Identity();
	vtkSmartPointer<vtkMatrix4x4> transform = dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->GetCalibrationMatrix();
	m_StylustipToStylusTransform->SetMatrix(transform);

	// Display calibration result stylus tip (by acquiring a new point and using it) //TODO do it an other way, without getting a new point
	if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
		double stylusTipPosition[4];
		vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTransformMatrix = NULL;

		if (referenceToolToStylusTransformMatrix = AcquireStylusTrackerPosition(stylusTipPosition)) {
			// Apply calibration
			vtkSmartPointer<vtkMatrix4x4> referenceToolToStylusTipTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			referenceToolToStylusTipTransformMatrix->Identity();
			vtkMatrix4x4::Multiply4x4(referenceToolToStylusTransformMatrix, m_StylustipToStylusTransform->GetMatrix(), referenceToolToStylusTipTransformMatrix);

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

  // Save result to session configuration
	if (SaveStylusCalibration(vtkFreehandController::GetInstance()->GetConfigurationData()) != PLUS_SUCCESS) {
		LOG_ERROR("Stylus calibration result could not be saved into session configuration data!");
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationAlgo::FeedStylusCalibrationMatrixToTool()
{
	LOG_TRACE("StylusCalibrationAlgo::FeedStylusCalibrationMatrixToTool");

	vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetTool(m_StylusPortNumber) < 0)) {
		LOG_ERROR("Tracker is not initialized properly!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkMatrix4x4> stylusToStylusTipTransformMatrix = dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->GetCalibrationMatrix();
	// If translation values are 0 (no calibration)
	if ((stylusToStylusTipTransformMatrix == NULL) || ((stylusToStylusTipTransformMatrix->GetElement(0,3) == 0.0) && (stylusToStylusTipTransformMatrix->GetElement(1,3) == 0.0) && (stylusToStylusTipTransformMatrix->GetElement(2,3) == 0.0)) ) {
		dataCollector->GetTracker()->GetTool(m_StylusPortNumber)->SetCalibrationMatrix(m_StylustipToStylusTransform->GetMatrix());		
	}

	return PLUS_SUCCESS;
}

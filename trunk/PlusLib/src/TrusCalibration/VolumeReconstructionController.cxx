#include "VolumeReconstructionController.h"

#include "vtkFreehandController.h"
#include "PlusConfigure.h"

//-----------------------------------------------------------------------------

VolumeReconstructionController *VolumeReconstructionController::m_Instance = NULL;

//-----------------------------------------------------------------------------

VolumeReconstructionController* VolumeReconstructionController::GetInstance() {
	if (! m_Instance) {
		m_Instance = new VolumeReconstructionController();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

VolumeReconstructionController::VolumeReconstructionController()
	:AbstractToolboxController()
{
}

//-----------------------------------------------------------------------------

VolumeReconstructionController::~VolumeReconstructionController()
{
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::Initialize()
{
	LOG_DEBUG("Initialize VolumeReconstructionController");
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::Clear()
{
	LOG_DEBUG("Clear VolumeReconstructionController");
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::DoAcquisition()
{
/* // a buggy solution for getting all data since last acquisition
void StylusCalibrationController::DoAcquisition()
{
	LOG_DEBUG("Acquire positions for stylus calibration"); 

	vtkFreehandController* controller = vtkFreehandController::GetInstance();

	if ((controller != NULL) && (controller->GetInitialized() == true)) {
		vtkDataCollector* dataCollector = controller->GetDataCollector();
		if (dataCollector == NULL) {
			LOG_ERROR("Data collector is not initialized!");
			return;
		}

		if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetTool(controller->GetMainToolNumber()) < 0)) {
			LOG_ERROR("Tracker is not initialized properly!");
			return;
		}

		// Get tracker names
		std::vector<std::string> toolNames;
		for (int tool = 0; tool < dataCollector->GetTracker()->GetNumberOfTools(); ++tool) {
			toolNames.push_back(dataCollector->GetTracker()->GetTool(tool)->GetToolName());
		}

		// Get transforms since last acquisition
		std::vector<std::vector<vtkMatrix4x4*>> toolTransformMatrixVectors;
		std::vector<std::vector<long>> flagsVectors;
		for (int i=0; i<dataCollector->GetTracker()->GetNumberOfTools(); ++i) {
			std::vector<vtkMatrix4x4*> toolTransformMatrixVector;
			std::vector<long> flagsVector;
			toolTransformMatrixVectors.push_back(toolTransformMatrixVector);
			flagsVectors.push_back(flagsVector);
		}

		unsigned int mainToolNumber = controller->GetMainToolNumber();
		unsigned int numberOfNewPositions = 0;

		for (int tool = 0; tool < dataCollector->GetTracker()->GetNumberOfTools(); ++tool) {
			double timestamp = dataCollector->GetTransformsByTimeInterval(toolTransformMatrixVectors[tool], flagsVectors[tool], m_LastTimestamp, -1, tool);

			if (tool == mainToolNumber) {
				m_LastTimestamp = timestamp;
				numberOfNewPositions = toolTransformMatrixVectors[tool].size();
			}
		}

		for (unsigned int i=0; i<numberOfNewPositions; ++i) {
			std::vector<vtkMatrix4x4*> transformVector;
			std::vector<long> flagsVector;

			for (int tool = 0; tool < dataCollector->GetTracker()->GetNumberOfTools(); ++tool) {
				vtkMatrix4x4* transform = toolTransformMatrixVectors[tool][i];
				long flags = flagsVectors[tool][i];

				transformVector.push_back(transform);
				flagsVector.push_back(flags);
			}

			if (flagsVector.size() > mainToolNumber && flagsVector[mainToolNumber] & (TR_MISSING | TR_OUT_OF_VIEW) ) {
				LOG_WARNING("Tracker out of view!");
			} else if (flagsVector.size() > mainToolNumber && flagsVector[mainToolNumber] & (TR_REQ_TIMEOUT) ) {
				LOG_WARNING("Tracker request timeout!"); 
			} else { // If no problem
				double timestamp = controller->GetNumberOfRecordedFrames(); // Just to distinguish the data (so that the IsDataUnique return false), the timestamp has no special meaning in this case 
				controller->AddTrackedFrame(NULL, transformVector, toolNames, flagsVector, timestamp);
			}

			// Refresh bounding box
			double elements[16];
			for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*i+j] = transformVector[mainToolNumber]->GetElement(i,j);
			double point[4] = {1.0, 0.0, 0.0, 1.0};
			double newPoint[4];
			
			vtkMatrix4x4::PointMultiply(elements, point, newPoint);

			if (m_BoundingBox[0] > newPoint[0]) { m_BoundingBox[0] = newPoint[0]; }
			if (m_BoundingBox[1] > newPoint[1]) { m_BoundingBox[1] = newPoint[1]; }
			if (m_BoundingBox[2] > newPoint[2]) { m_BoundingBox[2] = newPoint[2]; }
			if (m_BoundingBox[3] < newPoint[0]) { m_BoundingBox[3] = newPoint[0]; }
			if (m_BoundingBox[4] < newPoint[1]) { m_BoundingBox[4] = newPoint[1]; }
			if (m_BoundingBox[5] < newPoint[2]) { m_BoundingBox[5] = newPoint[2]; }
		}

		if (controller->GetNumberOfRecordedFrames() >= m_NumberOfPoints) {
			m_Toolbox->Stop();
		}
	}
}
*/
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::Start()
{
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::Stop()
{
}

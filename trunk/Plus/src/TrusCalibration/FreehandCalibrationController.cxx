#include "FreehandCalibrationController.h"

#include "vtkFreehandController.h"
#include "PlusConfigure.h"

//-----------------------------------------------------------------------------

FreehandCalibrationController *FreehandCalibrationController::m_Instance = NULL;

//-----------------------------------------------------------------------------

FreehandCalibrationController* FreehandCalibrationController::GetInstance() {
	if (! m_Instance) {
		m_Instance = new FreehandCalibrationController();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

FreehandCalibrationController::FreehandCalibrationController()
	:AbstractToolboxController()
{
}

//-----------------------------------------------------------------------------

FreehandCalibrationController::~FreehandCalibrationController()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Initialize()
{
	LOG_DEBUG("Initialize FreehandCalibrationController");
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Clear()
{
	LOG_DEBUG("Clear FreehandCalibrationController");
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::DoAcquisition()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Start()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Stop()
{
}


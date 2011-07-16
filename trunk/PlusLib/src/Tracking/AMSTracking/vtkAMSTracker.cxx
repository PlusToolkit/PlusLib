#include "PlusConfigure.h"
#include "AMSStepper.h"
#include "vtkAMSTracker.h"
#include <sstream>
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkAccurateTimer.h"
#include "vtkXMLDataElement.h"


//----------------------------------------------------------------------------
vtkAMSTracker* vtkAMSTracker::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkAMSTracker");
	if(ret)
	{
		return (vtkAMSTracker*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkAMSTracker;
}

//----------------------------------------------------------------------------
vtkAMSTracker::vtkAMSTracker()
{
	this->Device = 0;
	this->Version = NULL;
	this->ModelNumber = 0; 
	this->SerialNumber = 0; 
	this->Tracking = 0;
	this->SerialPort = NULL;
	this->SetSerialPort("COM1");
	this->BaudRate = 19200;
	this->SetNumberOfTools(NUMBER_OF_BRACHY_TOOLS); 
	this->SetToolName(PROBEHOME_TO_PROBE_TRANSFORM, "ProbeHomeToProbeUncalibrated"); 
	this->SetToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, "TemplateHomeToTemplateUncalibrated"); 
	this->SetToolName(RAW_ENCODER_VALUES, "StepperEncoderValues"); 

	this->Frequency = 50; 

	// Stepper calibration parameters
	this->SetProbeTranslationAxisOrientation(0,0,1); 
	this->SetTemplateTranslationAxisOrientation(0,0,1); 
	this->SetProbeRotationAxisOrientation(0,0,1); 
	this->SetProbeRotationEncoderScale(1.0); 
}

//----------------------------------------------------------------------------
vtkAMSTracker::~vtkAMSTracker() 
{
	if (this->Tracking)
	{
		this->StopTracking();
	}

	if (this->Version)
	{
		delete [] this->Version;
	}
}

//----------------------------------------------------------------------------
void vtkAMSTracker::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkTracker::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::Connect()
{
	if (this->Device == NULL)
	{
		this->Device = new AMSStepper(this->GetSerialPort(), this->GetBaudRate() ) ; 
	}

	return this->Device->StartTracking(); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::Disconnect()
{
	this->Device->StopTracking(); 
	return this->StopTracking(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::Probe()
{
	if (this->Tracking)
	{
		return PLUS_SUCCESS;
	}

	if ( !this->Connect() )
	{
		LOG_ERROR("Unable to connect to stepper on port: " << this->GetSerialPort() );
		return PLUS_FAIL; 
	}

	this->Disconnect(); 

	return PLUS_SUCCESS; 
} 

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::InternalStartTracking()
{
	if (this->Tracking)
	{
		return PLUS_SUCCESS;
	}

	if (!this->InitAMSTracker())
	{
		LOG_ERROR("Couldn't initialize AMS stepper.");
		this->Tracking = 0;
		return PLUS_FAIL;
	} 

  this->Tracking = 1;

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::InternalStopTracking()
{
	delete this->Device; 
	this->Device = 0;

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::InternalUpdate()
{
	TrackerStatus status = TR_OK;

	if (!this->Tracking)
	{
		LOG_ERROR("called Update() when AMS stepper was not tracking");
		return PLUS_FAIL;
	}

	// get the transforms from stepper
	double dProbePosition(0), dTemplatePosition(0), dProbeRotation(0); 
	unsigned long frameNum(0); 
	if (!this->Device->GetProbePositions(dProbePosition, dTemplatePosition, dProbeRotation, frameNum)) 
	{
		LOG_DEBUG("Tracker request timeout..."); 
		// Unable to get tracking information from tracker
		status = TR_REQ_TIMEOUT; 
	}

	// Save probe position to the matrix (0,3) element
	// Save probe rotation to the matrix (1,3) element
	// Save grid position to the matrix (2,3) element
	vtkSmartPointer<vtkMatrix4x4> probePosition = vtkSmartPointer<vtkMatrix4x4>::New(); 
	probePosition->SetElement(ROW_PROBE_POSITION, 3, dProbePosition); 
	probePosition->SetElement(ROW_PROBE_ROTATION, 3, dProbeRotation); 
	probePosition->SetElement(ROW_TEMPLATE_POSITION, 3, dTemplatePosition); 
	// send the transformation matrix and status to the tool
	this->ToolUpdate(RAW_ENCODER_VALUES, probePosition, status, frameNum);   

	// Save template home to template transform
	vtkSmartPointer<vtkTransform> tTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	double templateTranslationAxisVector[3]; 
	this->GetTemplateTranslationAxisOrientation(templateTranslationAxisVector); 
	vtkMath::MultiplyScalar(templateTranslationAxisVector, dTemplatePosition); 
	tTemplateHomeToTemplate->Translate(templateTranslationAxisVector); 
	// send the transformation matrix and status to the tool
	this->ToolUpdate(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, tTemplateHomeToTemplate->GetMatrix(), status, frameNum);   

	// Save probehome to probe transform
	vtkSmartPointer<vtkTransform> tProbeHomeToProbe = vtkSmartPointer<vtkTransform>::New(); 
	// Translate the probe to the desired position
	double probeTranslationVector[3]; 
	this->GetProbeTranslationAxisOrientation(probeTranslationVector); 
	vtkMath::MultiplyScalar(probeTranslationVector, dProbePosition); 
	tProbeHomeToProbe->Translate(probeTranslationVector);

	// Translate the probe to the compensated rotation axis before the rotation 
	double probeRotationVector[3]; 
	this->GetProbeRotationAxisOrientation(probeRotationVector); 
	vtkMath::MultiplyScalar(probeRotationVector, dProbePosition); 
	tProbeHomeToProbe->Translate(probeRotationVector);
	const double compensatedProbeRotation = this->ProbeRotationEncoderScale * dProbeRotation; 
	tProbeHomeToProbe->RotateZ(compensatedProbeRotation);
	// Translate back the probe to the original position
	tProbeHomeToProbe->Translate(-probeRotationVector[0], -probeRotationVector[1], probeRotationVector[2]); 
	// send the transformation matrix and status to the tool
	this->ToolUpdate(PROBEHOME_TO_PROBE_TRANSFORM, tProbeHomeToProbe->GetMatrix(), status, frameNum);   

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::InitAMSTracker()
{
	// Connect to device 
	if ( !this->Connect() )
	{
		LOG_ERROR("Unable to connect to stepper on port: " << this->GetSerialPort() );
		return PLUS_FAIL; 
	}

	int iVerHi=0; int iVerLo=0; int iModelNum=0; int iSerialNum=0;
	if (!this->Device->GetVersionInfo(iVerHi, iVerLo, iModelNum, iSerialNum))
	{
		LOG_ERROR("Couldn't get version info from AMS stepper.");
		return PLUS_FAIL; 
	}

	for ( int tool = 0; tool < NUMBER_OF_BRACHY_TOOLS; tool++ )
	{
		this->GetTool(tool)->EnabledOn(); 
	}

	std::ostringstream version; 
	version << iVerHi << "." << iVerLo; 

	this->SetVersion(version.str().c_str());

	this->SetModelNumber(iModelNum); 
	this->SetSerialNumber(iSerialNum); 

	if ( this->TrackerCalibrated )
	{
		this->SetToolName(PROBEHOME_TO_PROBE_TRANSFORM, "ProbeHomeToProbe"); 
		this->SetToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, "TemplateHomeToTemplate"); 
	}

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::ReadConfiguration(vtkXMLDataElement* config)
{
	// Read superclass configuration first
	Superclass::ReadConfiguration(config); 

	if ( config == NULL ) 
	{
		LOG_WARNING("Unable to find AMSTracker XML data element");
		return PLUS_FAIL; 
	}

	const char* serialPort = config->GetAttribute("SerialPort"); 
	if ( serialPort != NULL ) 
	{
		if ( !this->Tracking )
		{
			this->SetSerialPort(serialPort); 
		}
	}

	unsigned long baudRate = 0; 
	if ( config->GetScalarAttribute("BaudRate", baudRate) ) 
	{
		if ( !this->Tracking )
		{
			this->SetBaudRate(baudRate); 
		}
	}

	if ( !this->Tracking )
	{
		vtkXMLDataElement* modelInformation = config->FindNestedElementWithName("ModelInformation"); 
		if ( modelInformation != NULL ) 
		{
			const char* modelName = modelInformation->GetAttribute("Name"); 
			if ( modelName != NULL )
			{
				if (this->Device == NULL)
				{
					this->Device = new AMSStepper(this->GetSerialPort(), this->GetBaudRate() ) ; 
				}

				if ( STRCASECMP("Burdette Medical Systems Digital Stepper", modelName) == 0 )
				{
					this->Device->SetBrachyStepperType(AMSStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER); 
				}
				else if ( STRCASECMP("Burdette Medical Systems Digital Motorized Stepper", modelName) == 0 )
				{
					this->Device->SetBrachyStepperType(AMSStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER); 
				}
				else if ( STRCASECMP("CMS Accuseed DS300", modelName) == 0 )
				{
					this->Device->SetBrachyStepperType(AMSStepper::CMS_ACCUSEED_DS300); 
				}
				else
				{
					LOG_WARNING("Unable to recognize brachy stepper name: " << modelName << "(Use default: Burdette Medical Systems Digital Stepper)");
					this->Device->SetBrachyStepperType(AMSStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER); 
				}

			}

			double modelNumber = 0; 
			if ( modelInformation->GetScalarAttribute("Number", modelNumber) ) 
			{

			}

			double modelVersion = 0; 
			if ( modelInformation->GetScalarAttribute("Version", modelVersion) ) 
			{

			}

			double modelSerialNumber = 0; 
			if ( modelInformation->GetScalarAttribute("SerialNumber", modelSerialNumber) ) 
			{

			}
		}
	}

	vtkXMLDataElement* calibration = config->FindNestedElementWithName("Calibration"); 
	if ( calibration != NULL ) 
	{
		const char* calibrationAlgorithmVersion = calibration->GetAttribute("AlgorithmVersion"); 
		if ( calibrationAlgorithmVersion != NULL )
		{

		}

		const char* calibrationDate = calibration->GetAttribute("Date"); 
		if ( calibrationDate != NULL )
		{

		}

		double probeTranslationAxisOrientation[3] = {0,0,1}; 
		if ( calibration->GetVectorAttribute("ProbeTranslationAxisOrientation", 3, probeTranslationAxisOrientation) ) 
		{
			this->SetProbeTranslationAxisOrientation(probeTranslationAxisOrientation); 
		}

		double templateTranslationAxisOrientation[3] = {0,0,1}; 
		if ( calibration->GetVectorAttribute("TemplateTranslationAxisOrientation", 3, templateTranslationAxisOrientation) ) 
		{
			this->SetTemplateTranslationAxisOrientation(templateTranslationAxisOrientation); 
		}

		double probeRotationAxisOrientation[3] = {0,0,1}; 
		if ( calibration->GetVectorAttribute("ProbeRotationAxisOrientation", 3, probeRotationAxisOrientation) ) 
		{
			this->SetProbeRotationAxisOrientation(probeRotationAxisOrientation); 
		}
		
		double probeRotationEncoderScale = 1; 
		if ( calibration->GetScalarAttribute("ProbeRotationEncoderScale", probeRotationEncoderScale) ) 
		{
			this->ProbeRotationEncoderScale = probeRotationEncoderScale; 
		}

		this->TrackerCalibratedOn(); 
	}
	else
	{
		LOG_WARNING("Unable to find the stepper calibration result in AMSTracker XML data element"); 
	}

	if ( this->TrackerCalibrated )
	{
		LOG_INFO("AMSTracker is calibrated"); 
	}
	else
	{
		LOG_INFO("AMSTracker is uncalibrated"); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::WriteConfiguration(vtkXMLDataElement* config)
{
	if ( config == NULL )
	{
    LOG_ERROR("Config is invalid");
		return PLUS_FAIL;
	}

	config->SetName("BrachyStepper");  
	config->SetAttribute( "SerialPort", this->GetSerialPort() ); 
	config->SetDoubleAttribute( "BaudRate", this->GetBaudRate() ); 
	config->SetAttribute( "VersionNumber", this->GetVersion() ); 
	config->SetIntAttribute( "ModelNumber", this->GetModelNumber() ); 
	config->SetIntAttribute( "SerialNumber", this->GetSerialNumber() ); 

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
void vtkAMSTracker::ResetCalibration()
{
	this->Device->ResetStepper(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::CalibrateStepper( std::string &calibMsg )
{
	return this->Device->CalibrateStepper(calibMsg); 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetTrackerToolBufferStringList(double timestamp, 
												   std::map<std::string, std::string> &toolsBufferMatrices, 
												   std::map<std::string, std::string> &toolsCalibrationMatrices, 
												   std::map<std::string, std::string> &toolsStatuses,
												   bool calibratedTransform /*= false*/)
{
	toolsBufferMatrices.clear(); 
	toolsCalibrationMatrices.clear(); 
	toolsStatuses.clear(); 

	// PROBEHOME_TO_PROBE_TRANSFORM
	TrackerStatus probehome2probeStatus = TR_OK; 
  vtkSmartPointer<vtkMatrix4x4> probehome2probeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( this->GetProbeHomeToProbeTransform(timestamp, probehome2probeMatrix, probehome2probeStatus, calibratedTransform) != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to get probe home to probe transform from buffer!"); 
    return PLUS_FAIL; 
  }

	std::ostringstream strProbeHomeToProbeTransform; 
  for ( int r = 0; r < 4; ++r )
  {
    for ( int c = 0; c < 4; ++c )
    {
      strProbeHomeToProbeTransform << probehome2probeMatrix->GetElement(r,c) << " ";
    }
  }

	toolsBufferMatrices[ this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetToolName() ] = strProbeHomeToProbeTransform.str(); 
	toolsStatuses[ this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetToolName() ] = vtkTracker::ConvertTrackerStatusToString(probehome2probeStatus); 

	// TEMPLATEHOME_TO_TEMPLATE_TRANSFORM
	TrackerStatus templhome2templStatus = TR_OK; 
  vtkSmartPointer<vtkMatrix4x4> templhome2templMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if ( this->GetTemplateHomeToTemplateTransform(timestamp, templhome2templMatrix, templhome2templStatus, calibratedTransform) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get template home to template transform from buffer!"); 
    return PLUS_FAIL; 
  }

	std::ostringstream strTemplHomeToTemplTransform; 
  for ( int r = 0; r < 4; ++r )
  {
    for ( int c = 0; c < 4; ++c )
    {
      strTemplHomeToTemplTransform << templhome2templMatrix->GetElement(r,c) << " ";
    }
  }
	
  toolsBufferMatrices[ this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetToolName() ] = strTemplHomeToTemplTransform.str(); 
	toolsStatuses[ this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetToolName() ] = vtkTracker::ConvertTrackerStatusToString(templhome2templStatus); 

	// RAW_ENCODER_VALUES
	TrackerStatus rawEncoderValuesStatus = TR_OK; 
  vtkSmartPointer<vtkMatrix4x4> rawEncoderValuesMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
	if ( this->GetRawEncoderValuesTransform(timestamp, rawEncoderValuesMatrix, rawEncoderValuesStatus)  != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get raw encoder values from buffer!"); 
    return PLUS_FAIL; 
  }

	std::ostringstream strRawEncoderValuesTransform; 
  for ( int r = 0; r < 4; ++r )
  {
    for ( int c = 0; c < 4; ++c )
    {
      strRawEncoderValuesTransform << rawEncoderValuesMatrix->GetElement(r,c) << " ";
    }
  }
	
  toolsBufferMatrices[ this->GetTool(RAW_ENCODER_VALUES)->GetToolName() ] = strRawEncoderValuesTransform.str(); 
	toolsStatuses[ this->GetTool(RAW_ENCODER_VALUES)->GetToolName() ] = vtkTracker::ConvertTrackerStatusToString(rawEncoderValuesStatus); 
	
	// Get value for PROBE_POSITION, PROBE_ROTATION, TEMPLATE_POSITION tools
	TrackerStatus encoderStatus = TR_OK; 
	double probePos(0), probeRot(0), templatePos(0); 
	if ( this->GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, encoderStatus) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get stepper encoder values!"); 
    return PLUS_FAIL; 
  }
	
	// PROBE_POSITION
	std::ostringstream strProbePos; 
	strProbePos << probePos; 
	toolsBufferMatrices[ "ProbePosition" ] = strProbePos.str(); 
	toolsStatuses[ "ProbePosition" ] = vtkTracker::ConvertTrackerStatusToString(encoderStatus); 

	// PROBE_ROTATION
	std::ostringstream strProbeRot; 
	strProbeRot << probeRot; 
	toolsBufferMatrices[ "ProbeRotation" ] = strProbeRot.str(); 
	toolsStatuses[ "ProbeRotation" ] = vtkTracker::ConvertTrackerStatusToString(encoderStatus); 

	// TEMPLATE_POSITION
	std::ostringstream strTemplatePos; 
	strTemplatePos << templatePos; 
	toolsBufferMatrices[ "TemplatePosition" ] = strTemplatePos.str(); 
	toolsStatuses[ "TemplatePosition" ] = vtkTracker::ConvertTrackerStatusToString(encoderStatus); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetStepperEncoderValues( BufferItemUidType uid, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  TrackerBufferItem bufferItem; 
	if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem, false) != ITEM_OK )
  {
    LOG_ERROR("Failed to get stepper encoder values from buffer by UID: " << uid ); 
    return PLUS_FAIL; 
  }

  probePosition = bufferItem.GetMatrix()->GetElement(ROW_PROBE_POSITION,3); 
	probeRotation = bufferItem.GetMatrix()->GetElement(ROW_PROBE_ROTATION,3); 
	templatePosition = bufferItem.GetMatrix()->GetElement(ROW_TEMPLATE_POSITION,3); 
  status = bufferItem.GetStatus(); 
	
  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetStepperEncoderValues( double timestamp, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  BufferItemUidType uid(0); 
  if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get stepper encoder values from buffer by time: " << std::fixed << timestamp ); 
    return PLUS_FAIL; 
  }

  return this->GetStepperEncoderValues(uid, probePosition, probeRotation, templatePosition, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetProbeHomeToProbeTransform( BufferItemUidType uid, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
{
  if ( probeHomeToProbeMatrix == NULL )
  {
    LOG_ERROR("Failed to get probe home to probe transform - input transform is NULL!"); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
	if ( this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem, calibratedTransform) != ITEM_OK )
  {
    LOG_ERROR("Failed to get probe home to probe transform by UID: " << uid); 
    return PLUS_FAIL; 
  }

  status = bufferItem.GetStatus(); 
  probeHomeToProbeMatrix->DeepCopy( bufferItem.GetMatrix() ); 

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetProbeHomeToProbeTransform( double timestamp, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
{
  BufferItemUidType uid(0); 
  if ( this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get probe home to probe transform by timestamp: " << std::fixed << timestamp);
    PLUS_FAIL; 
  }

  return this->GetProbeHomeToProbeTransform(uid, probeHomeToProbeMatrix, status, calibratedTransform); 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetTemplateHomeToTemplateTransform( BufferItemUidType uid, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
{
  if ( templateHomeToTemplateMatrix == NULL )
  {
    LOG_ERROR("Failed to get template home to template transform - input transform is NULL!"); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
	if ( this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem, calibratedTransform) != ITEM_OK )
  {
    LOG_ERROR("Failed to get template home to template transform by UID: " << uid); 
    return PLUS_FAIL; 
  }

  status = bufferItem.GetStatus(); 
  templateHomeToTemplateMatrix->DeepCopy( bufferItem.GetMatrix() ); 

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetTemplateHomeToTemplateTransform( double timestamp, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
{
  BufferItemUidType uid(0); 
  if ( this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get template home to template transform by timestamp: " << std::fixed << timestamp);
    return PLUS_FAIL; 
  }

  return this->GetTemplateHomeToTemplateTransform(uid, templateHomeToTemplateMatrix, status, calibratedTransform); 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetRawEncoderValuesTransform( BufferItemUidType uid, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status )
{
  if ( rawEncoderValuesTransform == NULL )
  {
    LOG_ERROR("Failed to get raw encoder values transform from buffer - input transform NULL!"); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem, false) != ITEM_OK )
  {
    LOG_ERROR("Failed to get raw encoder values transform from buffer by UID: " << uid ); 
    return PLUS_FAIL;
  }

  rawEncoderValuesTransform->DeepCopy( bufferItem.GetMatrix() ); 
  status = bufferItem.GetStatus(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkAMSTracker::GetRawEncoderValuesTransform( double timestamp, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status )
{
  BufferItemUidType uid(0); 
  if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get raw encoder values transform by timestamp: " << std::fixed << timestamp);
    return PLUS_FAIL; 
  }
  
  return this->GetRawEncoderValuesTransform(uid, rawEncoderValuesTransform, status); 
}

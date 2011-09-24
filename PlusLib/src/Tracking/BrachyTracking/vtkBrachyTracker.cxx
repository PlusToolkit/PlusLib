#include "PlusConfigure.h"
#include "BrachyStepper.h"
#include "CmsBrachyStepper.h"
#include "CivcoBrachyStepper.h"
#include "vtkBrachyTracker.h"
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
#include "vtkTrackedFrameList.h"


//----------------------------------------------------------------------------
vtkBrachyTracker* vtkBrachyTracker::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkBrachyTracker");
	if(ret)
	{
		return (vtkBrachyTracker*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkBrachyTracker;
}

//----------------------------------------------------------------------------
vtkBrachyTracker::vtkBrachyTracker()
{
	this->Device = NULL;
	this->ModelVersion = NULL;
	this->ModelNumber = NULL; 
	this->ModelSerialNumber = NULL; 
  this->CalibrationAlgorithmVersion = NULL; 
  this->CalibrationDate = NULL; 

	this->SetSerialPort(1);
	this->BaudRate = 19200;
	this->SetNumberOfTools(NUMBER_OF_BRACHY_TOOLS); 
	this->SetToolName(PROBEHOME_TO_PROBE_TRANSFORM, "ProbeHomeToProbeUncalibrated"); 
  this->SetToolEnabled(PROBEHOME_TO_PROBE_TRANSFORM, true); 
  this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->SetToolType(TRACKER_TOOL_PROBE); 
	this->SetToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, "TemplateHomeToTemplateUncalibrated"); 
  this->SetToolEnabled(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, true); 
  this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->SetToolType(TRACKER_TOOL_GENERAL); 
	this->SetToolName(RAW_ENCODER_VALUES, "StepperEncoderValues"); 
  this->SetToolEnabled(RAW_ENCODER_VALUES, true); 
  this->GetTool(RAW_ENCODER_VALUES)->SetToolType(TRACKER_TOOL_GENERAL);
  this->BrachyStepperType = BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER; 

  // Stepper calibration parameters
	this->SetProbeTranslationAxisOrientation(0,0,1); 
	this->SetTemplateTranslationAxisOrientation(0,0,1); 
	this->SetProbeRotationAxisOrientation(0,0,1); 
	this->SetProbeRotationEncoderScale(1.0); 
}

//----------------------------------------------------------------------------
vtkBrachyTracker::~vtkBrachyTracker() 
{
	if (this->Tracking)
	{
		this->StopTracking();
	}

  if ( this->Device != NULL )
  {
    delete this->Device; 
    this->Device = NULL; 
  }

  this->SetModelVersion(NULL); 
  this->SetModelNumber(NULL); 
  this->SetModelSerialNumber(NULL); 
  this->SetCalibrationAlgorithmVersion(NULL); 
  this->SetCalibrationDate(NULL); 

}

//----------------------------------------------------------------------------
void vtkBrachyTracker::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkTracker::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::Connect()
{
	if (this->Device == NULL)
	{
		LOG_ERROR("Failed to connect to brachy tracker - BrachyStepperType not selected, device is NULL!"); 
    return PLUS_FAIL; 
	}

	return this->Device->Connect(); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::Disconnect()
{
	this->Device->Disconnect(); 
	return this->StopTracking(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::Probe()
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
PlusStatus vtkBrachyTracker::InternalStartTracking()
{
	if ( this->IsTracking() )
	{
		return PLUS_SUCCESS;
	}

	if (this->InitBrachyTracker() != PLUS_SUCCESS)
	{
		LOG_ERROR("Couldn't initialize brachy stepper.");
		return PLUS_FAIL;
	} 

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::InternalStopTracking()
{
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::InternalUpdate()
{
	TrackerStatus status = TR_OK;

	if (!this->Tracking)
	{
		LOG_ERROR("called Update() when Brachy stepper was not tracking");
		return PLUS_FAIL;
	}

	// get the transforms from stepper
	double dProbePosition(0), dTemplatePosition(0), dProbeRotation(0); 
	unsigned long frameNum(0); 
	if (!this->Device->GetEncoderValues(dProbePosition, dTemplatePosition, dProbeRotation, frameNum)) 
	{
		LOG_DEBUG("Tracker request timeout..."); 
		// Unable to get tracking information from tracker
		status = TR_REQ_TIMEOUT; 
	}

  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

	// Save probe position to the matrix (0,3) element
	// Save probe rotation to the matrix (1,3) element
	// Save grid position to the matrix (2,3) element
	vtkSmartPointer<vtkMatrix4x4> probePosition = vtkSmartPointer<vtkMatrix4x4>::New(); 
	probePosition->SetElement(ROW_PROBE_POSITION, 3, dProbePosition); 
	probePosition->SetElement(ROW_PROBE_ROTATION, 3, dProbeRotation); 
	probePosition->SetElement(ROW_TEMPLATE_POSITION, 3, dTemplatePosition); 
	// send the transformation matrix and status to the tool
	this->ToolTimeStampedUpdate(RAW_ENCODER_VALUES, probePosition, status, frameNum, unfilteredTimestamp);   

	// Save template home to template transform
	vtkSmartPointer<vtkTransform> tTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	double templateTranslationAxisVector[3]; 
	this->GetTemplateTranslationAxisOrientation(templateTranslationAxisVector); 
	vtkMath::MultiplyScalar(templateTranslationAxisVector, dTemplatePosition); 
	tTemplateHomeToTemplate->Translate(templateTranslationAxisVector); 
	// send the transformation matrix and status to the tool
	this->ToolTimeStampedUpdate(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, tTemplateHomeToTemplate->GetMatrix(), status, frameNum, unfilteredTimestamp);   

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
	tProbeHomeToProbe->Translate(-probeRotationVector[0], -probeRotationVector[1], -probeRotationVector[2]); 
	// send the transformation matrix and status to the tool
	this->ToolTimeStampedUpdate(PROBEHOME_TO_PROBE_TRANSFORM, tProbeHomeToProbe->GetMatrix(), status, frameNum, unfilteredTimestamp);   

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::InitBrachyTracker()
{
	// Connect to device 
	if ( !this->Connect() )
	{
		LOG_ERROR("Unable to connect to stepper on port: " << this->GetSerialPort() );
		return PLUS_FAIL; 
	}

  std::string version; 
  std::string model; 
  std::string serial; 
	if ( this->Device->GetDeviceModelInfo(version, model, serial) != PLUS_SUCCESS )
	{
		LOG_ERROR("Couldn't get version info from stepper.");
		return PLUS_FAIL; 
	}

	for ( int tool = 0; tool < NUMBER_OF_BRACHY_TOOLS; tool++ )
	{
		this->GetTool(tool)->EnabledOn(); 
	}

  this->SetModelVersion(version.c_str());
	this->SetModelNumber(model.c_str()); 
	this->SetModelSerialNumber(serial.c_str()); 

	if ( this->TrackerCalibrated )
	{
		this->SetToolName(PROBEHOME_TO_PROBE_TRANSFORM, "ProbeHomeToProbe"); 
		this->SetToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, "TemplateHomeToTemplate"); 
	}

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::ReadConfiguration(vtkXMLDataElement* config)
{
	// Read superclass configuration first
	Superclass::ReadConfiguration(config); 

	if ( config == NULL ) 
	{
		LOG_WARNING("Unable to find BrachyTracker XML data element");
		return PLUS_FAIL; 
	}

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

	unsigned long serialPort(0); 
  if ( trackerConfig->GetScalarAttribute("SerialPort", serialPort) ) 
	{
		if ( !this->IsTracking() )
		{
			this->SetSerialPort(serialPort); 
		}
	}

	unsigned long baudRate = 0; 
	if ( trackerConfig->GetScalarAttribute("BaudRate", baudRate) ) 
	{
		if ( !this->IsTracking() )
		{
			this->SetBaudRate(baudRate); 
		}
	}

  if ( !this->IsTracking() )
  {
    const char* brachyStepperType = trackerConfig->GetAttribute("BrachyStepperType"); 
    if ( brachyStepperType != NULL )
    {
      // Delete device before we change it 
      if (this->Device != NULL)
      {
        delete this->Device; 
      }
      
      if ( STRCASECMP(BrachyStepper::GetBrachyStepperTypeInString(BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER).c_str(), brachyStepperType) == 0 )
      {
        this->Device = new CmsBrachyStepper(this->GetSerialPort(), this->GetBaudRate() ) ; 
        this->Device->SetBrachyStepperType(BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER); 
        this->BrachyStepperType = BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER; 

      }
      else if ( STRCASECMP(BrachyStepper::GetBrachyStepperTypeInString(BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER).c_str(), brachyStepperType) == 0 )
      {
        this->Device = new CmsBrachyStepper(this->GetSerialPort(), this->GetBaudRate() ) ; 
        this->Device->SetBrachyStepperType(BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER); 
        this->BrachyStepperType = BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER; 
      }
      else if ( STRCASECMP(BrachyStepper::GetBrachyStepperTypeInString(BrachyStepper::CMS_ACCUSEED_DS300).c_str(), brachyStepperType) == 0 )
      {
        this->Device = new CmsBrachyStepper(this->GetSerialPort(), this->GetBaudRate() ) ; 
        this->Device->SetBrachyStepperType(BrachyStepper::CMS_ACCUSEED_DS300); 
        this->BrachyStepperType = BrachyStepper::CMS_ACCUSEED_DS300; 
      }
      else if ( STRCASECMP(BrachyStepper::GetBrachyStepperTypeInString(BrachyStepper::CIVCO_STEPPER).c_str(), brachyStepperType) == 0 )
      {
        this->Device = new CivcoBrachyStepper(this->GetSerialPort(), this->GetBaudRate()); 
        this->Device->SetBrachyStepperType(BrachyStepper::CIVCO_STEPPER); 
        this->BrachyStepperType = BrachyStepper::CIVCO_STEPPER; 
      }
      else
      {
        LOG_ERROR("Unable to recognize brachy stepper type: " << brachyStepperType);
        return PLUS_FAIL; 
      }
    }
    else
    {
      LOG_ERROR("Unable to find BrachyStepperType attribute in configuration file!"); 
      return PLUS_FAIL; 
    }

    const char* modelNumber =  trackerConfig->GetAttribute("ModelNumber"); 
    if ( modelNumber != NULL ) 
    {
      this->SetModelNumber(modelNumber); 
    }

    const char* modelVersion =  trackerConfig->GetAttribute("ModelVersion"); 
    if ( modelVersion != NULL ) 
    {
      this->SetModelVersion(modelVersion); 
    }

    const char* modelSerialNumber = trackerConfig->GetAttribute("ModelSerialNumber");
    if ( modelSerialNumber != NULL ) 
    {
      this->SetModelSerialNumber(modelSerialNumber); 
    }

  }

	vtkSmartPointer<vtkXMLDataElement> calibration = trackerConfig->FindNestedElementWithName("StepperCalibrationResult"); 
	if ( calibration != NULL ) 
	{
		const char* calibrationAlgorithmVersion = calibration->GetAttribute("AlgorithmVersion"); 
		if ( calibrationAlgorithmVersion != NULL )
		{
      this->SetCalibrationAlgorithmVersion(calibrationAlgorithmVersion); 
		}
    else
    {
      LOG_WARNING("Failed to read stepper calibration algorithm version from config file!"); 
      this->SetCalibrationAlgorithmVersion("Unknown"); 
    }

		const char* calibrationDate = calibration->GetAttribute("Date"); 
		if ( calibrationDate != NULL )
		{
      this->SetCalibrationDate(calibrationDate); 
		}
    else
    {
      LOG_WARNING("Failed to read stepper calibration date from config file!"); 
      this->SetCalibrationDate("Unknown"); 
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

	if ( this->TrackerCalibrated )
	{
    LOG_INFO("BrachyTracker is calibrated (Calibration date: " << this->GetCalibrationDate() << "  CalibrationAlgorithmVersion: " << this->GetCalibrationAlgorithmVersion() << ")" ); 
	}
	else
	{
		LOG_INFO("BrachyTracker is uncalibrated"); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
	if ( rootConfigElement == NULL )
	{
    LOG_ERROR("Config is invalid");
		return PLUS_FAIL;
	}

  // Get data collection and then Tracker configuration element
	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = rootConfigElement->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  if ( this->Device != NULL )
  {
    BrachyStepper::BRACHY_STEPPER_TYPE stepperType = this->Device->GetBrachyStepperType(); 
    std::string strStepperType = BrachyStepper::GetBrachyStepperTypeInString(stepperType); 
    trackerConfig->SetAttribute("BrachyStepperType", strStepperType.c_str()); 
  }

  trackerConfig->SetUnsignedLongAttribute( "SerialPort", this->GetSerialPort() ); 
  trackerConfig->SetDoubleAttribute( "BaudRate", this->GetBaudRate() ); 
  trackerConfig->SetAttribute( "ModelVersion", this->GetModelVersion() ); 
  trackerConfig->SetAttribute( "ModelNumber", this->GetModelNumber() ); 
  trackerConfig->SetAttribute( "ModelSerialNumber", this->GetModelSerialNumber() ); 

  if ( this->GetTrackerCalibrated() )
  {
    // Save stepper calibration results to file
  	vtkSmartPointer<vtkXMLDataElement> calibration = trackerConfig->FindNestedElementWithName("StepperCalibrationResult"); 
    if ( calibration == NULL )
    {
      // create new element and add to trackerTool 
      calibration = vtkSmartPointer<vtkXMLDataElement>::New(); 
      calibration->SetName("StepperCalibrationResult"); 
      calibration->SetParent(trackerConfig); 
      trackerConfig->AddNestedElement(calibration);
    } 

    calibration->SetAttribute("Date", this->GetCalibrationDate());

    calibration->SetAttribute("AlgorithmVersion", this->GetCalibrationAlgorithmVersion() ); 

    calibration->SetVectorAttribute("ProbeRotationAxisOrientation", 3, this->GetProbeRotationAxisOrientation() );		

    calibration->SetDoubleAttribute("ProbeRotationEncoderScale", this->GetProbeRotationEncoderScale() ); 

    calibration->SetVectorAttribute("ProbeTranslationAxisOrientation", 3, this->GetProbeTranslationAxisOrientation() ); 

    calibration->SetVectorAttribute("TemplateTranslationAxisOrientation", 3, this->GetTemplateTranslationAxisOrientation() ); 
 
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::ResetStepper()
{
	return this->Device->ResetStepper(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::InitializeStepper( std::string &calibMsg )
{
	return this->Device->InitializeStepper(calibMsg); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetTrackerToolBufferStringList(double timestamp, 
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
	if ( vtkBrachyTracker::GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, encoderStatus) != PLUS_SUCCESS )
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
PlusStatus vtkBrachyTracker::GetLatestStepperEncoderValues( double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  if (this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetNumberOfItems()<1)
  {
    LOG_DEBUG("The buffer is empty"); // do not report as an error, it may be normal after a buffer clear
    probePosition=0.0;
    probeRotation=0.0;
    templatePosition=0.0;
    status=TR_MISSING;
    return PLUS_SUCCESS;
  }
  BufferItemUidType latestUid = this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetLatestItemUidInBuffer(); 

  return vtkBrachyTracker::GetStepperEncoderValues(latestUid, probePosition, probeRotation, templatePosition, status);
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetStepperEncoderValues( BufferItemUidType uid, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  TrackerBufferItem bufferItem; 
	if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem, false) != ITEM_OK )
  {
    LOG_ERROR("Failed to get stepper encoder values from buffer by UID: " << uid ); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkMatrix4x4> mx=vtkSmartPointer<vtkMatrix4x4>::New();
  if (bufferItem.GetMatrix(mx)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get bufferitem matrix by UID: " << uid ); 
    return PLUS_FAIL;
  }
  probePosition = mx->GetElement(ROW_PROBE_POSITION,3); 
	probeRotation = mx->GetElement(ROW_PROBE_ROTATION,3); 
	templatePosition = mx->GetElement(ROW_TEMPLATE_POSITION,3); 
  status = bufferItem.GetStatus(); 
	
  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetStepperEncoderValues( double timestamp, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  BufferItemUidType uid(0); 
  if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get stepper encoder values from buffer by time: " << std::fixed << timestamp ); 
    return PLUS_FAIL; 
  }

  return vtkBrachyTracker::GetStepperEncoderValues(uid, probePosition, probeRotation, templatePosition, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetProbeHomeToProbeTransform( BufferItemUidType uid, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
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
  if (bufferItem.GetMatrix(probeHomeToProbeMatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get probeHomeToProbeMatrix"); 
    return PLUS_FAIL;
  }

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetProbeHomeToProbeTransform( double timestamp, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
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
PlusStatus vtkBrachyTracker::GetTemplateHomeToTemplateTransform( BufferItemUidType uid, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
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
  if (bufferItem.GetMatrix(templateHomeToTemplateMatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get templateHomeToTemplateMatrix"); 
    return PLUS_FAIL;
  }

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetTemplateHomeToTemplateTransform( double timestamp, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status, bool calibratedTransform /*= false*/ )
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
PlusStatus vtkBrachyTracker::GetRawEncoderValuesTransform( BufferItemUidType uid, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status )
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

  if (bufferItem.GetMatrix(rawEncoderValuesTransform)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get rawEncoderValuesTransform"); 
    return PLUS_FAIL;
  }

  status = bufferItem.GetStatus(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetRawEncoderValuesTransform( double timestamp, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status )
{
  BufferItemUidType uid(0); 
  if ( this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get raw encoder values transform by timestamp: " << std::fixed << timestamp);
    return PLUS_FAIL; 
  }
  
  return this->GetRawEncoderValuesTransform(uid, rawEncoderValuesTransform, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition)
{
  if ( trackedFrame == NULL )
  {
    LOG_ERROR("Unable to get stepper encoder values - input tracked frame is NULL!"); 
    return PLUS_FAIL; 
  }

  // Get the probe position from tracked frame info
  const char* cProbePos = trackedFrame->GetCustomFrameField("ProbePosition"); 
  if ( cProbePos != NULL )
  {
    probePosition = atof(cProbePos); 
  }
  else
  {
    double transform[16]; 
    if ( trackedFrame->GetDefaultFrameTransform(transform) )
    {
      // Get probe position from matrix (0,3) element
      probePosition = transform[3]; 
    }
    else
    {
      LOG_ERROR("Unable to get probe position from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  // Get the probe rotation from tracked frame info
  const char* cProbeRot = trackedFrame->GetCustomFrameField("ProbeRotation"); 
  if ( cProbeRot != NULL )
  {
    probeRotation = atof(cProbeRot); 
  }
  else
  {
    double transform[16]; 
    if ( trackedFrame->GetDefaultFrameTransform(transform) )
    {
      // Get probe rotation from matrix (1,3) element
      probeRotation = transform[7]; 
    }
    else
    {
      LOG_ERROR("Unable to get probe rotation from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  // Get the template position from tracked frame info
  const char* cTemplatePos = trackedFrame->GetCustomFrameField("TemplatePosition"); 
  if ( cTemplatePos != NULL )
  {
    templatePosition = atof(cTemplatePos); 
  }
  else
  {
    double transform[16]; 
    if ( trackedFrame->GetDefaultFrameTransform(transform) )
    {
      // Get template position from matrix (2,3) element
      templatePosition = transform[11]; 
    }
    else
    {
      LOG_ERROR("Unable to get template position from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS; 
}
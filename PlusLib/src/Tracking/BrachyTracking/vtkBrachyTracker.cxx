/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

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

vtkStandardNewMacro(vtkBrachyTracker);

//----------------------------------------------------------------------------
vtkBrachyTracker::vtkBrachyTracker()
{
  this->Device = NULL;
  this->ModelVersion = NULL;
  this->ModelNumber = NULL; 
  this->ModelSerialNumber = NULL; 
  this->CalibrationAlgorithmVersion = NULL; 
  this->CalibrationDate = NULL; 
  this->IsBrachyTrackerCompensationDefined = false; 

  this->SetSerialPort(1);
  this->BaudRate = 19200;

  this->SetToolReferenceFrameName("StepperHome"); 

  // Add tools to the tracker 
  vtkSmartPointer<vtkTrackerTool> probeTool = vtkSmartPointer<vtkTrackerTool>::New(); 
  probeTool->SetToolName("Probe"); 
  std::ostringstream probePortName; 
  probePortName << PROBEHOME_TO_PROBE_TRANSFORM; 
  probeTool->SetPortName(probePortName.str().c_str()); 
  this->AddTool(probeTool); 

  vtkSmartPointer<vtkTrackerTool> templateTool = vtkSmartPointer<vtkTrackerTool>::New(); 
  templateTool->SetToolName("Template"); 
  std::ostringstream templatePortName; 
  templatePortName << TEMPLATEHOME_TO_TEMPLATE_TRANSFORM; 
  templateTool->SetPortName(templatePortName.str().c_str()); 
  this->AddTool(templateTool); 

  vtkSmartPointer<vtkTrackerTool> encoderTool = vtkSmartPointer<vtkTrackerTool>::New(); 
  encoderTool->SetToolName("StepperEncoderValues"); 
  std::ostringstream encoderPortName; 
  encoderPortName << RAW_ENCODER_VALUES; 
  encoderTool->SetPortName(encoderPortName.str().c_str()); 
  this->AddTool(encoderTool); 

  this->BrachyStepperType = BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER; 

  // Stepper calibration parameters
  this->CompensationEnabledOn(); 
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
std::string vtkBrachyTracker::GetBrachyToolName(BRACHY_STEPPER_TOOL tool)
{
  std::string toolName; 

  std::ostringstream toolPortName; 
  toolPortName << tool; 
  vtkTrackerTool * trackerTool = NULL; 
  if ( this->GetToolByPortName(toolPortName.str().c_str(), trackerTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool by port: " << toolPortName.str() ); 
    return toolName; 
  }
  
  toolName = trackerTool->GetToolName(); 

  return toolName; 
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
  
  // Update encoder values tool 
  if ( this->ToolTimeStampedUpdate(this->GetBrachyToolName(RAW_ENCODER_VALUES).c_str(), probePosition, status, frameNum, unfilteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tool: " << this->GetBrachyToolName(RAW_ENCODER_VALUES) ); 
    return PLUS_FAIL;
  }

  if ( !this->CompensationEnabled )
  {

    // Update template transform tool 
    vtkSmartPointer<vtkTransform> tTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New();
    tTemplateHomeToTemplate->Translate(0,0,dTemplatePosition); 
    if ( this->ToolTimeStampedUpdate(this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM).c_str(), tTemplateHomeToTemplate->GetMatrix(), status, frameNum, unfilteredTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update tool: " << this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)); 
      return PLUS_FAIL;
    }

    // Update probe transform tool 
    vtkSmartPointer<vtkTransform> tProbeHomeToProbe = vtkSmartPointer<vtkTransform>::New();
    tProbeHomeToProbe->Translate(0,0,dProbePosition);
    tProbeHomeToProbe->RotateZ(dProbeRotation);
    if ( this->ToolTimeStampedUpdate(this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM).c_str(), tProbeHomeToProbe->GetMatrix(), status, frameNum, unfilteredTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update tool: " << this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM)); 
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  // Save template home to template transform
  vtkSmartPointer<vtkTransform> tTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New(); 
  double templateTranslationAxisVector[3]; 
  this->GetTemplateTranslationAxisOrientation(templateTranslationAxisVector); 
  vtkMath::MultiplyScalar(templateTranslationAxisVector, dTemplatePosition); 
  tTemplateHomeToTemplate->Translate(templateTranslationAxisVector); 
  // send the transformation matrix and status to the tool
  if ( this->ToolTimeStampedUpdate(this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM).c_str(), tTemplateHomeToTemplate->GetMatrix(), status, frameNum, unfilteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tool: " << this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)); 
    return PLUS_FAIL;
  }

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
  if ( this->ToolTimeStampedUpdate(this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM).c_str(), tProbeHomeToProbe->GetMatrix(), status, frameNum, unfilteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tool: " << this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM)); 
    return PLUS_FAIL;
  }

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

  this->SetModelVersion(version.c_str());
  this->SetModelNumber(model.c_str()); 
  this->SetModelSerialNumber(serial.c_str()); 

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

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
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

	vtkXMLDataElement* calibration = trackerConfig->FindNestedElementWithName("StepperCalibrationResult"); 
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

    this->IsBrachyTrackerCompensationDefined = true; 
  }

  if ( this->IsBrachyTrackerCompensationDefined )
  {
    LOG_INFO("BrachyTracker compensation parameters are defined (Calibration date: " << this->GetCalibrationDate() << "  CalibrationAlgorithmVersion: " << this->GetCalibrationAlgorithmVersion() << ")" ); 
  }
  else
  {
    LOG_INFO("BrachyTracker compensation parameters are undefined "); 
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
	vtkXMLDataElement* dataCollectionConfig = rootConfigElement->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
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

  if ( this->IsBrachyTrackerCompensationDefined )
  {
    // Save stepper calibration results to file
  	vtkSmartPointer<vtkXMLDataElement> calibration = trackerConfig->FindNestedElementWithName("StepperCalibrationResult"); 
    if ( calibration == NULL) 
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
PlusStatus vtkBrachyTracker::GetAllTransforms(double timestamp, TrackedFrame* aTrackedFrame )
{

  // PROBEHOME_TO_PROBE_TRANSFORM
  TrackerStatus probehome2probeStatus = TR_OK; 
  vtkSmartPointer<vtkMatrix4x4> probehome2probeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( this->GetProbeHomeToProbeTransform(timestamp, probehome2probeMatrix, probehome2probeStatus) != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to get probe home to probe transform from buffer!"); 
    return PLUS_FAIL; 
  }
  
  PlusTransformName probeToReferenceTransformName(this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM), this->ToolReferenceFrameName ); 
  if ( !probeToReferenceTransformName.IsValid())
  {
    LOG_ERROR("Invalid probe to reference tranform name!"); 
    return PLUS_FAIL; 
  }

  if ( aTrackedFrame->SetCustomFrameTransform(probeToReferenceTransformName, probehome2probeMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform for tool " << this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM) ); 
    return PLUS_FAIL; 
  }

  if ( aTrackedFrame->SetCustomFrameTransformStatus(probeToReferenceTransformName, vtkTracker::ConvertTrackerStatusToString(probehome2probeStatus) ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform status for tool " << this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM) ); 
    return PLUS_FAIL; 
  }


  // TEMPLATEHOME_TO_TEMPLATE_TRANSFORM
  TrackerStatus templhome2templStatus = TR_OK; 
  vtkSmartPointer<vtkMatrix4x4> templhome2templMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if ( this->GetTemplateHomeToTemplateTransform(timestamp, templhome2templMatrix, templhome2templStatus ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get template home to template transform from buffer!"); 
    return PLUS_FAIL; 
  }

  PlusTransformName templateToReferenceTransformName(this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM), this->ToolReferenceFrameName ); 
  if ( !templateToReferenceTransformName.IsValid())
  {
    LOG_ERROR("Invalid template to reference tranform name!"); 
    return PLUS_FAIL; 
  }

  if ( aTrackedFrame->SetCustomFrameTransform(templateToReferenceTransformName, templhome2templMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform for tool " << this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM) ); 
    return PLUS_FAIL;  
  }

  if ( aTrackedFrame->SetCustomFrameTransformStatus(templateToReferenceTransformName, vtkTracker::ConvertTrackerStatusToString(templhome2templStatus) ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform status for tool " << this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM) ); 
    return PLUS_FAIL;  
  }


  // RAW_ENCODER_VALUES
  TrackerStatus rawEncoderValuesStatus = TR_OK; 
  vtkSmartPointer<vtkMatrix4x4> rawEncoderValuesMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( this->GetRawEncoderValuesTransform(timestamp, rawEncoderValuesMatrix, rawEncoderValuesStatus)  != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get raw encoder values from buffer!"); 
    return PLUS_FAIL; 
  }

  PlusTransformName encoderToReferenceTransformName(this->GetBrachyToolName(RAW_ENCODER_VALUES), this->ToolReferenceFrameName ); 
  if ( !encoderToReferenceTransformName.IsValid())
  {
    LOG_ERROR("Invalid encoder to reference tranform name!"); 
    return PLUS_FAIL; 
  }

  if ( aTrackedFrame->SetCustomFrameTransform(encoderToReferenceTransformName, rawEncoderValuesMatrix) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform for tool " << this->GetBrachyToolName(RAW_ENCODER_VALUES) ); 
    return PLUS_FAIL; 
  }

  if ( aTrackedFrame->SetCustomFrameTransformStatus(encoderToReferenceTransformName, vtkTracker::ConvertTrackerStatusToString(rawEncoderValuesStatus) ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform status for tool " << this->GetBrachyToolName(RAW_ENCODER_VALUES) ); 
    return PLUS_FAIL; 
  }

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
  aTrackedFrame->SetCustomFrameField("ProbePosition", strProbePos.str()); 

  // PROBE_ROTATION
  std::ostringstream strProbeRot; 
  strProbeRot << probeRot; 
  aTrackedFrame->SetCustomFrameField("ProbeRotation", strProbeRot.str()); 

  // TEMPLATE_POSITION
  std::ostringstream strTemplatePos; 
  strTemplatePos << templatePos; 
  aTrackedFrame->SetCustomFrameField("TemplatePosition", strTemplatePos.str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetLatestStepperEncoderValues( double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  std::string encoderToolName = this->GetBrachyToolName(RAW_ENCODER_VALUES); 
  if ( encoderToolName.empty() )
  {
    LOG_ERROR("Failed to get encoder values tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *encoderTool = NULL; 
  if ( this->GetTool(encoderToolName.c_str(), encoderTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << encoderToolName ); 
    return PLUS_FAIL; 
  }

  if (encoderTool->GetBuffer()->GetNumberOfItems()<1)
  {
    LOG_DEBUG("The buffer is empty"); // do not report as an error, it may be normal after a buffer clear
    probePosition=0.0;
    probeRotation=0.0;
    templatePosition=0.0;
    status=TR_MISSING;
    return PLUS_SUCCESS;
  }
  BufferItemUidType latestUid = encoderTool->GetBuffer()->GetLatestItemUidInBuffer(); 

  return vtkBrachyTracker::GetStepperEncoderValues(latestUid, probePosition, probeRotation, templatePosition, status);
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetStepperEncoderValues( BufferItemUidType uid, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status )
{
  std::string encoderToolName = this->GetBrachyToolName(RAW_ENCODER_VALUES); 
  if ( encoderToolName.empty() )
  {
    LOG_ERROR("Failed to get encoder values tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *encoderTool = NULL; 
  if ( this->GetTool(encoderToolName.c_str(), encoderTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << encoderToolName ); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( encoderTool->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem) != ITEM_OK )
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
  std::string encoderToolName = this->GetBrachyToolName(RAW_ENCODER_VALUES); 
  if ( encoderToolName.empty() )
  {
    LOG_ERROR("Failed to get encoder values tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *encoderTool = NULL; 
  if ( this->GetTool(encoderToolName.c_str(), encoderTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << encoderToolName ); 
    return PLUS_FAIL; 
  }

  BufferItemUidType uid(0); 
  if ( encoderTool->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get stepper encoder values from buffer by time: " << std::fixed << timestamp ); 
    return PLUS_FAIL; 
  }

  return vtkBrachyTracker::GetStepperEncoderValues(uid, probePosition, probeRotation, templatePosition, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetProbeHomeToProbeTransform( BufferItemUidType uid, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status )
{
  if ( probeHomeToProbeMatrix == NULL )
  {
    LOG_ERROR("Failed to get probe home to probe transform - input transform is NULL!"); 
    return PLUS_FAIL; 
  }

  std::string probeToolName = this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM); 
  if ( probeToolName.empty() )
  {
    LOG_ERROR("Failed to get probe tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *probeTool = NULL; 
  if ( this->GetTool(probeToolName.c_str(), probeTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << probeToolName ); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( probeTool->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem) != ITEM_OK )
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
PlusStatus vtkBrachyTracker::GetProbeHomeToProbeTransform( double timestamp, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status)
{
  std::string probeToolName = this->GetBrachyToolName(PROBEHOME_TO_PROBE_TRANSFORM); 
  if ( probeToolName.empty() )
  {
    LOG_ERROR("Failed to get probe tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *probeTool = NULL; 
  if ( this->GetTool(probeToolName.c_str(), probeTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << probeToolName ); 
    return PLUS_FAIL; 
  }

  BufferItemUidType uid(0); 
  if ( probeTool->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get probe home to probe transform by timestamp: " << std::fixed << timestamp);
    PLUS_FAIL; 
  }

  return this->GetProbeHomeToProbeTransform(uid, probeHomeToProbeMatrix, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetTemplateHomeToTemplateTransform( BufferItemUidType uid, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status )
{
  if ( templateHomeToTemplateMatrix == NULL )
  {
    LOG_ERROR("Failed to get template home to template transform - input transform is NULL!"); 
    return PLUS_FAIL; 
  }

  std::string templateToolName = this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM); 
  if ( templateToolName.empty() )
  {
    LOG_ERROR("Failed to get template tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *templateTool = NULL; 
  if ( this->GetTool(templateToolName.c_str(), templateTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << templateToolName ); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( templateTool->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem) != ITEM_OK )
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
PlusStatus vtkBrachyTracker::GetTemplateHomeToTemplateTransform( double timestamp, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status )
{
  std::string templateToolName = this->GetBrachyToolName(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM); 
  if ( templateToolName.empty() )
  {
    LOG_ERROR("Failed to get template tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *templateTool = NULL; 
  if ( this->GetTool(templateToolName.c_str(), templateTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << templateToolName ); 
    return PLUS_FAIL; 
  }

  BufferItemUidType uid(0); 
  if ( templateTool->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get template home to template transform by timestamp: " << std::fixed << timestamp);
    return PLUS_FAIL; 
  }

  return this->GetTemplateHomeToTemplateTransform(uid, templateHomeToTemplateMatrix, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetRawEncoderValuesTransform( BufferItemUidType uid, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status )
{
  if ( rawEncoderValuesTransform == NULL )
  {
    LOG_ERROR("Failed to get raw encoder values transform from buffer - input transform NULL!"); 
    return PLUS_FAIL; 
  }

  std::string encoderToolName = this->GetBrachyToolName(RAW_ENCODER_VALUES); 
  if ( encoderToolName.empty() )
  {
    LOG_ERROR("Failed to get encoder values tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *encoderTool = NULL; 
  if ( this->GetTool(encoderToolName.c_str(), encoderTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << encoderToolName ); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( encoderTool->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem) != ITEM_OK )
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
  std::string encoderToolName = this->GetBrachyToolName(RAW_ENCODER_VALUES); 
  if ( encoderToolName.empty() )
  {
    LOG_ERROR("Failed to get encoder values tool name!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool *encoderTool = NULL; 
  if ( this->GetTool(encoderToolName.c_str(), encoderTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool: " << encoderToolName ); 
    return PLUS_FAIL; 
  }

  BufferItemUidType uid(0); 
  if ( encoderTool->GetBuffer()->GetItemUidFromTime(timestamp, uid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get raw encoder values transform by timestamp: " << std::fixed << timestamp);
    return PLUS_FAIL; 
  }

  return this->GetRawEncoderValuesTransform(uid, rawEncoderValuesTransform, status); 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyTracker::GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition, PlusTransformName& defaultTransformName)
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
    if ( trackedFrame->GetCustomFrameTransform(defaultTransformName, transform) )
    {
      // Get probe position from matrix (0,3) element
      probePosition = transform[ 4 * ROW_PROBE_POSITION + 3]; 
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
    if ( trackedFrame->GetCustomFrameTransform(defaultTransformName, transform) )      
    {
      // Get probe rotation from matrix (1,3) element
      probeRotation = transform[ 4 * ROW_PROBE_ROTATION + 3]; 
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
    if ( trackedFrame->GetCustomFrameTransform(defaultTransformName, transform) )
    {
      // Get template position from matrix (2,3) element
      templatePosition = transform[ 4 * ROW_TEMPLATE_POSITION + 3]; 
    }
    else
    {
      LOG_ERROR("Unable to get template position from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS; 
}

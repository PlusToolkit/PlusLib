#include "PlusConfigure.h"
#include "AMSStepper.h"
#include "vtkAMSTracker.h"
#include <sstream>
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkFrameToTimeConverter.h"
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

	this->ProbeHomeToProbeTransform = vtkTransform::New(); 
	this->TemplateHomeToTemplateTransform = vtkTransform::New();
	this->RawEncoderValuesTransform = vtkTransform::New();

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

	if ( this->ProbeHomeToProbeTransform != NULL )
	{
		this->ProbeHomeToProbeTransform->Delete(); 
		this->ProbeHomeToProbeTransform = NULL; 
	}

	if ( this->TemplateHomeToTemplateTransform != NULL )
	{
		this->TemplateHomeToTemplateTransform->Delete(); 
		this->TemplateHomeToTemplateTransform = NULL; 
	}

	if ( this->RawEncoderValuesTransform != NULL ) 
	{
		this->RawEncoderValuesTransform->Delete(); 
		this->RawEncoderValuesTransform = NULL; 
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

	// for accurate timing
	this->Timer->Initialize();

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
	long flags=0;

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
		flags = TR_REQ_TIMEOUT; 
	}

	// Create timestamp 
	double unfilteredtimestamp(0), filteredtimestamp(0); 
	this->Timer->GetTimeStampForFrame(frameNum, unfilteredtimestamp, filteredtimestamp);

	// Save probe position to the matrix (0,3) element
	// Save probe rotation to the matrix (1,3) element
	// Save grid position to the matrix (2,3) element
	vtkSmartPointer<vtkMatrix4x4> probePosition = vtkSmartPointer<vtkMatrix4x4>::New(); 
	probePosition->SetElement(ROW_PROBE_POSITION, 3, dProbePosition); 
	probePosition->SetElement(ROW_PROBE_ROTATION, 3, dProbeRotation); 
	probePosition->SetElement(ROW_TEMPLATE_POSITION, 3, dTemplatePosition); 
	// send the transformation matrix and flags to the tool
	this->ToolUpdate(RAW_ENCODER_VALUES, probePosition, flags, frameNum, unfilteredtimestamp, filteredtimestamp);   

	// Save template home to template transform
	vtkSmartPointer<vtkTransform> tTemplateHomeToTemplate = vtkSmartPointer<vtkTransform>::New(); 
	double templateTranslationAxisVector[3]; 
	this->GetTemplateTranslationAxisOrientation(templateTranslationAxisVector); 
	vtkMath::MultiplyScalar(templateTranslationAxisVector, dTemplatePosition); 
	tTemplateHomeToTemplate->Translate(templateTranslationAxisVector); 
	// send the transformation matrix and flags to the tool
	this->ToolUpdate(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, tTemplateHomeToTemplate->GetMatrix(), flags, frameNum, unfilteredtimestamp, filteredtimestamp);   

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
	// send the transformation matrix and flags to the tool
	this->ToolUpdate(PROBEHOME_TO_PROBE_TRANSFORM, tProbeHomeToProbe->GetMatrix(), flags, frameNum, unfilteredtimestamp, filteredtimestamp);   

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
void vtkAMSTracker::GetTrackerToolBufferStringList(const double timestamp, 
												   std::map<std::string, std::string> &toolsBufferMatrices, 
												   std::map<std::string, std::string> &toolsCalibrationMatrices, 
												   std::map<std::string, std::string> &toolsStatuses,
												   bool calibratedTransform /*= false*/)
{
	toolsBufferMatrices.clear(); 
	toolsCalibrationMatrices.clear(); 
	toolsStatuses.clear(); 

	// PROBEHOME_TO_PROBE_TRANSFORM
	long probehome2probeFlags(TR_OK); 
	double probehome2probeMatrix[16]; 
	vtkMatrix4x4::DeepCopy(probehome2probeMatrix, this->GetProbeHomeToProbeTransform(timestamp, probehome2probeFlags, calibratedTransform)->GetMatrix()); 
	std::ostringstream strProbeHomeToProbeTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strProbeHomeToProbeTransform << probehome2probeMatrix[i] << " ";
	}
	toolsBufferMatrices[ this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetToolName() ] = strProbeHomeToProbeTransform.str(); 
	toolsStatuses[ this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetToolName() ] = vtkTracker::ConvertFlagToString(probehome2probeFlags); 

	// TEMPLATEHOME_TO_TEMPLATE_TRANSFORM
	long templhome2templFlags(TR_OK); 
	double templhome2templMatrix[16]; 
	vtkMatrix4x4::DeepCopy(templhome2templMatrix, this->GetTemplateHomeToTemplateTransform(timestamp, templhome2templFlags, calibratedTransform)->GetMatrix()); 
	std::ostringstream strTemplHomeToTemplTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strTemplHomeToTemplTransform << templhome2templMatrix[i] << " ";
	}
	toolsBufferMatrices[ this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetToolName() ] = strTemplHomeToTemplTransform.str(); 
	toolsStatuses[ this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetToolName() ] = vtkTracker::ConvertFlagToString(templhome2templFlags); 

	// RAW_ENCODER_VALUES
	long rawEncoderValuesFlags(TR_OK); 
	double rawEncoderValuesMatrix[16]; 
	vtkMatrix4x4::DeepCopy(rawEncoderValuesMatrix, this->GetRawEncoderValuesTransform(timestamp, rawEncoderValuesFlags)->GetMatrix()); 
	std::ostringstream strRawEncoderValuesTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strRawEncoderValuesTransform << rawEncoderValuesMatrix[i] << " ";
	}
	toolsBufferMatrices[ this->GetTool(RAW_ENCODER_VALUES)->GetToolName() ] = strRawEncoderValuesTransform.str(); 
	toolsStatuses[ this->GetTool(RAW_ENCODER_VALUES)->GetToolName() ] = vtkTracker::ConvertFlagToString(rawEncoderValuesFlags); 
	
	// Get value for PROBE_POSITION, PROBE_ROTATION, TEMPLATE_POSITION tools
	long encoderflags(TR_OK); 
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, encoderflags); 
	
	// PROBE_POSITION
	std::ostringstream strProbePos; 
	strProbePos << probePos; 
	toolsBufferMatrices[ "ProbePosition" ] = strProbePos.str(); 
	toolsStatuses[ "ProbePosition" ] = vtkTracker::ConvertFlagToString(encoderflags); 

	// PROBE_ROTATION
	std::ostringstream strProbeRot; 
	strProbeRot << probeRot; 
	toolsBufferMatrices[ "ProbeRotation" ] = strProbeRot.str(); 
	toolsStatuses[ "ProbeRotation" ] = vtkTracker::ConvertFlagToString(encoderflags); 

	// TEMPLATE_POSITION
	std::ostringstream strTemplatePos; 
	strTemplatePos << templatePos; 
	toolsBufferMatrices[ "TemplatePosition" ] = strTemplatePos.str(); 
	toolsStatuses[ "TemplatePosition" ] = vtkTracker::ConvertFlagToString(encoderflags); 
}

//----------------------------------------------------------------------------
void vtkAMSTracker::GetStepperEncoderValues( int bufferIndex, double &probePosition, double &probeRotation, double &templatePosition, long &flags )
{
	double timestamp(0); 

	this->Lock(); 
	timestamp += this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetTimeStamp(bufferIndex); 
	this->Unlock();

	this->GetStepperEncoderValues(timestamp, probePosition, probeRotation, templatePosition, flags); 
}


//----------------------------------------------------------------------------
void vtkAMSTracker::GetStepperEncoderValues( double timestamp, double &probePosition, double &probeRotation, double &templatePosition, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> rawEncValMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	flags = TR_OK; 
	
	this->Lock(); 
	int buffIndex = this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetIndexFromTime(timestamp); 

	// Get flags
	flags += this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetFlags(buffIndex); 

	// Get raw encoder values
	this->GetTool(RAW_ENCODER_VALUES)->GetBuffer()->GetMatrix(rawEncValMatrix, buffIndex); 

	this->Unlock(); 

	probePosition = rawEncValMatrix->GetElement(ROW_PROBE_POSITION,3); 
	probeRotation = rawEncValMatrix->GetElement(ROW_PROBE_ROTATION,3); 
	templatePosition = rawEncValMatrix->GetElement(ROW_TEMPLATE_POSITION,3); 
}

//----------------------------------------------------------------------------
double vtkAMSTracker::GetProbePosition( int bufferIndex, long &flags)
{
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(bufferIndex, probePos, probeRot, templatePos, flags); 
	return probePos; 
}

//----------------------------------------------------------------------------
double vtkAMSTracker::GetProbePosition( double timestamp, long &flags )
{
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, flags); 
	return probePos; 
}

//----------------------------------------------------------------------------
double vtkAMSTracker::GetProbeRotation( int bufferIndex, long &flags)
{
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(bufferIndex, probePos, probeRot, templatePos, flags); 
	return probeRot; 
}

//----------------------------------------------------------------------------
double vtkAMSTracker::GetProbeRotation( double timestamp, long &flags )
{
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, flags); 
	return probeRot; 
}

//----------------------------------------------------------------------------
double vtkAMSTracker::GetTemplatePosition( int bufferIndex, long &flags)
{
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(bufferIndex, probePos, probeRot, templatePos, flags); 
	return templatePos; 
}

//----------------------------------------------------------------------------
double vtkAMSTracker::GetTemplatePosition( double timestamp, long &flags )
{
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, flags); 
	return templatePos;
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetProbeHomeToProbeTransform( int bufferIndex, long &flags, bool calibratedTransform /*= false*/ )
{
	vtkSmartPointer<vtkMatrix4x4> probeHomeToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	flags = trackerBuffer->GetFlags(bufferIndex); 
	if ( calibratedTransform )
	{
		trackerBuffer->GetCalibratedMatrix(probeHomeToProbeMatrix, bufferIndex); 
	}
	else
	{
		trackerBuffer->GetMatrix(probeHomeToProbeMatrix, bufferIndex); 
	}

	trackerBuffer->Unlock(); 

	this->ProbeHomeToProbeTransform->SetMatrix( probeHomeToProbeMatrix ); 

	return this->ProbeHomeToProbeTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetProbeHomeToProbeTransform( double timestamp, long &flags, bool calibratedTransform /*= false*/ )
{
	vtkSmartPointer<vtkMatrix4x4> probeHomeToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	int bufferIndex = trackerBuffer->GetIndexFromTime(timestamp); 
	flags = trackerBuffer->GetFlags(bufferIndex); 

	if ( calibratedTransform )
	{
		trackerBuffer->GetCalibratedMatrix(probeHomeToProbeMatrix, bufferIndex); 
	}
	else
	{
		trackerBuffer->GetMatrix(probeHomeToProbeMatrix, bufferIndex); 
	}

	trackerBuffer->Unlock(); 

	this->ProbeHomeToProbeTransform->SetMatrix( probeHomeToProbeMatrix ); 

	return this->ProbeHomeToProbeTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetTemplateHomeToTemplateTransform( int bufferIndex, long &flags, bool calibratedTransform /*= false*/ )
{
	vtkSmartPointer<vtkMatrix4x4> templateHomeToTemplateMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	flags = trackerBuffer->GetFlags(bufferIndex); 
	if ( calibratedTransform )
	{
		trackerBuffer->GetCalibratedMatrix(templateHomeToTemplateMatrix, bufferIndex); 
	}
	else
	{
		trackerBuffer->GetMatrix(templateHomeToTemplateMatrix, bufferIndex); 
	}

	trackerBuffer->Unlock(); 

	this->TemplateHomeToTemplateTransform->SetMatrix( templateHomeToTemplateMatrix ); 

	return this->TemplateHomeToTemplateTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetTemplateHomeToTemplateTransform( double timestamp, long &flags, bool calibratedTransform /*= false*/ )
{
	vtkSmartPointer<vtkMatrix4x4> templateHomeToTemplateMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	int bufferIndex = trackerBuffer->GetIndexFromTime(timestamp); 
	flags = trackerBuffer->GetFlags(bufferIndex); 
	
	if ( calibratedTransform )
	{
		trackerBuffer->GetCalibratedMatrix(templateHomeToTemplateMatrix, bufferIndex); 
	}
	else
	{
		trackerBuffer->GetMatrix(templateHomeToTemplateMatrix, bufferIndex); 
	}

	trackerBuffer->Unlock(); 

	this->TemplateHomeToTemplateTransform->SetMatrix( templateHomeToTemplateMatrix ); 

	return this->TemplateHomeToTemplateTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetRawEncoderValuesTransform( int bufferIndex, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> rawEncoderValuesMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(RAW_ENCODER_VALUES)->GetBuffer(); 

	trackerBuffer->Lock(); 

	flags = trackerBuffer->GetFlags(bufferIndex); 
	trackerBuffer->GetMatrix(rawEncoderValuesMatrix, bufferIndex); 

	trackerBuffer->Unlock(); 

	this->RawEncoderValuesTransform->SetMatrix( rawEncoderValuesMatrix ); 

	return this->RawEncoderValuesTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetRawEncoderValuesTransform( double timestamp, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> rawEncoderValuesMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(RAW_ENCODER_VALUES)->GetBuffer(); 

	trackerBuffer->Lock(); 

	int bufferIndex = trackerBuffer->GetIndexFromTime(timestamp); 
	flags = trackerBuffer->GetFlags(bufferIndex); 
	trackerBuffer->GetMatrix(rawEncoderValuesMatrix, bufferIndex); 

	trackerBuffer->Unlock(); 

	this->RawEncoderValuesTransform->SetMatrix( rawEncoderValuesMatrix ); 

	return this->RawEncoderValuesTransform; 
}

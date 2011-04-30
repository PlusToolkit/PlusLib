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
int vtkAMSTracker::Connect()
{
	if (this->Device == NULL)
	{
		this->Device = new AMSStepper(this->GetSerialPort(), this->GetBaudRate() ) ; 
	}

	return this->Device->StartTracking(); 
}

//-----------------------------------------------------------------------------
void vtkAMSTracker::Disconnect()
{
	this->Device->StopTracking(); 
	this->StopTracking(); 
}

//----------------------------------------------------------------------------
int vtkAMSTracker::Probe()
{
	if (this->Tracking)
	{
		return 1;
	}

	if ( !this->Connect() )
	{
		LOG_ERROR("Unable to connect to stepper on port: " << this->GetSerialPort() );
		return 0; 
	}

	this->Disconnect(); 

	return 1; 
} 

//----------------------------------------------------------------------------
int vtkAMSTracker::InternalStartTracking()
{
	if (this->Tracking)
	{
		return 1;
	}

	if (!this->InitAMSTracker())
	{
		LOG_ERROR("Couldn't initialize AMS stepper.");
		this->Tracking = 0;
		return 0;
	} 

	// for accurate timing
	this->Timer->Initialize();

	this->Tracking = 1;

	return 1;
}

//----------------------------------------------------------------------------
int vtkAMSTracker::InternalStopTracking()
{
	delete this->Device; 

	this->Device = 0;

	return 1;
}

//----------------------------------------------------------------------------
void vtkAMSTracker::InternalUpdate()
{
	long flags=0;

	if (!this->Tracking)
	{
		LOG_WARNING("called Update() when AMS stepper was not tracking");
		return;
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
}

//----------------------------------------------------------------------------
bool vtkAMSTracker::InitAMSTracker()
{
	// Connect to device 
	if ( !this->Connect() )
	{
		LOG_ERROR("Unable to connect to stepper on port: " << this->GetSerialPort() );
		return false; 
	}

	int iVerHi=0; int iVerLo=0; int iModelNum=0; int iSerialNum=0;
	if (!this->Device->GetVersionInfo(iVerHi, iVerLo, iModelNum, iSerialNum))
	{
		LOG_ERROR("Couldn't get version info from AMS stepper.");
		return false; 
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

	return true; 
}

//----------------------------------------------------------------------------
void vtkAMSTracker::ReadConfiguration(vtkXMLDataElement* config)
{
	if ( config == NULL ) 
	{
		LOG_WARNING("Unable to find AMSTracker XML data element");
		return; 
	}

	if ( this->ConfigurationData == NULL ) 
	{
		this->ConfigurationData = vtkXMLDataElement::New(); 
	}

	// Save config data
	this->ConfigurationData->DeepCopy(config); 

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
		vtkSmartPointer<vtkXMLDataElement> modelInformation = config->FindNestedElementWithName("ModelInformation"); 
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

	vtkSmartPointer<vtkXMLDataElement> calibration = config->FindNestedElementWithName("Calibration"); 
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
}

//----------------------------------------------------------------------------
void vtkAMSTracker::WriteConfiguration(vtkXMLDataElement* config)
{
	if ( config == NULL )
	{
		config = vtkXMLDataElement::New(); 
	}

	config->SetName("BrachyStepper");  

	config->SetAttribute( "SerialPort", this->GetSerialPort() ); 

	config->SetDoubleAttribute( "BaudRate", this->GetBaudRate() ); 

	config->SetAttribute( "VersionNumber", this->GetVersion() ); 

	config->SetIntAttribute( "ModelNumber", this->GetModelNumber() ); 

	config->SetIntAttribute( "SerialNumber", this->GetSerialNumber() ); 

}


//----------------------------------------------------------------------------
void vtkAMSTracker::ResetCalibration()
{
	this->Device->ResetStepper(); 
}

//----------------------------------------------------------------------------
bool vtkAMSTracker::CalibrateStepper( std::string &calibMsg )
{
	return this->Device->CalibrateStepper(calibMsg); 
}

//----------------------------------------------------------------------------
void vtkAMSTracker::GetTrackerToolBufferStringList(const double timestamp, std::vector<std::string> &toolNames, std::vector<std::string> &toolBufferValues, std::vector<std::string> &toolBufferStatuses)
{
	toolNames.clear(); 
	toolBufferValues.clear(); 
	toolBufferStatuses.clear(); 

	// PROBEHOME_TO_PROBE_TRANSFORM
	long probehome2probeFlags(TR_OK); 
	double probehome2probeMatrix[16]; 
	vtkMatrix4x4::DeepCopy(probehome2probeMatrix, this->GetProbeHomeToProbeTransform(timestamp, probehome2probeFlags)->GetMatrix()); 
	std::ostringstream strProbeHomeToProbeTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strProbeHomeToProbeTransform << probehome2probeMatrix[i] << " ";
	}
	toolBufferStatuses.push_back(vtkTracker::ConvertFlagToString(probehome2probeFlags)); 
	toolNames.push_back( this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetToolName()); 
	toolBufferValues.push_back( strProbeHomeToProbeTransform.str() ); 

	// TEMPLATEHOME_TO_TEMPLATE_TRANSFORM
	long templhome2templFlags(TR_OK); 
	double templhome2templMatrix[16]; 
	vtkMatrix4x4::DeepCopy(templhome2templMatrix, this->GetTemplateHomeToTemplateTransform(timestamp, templhome2templFlags)->GetMatrix()); 
	std::ostringstream strTemplHomeToTemplTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strTemplHomeToTemplTransform << templhome2templMatrix[i] << " ";
	}
	toolBufferStatuses.push_back(vtkTracker::ConvertFlagToString(templhome2templFlags)); 
	toolNames.push_back( this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetToolName()); 
	toolBufferValues.push_back( strTemplHomeToTemplTransform.str() ); 

	// RAW_ENCODER_VALUES
	long rawEncoderValuesFlags(TR_OK); 
	double rawEncoderValuesMatrix[16]; 
	vtkMatrix4x4::DeepCopy(rawEncoderValuesMatrix, this->GetRawEncoderValuesTransform(timestamp, rawEncoderValuesFlags)->GetMatrix()); 
	std::ostringstream strRawEncoderValuesTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strRawEncoderValuesTransform << rawEncoderValuesMatrix[i] << " ";
	}
	toolBufferStatuses.push_back(vtkTracker::ConvertFlagToString(rawEncoderValuesFlags)); 
	toolNames.push_back( this->GetTool(RAW_ENCODER_VALUES)->GetToolName()); 
	toolBufferValues.push_back( strRawEncoderValuesTransform.str() ); 
	
	// Get value for PROBE_POSITION, PROBE_ROTATION, TEMPLATE_POSITION tools
	long encoderflags(TR_OK); 
	double probePos(0), probeRot(0), templatePos(0); 
	this->GetStepperEncoderValues(timestamp, probePos, probeRot, templatePos, encoderflags); 
	
	// PROBE_POSITION
	toolBufferStatuses.push_back( vtkTracker::ConvertFlagToString(encoderflags) ); 
	toolNames.push_back( "ProbePosition" ); 
	std::ostringstream strProbePos; 
	strProbePos << probePos; 
	toolBufferValues.push_back( strProbePos.str() ); 

	// PROBE_ROTATION
	toolBufferStatuses.push_back( vtkTracker::ConvertFlagToString(encoderflags) ); 
	toolNames.push_back( "ProbeRotation" ); 
	std::ostringstream strProbeRot; 
	strProbeRot << probeRot; 
	toolBufferValues.push_back( strProbeRot.str() ); 

	// TEMPLATE_POSITION
	toolBufferStatuses.push_back( vtkTracker::ConvertFlagToString(encoderflags) ); 
	toolNames.push_back( "TemplatePosition" ); 
	std::ostringstream strTemplatePos; 
	strTemplatePos << templatePos; 
	toolBufferValues.push_back( strTemplatePos.str() );
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
vtkTransform* vtkAMSTracker::GetProbeHomeToProbeTransform( int bufferIndex, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> probeHomeToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	flags = trackerBuffer->GetFlags(bufferIndex); 
	trackerBuffer->GetMatrix(probeHomeToProbeMatrix, bufferIndex); 

	trackerBuffer->Unlock(); 

	this->ProbeHomeToProbeTransform->SetMatrix( probeHomeToProbeMatrix ); 

	return this->ProbeHomeToProbeTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetProbeHomeToProbeTransform( double timestamp, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> probeHomeToProbeMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(PROBEHOME_TO_PROBE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	int bufferIndex = trackerBuffer->GetIndexFromTime(timestamp); 
	flags = trackerBuffer->GetFlags(bufferIndex); 
	trackerBuffer->GetMatrix(probeHomeToProbeMatrix, bufferIndex); 

	trackerBuffer->Unlock(); 

	this->ProbeHomeToProbeTransform->SetMatrix( probeHomeToProbeMatrix ); 

	return this->ProbeHomeToProbeTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetTemplateHomeToTemplateTransform( int bufferIndex, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> templateHomeToTemplateMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	flags = trackerBuffer->GetFlags(bufferIndex); 
	trackerBuffer->GetMatrix(templateHomeToTemplateMatrix, bufferIndex); 

	trackerBuffer->Unlock(); 

	this->TemplateHomeToTemplateTransform->SetMatrix( templateHomeToTemplateMatrix ); 

	return this->TemplateHomeToTemplateTransform; 
}

//----------------------------------------------------------------------------
vtkTransform* vtkAMSTracker::GetTemplateHomeToTemplateTransform( double timestamp, long &flags )
{
	vtkSmartPointer<vtkMatrix4x4> templateHomeToTemplateMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	vtkTrackerBuffer *trackerBuffer = this->GetTool(TEMPLATEHOME_TO_TEMPLATE_TRANSFORM)->GetBuffer(); 

	trackerBuffer->Lock(); 

	int bufferIndex = trackerBuffer->GetIndexFromTime(timestamp); 
	flags = trackerBuffer->GetFlags(bufferIndex); 
	trackerBuffer->GetMatrix(templateHomeToTemplateMatrix, bufferIndex); 

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

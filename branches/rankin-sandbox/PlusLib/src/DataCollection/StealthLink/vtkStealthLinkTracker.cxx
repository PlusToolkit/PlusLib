/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/


#include "PlusConfigure.h"
#include "vtkStealthLinkTracker.h"
#include "StealthLink\StealthLink.h"

#include <vtkDirectory.h>
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include <vtkMatrix3x3.h>

#include <fstream>
#include <iostream>
#include <sstream>

 
double cumulativeTime = 0;

class vtkStealthLinkTracker::vtkInternal
{
public:

	friend class vtkStealthLinkTracker;
	
private:

	vtkStealthLinkTracker *External;

	MNavStealthLink::StealthServer	*StealthLinkServer;
	std::string						CurrentFrameName;
	std::string						CurrentInstrumentName;
	MNavStealthLink::Exam 			CurrentExam;
	MNavStealthLink::NavData		NavData;
	MNavStealthLink::Registration   Registration;
	
	vtkSmartPointer<vtkMatrix4x4> ImageToLpsTransformationMatrix;
	vtkSmartPointer<vtkMatrix4x4> InsToTrackerTransMatrix;   // Instrument to Tracker transformation matrix
	vtkSmartPointer<vtkMatrix4x4> FrameToTrackerTransMatrix; // Frame to Tracker transformation matrix
	vtkSmartPointer<vtkMatrix4x4> ImageToTrackerTransMatrix;   /* RAS to Tracker transformation matrix. This matrix is 
															    calculated by multiplying RasToFrame with FrameToTracker
															    transformation matrices. Thus, if frame is out of view,
	                                                            this transformation matrix will be invalid as well.*/

	std::string ServerAddress; // Host IP Address
	std::string PortAddress; // Host Port Address

	static const char* Frame() { return "Frame"; }
	static const char* ImageToFrame() { return "ImageToFrameRegistration"; }

	double ServerInitialTimeInMicroSeconds;
	double AcquiringRegistrationInformationTimeStamp; // Time stamp for when registration is updated
	double RegistrationUpdatePeriodInSec;             // Registration will be updated based on this frequency. If 5, every 5 seconds

	bool InstrumentOutOfView;
	bool FrameOutOfView;


	/*~ Constructor ~*/
	vtkInternal(vtkStealthLinkTracker* external) 
    : External(external)
    , StealthLinkServer(NULL)
	, InstrumentOutOfView(FALSE)
	, FrameOutOfView(FALSE)
	{
		this->InsToTrackerTransMatrix      = vtkSmartPointer<vtkMatrix4x4>::New();
		this->FrameToTrackerTransMatrix    = vtkSmartPointer<vtkMatrix4x4>::New();
		this->ImageToTrackerTransMatrix      = vtkSmartPointer<vtkMatrix4x4>::New();
		this->ImageToLpsTransformationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		this->ServerAddress.clear();
		this->PortAddress.clear();
	}

	/*! Destructor !*/
	~vtkInternal()
	{	
		delete this->StealthLinkServer;
		this->StealthLinkServer=NULL;
		this->External = NULL;
	}

	bool IsStealthServerInitialized()
	{
		if(this->StealthLinkServer == NULL)
		{
			return false;
		}
		return true;
	}
	/*! Check if the Instruments Are Verified !*/
	PlusStatus IsInstrumentVerified(bool &verified)
	{
		MNavStealthLink::Instrument instrument;
		MNavStealthLink::Error err;
		if(!this->StealthLinkServer->get(this->CurrentInstrumentName,instrument,this->StealthLinkServer->getServerTime(err)))
		{
			LOG_ERROR("Error retrieveing the current instrument from the StealthLink Server " << err.what() << "\n");
			return PLUS_FAIL;
		}
		if(instrument.isVerified)
		{
			verified = true;
			return PLUS_SUCCESS;
		}
		verified = false;
		return PLUS_SUCCESS;
	}
	vtkSmartPointer<vtkMatrix4x4> GetInsToTrackerTransMatrix()
	{
		return this->InsToTrackerTransMatrix;
	}
	vtkSmartPointer<vtkMatrix4x4> GetFrameToTrackerTransMatrix()
	{
		return this->FrameToTrackerTransMatrix;
	}
	vtkSmartPointer<vtkMatrix4x4> GetImageToTrackerTransMatrix()
	{
		return this->ImageToTrackerTransMatrix;
	}
	PlusStatus GetValidToolPortNames(std::map<std::string,bool>& validToolPortNames)
	{
		MNavStealthLink::Error err;
		MNavStealthLink::InstrumentNameList instrumentNameList;
		// Get the instrument list
		if(!this->StealthLinkServer->get(instrumentNameList,err))
		{
			LOG_ERROR (" Could not retrieve the instument name list: " << err.reason() << std::endl);
			return PLUS_FAIL;
		}
		for (MNavStealthLink::InstrumentNameList::iterator instrumentNameIterator = instrumentNameList.begin(); instrumentNameIterator!=instrumentNameList.end(); instrumentNameIterator++)
		{
			validToolPortNames[*instrumentNameIterator] = false;
		}
		validToolPortNames[this->Frame()] = false;
		validToolPortNames[this->ImageToFrame()] = false;
		return PLUS_SUCCESS;
	}
	/*! Update the transformation maxtrix of the current instrument !*/
	void UpdateInsToTrackerTransMatrix()
	{
		if( !(this->NavData.instVisibility == MNavStealthLink::Instrument::VISIBLE) && !(this->NavData.instVisibility == MNavStealthLink::Instrument::ALMOST_BLOCKED))
		{
			if(!this->InstrumentOutOfView)
			{
				LOG_TRACE("The instrument is out of the view. Please check the current positions of the instrument\n");
				this->InstrumentOutOfView = TRUE;
			}
			return;
		}
		if(this->InstrumentOutOfView)
		{
			LOG_TRACE("The instrument returned to field of view\n");
			this->InstrumentOutOfView = FALSE;
		}	
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				this->InsToTrackerTransMatrix->SetElement(row, col, this->NavData.localizer_T_instrument [row][col]);
			}
		}
	}
	/*! Update the transformation maxtrix of the current frame !*/
	void UpdateFrameToTrackerTransMatrix()
	{
		if( !(this->NavData.frameVisibility == MNavStealthLink::Frame::VISIBLE) && !(this->NavData.frameVisibility == MNavStealthLink::Frame::ALMOST_BLOCKED))
		{
			if(!this->FrameOutOfView)
			{
				LOG_TRACE("The frame is not trackable. Please check the current positions of the tools.\n");
				this->FrameOutOfView = TRUE;
			}
			return;
		}
		if(this->FrameOutOfView)
		{
			LOG_TRACE("The frame is now trackable.\n");
			this->FrameOutOfView = FALSE;
		}
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				this->FrameToTrackerTransMatrix->SetElement(row, col, this->NavData.frame_T_localizer [row][col]); // from localizer Space to frame space
			}
		}
		//the transformation matrix given by NavData is from Localizer(Tracker) space to frame space and we need the inverse of it
		this->FrameToTrackerTransMatrix->Invert();   
	}
	/* Update the transformation maxtrix of the current registration !*/
	void UpdateImageToTrackerTransMatrix()
	{
		//The matrix is calculated by ImageToFrameTransMatrix*FrameToTrackerTransMatrix. So if frame is out of view, no need to do the math. 
		if(this->FrameOutOfView)
		{
			return;
		}
		vtkMatrix4x4* frameToLpsTransMatrix = vtkMatrix4x4::New();
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				frameToLpsTransMatrix->SetElement(row, col, this->Registration.regExamMM_T_frame [row][col]); // from Frame coordinates to lps coordinates
			}
		}
		vtkSmartPointer<vtkMatrix4x4> lpsToFrameTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Invert(frameToLpsTransMatrix,lpsToFrameTransMatrix);
	//	ImageToTrackerTransMatrix = trackerFromImage = trackerFromFrame*FrameFromImage = trackerFromFrame*FrameFromLps * LpsFromImage 
	//																				   = FrameToTrakcer * LpsToFrame * ImageToLps
	//																				   = FrameToTrakcer * inv(frameToLps) * ImageToLps
       vtkSmartPointer<vtkMatrix4x4> lpsToTrackerTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(this->FrameToTrackerTransMatrix,lpsToFrameTransMatrix,lpsToTrackerTransMatrix);
		vtkMatrix4x4::Multiply4x4(lpsToTrackerTransMatrix,this->ImageToLpsTransformationMatrix,this->ImageToTrackerTransMatrix);
	}
};

/****************************************************************************/ 

vtkStandardNewMacro(vtkStealthLinkTracker);

//----------------------------------------------------------------------------
vtkStealthLinkTracker::vtkStealthLinkTracker()
{
	this->Internal = new vtkInternal(this);
	this->TrackerTimeToSystemTimeComputed = false;
	this->FrameNumber = 0;
	this->ApplicationStartTimestamp = vtkAccurateTimer::GetInstance()->GetDateAndTimeString();

	this->RequireImageOrientationInConfiguration = false;
	this->RequireFrameBufferSizeInDeviceSetConfiguration = false;
	this->RequireAcquisitionRateInDeviceSetConfiguration = false;
	this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = true;
	this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
	this->RequireUsImageOrientationInDeviceSetConfiguration = false;
	this->RequireRfElementInDeviceSetConfiguration = false;

	// No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
	this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkStealthLinkTracker::~vtkStealthLinkTracker() 
{
	if (this->Internal->IsStealthServerInitialized())
	{
		this->InternalDisconnect();
	}
	delete this->Internal;
	this->Internal = NULL;
}
//---------------------------------------------------------------------------
void vtkStealthLinkTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetSdkVersion(std::string& version)
{
	MNavStealthLink::Version serverVersion;
	MNavStealthLink::Error err;
	if(!this->Internal->StealthLinkServer->get(serverVersion,this->Internal->StealthLinkServer->getServerTime(),err))
	{
		LOG_ERROR("Failed to acquire the version of the StealthLinkServer " << err.reason() << "\n");
		return PLUS_FAIL;
	}
	version = (unsigned) serverVersion.major + (unsigned) serverVersion.minor;
	return PLUS_SUCCESS;
}
PlusStatus vtkStealthLinkTracker::AreInstrumentPortNamesValid(bool& valid)
{
	std::map<std::string,bool> validToolPortNames;
	if(!this->Internal->GetValidToolPortNames(validToolPortNames))
	{
		valid = false;
		return PLUS_FAIL;
	}
	for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
	{
		if(validToolPortNames.find(std::string(toolIterator->second->GetPortName())) == validToolPortNames.end()) // means that the tool is not found in the map
		{
			LOG_ERROR( toolIterator->second->GetPortName() << " has not been found in the .xml file. Please make sure to use correct port names for the tools\n");
			valid = false;
			return PLUS_FAIL;
		}
		if(validToolPortNames.find(std::string(toolIterator->second->GetPortName()))->second == true)
		{
			LOG_ERROR( toolIterator->second->GetPortName() << " has been defined in the .xml file. Please make sure to define every tool once\n");
			valid = false;
			return PLUS_FAIL;
		}
		validToolPortNames.find(std::string(toolIterator->second->GetPortName()))->second = true;
	}
	valid = true;
	return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------
void vtkStealthLinkTracker::AcquireInitialToolsVisibilityStatus()
{
	/*! Instrument Visibility Status - No need for Visible since the default value is TRACKABLE !*/
	if(!(this->Internal->NavData.instVisibility == MNavStealthLink::Instrument::VISIBLE || this->Internal->NavData.instVisibility == MNavStealthLink::Instrument::ALMOST_BLOCKED))
	{
		this->Internal->InstrumentOutOfView = TRUE;		
		LOG_TRACE("The Instrument is not trackable\n");
	}

	/*! Frame Visibility Status - No need for Visible since the default value is TRACKABLE !*/
	if(!(this->Internal->NavData.frameVisibility != MNavStealthLink::Frame::VISIBLE || this->Internal->NavData.frameVisibility != MNavStealthLink::Frame::ALMOST_BLOCKED))
	{
		this->Internal->FrameOutOfView = TRUE;	
		LOG_TRACE("The Frame is not trackable\n");
	}
}
//------------------------------------------------------------------------------
void vtkStealthLinkTracker::UpdateCurrentToolsNames()
{
	if(this->Internal->NavData.frameName != this->Internal->CurrentFrameName && !this->Internal->NavData.frameName.empty())
	{
		LOG_TRACE (" The current frame has changed from: " << this->Internal->CurrentFrameName << " to: " << this->Internal->NavData.frameName << "\n");
		this->Internal->CurrentFrameName = this->Internal->NavData.frameName;
	}
	if(this->Internal->NavData.instrumentName != this->Internal->CurrentInstrumentName && !this->Internal->NavData.instrumentName.empty())
	{
		LOG_TRACE (" The current frame has been change from: " << this->Internal->CurrentInstrumentName << " to: " << this->Internal->NavData.instrumentName << "\n");
		this->Internal->CurrentInstrumentName = this->Internal->NavData.instrumentName;
		bool verified;
		this->Internal->IsInstrumentVerified(verified);
		if(!verified)
			LOG_INFO("The instrument: " << this->Internal->CurrentInstrumentName << " is not verified\n");
	}
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalStartRecording()
{
  if (!this->Internal->IsStealthServerInitialized())
  {
    LOG_ERROR("InternalStartRecording failed: StealthLinkServer has not been initialized");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalStopRecording()
{
  // No need to do anything here, as the StealthLinkServer only performs grabbing on request
  return PLUS_SUCCESS;
}



//----------------------------------------------------------------------------

PlusStatus vtkStealthLinkTracker::ReadConfiguration( vtkXMLDataElement* config )
{
  // Read superclass configuration first
  Superclass::ReadConfiguration(config); 

  LOG_TRACE( "vtkStealthLinkTracker::ReadConfiguration" ); 
  if ( config == NULL ) 
  {
    LOG_ERROR("Unable to find vtkStealthLinkTracker XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(config);
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }  
  if(!trackerConfig->GetAttribute("ServerAddress"))
  {
	  LOG_ERROR("Cannot find the ServerAddress information. Please make sure that the xml file contains the information\n");
	  return PLUS_FAIL;
  }
  if(!trackerConfig->GetAttribute("PortAddress"))
  {
	  LOG_ERROR("Cannot find the PortAddress information. Please make sure that the xml file contains the information\n");
	  return PLUS_FAIL;
  }
  if(!trackerConfig->GetAttribute("RegistrationUpdatePeriod"))
  {
	  LOG_INFO("RegistrationUpdatePeriod not found in the xml file. The registration will be acquired only when connected to the Stealth Server\n");
	  this->Internal->RegistrationUpdatePeriodInSec = 0;
  }
  else
  {
	  this->Internal->RegistrationUpdatePeriodInSec = std::atof(trackerConfig->GetAttribute("RegistrationUpdatePeriod"));
	  LOG_INFO("RegistrationUpdatePeriod found. Registration will be updated every " << trackerConfig->GetAttribute("RegistrationUpdatePeriod") << " second\n");
  }
  this->Internal->ServerAddress = trackerConfig->GetAttribute("ServerAddress");
  this->Internal->PortAddress   = trackerConfig->GetAttribute("PortAddress");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  if ( rootConfigElement == NULL )
  {
    LOG_ERROR("Configuration is invalid");
    return PLUS_FAIL;
  }

  // Write configuration 
  Superclass::WriteConfiguration(rootConfigElement); 

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(rootConfigElement);
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
} 
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalConnect()
{ 

	LOG_TRACE( "vtkStealthLinkTracker::InternalConnect" );
	if (this->Internal->IsStealthServerInitialized())
	{
		LOG_DEBUG("Already connected to StealthLink");
		return PLUS_SUCCESS;
	}

	if (this->Internal->ServerAddress.empty() || this->Internal->PortAddress.empty())
	{
		LOG_ERROR("Cannot connect: Server or Port Addresses are not assigned\n");
		return PLUS_FAIL;
	}
	LOG_TRACE("Server Address: " << this->Internal->ServerAddress << " " << "Port Address: " << this->Internal->PortAddress << "\n");
	
	delete this->Internal->StealthLinkServer;
	this->Internal->StealthLinkServer = NULL;
	this->Internal->StealthLinkServer = new MNavStealthLink::StealthServer(this->Internal->ServerAddress,this->Internal->PortAddress);
	MNavStealthLink::Error error;
	if (!this->Internal->StealthLinkServer->connect(error))
	{
		LOG_ERROR(" Failed to connect to Stealth server application on host: " << error.what() << "\n");
		return PLUS_FAIL;
	}
	this->Internal->ServerInitialTimeInMicroSeconds = (double) this->Internal->StealthLinkServer->getServerTime();
	this->FrameNumber = 0;
	//Check if the instrument port names in the config file are valid
	bool valid;
	this->AreInstrumentPortNamesValid(valid);
	if(!valid)
		return PLUS_FAIL;
	//Check of the Localizer(Tracker) is connected
	bool connected;
	this->IsLocalizerConnected(connected);
	if(!connected)
	{
		LOG_ERROR("Localizer(Tracker) is not connected. Please check the StealthLink Server\n")
		return PLUS_FAIL;
	}
	//Update current navigation data information in order to acquire initial the current instrument and frame name 
	this->UpdateCurrentNavigationData();
	//Get the initial information regarding the current registration
	this->UpdateCurrentRegistration();

	this->Internal->CurrentFrameName = this->Internal->NavData.frameName;
	this->Internal->CurrentInstrumentName = this->Internal->NavData.instrumentName;
	this->Internal->AcquiringRegistrationInformationTimeStamp = 0.0;

	this->AcquireInitialToolsVisibilityStatus();

	//Get the time difference between the StealthServer and the vtkTimer
	try
	{
		this->TrackerTimeToSystemTimeSec =  vtkAccurateTimer::GetSystemTime()- ((double) this->Internal->StealthLinkServer->getServerTime() - this->Internal->ServerInitialTimeInMicroSeconds);
	}
	catch(MNavStealthLink::Error err)
	{
		LOG_ERROR (" Could not retrieve the server time: " << err.reason() << std::endl);
		return PLUS_FAIL;
	}
	this->TrackerTimeToSystemTimeComputed = true;
	return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalDisconnect()
{ 

	if (this->Internal->IsStealthServerInitialized())
	{
		this->Internal->StealthLinkServer->disconnect(); 
		delete this->Internal->StealthLinkServer;
		this->Internal->StealthLinkServer = NULL;
		this->TrackerTimeToSystemTimeComputed = false;
		this->Internal->AcquiringRegistrationInformationTimeStamp = 0.0;
		this->Internal->RegistrationUpdatePeriodInSec = 0.0;
	}
	return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalUpdate()
{
	PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

	if (!this->Internal->IsStealthServerInitialized())
	{
		LOG_ERROR("InternalUpdate failed: StealthLinkServer has not been initialized");
		return PLUS_FAIL;
	}	
	if(!this->TrackerTimeToSystemTimeComputed)
	{
		LOG_ERROR (" Tracker time to system time difference is not calculated\n");
	}

	double timeSystemSec=0.0;
	double timeAtStart = vtkAccurateTimer::GetSystemTime();
	try
	{
		timeSystemSec = (double) this->Internal->StealthLinkServer->getServerTime() - this->Internal->ServerInitialTimeInMicroSeconds + this->TrackerTimeToSystemTimeSec;
	}
	catch(MNavStealthLink::Error err)
	{
		LOG_ERROR (" Could not retrieve the server time: " << err.reason() << std::endl);
		return PLUS_FAIL;
	}	
	//LOG_INFO("The time difference is = " << vtkAccurateTimer::GetSystemTime() - timeAtStart << " Cumulative time = " << vtkAccurateTimer::GetSystemTime() << " Frame = " << this->FrameNumber << "\n");

	//----------------------------------------------------------
	if(!this->UpdateCurrentNavigationData())
	{
		return PLUS_FAIL;
	}
	for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
	{
		//if the wanted transformation is the frameToTracker
		if(!strcmp(toolIterator->second->GetPortName(),this->Internal->Frame()))
		{
			if(this->Internal->FrameOutOfView == FALSE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetFrameToTrackerTransMatrix(), TOOL_OK, timeSystemSec, timeSystemSec);
			}
			else if(this->Internal->FrameOutOfView == TRUE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetFrameToTrackerTransMatrix(), TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
			}
		}
		// if the wanted transformation is rasToTracker
		else if(!strcmp(toolIterator->second->GetPortName(),this->Internal->ImageToFrame()))
		{
			//Acquire the registration information with a frequency 1/RegistrationUpdatePeriodInSec as defined in the config file
			if(this->Internal->RegistrationUpdatePeriodInSec> 0 && (vtkAccurateTimer::GetSystemTime() - this->Internal->AcquiringRegistrationInformationTimeStamp) >= this->Internal->RegistrationUpdatePeriodInSec)
			{
				if(!this->UpdateCurrentRegistration())
				{
					return PLUS_FAIL;
				}
			}
			// If frame is not out of view, the RasToTrackerTransMatrix is valid
			this->Internal->UpdateImageToTrackerTransMatrix();
			if(this->Internal->FrameOutOfView == FALSE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetImageToTrackerTransMatrix(), TOOL_OK, timeSystemSec, timeSystemSec);
			}
			else if(this->Internal->FrameOutOfView == TRUE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetImageToTrackerTransMatrix(), TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
			}
		}
		// if the wanted transformation is any tool to Trackker example sytlusToTracker, probeToTracker etc
		else
		{
			if(!strcmp(toolIterator->second->GetPortName(),this->Internal->CurrentInstrumentName.c_str()))
			{
				if(this->Internal->InstrumentOutOfView == FALSE)
				{
					this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetInsToTrackerTransMatrix(), TOOL_OK, timeSystemSec, timeSystemSec);
				}
				else if(this->Internal->InstrumentOutOfView == TRUE)
				{
					this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetInsToTrackerTransMatrix(), TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
				}
			}
			else
			{
				vtkMatrix4x4* transformMatrixForNotTrackedTool = vtkMatrix4x4::New();
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), transformMatrixForNotTrackedTool, TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
				transformMatrixForNotTrackedTool->Delete();
			}
		}
	}
	this->FrameNumber++;
	return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::UpdateCurrentNavigationData()
{
	MNavStealthLink::Error err;
	if(!this->Internal->StealthLinkServer->get(this->Internal->NavData,this->Internal->StealthLinkServer->getServerTime(),err))
	{	
		LOG_ERROR(" Failed to acquire the navigation data from StealthLink Server: " <<  err.reason() << " " << err.what() << "\n");
		return PLUS_FAIL;
	}
	this->UpdateCurrentToolsNames();
	this->Internal->UpdateFrameToTrackerTransMatrix();
	this->Internal->UpdateInsToTrackerTransMatrix();
	return PLUS_SUCCESS;
}

PlusStatus vtkStealthLinkTracker::IsLocalizerConnected(bool& connected)
{
	MNavStealthLink::LocalizerInfo localizerInfo;
	MNavStealthLink::Error		   err;
	if(!this->Internal->StealthLinkServer->get(localizerInfo,err))
	{
		LOG_ERROR("Cannot retrieve the localizer info " << err.reason() << "\n");
		connected = false;
		return PLUS_FAIL;
	}
	if(localizerInfo.isConnected)
	{
		connected = true;
		return PLUS_SUCCESS;
	}
	connected = false;
	return PLUS_SUCCESS;
}
void vtkStealthLinkTracker::RemoveForbiddenCharactersFromPatientsName(std::string& patientName)
{
	std::vector<int> spacePlaces;
	for(int i=0; i<patientName.size(); i++)\
	{
		int asciiCode = (int) patientName[i];
		if(patientName[i] == ' ')
		{
			spacePlaces.push_back(i);
		}
		else if(patientName[i] == '\\' || patientName[i] == '/')
		{	
			patientName[i] = '-';
		}
		else if(asciiCode<48 || (asciiCode>57 && asciiCode<65) || (asciiCode>90 && asciiCode<97) || asciiCode>122)
		{
				patientName[i] = ' ';
				spacePlaces.push_back(i);
		}
	}
	std::string subStrPatientName;
	int placement = 0;
	for(std::vector<int>::iterator it = spacePlaces.begin(); it!=spacePlaces.end(); it++)
	{
		subStrPatientName=patientName.substr(*it+1-placement);
		patientName=patientName.substr(0,*it-placement);
		patientName = patientName + subStrPatientName;
		placement++;
	}
}
//--------
PlusStatus vtkStealthLinkTracker::UpdateCurrentExam()
{
	PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
	MNavStealthLink::Error err;
	if(!this->Internal->StealthLinkServer->get(this->Internal->CurrentExam,this->Internal->StealthLinkServer->getServerTime(),err))
	{
		LOG_ERROR(" Failed to acquire the current exam. Please make sure to select an exam: " <<  err.what() << "\n");
		return PLUS_FAIL;
	}
	return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetDicomImage(std::string dicomImagesOutputDirectory, std::string& examImageDirectory)
{
	PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

 	vtkDirectory::MakeDirectory(dicomImagesOutputDirectory.c_str());
	std::string patientName;
	if(!this->GetPatientName(patientName))
	{
		return PLUS_FAIL;
	}
	this->RemoveForbiddenCharactersFromPatientsName(patientName);
	examImageDirectory = std::string(dicomImagesOutputDirectory) + std::string("/") + std::string(patientName) + std::string("_") + std::string(this->ApplicationStartTimestamp);
	try 
	{
		this->Internal->CurrentExam.getExamData(*(this->Internal->StealthLinkServer),examImageDirectory);
	}
	catch (MNavStealthLink::Error error) 
	{
		LOG_ERROR("Error getting images from StealthLink: " << error.what() << "\n");
		return PLUS_FAIL;
	}
	return PLUS_SUCCESS;
}
//--------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::UpdateCurrentRegistration()
{
	PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
	MNavStealthLink::Error err;
	if(!this->Internal->StealthLinkServer->get(this->Internal->Registration,this->Internal->StealthLinkServer->getServerTime(),err))
	{
		LOG_ERROR(" Failed to acquire the current registraion: " <<  err.what() << "\n");
		return PLUS_FAIL;
	}
	this->Internal->UpdateImageToTrackerTransMatrix();
	this->Internal->AcquiringRegistrationInformationTimeStamp = vtkAccurateTimer::GetSystemTime(); 
	return PLUS_SUCCESS;
}
//------
PlusStatus vtkStealthLinkTracker::GetPatientName(std::string& patientName)
{
	if(this->Internal->CurrentExam.patientName.empty())
	{
		LOG_INFO("Please call UpdateCurrentExam before calling GetPatientName function");
		return PLUS_FAIL;
	}
	patientName = this->Internal->CurrentExam.patientName;
	return PLUS_SUCCESS;
}
PlusStatus vtkStealthLinkTracker::GetPatientId(std::string& patientId)
{
	if(this->Internal->CurrentExam.patientId.empty())
	{
		LOG_INFO("Please call UpdateCurrentExam before calling GetPatientName function");
		return PLUS_FAIL;
	}
	patientId = this->Internal->CurrentExam.patientId;
	return PLUS_SUCCESS;
}
//-----------------------------------------------
std::string vtkStealthLinkTracker::GetApplicationStartTimestamp()
{
	return this->ApplicationStartTimestamp;
}
void vtkStealthLinkTracker::SetImageToLpsTransformationMatrix(vtkMatrix4x4* imageToLpsTransformationMatrix)
{
	this->Internal->ImageToLpsTransformationMatrix->DeepCopy(imageToLpsTransformationMatrix);
}
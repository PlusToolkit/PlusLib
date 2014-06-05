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
#include "vtkDICOMImageReader.h"
#include <vtkImageFlip.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

class vtkStealthLinkTracker::vtkInternal
{
public:

	friend class vtkStealthLinkTracker;

private:

	vtkStealthLinkTracker *External;

	MNavStealthLink::StealthServer *StealthLinkServer;
	MNavStealthLink::Exam CurrentExam;
	MNavStealthLink::NavData NavData;
	MNavStealthLink::Registration Registration;

	vtkSmartPointer<vtkTransformRepository> TransformRepository; //this is used to calculate the transformation of the image in the demanded reference frame
	vtkSmartPointer<vtkMatrix4x4> IjkToExamRpiTransMatrix; // Medtronic excludes the orientation, this is the ijkToRpi without taking orientation into accout and thinking the origin is at 0 0 0
	vtkSmartPointer<vtkMatrix4x4> IjkToRasTransMatrix; // ijkToRpi with orientation taken into account
	vtkSmartPointer<vtkMatrix4x4> InsToTrackerTransMatrix;   // Instrument to Tracker transformation matrix
	vtkSmartPointer<vtkMatrix4x4> FrameToTrackerTransMatrix; // Frame to Tracker transformation matrix
	vtkSmartPointer<vtkMatrix4x4> RasToTrackerTransMatrix;   /* RastoTracker transformation matrix. This matrix is 
																													 calculated by multiplying FramToTracker * RasToFrame
																													 transformation matrices. Thus, if frame is out of view,
																													 this transformation matrix will be invalid as well.*/

	std::string CurrentFrameName;
	std::string CurrentInstrumentName;
	std::pair<std::string,std::string> ExamIdImageIdPair; // imageId as created for slicer and unique exam name for stealthlink 
	std::string ServerAddress; // Host IP Address
	std::string PortAddress; // Host Port Address
	std::string DicomImagesOutputDirectory; //The folder where dicom will be sent from the server
	std::string DeviceId;

	static const char* Frame() { return "Frame"; }
	static const char* RasToFrame() { return "RasToFrameRegistration"; }

	double ServerInitialTimeInMicroSeconds;
	double AcquiringRegistrationInformationTimeStamp; // Time stamp for when registration is updated
	double RegistrationUpdatePeriodInSec;             // Registration will be updated based on this frequency. If 5, every 5 seconds

	int ExamCounter; 

	bool InstrumentOutOfView;
	bool FrameOutOfView;
	bool GetImageCommandRequested;
	bool TransformRepositoryUpdated;

	/*~ Constructor ~*/
	vtkInternal(vtkStealthLinkTracker* external) 
		: External(external)
		, StealthLinkServer(NULL)
		, InstrumentOutOfView(FALSE)
		, FrameOutOfView(FALSE)
		, GetImageCommandRequested(FALSE)
		, TransformRepositoryUpdated(FALSE)
	{
		this->InsToTrackerTransMatrix   = vtkSmartPointer<vtkMatrix4x4>::New();
		this->FrameToTrackerTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		this->RasToTrackerTransMatrix   = vtkSmartPointer<vtkMatrix4x4>::New();
		this->IjkToExamRpiTransMatrix   = vtkSmartPointer<vtkMatrix4x4>::New();
		this->IjkToRasTransMatrix			  = vtkSmartPointer<vtkMatrix4x4>::New();
		this->TransformRepository       = vtkSmartPointer<vtkTransformRepository>::New();
		this->ServerAddress.clear();
		this->PortAddress.clear();
		this->DicomImagesOutputDirectory.clear();
		this->DeviceId.clear();
	}

	/*! Destructor !*/
	~vtkInternal()
	{	
		delete this->StealthLinkServer;
		this->StealthLinkServer=NULL;
		this->External = NULL;
		this->ServerAddress.clear();
		this->PortAddress.clear();
		this->DicomImagesOutputDirectory.clear();
		this->DeviceId.clear();
		this->ExamCounter = 1;
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
	void SetIjkToExamRpiTransformationMatrix(vtkMatrix4x4* ijkToExamRpiTransformationMatrix)
	{
		this->IjkToExamRpiTransMatrix = ijkToExamRpiTransformationMatrix;
	}
	void SetIjkToRasTransformationMatrix(vtkMatrix4x4* ijkToRasTransformationMatrix)
	{
		this->IjkToRasTransMatrix = ijkToRasTransformationMatrix;
	}
	vtkSmartPointer<vtkMatrix4x4> GetInsToTrackerTransMatrix()
	{
		return this->InsToTrackerTransMatrix;
	}
	vtkSmartPointer<vtkMatrix4x4> GetFrameToTrackerTransMatrix()
	{
		return this->FrameToTrackerTransMatrix;
	}
	vtkSmartPointer<vtkMatrix4x4> GetRasToTrackerTransMatrix()
	{
		return this->RasToTrackerTransMatrix;
	}
	void SetGetImageCommandRequested(bool getImageCommandRequested)
	{
		this->GetImageCommandRequested = getImageCommandRequested;
	}
	bool GetGetImageCommandRequested()
	{
		return this->GetImageCommandRequested;
	}
	void SetTransformRepositoryUpdated(bool transformRepositoryUpdated)
	{
		this->TransformRepositoryUpdated = transformRepositoryUpdated;
	}
	bool GetTransformRepositoryUpdated()
	{
		return this->TransformRepositoryUpdated;
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
		validToolPortNames[this->RasToFrame()] = false;
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
		vtkSmartPointer<vtkMatrix4x4> insToTracker_Lps = vtkSmartPointer<vtkMatrix4x4>::New();
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
		vtkSmartPointer<vtkMatrix4x4> trackerToFrame = vtkSmartPointer<vtkMatrix4x4>::New();
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				trackerToFrame->SetElement(row, col, this->NavData.frame_T_localizer [row][col]); // from localizer Space to frame space
			}
		}
		//the transformation matrix given by NavData is from Localizer(Tracker) space to frame space and we need the inverse of it
		vtkMatrix4x4::Invert(trackerToFrame,this->FrameToTrackerTransMatrix);   
	}
	/* Update the transformation maxtrix of the current image !*/
	void UpdateRasToTrackerTransMatrix()
	{
		//The matrix is calculated by ImageToFrameTransMatrix*FrameToTrackerTransMatrix. So if frame is out of view, no need to do the math. 
		if(this->FrameOutOfView)
		{
			return;
		}
		vtkSmartPointer<vtkMatrix4x4> frameToRegExamTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // this is from frame to image used in the registration 
		vtkSmartPointer<vtkMatrix4x4> regExamToExamRpiTransMatrix  = vtkSmartPointer<vtkMatrix4x4>::New();  // from the exam image to registration image, if they are the same then the matrix is idendity
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				frameToRegExamTransMatrix->SetElement(row, col, this->Registration.regExamMM_T_frame [row][col]);
				regExamToExamRpiTransMatrix->SetElement(row,col,this->CurrentExam.examMM_T_regExamMM[row][col]);
			}
		}
		//examRpiToFrame = regExamToFrame * examRpiToRegExam so we need the inverse of the two matrices
		vtkSmartPointer<vtkMatrix4x4> regExamToFrameTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Invert(frameToRegExamTransMatrix,regExamToFrameTransMatrix);
		vtkSmartPointer<vtkMatrix4x4> examRpiToRegExamTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Invert(regExamToExamRpiTransMatrix,examRpiToRegExamTransMatrix);

		//examToFrame = regExamToFrame * ExamToRegExam
		vtkSmartPointer<vtkMatrix4x4> examRpiToFrameTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(regExamToFrameTransMatrix,examRpiToRegExamTransMatrix,examRpiToFrameTransMatrix);

		// medtronic stores the image in rpi and does not include orientation.  so we need to do some extra math to include the orientation
		// We need to get to RasToFrame. We have examRpiToFrame which is stored in rpi with no information of orientation. We have ijkToRas and ijkToExamRpi transformation matrices.
		// RasToFrame = FrameFromRas = FrameFromExamRpi * ExamRpiFromRas
		//													 = FrameFromExamRpi * ExamRpiFromIjk * IjkFromRas
		//													 = ExamRpiToFrame * IjkToExamRpi * RasToIjk

		// we have ijkToRas so we need to invert it to have rasToIjk
		vtkSmartPointer<vtkMatrix4x4> rasToIjkTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Invert(this->IjkToRasTransMatrix,rasToIjkTransMatrix);

		//Now we just need to multiply the matrices
		//First, let's multiplly examRpiToFrame and IjkToExamRpi: this will give us ijkToFrame
		vtkSmartPointer<vtkMatrix4x4> ijkToFrameTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(examRpiToFrameTransMatrix,this->IjkToExamRpiTransMatrix,ijkToFrameTransMatrix);

		// we now multiply ijkToFrameTransMatrix and RasToIjk which will give rasToFrame
		vtkSmartPointer<vtkMatrix4x4> rasToFrameTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(ijkToFrameTransMatrix,rasToIjkTransMatrix,rasToFrameTransMatrix);

		//Final step is to get rasToTracker which is done by frameToTracker * rasToFrame 
		vtkSmartPointer<vtkMatrix4x4> lpsToTrackerTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(this->FrameToTrackerTransMatrix,rasToFrameTransMatrix,this->RasToTrackerTransMatrix);
	}
};

/****************************************************************************/ 

vtkStandardNewMacro(vtkStealthLinkTracker);

//----------------------------------------------------------------------------
vtkStealthLinkTracker::vtkStealthLinkTracker()
{
	this->Internal = new vtkInternal(this);
	this->TrackerTimeToSystemTimeComputed = false;
	this->DateAndTimeString = vtkAccurateTimer::GetInstance()->GetDateAndTimeString();

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
		{
			LOG_INFO("The instrument: " << this->Internal->CurrentInstrumentName << " is not verified\n");
		}
	}
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::UpdateTransformRepository(vtkTransformRepository* sharedTransformRepository)
{
	//PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
	if (sharedTransformRepository==NULL)
	{
		LOG_ERROR("vtkVirtualVolumeReconstructor::UpdateTransformRepository: shared transform repository is invalid");
		return PLUS_FAIL;
	}
	// Create a copy of the transform repository to allow using it for volume reconstruction while being also used in other threads
	this->Internal->TransformRepository->DeepCopy(sharedTransformRepository,true);
	this->Internal->SetTransformRepositoryUpdated(TRUE);
	return PLUS_SUCCESS;
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
	if(trackerConfig->GetAttribute("Id"))
	{
		std::string deviceId(this->DeviceId);
		if(deviceId.size() > 17)
		{
			LOG_ERROR("The device id is too long for message sending purposes. Please choose a shorter device Id. Example: SLD1");
			return PLUS_FAIL;
		}
		else if(deviceId.size() > 10)
		{
			LOG_WARNING("The id is too long for message sending purposes. If you want to receive images from the server, the image name may not contain the desciption of the image. Please choose a shorter device Id. Example: SLD1");
		}
	}
	if(!trackerConfig->GetAttribute("RegistrationUpdatePeriod"))
	{
		LOG_INFO("RegistrationUpdatePeriod not found in the xml file. The registration will be acquired only when connected to the Stealth Server\n");
		this->Internal->RegistrationUpdatePeriodInSec = 0;
	}
	else
	{
		this->Internal->RegistrationUpdatePeriodInSec = std::atof(trackerConfig->GetAttribute("RegistrationUpdatePeriod"));
		if(this->Internal->RegistrationUpdatePeriodInSec <=0)
		{
			LOG_INFO("RegistrationUpdatePeriod is found. It is equal to 0, thus the registration will be acquired only when connected to the Stealth Server");
		}
		else
		{
			LOG_INFO("RegistrationUpdatePeriod found. Registration will be updated every " << trackerConfig->GetAttribute("RegistrationUpdatePeriod") << " second\n");
		}
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
	this->Internal->DicomImagesOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory() +  std::string("/StealthLinkDicomOutput");
	this->Internal->ExamCounter = 1;
	//PlusCommon::ImageMetaDataList imageMetaData;
	//this->GetImageMetaData(imageMetaData);
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
		this->Internal->GetImageCommandRequested = FALSE;
		this->Internal->TransformRepositoryUpdated = FALSE;
		this->Internal->AcquiringRegistrationInformationTimeStamp = 0.0;
		this->Internal->RegistrationUpdatePeriodInSec = 0.0;
		this->FrameNumber = 0;
		this->Internal->ExamCounter = 1;
		//TODO  ask if we need to delete the matrices and other elements
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
	try
	{
		timeSystemSec = (double) this->Internal->StealthLinkServer->getServerTime() - this->Internal->ServerInitialTimeInMicroSeconds + this->TrackerTimeToSystemTimeSec;
	}
	catch(MNavStealthLink::Error err)
	{
		LOG_ERROR (" Could not retrieve the server time: " << err.reason() << std::endl);
		return PLUS_FAIL;
	}	
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
		else if(!strcmp(toolIterator->second->GetPortName(),this->Internal->RasToFrame()))
		{
			//Acquire the registration information with a frequency 1/RegistrationUpdatePeriodInSec as defined in the config file
			static double timePassed = vtkAccurateTimer::GetSystemTime() - this->Internal->AcquiringRegistrationInformationTimeStamp;
			if(this->Internal->RegistrationUpdatePeriodInSec> 0 && timePassed >= this->Internal->RegistrationUpdatePeriodInSec && this->Internal->GetImageCommandRequested == TRUE)
			{
				if(!this->UpdateCurrentRegistration())
				{
					return PLUS_FAIL;
				}
			}
			// If frame is not out of view, the RasToTrackerTransMatrix is valid
			this->Internal->UpdateRasToTrackerTransMatrix();
			if(this->Internal->FrameOutOfView == FALSE && this->Internal->GetImageCommandRequested == TRUE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetRasToTrackerTransMatrix(), TOOL_OK, timeSystemSec, timeSystemSec);
			}
			else if(this->Internal->FrameOutOfView == TRUE || this->Internal->GetImageCommandRequested == FALSE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->GetRasToTrackerTransMatrix(), TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
			}
		}
		// if the wanted transformation is any tool to Tracker example sytlusToTracker, probeToTracker etc
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
//-----------------------------------------------------
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
//---------------------------------------------------------------------------
void vtkStealthLinkTracker::GetIjkToEmbeddedReferenceFrameTransform(vtkMatrix4x4* ijkToEmbeddedReferenceFrameTransform, const PlusTransformName transformName)
{
	this->Internal->TransformRepository->GetTransform(transformName,ijkToEmbeddedReferenceFrameTransform);
}
//---------------------------------------------------------------------------
std::string vtkStealthLinkTracker::GetDicomImagesOutputDirectory()
{
	return this->Internal->DicomImagesOutputDirectory;
}
void vtkStealthLinkTracker::SetDicomImagesOutputDirectory(std::string dicomImagesOutputDirectory)
{
	this->Internal->DicomImagesOutputDirectory = dicomImagesOutputDirectory;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::AcquireDicomImage(std::string dicomImagesOutputDirectory, std::string& examImageDirectory)
{
	PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

	vtkDirectory::MakeDirectory(dicomImagesOutputDirectory.c_str());
	std::string patientName;
	if(!this->GetPatientName(patientName))
	{
		return PLUS_FAIL;
	}
	this->RemoveForbiddenCharactersFromPatientsName(patientName);
	this->UpdateDateAndTimeString();
	examImageDirectory = std::string(dicomImagesOutputDirectory) + std::string("/") + std::string(patientName) + std::string("_") + std::string(this->DateAndTimeString);
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
PlusStatus vtkStealthLinkTracker::GetImageMetaData(PlusCommon::ImageMetaDataList &imageMetaData)
{
	PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
	if(!this->UpdateCurrentExam())
	{
		return PLUS_FAIL;
	}
	PlusCommon::ImageMetaDataItem imageMetaDataItem;
	std::string strDeviceId(this->DeviceId);
	int totalSize = strDeviceId.size() + this->Internal->CurrentExam.description.size() + 1; // the name will look like DeviceID_examDescription 
		                                                              // total size cannot exceed 20. so deviceId+description+1 should be inferiour to 20
																																  // 1 for the under score
	if(totalSize > 20)
	{
		imageMetaDataItem.Id = strDeviceId + std::string("-") + GetExamCountInString();
	}
	else
	{
		imageMetaDataItem.Id = strDeviceId + std::string("-") + this->Internal->CurrentExam.description;
	}
	imageMetaDataItem.Description = this->Internal->CurrentExam.description;
	imageMetaDataItem.Modality = this->Internal->CurrentExam.modality;
	imageMetaDataItem.PatientId= this->Internal->CurrentExam.patientId;
	imageMetaDataItem.PatientName = this->Internal->CurrentExam.patientName ;
	imageMetaDataItem.ScalarType = 3; //TODO check this and correct this
	imageMetaDataItem.Size[0] = this->Internal->CurrentExam.size[0];
	imageMetaDataItem.Size[1] = this->Internal->CurrentExam.size[1];
	imageMetaDataItem.Size[2] = this->Internal->CurrentExam.size[2];
	imageMetaDataItem.TimeStampUtc = vtkAccurateTimer::GetUniversalTime();
	imageMetaData.push_back(imageMetaDataItem);
	this->Internal->ExamIdImageIdPair.first = imageMetaDataItem.Id;
	this->Internal->ExamIdImageIdPair.second = this->Internal->CurrentExam.name;
	return PLUS_SUCCESS;
}
//-------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetImage(const std::string& imageId, const std::string& imageReferencFrameName, vtkImageData* imageData, vtkMatrix4x4* ijkToReferenceTransform)
{
	if(!this->Internal->GetTransformRepositoryUpdated())
	{
		if(this->Internal->ExamIdImageIdPair.first.compare(imageId) !=0)
		{
			return PLUS_FAIL;
		}
	}
	if(!this->UpdateCurrentRegistration())
	{
		return PLUS_FAIL;
	}

	std::string examImageDirectory;
	if(!this->AcquireDicomImage(this->GetDicomImagesOutputDirectory(),examImageDirectory))
	{
		return PLUS_FAIL;
	}
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName(examImageDirectory.c_str()); 
	reader->Update();

	//to go from the vtk orientation to lps orientation, the vtk image has to be flipped around y and z axis
	vtkSmartPointer<vtkImageFlip> flipYFilter = vtkSmartPointer<vtkImageFlip>::New();
	flipYFilter->SetFilteredAxis(1); // flip y axis
	flipYFilter->SetInputConnection(reader->GetOutputPort());
	flipYFilter->Update();

	vtkSmartPointer<vtkImageFlip> flipZFilter = vtkSmartPointer<vtkImageFlip>::New();
	flipZFilter->SetFilteredAxis(2); // flip z axis
	flipZFilter->SetInputConnection(flipYFilter->GetOutputPort());
	flipZFilter->Update();
	imageData->DeepCopy(flipZFilter->GetOutput());
	imageData->Update();

	float*  ijkOrigin_LPS = reader->GetImagePositionPatient(); //(0020,0032) ImagePositionPatient
	double* ijkVectorMagnitude_LPS = reader->GetPixelSpacing(); //(0020,0037) ImageOrientationPatient

	float*  iDirectionVector_LPS = reader->GetImageOrientationPatient(); 
	float*  jDirectionVector_LPS = reader->GetImageOrientationPatient()+3;
	float   kDirectionVector_LPS[3]={0}; // the third cosine direction is the cross product of the other two vectors
	vtkMath::Cross(iDirectionVector_LPS,jDirectionVector_LPS,kDirectionVector_LPS);

	vtkSmartPointer<vtkMatrix4x4> ijkToExamRpiTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // image to ExamRpi, medtronic exludes orientation
	ijkToExamRpiTransMatrix->SetElement(0,0,-1);
	ijkToExamRpiTransMatrix->SetElement(2,2,-1);
	int xMin,xMax,yMin,yMax,zMin,zMax; //Dimensions, necessary to calculate the new origin in exam rpi
	reader->GetDataExtent(xMin,xMax,yMin,yMax,zMin,zMax);
	//the origin is shifted from lps to rpi aka shifted along x and z axes
	double newOrigin_ExamRpi[3]; // medtronic uses rpi and considers the dicom origin to be zero and also the orientation to be idendity
	newOrigin_ExamRpi[0] = (xMax-xMin+1)*ijkVectorMagnitude_LPS[0];
	newOrigin_ExamRpi[1] = 0;
	newOrigin_ExamRpi[2] = (zMax-zMin+1)*ijkVectorMagnitude_LPS[2];
	ijkToExamRpiTransMatrix->SetElement(0,3,newOrigin_ExamRpi[0]);
	ijkToExamRpiTransMatrix->SetElement(1,3,newOrigin_ExamRpi[1]);
	ijkToExamRpiTransMatrix->SetElement(2,3,newOrigin_ExamRpi[2]);

	vtkSmartPointer<vtkMatrix4x4> ijkToRasTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // ijkToRas: 3x3 direction cosines + origin 1X3
	for(int i=0;i<3;i++)
	{	
		//the dicom image is in lps. To translate it to ras, x and y are "flipped" aka multipled by -1
		if(i==0 || i==1)
		{
			ijkToRasTransMatrix->SetElement(i,0,-iDirectionVector_LPS[i]);
			ijkToRasTransMatrix->SetElement(i,1,-jDirectionVector_LPS[i]);
			ijkToRasTransMatrix->SetElement(i,2,-kDirectionVector_LPS[i]);
		}
		else
		{
			ijkToRasTransMatrix->SetElement(i,0,iDirectionVector_LPS[i]);
			ijkToRasTransMatrix->SetElement(i,1,jDirectionVector_LPS[i]);
			ijkToRasTransMatrix->SetElement(i,2,kDirectionVector_LPS[i]);
		}	
	}
	//Set the elements of the transformation matrix, x and y are flipped here as well, multiplied by 1
	ijkToRasTransMatrix->SetElement(0,3,-ijkOrigin_LPS[0]);
	ijkToRasTransMatrix->SetElement(1,3,-ijkOrigin_LPS[1]);
	ijkToRasTransMatrix->SetElement(2,3,ijkOrigin_LPS[2]);

	//These matrices are needed to calculate rasToTracker
	{ //TODO Ask if this is a correct use
		PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
	  this->Internal->SetIjkToExamRpiTransformationMatrix(ijkToExamRpiTransMatrix);
	  this->Internal->SetIjkToRasTransformationMatrix(ijkToRasTransMatrix);
	  this->Internal->SetGetImageCommandRequested(TRUE);
	}
	if(this->Internal->GetTransformRepositoryUpdated()) //vtkPlusStealthLinkCommand requested
	{
		vtkAccurateTimer::Delay(1);  // Delay 1 second to make sure that this->Internal->RasToTracker is calculated correctly based on the new matrices, ijkToExamRpiTransMatrix and ijkToRasTransMatrix
		const PlusTransformName rasToTrackerTransformName("Ras","Tracker");
	  this->Internal->TransformRepository->SetTransform(rasToTrackerTransformName,this->Internal->RasToTrackerTransMatrix);
	  vtkSmartPointer<vtkMatrix4x4> rasToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
	  const PlusTransformName rasToReferenceTransformName("Ras",imageReferencFrameName.c_str());
	  this->Internal->TransformRepository->GetTransform(rasToReferenceTransformName,rasToReferenceTransform);
		vtkMatrix4x4::Multiply4x4(rasToReferenceTransform,ijkToRasTransMatrix,ijkToReferenceTransform);
	}
	else //GET_IMAGE command, the image is represented in the Ras coordinate frame
	{
		for(int i=0; i<4; i++)
		{
			for(int j=0;j<4;j++)
			{	
				ijkToReferenceTransform->SetElement(i,j,ijkToRasTransMatrix->GetElement(i,j));
			}
		}
	}
	this->Internal->SetTransformRepositoryUpdated(FALSE);
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
	this->Internal->UpdateRasToTrackerTransMatrix();
	this->Internal->AcquiringRegistrationInformationTimeStamp = vtkAccurateTimer::GetSystemTime(); 
	return PLUS_SUCCESS;
}
//------
PlusStatus vtkStealthLinkTracker::GetPatientName(std::string& patientName)
{
	if(this->Internal->CurrentExam.patientName.empty())
	{
		LOG_INFO("Please make sure that the exam is requested from the server before calling GetPatientName function");
		return PLUS_FAIL;
	}
	patientName = this->Internal->CurrentExam.patientName;
	return PLUS_SUCCESS;
}
PlusStatus vtkStealthLinkTracker::GetPatientId(std::string& patientId)
{
	if(this->Internal->CurrentExam.patientId.empty())
	{
		LOG_INFO("Please make sure that the exam is requested from the server before calling GetPatientId function");
		return PLUS_FAIL;
	}
	patientId = this->Internal->CurrentExam.patientId;
	return PLUS_SUCCESS;
}
//-----------------------------------------------
void vtkStealthLinkTracker::UpdateDateAndTimeString()
{
	this->DateAndTimeString = vtkAccurateTimer::GetInstance()->GetDateAndTimeString();
}
//---------------------------------------------------------------------------
std::string vtkStealthLinkTracker::GetExamCountInString()
{
	char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	std::string examCountInStr;
	if(this->Internal->ExamCounter > 9)
	{
		this->Internal->ExamCounter= (this->Internal->ExamCounter>99)?1:(this->Internal->ExamCounter);
		examCountInStr.push_back(digits[this->Internal->ExamCounter/10]);
		examCountInStr.push_back(digits[this->Internal->ExamCounter%10]);	
	}
	else
	{
		examCountInStr.push_back(digits[0]);
		examCountInStr.push_back(digits[this->Internal->ExamCounter%10]);
	}
	return examCountInStr;
}
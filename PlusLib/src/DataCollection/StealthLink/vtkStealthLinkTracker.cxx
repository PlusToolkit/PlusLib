/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// TODO: need to evaluate if USE_MICRONTRACKER_TIMESTAMPS without filtering
// is better then simply using accurate timestamp with filtering.
// E.g., it could be checked by performing temporal calibration.
// #define USE_MICRONTRACKER_TIMESTAM

#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkHomogeneousTransform.h"

#include "vtkStealthLinkTracker.h"
#include "StealthLink\StealthLink.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <Windows.h>

  
    
class vtkStealthLinkTracker::vtkInternal
{
public:

	friend class vtkStealthLinkTracker;
	
private:



	vtkStealthLinkTracker *External;

	MNavStealthLink::StealthServer		*SteahLinkServer;
	//MNavStealthLink::Frame				currentFrame;  // We are retrieving the transform matrix by using navData - if needed, this part could be decommented
	//MNavStealthLink::Instrument			currentInstrument;  // We are retrieving the transform matrix by using navData - if needed, this part could be decommented
	std::string currentFrameName;
	std::string currentInstrumentName;
	MNavStealthLink::Exam 				currentExam;
	MNavStealthLink::InstrumentNameList instrumentList;
	MNavStealthLink::NavData			navData;
	
	vtkSmartPointer<vtkMatrix4x4> instrumentTransformMatrix;
	vtkSmartPointer<vtkMatrix4x4> frameTransformMatrix;

	char const* HOST; // Host IP Address
	char const* PORT; // Host open Port 
	char const* dicomOutputFile; // Where the dicom file will be saved after having been acquired from the StealthLink Server

	std::ostringstream examImageName;

	double serverInitialTimeInMicroSeconds;
	vtkImageData* dicomImage; // The dicom image

	struct VisibilityStatus
	{
		enum Type
		{
			TRACKABLE, NOTTRACKABLE
		};
		Type t_;
		VisibilityStatus(Type t) : t_(t) {}
		operator Type () const {return t_;}
	private:
		//prevent automatic conversion for any other built-in types such as bool, int, etc
		template<typename T>
		operator T () const;
	};

	VisibilityStatus instrumentVisibility;
	VisibilityStatus frameVisibility;


	/*~ Constructor ~*/
	vtkInternal(vtkStealthLinkTracker* external) 
    : External(external)
    , SteahLinkServer(NULL)
    , HOST("130.15.7.88")
    , PORT("6996")
    , dicomOutputFile("C:/StealthLinkDicomOutput")
	, examImageName("")
	,instrumentVisibility(VisibilityStatus::TRACKABLE)
	,frameVisibility(VisibilityStatus::TRACKABLE)
	{
		this->dicomImage				= vtkImageData::New();
		this->instrumentTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		this->frameTransformMatrix		= vtkSmartPointer<vtkMatrix4x4>::New();
		this->frameTransformMatrix->Identity();
		this->instrumentTransformMatrix->Identity();
	}

	/*! Destructor !*/
	~vtkInternal()
	{
		this->HOST = NULL;
		this->PORT = NULL;
		this->dicomOutputFile = NULL;

		delete this->SteahLinkServer;
		this->SteahLinkServer=NULL;
		this->External = NULL;

		this->dicomImage->Delete();
		this->dicomImage=NULL;
	}

	/*! Check if the Instruments Are Verified !*/
	PlusStatus IsInstrumentVerified()
	{
		MNavStealthLink::Instrument instrument;
		MNavStealthLink::Error		error;
		try
		{
			this->SteahLinkServer->get(this->currentInstrumentName,instrument,this->SteahLinkServer->getServerTime(),error);
		}
		catch (MNavStealthLink::Error error)
		{
			LOG_ERROR("Error retrieveing the current instrument from the StealthLink Server " << error.what() << "\n");
		}
		if(instrument.isVerified)
		{
			return PLUS_SUCCESS;
		}
		return PLUS_FAIL;
	}

	/*! Get the transformation maxtrix of the current instrument !*/
	void GetInstrumentTransformMatrix()
	{
		if( this->navData.instVisibility == MNavStealthLink::Instrument::VISIBLE || this->navData.instVisibility == MNavStealthLink::Instrument::ALMOST_BLOCKED)
		{
			if(this->instrumentVisibility != VisibilityStatus::TRACKABLE)
			{
				LOG_TRACE("The instrument is trackable now\n");
				this->instrumentVisibility = VisibilityStatus::TRACKABLE;
		}	
			for(int col=0; col < 4; col++)
			{
				for (int row=0; row < 4; row++)
				{
					this->instrumentTransformMatrix->SetElement(row, col, this->navData.localizer_T_instrument [row][col]);
				}
			}
		}
		else
		{
			if(this->instrumentVisibility != VisibilityStatus::NOTTRACKABLE)
			{
				LOG_TRACE("The instrument is not trackable. Please check the current positions of the instrument\n");
				this->instrumentVisibility = VisibilityStatus::NOTTRACKABLE;
			}
		}
	}
	/*! Get the transformation maxtrix of the current frame !*/
	void GetFrameTransformMatrix()
	{
		if( this->navData.frameVisibility == MNavStealthLink::Frame::VISIBLE || this->navData.frameVisibility == MNavStealthLink::Frame::ALMOST_BLOCKED)
		{
			if(this->frameVisibility != VisibilityStatus::TRACKABLE)
			{
				LOG_TRACE("The frame is now trackable.\n");
				this->frameVisibility = VisibilityStatus::TRACKABLE;
			}
			vtkSmartPointer<vtkMatrix4x4> transformFrameTLocalizer;
			transformFrameTLocalizer = vtkSmartPointer<vtkMatrix4x4>::New();
			for(int col=0; col < 4; col++)
			{
				for (int row=0; row < 4; row++)
				{
					transformFrameTLocalizer->SetElement(row, col, this->navData.frame_T_localizer [row][col]); // from localizer Space to frame space
				}
			}
			//the transformation matrix given by navData is from Localizer(Tracker) space to frame space and we need the inverse of it
			vtkSmartPointer<vtkTransform> transform  = vtkSmartPointer<vtkTransform>::New();
			transform->SetMatrix(transformFrameTLocalizer);
			transform->Inverse(); 
			transform->GetMatrix(this->frameTransformMatrix);   
		}
		else
		{
			if(this->frameVisibility != VisibilityStatus::NOTTRACKABLE)
			{
				LOG_TRACE("The frame is not trackable. Please check the current positions of the tools.\n");
				this->frameVisibility = VisibilityStatus::NOTTRACKABLE;
			}
		}
	}
};
/****************************************************************************/ 

vtkStandardNewMacro(vtkStealthLinkTracker);

//----------------------------------------------------------------------------
vtkStealthLinkTracker::vtkStealthLinkTracker()
{
	this->Internal = new vtkInternal(this);
	this->IsStealthServerInitialized = false;
	this->TrackerTimeToSystemTimeComputed = false;
	this->stealthFrameNumber = 0;

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
  if (this->IsStealthServerInitialized)
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
std::string vtkStealthLinkTracker::GetSdkVersion()
{
	MNavStealthLink::Version version;
	MNavStealthLink::Error error;
	std::ostringstream oStr;
	this->Internal->SteahLinkServer->get(version,this->Internal->SteahLinkServer->getServerTime(),error);
	oStr << (unsigned) version.major << '.' << (unsigned) version.minor;
	return oStr.str();
}

//-----------------------------------------------------------------------------
void vtkStealthLinkTracker::checkInstrumentVerification()
{
	this->instrumentVerification = (this->Internal->IsInstrumentVerified()) ? "VERIFIED" : "NOT VERIFIED. PLEASE VERIFY THE INSTRUMENT TO HAVE AN ACCURATE TRANSFORMATION MATRIX";
	LOG_TRACE (" The current instrument: " << this->Internal->currentInstrumentName << " is " << this->instrumentVerification << "." << std::endl);
}

//-----------------------------------------------------------------------------
void vtkStealthLinkTracker::getFirstToolVisibilityStatus()
{
	/*! Instrument Visibility Status - No need for Visible since the default value is TRACKABLE !*/
	if(!(this->Internal->navData.instVisibility == MNavStealthLink::Instrument::VISIBLE || this->Internal->navData.instVisibility == MNavStealthLink::Instrument::ALMOST_BLOCKED))
	{
		this->Internal->instrumentVisibility = vtkInternal::VisibilityStatus::NOTTRACKABLE;		
		LOG_TRACE("The Instrument is not trackable\n");
	}

	/*! Frame Visibility Status - No need for Visible since the default value is TRACKABLE !*/
	if(!(this->Internal->navData.frameVisibility != MNavStealthLink::Frame::VISIBLE || this->Internal->navData.frameVisibility != MNavStealthLink::Frame::ALMOST_BLOCKED))
	{
		this->Internal->frameVisibility = vtkInternal::VisibilityStatus::NOTTRACKABLE;	
		LOG_TRACE("The Frame is not trackable\n");
	}
}

//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalStartRecording()
{
  if (!this->IsStealthServerInitialized)
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
PlusStatus vtkStealthLinkTracker::GetDicomImage(vtkImageData* dicomImage)
{
	//PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
	if(this->Internal->examImageName.str().empty())
	{
		CreateDirectory(this->Internal->dicomOutputFile,NULL); // creates the folder if does not exist
		if(!this->GetCurrentExam())
		{
			return PLUS_FAIL;
		}
		std::string patientName(this->Internal->currentExam.patientName);
		vtkStealthLinkTracker::ModifyPatientName(patientName);
		this->Internal->examImageName << this->Internal->dicomOutputFile << '/' << patientName;
		MNavStealthLink::Error error;
		try 
		{
			this->Internal->currentExam.getExamData(*(this->Internal->SteahLinkServer),this->Internal->examImageName.str(),error);
		}
		catch (MNavStealthLink::Error error) 
		{
			LOG_ERROR("Error getting images from StealthLink: " << error.what() << "\n");
			return PLUS_FAIL;
		}
	}
  
	int sizeX = this->Internal->currentExam.size[0];
	int sizeY = this->Internal->currentExam.size[1];
	int sizeZ = this->Internal->currentExam.size[2];
  
	int spacingX = this->Internal->currentExam.sliceSpacing;
	int spacingY = this->Internal->currentExam.sliceSpacing;
	int spacingZ = this->Internal->currentExam.sliceThickness;
  
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName((this->Internal->examImageName.str()).c_str()); 
	reader->SetDataExtent(0,sizeX, 0,sizeY, 0,sizeY);
	reader->SetDataSpacing(spacingX,spacingY,spacingZ);
	reader->Update();
	dicomImage->DeepCopy(reader->GetOutput());
	
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
	if (this->IsStealthServerInitialized)
	{
		LOG_DEBUG("Already connected to StealthLink");
		return PLUS_SUCCESS;
	}

	if (this->Internal->HOST == NULL || this->Internal->PORT == NULL)
	{
		LOG_ERROR("Cannot connect: HOST or PORT Address are not assigned\n");
		return PLUS_FAIL;
	}
	LOG_TRACE("HOST Address: " << this->Internal->HOST << " " << "PORT Address: " << this->Internal->PORT << "\n");
	
	delete this->Internal->SteahLinkServer;
	this->Internal->SteahLinkServer = NULL;
	this->Internal->SteahLinkServer = new MNavStealthLink::StealthServer(this->Internal->HOST,this->Internal->PORT);
	MNavStealthLink::Error error;
	if (!this->Internal->SteahLinkServer->connect(error))
	{
		LOG_ERROR(" Failed to connect to Stealth server application on host: " << error.reason() << "\n");
		return PLUS_FAIL;
	}
	this->IsStealthServerInitialized=true;
	this->Internal->serverInitialTimeInMicroSeconds = (double) this->Internal->SteahLinkServer->getServerTime();
	return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalDisconnect()
{ 
  if (this->IsStealthServerInitialized)
  {
    this->Internal->SteahLinkServer->disconnect();  
	delete this->Internal->SteahLinkServer;
	this->Internal->SteahLinkServer = NULL;
    this->IsStealthServerInitialized=false;
  }  
  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalUpdate()
{
	if (!this->IsStealthServerInitialized)
	{
		LOG_ERROR("InternalUpdate failed: StealthLinkServer has not been initialized");
		return PLUS_FAIL;
	}	
	if(!this->TrackerTimeToSystemTimeComputed)
	{
		try
		{
			MNavStealthLink::Error err;
			this->TrackerTimeToSystemTimeSec =  vtkAccurateTimer::GetSystemTime()- ((double) this->Internal->SteahLinkServer->getServerTime(err) - this->Internal->serverInitialTimeInMicroSeconds);
		}
		catch(MNavStealthLink::Error err)
		{
			LOG_ERROR (" Could not retrieve the server time: " << err.reason() << std::endl);
			return PLUS_FAIL;
		}
		this->TrackerTimeToSystemTimeComputed = true;
	}

	double timeSystemSec;
	try
	{
		MNavStealthLink::Error err;
		timeSystemSec = (double) this->Internal->SteahLinkServer->getServerTime(err) - this->Internal->serverInitialTimeInMicroSeconds + this->TrackerTimeToSystemTimeSec;
	}
	catch(MNavStealthLink::Error err)
	{
		LOG_ERROR (" Could not retrieve the server time: " << err.reason() << std::endl);
		return PLUS_FAIL;
	}
	if(this->stealthFrameNumber == 0)
	{
		MNavStealthLink::Error err;
		// Get the instrument list for the first and only time
		if(!this->Internal->SteahLinkServer->get(this->Internal->instrumentList,err))
		{
			LOG_ERROR (" Could not retrieve the instument name list: " << err.reason() << std::endl);
			return PLUS_FAIL;
		}
		// If Localizer (Tracker) is not connected, then send an error message
		if(!this->IsLocalizerConencted())
		{
			LOG_ERROR("Localizer(Tracker) is not connected. Please check the StealthLink Server\n")
			return PLUS_FAIL;
		}
	}
	
	
	/*! Check if the port names in the .xml file are valid !*/
	bool foundInTheNameList = false;
	bool referenceFrameTransformNeeded = false;
	for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
	{
		if(!strcmp(toolIterator->second->GetPortName(),"Frame") && referenceFrameTransformNeeded)
		{
			LOG_ERROR(" Reference frame has been required twice in the .xml file. Please check the file\n");
			return PLUS_FAIL;
		}
		else if(!strcmp(toolIterator->second->GetPortName(),"Frame") && !referenceFrameTransformNeeded)
		{
			referenceFrameTransformNeeded = true;
		}
		else
		{
			for (MNavStealthLink::InstrumentNameList::iterator instrumentNameIterator = this->Internal->instrumentList.begin(); instrumentNameIterator!=this->Internal->instrumentList.end(); instrumentNameIterator++)
			{
				if(!strcmp((*instrumentNameIterator).c_str(), toolIterator->second->GetPortName()))
				{
					foundInTheNameList = true;
					break;
				}
			}
			if(!foundInTheNameList)
			{
				std::ostringstream errorString;
				errorString << " The instrument port: " << toolIterator->second->GetPortName() << "in the .xml file is not valid. The valid tool ports are:\n";
				for (MNavStealthLink::InstrumentNameList::iterator writeAllDefinedIns = this->Internal->instrumentList.begin(); writeAllDefinedIns!=this->Internal->instrumentList.end(); writeAllDefinedIns++)
				{
					errorString << *writeAllDefinedIns << " ";
				}
				LOG_ERROR(errorString.str().c_str() << "\n");
				return PLUS_FAIL;
			}
			foundInTheNameList = false;
		}
	}

	//----------------------------------------------------------
	this->GetCurrentNavigationData();

	for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
	{
		if(!strcmp(toolIterator->second->GetPortName(),"Frame") && referenceFrameTransformNeeded)
		{
			this->Internal->GetFrameTransformMatrix();
			if(this->Internal->frameVisibility == vtkInternal::VisibilityStatus::TRACKABLE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->frameTransformMatrix, TOOL_OK, timeSystemSec, timeSystemSec);
			}
			else if(this->Internal->frameVisibility == vtkInternal::VisibilityStatus::NOTTRACKABLE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->frameTransformMatrix, TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
			}
		}
		else
		{
			this->Internal->GetInstrumentTransformMatrix();
			if(this->Internal->instrumentVisibility == vtkInternal::VisibilityStatus::TRACKABLE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->instrumentTransformMatrix, TOOL_OK, timeSystemSec, timeSystemSec);
			}
			else if(this->Internal->instrumentVisibility == vtkInternal::VisibilityStatus::NOTTRACKABLE)
			{
				this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), this->Internal->instrumentTransformMatrix, TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
			}
		}
	}
	this->stealthFrameNumber++;
	return PLUS_SUCCESS;
}
//------------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetCurrentNavigationData()
{
	MNavStealthLink::Error error;
	try
	{
		this->Internal->SteahLinkServer->get(this->Internal->navData,this->Internal->SteahLinkServer->getServerTime(),error);
	}
	catch (MNavStealthLink::Error error)
	{
		LOG_ERROR(" Failed to acquire the navigation data from StealthLink Server: " <<  error.reason() << "\n");
		return PLUS_FAIL;
	}
	if(this->stealthFrameNumber == 0)
	{
		this->Internal->currentFrameName = this->Internal->navData.frameName;
		this->Internal->currentInstrumentName = this->Internal->navData.instrumentName;
		this->checkInstrumentVerification();
		this->getFirstToolVisibilityStatus();
	}
	if(this->Internal->navData.frameName != this->Internal->currentFrameName && !this->Internal->navData.frameName.empty())
	{
		LOG_TRACE (" The current frame has been change from: " << this->Internal->currentFrameName << " to: " << this->Internal->navData.frameName << "\n");
		this->Internal->currentFrameName = this->Internal->navData.frameName;
	}
	if(this->Internal->navData.instrumentName != this->Internal->currentInstrumentName && !this->Internal->navData.instrumentName.empty())
	{
		LOG_TRACE (" The current frame has been change from: " << this->Internal->currentInstrumentName << " to: " << this->Internal->navData.instrumentName << "\n");
		this->Internal->currentInstrumentName = this->Internal->navData.instrumentName;
		this->checkInstrumentVerification();
	}
	return PLUS_SUCCESS;
}

PlusStatus vtkStealthLinkTracker::IsLocalizerConencted()
{
	MNavStealthLink::LocalizerInfo localizerInfo;
	MNavStealthLink::Error err;
	this->Internal->SteahLinkServer->get(localizerInfo,err);
	if(localizerInfo.isConnected)
	{
		return PLUS_SUCCESS;
	}
	return PLUS_FAIL;
}
void vtkStealthLinkTracker::ModifyPatientName(std::string& patientName)
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
	std::string second;
	int placement = 0;
	for(std::vector<int>::iterator it = spacePlaces.begin(); it!=spacePlaces.end(); it++)
	{
		second=patientName.substr(*it+1-placement);
		patientName=patientName.substr(0,*it-placement);
		patientName = patientName + second;
		placement++;
	}
}
//--------
PlusStatus vtkStealthLinkTracker::GetCurrentExam()
{
	MNavStealthLink::Error err;
	MNavStealthLink::Exam ex;
	MNavStealthLink::DateTime time;// = this->Internal->SteahLinkServer->getServerTime();
	this->Internal->SteahLinkServer->get(ex,time,err);
	try 
	{
		this->Internal->SteahLinkServer->get(this->Internal->currentExam,time,err);
    }
    catch (MNavStealthLink::Error err) 
	{
       LOG_ERROR(" Failed to acquire the current exam. Please make sure to select an exam: " <<  err.what() << "\n");
       return PLUS_FAIL;
    }
	return PLUS_SUCCESS;
}
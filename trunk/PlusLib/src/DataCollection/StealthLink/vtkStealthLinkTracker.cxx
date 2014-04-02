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
	MNavStealthLink::Frame				currentFrame;
	MNavStealthLink::Instrument			currentInstrument;
	MNavStealthLink::Exam 				currentExam;
	MNavStealthLink::InstrumentNameList instrumentList;
	MNavStealthLink::NavData			navData;

	char const* HOST; // Host IP Address
	char const* PORT; // Host open Port 
	char const* dicomOutputFile; // Where the dicom file will be saved after having been acquired from the StealthLink Server
	std::ostringstream examImageName;

	double serverInitialTimeInMicroSeconds;
	vtkImageData* dicomImage; // The dicom image
	vtkMatrix4x4* transformInsToTracker; // The transform matrix from Instrument Space to Localizer space

	/*~ Constructor ~*/
	vtkInternal(vtkStealthLinkTracker* external) 
    : External(external)
    , SteahLinkServer(NULL)
    , HOST("130.15.7.88")
    , PORT("6996")
    , dicomOutputFile("C:/StealthLinkDicomOutput")
	, examImageName("")
	{
		this->dicomImage=vtkImageData::New();
		this->transformInsToTracker = vtkMatrix4x4::New();
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
		this->transformInsToTracker->Delete();
		this->transformInsToTracker = NULL;
	}

	/*! Check if the Instruments Are Verified !*/
	PlusStatus IsInstrumentVerified(MNavStealthLink::Instrument instrument)
	{
		if (instrument.isVerified)
		{
			return PLUS_SUCCESS;
		}
		return PLUS_FAIL;
	}
	/*! Get the transformation maxtrix of each instrument !*/
	void GetInstrumentTransformMatrix(vtkMatrix4x4* transformMatrix)
	{
		if(this->navData.instrumentName.empty())
		{
			LOG_ERROR("Error retrieveing the current instrument from the StealthLink Server\n");
		}
		if(this->navData.instVisibility != MNavStealthLink::Frame::VISIBLE) // checks if the reference frame is visible
		{
			LOG_ERROR("The instrument is not visible. Please check the current setting of the tools.\n");
		}
		transformMatrix->Identity();
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				transformMatrix->SetElement(row, col, this->navData.localizer_T_instrument [row][col]); // from Instrument Space to Localizer space
			}
		}
	}
	void GetCurrentFrameTransformMatrix(vtkMatrix4x4* transformMatrix)
	{
		if(this->navData.frameName.empty())
		{
			LOG_ERROR("Error retrieveing the current frame from the StealthLink Server\n");
		}
		if(this->navData.frameVisibility != MNavStealthLink::Frame::VISIBLE) // checks if the reference frame is visible
		{
			LOG_ERROR("The reference frame is not visible. Please check the current setting of the tools.\n");
		}
		transformMatrix->Identity();
		for(int col=0; col < 4; col++)
		{
			for (int row=0; row < 4; row++)
			{
				transformMatrix->SetElement(row, col, this->navData.frame_T_localizer [row][col]); // from Instrument Space to Localizer space
			}
		}
	}
};
// Note that "MTC.h" is not included directly, as it causes compilation warnings
// and unnecessary coupling to lower-level MTC functions.
// All MTC internal functions shall be accessed through MicronTrackerInterface.

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

//----------------------------------------------------------------------------
/*
PlusStatus vtkStealthLinkTracker::Probe()
{  
  if (this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("vtkMicronTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  std::string iniFilePath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->IniFile);
  LOG_DEBUG("Use MicronTracker ini file: "<<iniFilePath);
  if ( !vtksys::SystemTools::FileExists( iniFilePath.c_str(), true) )
  {
    LOG_DEBUG("Unable to find MicronTracker IniFile file at: " << iniFilePath);
  }
  std::string templateFullPath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->TemplateDirectory.c_str());
  LOG_DEBUG("Loading the marker templates from "<<templateFullPath);
  if ( !vtksys::SystemTools::FileExists( templateFullPath.c_str(), false) )
  {
    LOG_DEBUG("Unable to find MicronTracker TemplateDirectory at: " << templateFullPath);
  }

  if (this->MT->mtInit(iniFilePath)!=1)
  {
    LOG_ERROR("Error in initializing Micron Tracker");
    return PLUS_FAIL;
  }

  // Try to attach the cameras till find the cameras
  if (this->MT->mtSetupCameras()!=1)
  {
    LOG_ERROR("Error in initializing Micron Tracker: setup cameras failed. Check the camera connections.");
    return PLUS_FAIL;
  }

  int numOfCameras = this->MT->mtGetNumOfCameras();
  if (numOfCameras==0)
  {
    LOG_ERROR("Error in initializing Micron Tracker: no cameras attached. Check the camera connections.");
    return PLUS_FAIL;
  }

  LOG_DEBUG("Number of attached cameras: " << numOfCameras );

  this->MT->mtEnd();
  this->IsMicronTrackingInitialized=false;

  return PLUS_SUCCESS;
} 
*/
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


//----------------------------------------------------------------------------
/*PlusStatus vtkMicronTracker::RefreshMarkerTemplates()
{
  std::vector<std::string> vTemplatesName;
  std::vector<std::string> vTemplatesError;

  std::string templateFullPath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->TemplateDirectory.c_str());
  LOG_DEBUG("Loading the marker templates from "<<templateFullPath);
  if ( !vtksys::SystemTools::FileExists( templateFullPath.c_str(), false) )
  {
    LOG_WARNING("Unable to find MicronTracker TemplateDirectory at: " << templateFullPath);
  }
  int callResult = this->MT->mtRefreshTemplates(vTemplatesName, vTemplatesError, templateFullPath);
  for (int i=0; i<vTemplatesName.size(); i++)
  {
    LOG_DEBUG("Loaded " << vTemplatesName[i]);
  }
  if (callResult != 0)
  {
    LOG_ERROR("Failed to load marker templates from "<<templateFullPath);
    for (int i=0; i<vTemplatesError.size(); i++)
    {
      LOG_ERROR("Error loading template: " << vTemplatesError[i]);
    }
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
*/
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
		std::cout << " EMPTY \n " << std::endl;
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
	LOG_DEBUG("HOST Address: " << this->Internal->HOST << " " << "PORT Address: " << this->Internal->PORT << "\n");
	
	delete this->Internal->SteahLinkServer;
	this->Internal->SteahLinkServer = NULL;
	this->Internal->SteahLinkServer = new MNavStealthLink::StealthServer(this->Internal->HOST,this->Internal->PORT);
	MNavStealthLink::Error error;
	if (!this->Internal->SteahLinkServer->connect(error))
	{
		LOG_ERROR(" Failed to connect to Stealth server application on host: " << error.reason() << "\n");
		return PLUS_FAIL;
	}
	std::cout << " Server info: " << this->Internal->SteahLinkServer->getCurrentTask().name << std::endl;
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
  std::cout << "Disconnected" << std::endl;
  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalUpdate()
{
	double unfilteredTimestampBefore = vtkAccurateTimer::GetSystemTime();
	int checkIndex = 0;
	if (!this->IsStealthServerInitialized)
	{
		LOG_ERROR("InternalUpdate failed: StealthLinkServer has not been initialized");
		return PLUS_FAIL;
	}	
	if(!this->TrackerTimeToSystemTimeComputed)
	{
		this->TrackerTimeToSystemTimeSec =  vtkAccurateTimer::GetSystemTime()- ((double) this->Internal->SteahLinkServer->getServerTime() - this->Internal->serverInitialTimeInMicroSeconds); 
		this->TrackerTimeToSystemTimeComputed = true;
	}

	const double timeSystemSec = (double) this->Internal->SteahLinkServer->getServerTime() - this->Internal->serverInitialTimeInMicroSeconds + this->TrackerTimeToSystemTimeSec;
	
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
	// Set status and transform for tools with detected markers
	vtkSmartPointer<vtkMatrix4x4> toolTransformMatrix      =vtkSmartPointer<vtkMatrix4x4>::New(); // From instrument space to localizer space
	toolTransformMatrix->Identity();
	int index = 0;
	std::string verified;

	this->GetCurrentInstrumentAndFrame();
	for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
	{
		
		if(!strcmp(toolIterator->second->GetPortName(),"Frame") && referenceFrameTransformNeeded)
		{
			this->Internal->GetCurrentFrameTransformMatrix(toolTransformMatrix);
		}
		else
		{
			//verified = (this->Internal->IsInstrumentVerified(instrument)) ? "VERIFIED" : "NOT VERIFIED. PLEASE VERIFY THE INSTRUMENT TO HAVE AN ACCURATE TRANSFORMATION MATRIX";
			LOG_DEBUG (" The instrument number: " << index++ << " is " << toolIterator->second->GetPortName() << "." << "The instrument is " << verified << "." << std::endl);
			this->Internal->GetInstrumentTransformMatrix(toolTransformMatrix);
		//	if(std::strcmp(verified.c_str(),"VERIFIED"))
		//	{
		//		toolTransformMatrix->Identity();
		//	}
		}
		//this->ToolTimeStampedUpdate(toolIterator->second->GetSourceId(), toolTransformMatrix, TOOL_OK, this->stealthFrameNumber, unfilteredTimestamp);
		this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), toolTransformMatrix, TOOL_OK, timeSystemSec, timeSystemSec);
		toolTransformMatrix->Identity();
	}
	this->stealthFrameNumber++;
	double unfilteredTimestampAfter = vtkAccurateTimer::GetSystemTime();
	LOG_INFO("Calling the toolTImeStampedUpdate with Frame number: " << this->stealthFrameNumber << "index = " << checkIndex++ << " time difference= " << unfilteredTimestampAfter - unfilteredTimestampBefore);
	return PLUS_SUCCESS;
}
//------------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetCurrentInstrumentAndFrame()
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
		cout << "it = " << *it << endl;
		second=patientName.substr(*it+1-placement);
		cout << "second = " << second << endl;
		patientName=patientName.substr(0,*it-placement);
		patientName = patientName + second;
		cout << " PName = " << patientName << endl;
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
	std::cout << "exam name: " << ex.patientName << std::endl;
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
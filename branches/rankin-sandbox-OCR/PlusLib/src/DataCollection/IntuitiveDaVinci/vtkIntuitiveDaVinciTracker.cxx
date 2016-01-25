#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkIntuitiveDaVinciTracker.h"
#include <fstream>
#include <iostream>
#include <set>

/****************************************************************************/

vtkStandardNewMacro(vtkIntuitiveDaVinciTracker);

//----------------------------------------------------------------------------
vtkIntuitiveDaVinciTracker::vtkIntuitiveDaVinciTracker()
{
#ifdef USE_DAVINCI_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif

  this->IsDaVinciTrackingInitialized = false;
  this->mDaVinci = new IntuitiveDaVinci();

  // for accurate timing
  this->FrameNumber = 0;

  this->RequirePortNameInDeviceSetConfiguration = true;

  this->AcquisitionRate = 20;

  this->IpAddr = "10.0.0.5";
  this->Port = 5002;
  this->Password = "";
}

//----------------------------------------------------------------------------
vtkIntuitiveDaVinciTracker::~vtkIntuitiveDaVinciTracker() 
{
  if(this->Recording)
  {
    this->StopRecording();
  }

  if ( this->mDaVinci != NULL )
  {    
    this->mDaVinci->stop();
    this->IsDaVinciTrackingInitialized=false;
    delete this->mDaVinci;
    this->mDaVinci = NULL;
  }
}

//----------------------------------------------------------------------------
std::string vtkIntuitiveDaVinciTracker::GetSdkVersion()
{
  return this->mDaVinci->getLibraryVersion(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::Probe()
{  

  if (this->IsDaVinciTrackingInitialized)
  {
    LOG_ERROR("vtkIntuitiveDaVinciTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  if(this->mDaVinci->connect() != ISI_SUCCESS)
  {
    LOG_ERROR("vtkIntuitiveDaVinciTracker::Probe could not connect to the da Vinci!");
    return PLUS_FAIL;
  }

  // Get the list of manipulators for the da Vinci. 
  // If the list is zero, we've failed terribly. 
  // Possibly also check for names and verify we have what we expect.

  std::vector<std::string> manipNames = this->mDaVinci->getManipulatorNames();

  if(manipNames.size() == 0 )
  {
    LOG_ERROR("Error in retrieving manipulator information. Zero manipulators found.");
    return PLUS_FAIL;
  }

  for(int i = 0; i < manipNames.size(); i++)
  {
    LOG_DEBUG("Manipulator " << i << " name: " << manipNames[i]<< "\n");
  }

  // TODO: Probe to see what we've specified in the XML file actually exists in the da Vinci

  this->mDaVinci->stop();
  this->IsDaVinciTrackingInitialized = false;
  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::InternalStartRecording()
{
  if (!this->IsDaVinciTrackingInitialized)
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci has not been initialized");
    return PLUS_FAIL;
  }

  if(!this->mDaVinci->isConnected())
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci is not connected");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::InternalStopRecording()
{
  // Stop the stream and disconnect from the da Vinci.
  this->mDaVinci->stop();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::InternalUpdate()
{

  /* We don't (shouldn't) come here, as all the data we
  require is done through a callback
  */
  if(Superclass::InternalUpdate() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  LOG_TRACE("vtkIntuitiveDaVinciTracker::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // Determine which manipulators we're interested in.
  // Store those names for later, where we can then create the data source pointers.

  /* Because of how PLUS works when we create a vtkPlusDevice,
  the XML file is already parsed. The parent class reads in the configuration
  file, and finds all the data sources that are tools.

  It then parses the tool tags and adds them (if possible) to this device.

  See vtkPlusDevice.cxx : ReadConfiguration( ... )

  */

  XML_READ_STRING_ATTRIBUTE_WARNING(IpAddr, deviceConfig);
  // XML_READ_SCALAR_ATTRIBUTE_WARNING(unsigned int, Port, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_WARNING(Password, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::InternalConnect()
{ 
  // Before trying to get to the da Vinci, let's see what was read in our XML config file.
  // That is, what manipulators did we say we were interested in?

  LOG_TRACE("vtkIntuitiveDaVinciTracker::InternalConnect");

  LOG_DEBUG("Connecting to da Vinci");

  if (this->IsDaVinciTrackingInitialized)
  {
    LOG_DEBUG("Already connected to da Vinci");
    return PLUS_SUCCESS;
  }

  // Try to connect a few different times. If at first you don't
  // succeed, try again!

  int connectionAttempts = 0;
  bool initCompleted = false;

  // Set our connection parameters obtained from the config file
  this->mDaVinci->setHostInfo(IpAddr, Port, Password);

  while(!initCompleted && connectionAttempts < MAX_ATTEMPTS)
  {
    if(this->mDaVinci->connect() != 0)
    {
      LOG_DEBUG("Failed to connect to da Vinci. Retry: " << connectionAttempts);
      vtkAccurateTimer::Delay(1.0);
      connectionAttempts++;
    }

    initCompleted = true;
  }

  if(connectionAttempts == MAX_ATTEMPTS)
  {
    LOG_ERROR("Error in initializing da Vinci");
    return PLUS_FAIL;
  }

  // For now, let's NOT use an event callback. 
  if(this->mDaVinci->subscribe(NULL, vtkIntuitiveDaVinciTrackerUtilities::streamCB, NULL, this) != ISI_SUCCESS)
  {
    LOG_ERROR("Error in subscribing to events and stream! Stream not started!");
    return PLUS_FAIL;
  }

  // Otherwise we're a-okay! 

#ifdef USE_DAVINCI_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif

  this->IsDaVinciTrackingInitialized = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntuitiveDaVinciTracker::InternalDisconnect()
{ 
  if (this->IsDaVinciTrackingInitialized)
  {
    this->mDaVinci->stop();  
    this->IsDaVinciTrackingInitialized = false;
  }  
  return PLUS_SUCCESS;
}

void vtkIntuitiveDaVinciTracker::StreamCallback(void)
{
  if (!this->IsDaVinciTrackingInitialized)
  {
    LOG_ERROR("InternalUpdate failed: daVinci has not been initialized");
    return;
  }

  if(!this->Recording)
  {
    // Drop the frame, we're not recording.
    LOG_DEBUG("Dropped frame : daVinci is not recording");
    return;
  }
  // Generate a frame number, as the tool does not provide a frame number.
  // FrameNumber will be used in ToolTimeStampedUpdate for timestamp filtering
  ++this->FrameNumber;

  // Setting the timestamp
  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

#ifdef USE_DAVINCI_TIMESTAMPS
  if (!this->TrackerTimeToSystemTimeComputed)
  {
    const double timeSystemSec = unfilteredTimestamp;
    const double timeTrackerSec = this->mDaVinci->mtGetLatestFrameTime();
    this->TrackerTimeToSystemTimeSec = timeSystemSec-timeTrackerSec;
    this->TrackerTimeToSystemTimeComputed = true;
  }
  const double timeTrackerSec = this->mDaVinci->mtGetLatestFrameTime();
  const double timeSystemSec = timeTrackerSec + this->TrackerTimeToSystemTimeSec;        
#endif

  vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  for( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    ISI_STREAM_FIELD stream_data;
    ISI_TRANSFORM *transform = new ISI_TRANSFORM();

    std::string toolName = it->second->GetPortName();

    ISI_MANIP_INDEX manipIndex = getManipIndexFromName(toolName);

    if(manipIndex == -1) continue;

	bool inverseTransform = false; 
    if(toolName.find("_TIP") != std::string::npos)
    {
      // Get the tip transform for this manipulator
      isi_get_stream_field( manipIndex , ISI_TIP_TRANSFORM, &stream_data);
      transform = (ISI_TRANSFORM*) stream_data.data;
    }
	else if(toolName.find("_EYE_FRAME") != std::string::npos)
    {
      // Get the eye_frame (current camera frame w.r.t to world coordinates) 
	  // It doesn't matter what the manipulator index is, the EYE FRAME will be the same fo
      // all manipulators.
      isi_get_reference_frame(ISI_PSM2 , ISI_EYE_FRAME, transform);
      inverseTransform = true;  // This transform is the camera to world coordinates. We want our reference to be the inverse.
    }

    // If we really don't have data, keep on keeping on.
    if(transform == NULL) continue;

    setVtkMatrixFromISITransform(transformMatrix, transform);

	if(inverseTransform)
	{
		vtkMatrix4x4::Invert(transformMatrix, transformMatrix);
	}

	std::ostringstream transformStream;
	transformMatrix->Print(transformStream);
	LOG_TRACE("Updating toolname: " << toolName << " with transform:\n\t" << transformStream << "\n");

#ifdef USE_DAVINCI_TIMESTAMPS
    this->ToolTimeStampedUpdateWithoutFiltering( it->second->GetSourceId(), transformMatrix, TOOL_OK, timeSystemSec, timeSystemSec);
#else
    this->ToolTimeStampedUpdate( it->second->GetSourceId(), transformMatrix, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
#endif
  }

  LOG_TRACE("All subscribed fields from da Vinci have been updated");

}


ISI_MANIP_INDEX vtkIntuitiveDaVinciTracker::getManipIndexFromName(std::string toolName)
{
  ISI_MANIP_INDEX manipIndex = ISI_PSM1;

  toolName = toolName.substr(0, toolName.size() - 4);

  if(STRCASECMP("ISI_PSM1", toolName.c_str()) == 0)
  {
    manipIndex = ISI_PSM1;
  }
  else if(STRCASECMP("ISI_PSM2", toolName.c_str()) == 0)
  {
    manipIndex = ISI_PSM2;
  }
  else if(STRCASECMP("ISI_ECM", toolName.c_str()) == 0)
  {
    manipIndex = ISI_ECM;
  }
  else if(STRCASECMP("ISI_MTML1", toolName.c_str()) == 0)
  {
    manipIndex = ISI_MTML1;
  }
  else if(STRCASECMP("ISI_MTMR1", toolName.c_str()) == 0)
  {
    manipIndex = ISI_MTMR1;
  }
  else if(STRCASECMP("ISI_PSM3", toolName.c_str()) == 0)
  {
    manipIndex = ISI_PSM3;
  }
  else if(STRCASECMP("ISI_GANTRY", toolName.c_str()) == 0)
  {
    manipIndex = ISI_GANTRY;
  }
  else if(STRCASECMP("ISI_MTML2", toolName.c_str()) == 0)
  {
    manipIndex = ISI_MTML2;
  }
  else if(STRCASECMP("ISI_MTMR2", toolName.c_str()) == 0)
  {
    manipIndex = ISI_MTMR2;
  }
  else if(STRCASECMP("ISI_CONSOLE1", toolName.c_str()) == 0)
  {
    manipIndex = ISI_CONSOLE1;
  }
  else if(STRCASECMP("ISI_CONSOLE2", toolName.c_str()) == 0)
  {
    manipIndex = ISI_CONSOLE2;
  }
  else if(STRCASECMP("ISI_CORE", toolName.c_str()) == 0)
  {
    manipIndex = ISI_CORE;
  }
  else
  {
    LOG_DEBUG("Could not set manipIndex. Defaulting to PSM1");
  }

  return manipIndex;
}

void vtkIntuitiveDaVinciTracker::setVtkMatrixFromISITransform(vtkMatrix4x4* vtkMatrix, ISI_TRANSFORM* isiMatrix)
{
  vtkMatrix->Identity();

  // Let's VERY EXPLCITLY copy over the values. 

  vtkMatrix->SetElement(0,0, isiMatrix->rot.row0.x);
  vtkMatrix->SetElement(1,0, isiMatrix->rot.row0.y);
  vtkMatrix->SetElement(2,0, isiMatrix->rot.row0.z);
  vtkMatrix->SetElement(3,0,0);

  vtkMatrix->SetElement(0,1,isiMatrix->rot.row1.x);
  vtkMatrix->SetElement(1,1,isiMatrix->rot.row1.y);
  vtkMatrix->SetElement(2,1,isiMatrix->rot.row1.z);
  vtkMatrix->SetElement(3,1,0);

  vtkMatrix->SetElement(0,2,isiMatrix->rot.row2.x);
  vtkMatrix->SetElement(1,2,isiMatrix->rot.row2.y);
  vtkMatrix->SetElement(2,2,isiMatrix->rot.row2.z);
  vtkMatrix->SetElement(3,2,0);

  vtkMatrix->SetElement(0,3, isiMatrix->pos.x);
  vtkMatrix->SetElement(1,3, isiMatrix->pos.y);
  vtkMatrix->SetElement(2,3, isiMatrix->pos.z);
  vtkMatrix->SetElement(3,3,1);

  return;
}

namespace vtkIntuitiveDaVinciTrackerUtilities{

  void ISICALLBACK eventCB(ISI_MANIP_INDEX mid, ISI_EVENT_ID event_id, 
    ISI_INT args[ISI_NUM_EVENT_ARGS], void *userdata)
  {
    printf("(%.3f) mid: %s, \tevent: %s, args: (%d %d %d %d)\n", 
      isi_get_event_timestamp(mid, event_id),
      isi_get_manip_name(mid),
      isi_get_event_name(event_id), 
      args[0], args[1], args[2], args[3]);

    // for now, let's not actually do anything when an event occurs.
  }

  void ISICALLBACK streamCB(void *userData)
  {
    vtkIntuitiveDaVinciTracker* trackerInstance = reinterpret_cast<vtkIntuitiveDaVinciTracker*> (userData);
    if(trackerInstance)
    {
      trackerInstance->StreamCallback();
    }
  }

};

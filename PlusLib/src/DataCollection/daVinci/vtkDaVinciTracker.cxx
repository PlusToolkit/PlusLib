#include "daVinci.h"
#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkDaVinciTracker.h"
#include <fstream>
#include <iostream>
#include <set>

/****************************************************************************/

vtkStandardNewMacro(vtkDaVinciTracker);

//----------------------------------------------------------------------------
vtkDaVinciTracker::vtkDaVinciTracker()
{
#ifdef USE_DAVINCI_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif

  this->IsDaVinciTrackingInitialized = false;
  this->mDaVinci = new daVinci();

  // for accurate timing
  this->FrameNumber = 0;

  this->RequirePortNameInDeviceSetConfiguration = true;

  this->AcquisitionRate = 20;
}

//----------------------------------------------------------------------------
vtkDaVinciTracker::~vtkDaVinciTracker() 
{
  if(this->Recording)
  {
    this->StopRecording();
  }


  if ( this->mDaVinci != NULL )
  {    
    this->mDaVinci->Stop();
    this->IsDaVinciTrackingInitialized=false;
    delete this->mDaVinci;
    this->mDaVinci = NULL;
  }
}

//----------------------------------------------------------------------------
std::string vtkDaVinciTracker::GetSdkVersion()
{
  return this->mDaVinci->get_library_version(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDaVinciTracker::Probe()
{  

  if (this->IsDaVinciTrackingInitialized)
  {
    LOG_ERROR("vtkDaVinciTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  if(this->mDaVinci->Connect() != ISI_SUCCESS)
  {
    LOG_ERROR("vtkDaVinciTracker::Probe could not connect to the da Vinci!");
    return PLUS_FAIL;
  }

  // Get the list of manipulators for the da Vinci. 
  // If the list is zero, we've failed terribly. 
  // Possibly also check for names and verify we have what we expect.

  std::vector<std::string> manipNames = this->mDaVinci->get_manip_names_list();

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

  this->mDaVinci->Stop();
  this->IsDaVinciTrackingInitialized = false;
  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkDaVinciTracker::InternalStartRecording()
{
  if (!this->IsDaVinciTrackingInitialized)
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci has not been initialized");
    return PLUS_FAIL;
  }

  if(!this->mDaVinci->IsConnected())
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci is not connected");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDaVinciTracker::InternalStopRecording()
{
  // Stop the stream and disconnect from the da Vinci.
  this->mDaVinci->Stop();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDaVinciTracker::InternalUpdate()
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
PlusStatus vtkDaVinciTracker::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  LOG_TRACE("vtkDaVinciTracker::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // Determine which manipulators we're interested in.
  // Store those names for later, where we can then create the data source pointers.

  /* Because of how PLUS works when we create a vtkPlusDevice,
  the XML file is already parsed. The parent class reads in the configuration
  file, and finds all the data sources that are tools.

  It then parses the tool tags and adds them (if possible) to this device.

  I think.

  See vtkPlusDevice.cxx : ReadConfiguration( ... )

  */

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDaVinciTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkDaVinciTracker::InternalConnect()
{ 
  // Before trying to get to the da Vinci, let's see what was read in our XML config file.
  // That is, what manipulators did we say we were interested in?

  LOG_TRACE("vtkDaVinciTracker::InternalConnect");

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

  while(!initCompleted && connectionAttempts < MAX_ATTEMPTS)
  {
    if(this->mDaVinci->Connect() != 0)
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
  if(!this->mDaVinci->Subscribe(NULL, streamCB))
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
PlusStatus vtkDaVinciTracker::InternalDisconnect()
{ 
  if (this->IsDaVinciTrackingInitialized)
  {
    this->mDaVinci->Stop();  
    this->IsDaVinciTrackingInitialized = false;
  }  
  return PLUS_SUCCESS;
}

void ISICALLBACK vtkDaVinciTracker::eventCB(ISI_MANIP_INDEX mid, ISI_EVENT_ID event_id, 
  ISI_INT args[ISI_NUM_EVENT_ARGS], void *userdata)
{
  printf("(%.3f) mid: %s, \tevent: %s, args: (%d %d %d %d)\n", 
    isi_get_event_timestamp(mid, event_id),
    isi_get_manip_name(mid),
    isi_get_event_name(event_id), 
    args[0], args[1], args[2], args[3]);

  // for now, let's not actually do anything when an event occurs.
}

void ISICALLBACK vtkDaVinciTracker::streamCB(void *userdata)
{
  if (!this->IsDaVinciTrackingInitialized)
  {
    LOG_ERROR("InternalUpdate failed: daVinci has not been initialized");
    return PLUS_FAIL;
  }

  if(!this->Recording)
  {
    // Drop the frame, we're not recording.
    LOG_DEBUG("Dropped frame : daVinci is not recording");
    return PLUS_SUCCESS;
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
    ISI_TRANSFORM *T;

    LOG_TRACE("All subscribed fields from da Vinci have been updated");

    std::string toolName = it->second->GetPortName();

    int manipIndex = getManipIndexFromName(toolName);

    if(manipIndex == -1) continue;

    if(toolName.find("_TIP") != std::string::npos)
    {
      // Get the tip transform for this manipulator
      isi_get_stream_field( manipIndex , ISI_TIP_TRANSFORM, &stream_data);

    }
    else
    {
      // For this manipulator get the eye_frame (current coordinate frame w.r.t to world coordinates) 
      isi_get_reference_frame(manipIndex , ISI_EYE_FRAME, &stream_data);

    }

    T = (ISI_TRANSFORM*) stream_data.data;

    // If we really don't have data, keep on keeping on.
    if(T == NULL) continue;

    setVtkMatrixFromISITransform(transformMatrix, T);

    delete T;
    T = NULL;

#ifdef USE_DAVINCI_TIMESTAMPS
    this->ToolTimeStampedUpdateWithoutFiltering( tool->GetSourceId(), transformMatrix, TOOL_OK, timeSystemSec, timeSystemSec);
#else
    this->ToolTimeStampedUpdate( tool->GetSourceId(), transformMatrix, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
#endif
  }
}

int vtkDaVinciTracker::getManipIndexFromName(std::string& toolName)
{
  int manipIndex == -1;

  toolName = toolName.substr(0, toolName.size() - 4);

  if(STRCASECMP("ISI_PSM1", toolName) == 0)
  {
    manipIndex = ISI_PSM1
  }
  else if(STRCASECMP("ISI_PSM2", toolName) == 0)
  {
    manipIndex = ISI_PSM2
  }
  else if(STRCASECMP("ISI_ECM", toolName) == 0)
  {
    manipIndex = ISI_ECM
  }
  else if(STRCASECMP("ISI_MTML1", toolName) == 0)
  {
    manipIndex = ISI_MTML1
  }
  else if(STRCASECMP("ISI_MTMR1", toolName) == 0)
  {
    manipIndex = ISI_MTMR1
  }
  else if(STRCASECMP("ISI_PSM3", toolName) == 0)
  {
    manipIndex = ISI_PSM3
  }
  else if(STRCASECMP("ISI_GANTRY", toolName) == 0)
  {
    manipIndex = ISI_GANTRY
  }
  else if(STRCASECMP("ISI_MTML2", toolName) == 0)
  {
    manipIndex = ISI_MTML2
  }
  else if(STRCASECMP("ISI_MTMR2", toolName) == 0)
  {
    manipIndex = ISI_MTMR2
  }
  else if(STRCASECMP("ISI_CONSOLE1", toolName) == 0)
  {
    manipIndex = ISI_CONSOLE1
  }
  else if(STRCASECMP("ISI_CONSOLE2", toolName) == 0)
  {
    manipIndex = ISI_CONSOLE2
  }
  else if(STRCASECMP("ISI_CORE", toolName) == 0)
  {
    manipIndex = ISI_CORE
  }
  else
  {
    LOG_DEBUG("Could not set manipIndex.");
  }

  return manipIndex;
}

void vtkDaVinciTracker::setVtkMatrixFromISITransform(vtkMtarix4x4* vtkMatrix, ISI_TRANSFORM* isiMatrix)
{
  transformMatrix->Identity();

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

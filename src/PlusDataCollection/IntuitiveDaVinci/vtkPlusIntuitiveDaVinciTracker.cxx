/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "igsioCommon.h"
#include "PlusConfigure.h"
#include "vtkPlusIntuitiveDaVinciTracker.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>

// OS includes
#include <ctype.h>
#include <float.h>
#include <iomanip>
#include <limits.h>
#include <math.h>
#include <time.h>

// STL
#include <fstream>
#include <iostream>
#include <set>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusIntuitiveDaVinciTracker);

//----------------------------------------------------------------------------
vtkPlusIntuitiveDaVinciTracker::vtkPlusIntuitiveDaVinciTracker()
  : vtkPlusDevice()
  , DaVinci(new IntuitiveDaVinci())
  , LastFrameNumber(0)
  , FrameNumber(0)
  , IpAddr("10.0.0.5")
  , Port(5002)
  , Password("")
#ifdef USE_DAVINCI_TIMESTAMPS
  , TrackerTimeToSystemTimeSec(0.0)
  , TrackerTimeToSystemTimeComputed(false)
#endif
{
  this->StartThreadForInternalUpdates = false; // Callback based system
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->AcquisitionRate = 20;
}

//----------------------------------------------------------------------------
vtkPlusIntuitiveDaVinciTracker::~vtkPlusIntuitiveDaVinciTracker()
{
  this->StopRecording();
  this->Disconnect();

  if (this->DaVinci != nullptr)
  {
    this->DaVinci->stop();
    delete this->DaVinci;
    this->DaVinci = nullptr;
  }
}

//----------------------------------------------------------------------------
void vtkPlusIntuitiveDaVinciTracker::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
std::string vtkPlusIntuitiveDaVinciTracker::GetSdkVersion()
{
  return this->DaVinci->getLibraryVersion();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::Probe()
{
  if (this->Connected)
  {
    LOG_ERROR("vtkPlusIntuitiveDaVinciTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  if (this->DaVinci->connect() != ISI_SUCCESS)
  {
    LOG_ERROR("vtkPlusIntuitiveDaVinciTracker::Probe could not connect to the da Vinci!");
    return PLUS_FAIL;
  }

  // Get the list of manipulators for the da Vinci.
  // If the list is zero, we've failed terribly.
  // Possibly also check for names and verify we have what we expect.
  std::vector<std::string> manipNames = this->DaVinci->getManipulatorNames();

  if (manipNames.size() == 0)
  {
    LOG_ERROR("Error in retrieving manipulator information. Zero manipulators found.");
    return PLUS_FAIL;
  }

  for (int i = 0; i < manipNames.size(); i++)
  {
    LOG_DEBUG("Manipulator " << i << " name: " << manipNames[i] << "\n");
  }

  // TODO: Probe to see what we've specified in the XML file actually exists in the da Vinci

  this->DaVinci->stop();
  this->DaVinci->disconnect();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalStartRecording()
{
  if (!this->Connected)
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci has not been initialized");
    return PLUS_FAIL;
  }

  if (!this->DaVinci->isConnected())
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci is not connected");
    return PLUS_FAIL;
  }

  if (!this->DaVinci->start())
  {
    LOG_ERROR("InternalStartRecording: Unable to start streaming.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalStopRecording()
{
  // Stop the stream and disconnect from the da Vinci.
  this->DaVinci->stop();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusIntuitiveDaVinciTracker::ReadConfiguration");
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
  XML_READ_SCALAR_ATTRIBUTE_WARNING(unsigned int, Port, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_WARNING(Password, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(IpAddr, trackerConfig);
  trackerConfig->SetIntAttribute("Port", this->Port);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(Password, trackerConfig)''

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalConnect()
{
  // Before trying to get to the da Vinci, let's see what was read in our XML config file.
  // That is, what manipulators did we say we were interested in?
  LOG_TRACE("vtkPlusIntuitiveDaVinciTracker::InternalConnect");

  if (this->Connected)
  {
    LOG_DEBUG("Already connected to da Vinci");
    return PLUS_SUCCESS;
  }

  // Try to connect a few different times. If at first you don't
  // succeed, try again!
  int connectionAttempts(0);
  bool initCompleted(false);

  // Set our connection parameters obtained from the config file
  this->DaVinci->setHostInfo(IpAddr, Port, Password);

  while (!initCompleted && connectionAttempts < MAX_ATTEMPTS)
  {
    if (this->DaVinci->connect() != 0)
    {
      LOG_DEBUG("Failed to connect to da Vinci. Retry: " << connectionAttempts);
      vtkIGSIOAccurateTimer::Delay(1.0);
      connectionAttempts++;
    }

    initCompleted = true;
  }

  if (connectionAttempts == MAX_ATTEMPTS)
  {
    LOG_ERROR("Error in initializing da Vinci");
    return PLUS_FAIL;
  }

  if (this->DaVinci->subscribe(NULL, vtkPlusIntuitiveDaVinciTrackerUtilities::streamCB, NULL, this) != ISI_SUCCESS)
  {
    LOG_ERROR("Error in subscribing to events and stream! Stream not started!");
    return PLUS_FAIL;
  }

#ifdef USE_DAVINCI_TIMESTAMPS
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
#endif

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalDisconnect()
{
  this->DaVinci->disconnect();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
IntuitiveDaVinci* vtkPlusIntuitiveDaVinciTracker::GetDaVinci() const
{
  return this->DaVinci;
}

//----------------------------------------------------------------------------
void vtkPlusIntuitiveDaVinciTracker::StreamCallback(void)
{
  if (!this->Connected)
  {
    LOG_ERROR("InternalUpdate failed: daVinci has not been initialized");
    return;
  }

  if (!this->Recording)
  {
    // Drop the frame, we're not recording.
    LOG_DEBUG("Dropped frame: daVinci is not recording");
    return;
  }

  // Generate a frame number, as the tool does not provide a frame number.
  // FrameNumber will be used in ToolTimeStampedUpdate for timestamp filtering
  ++this->FrameNumber;

  // Setting the timestamp
  const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

#ifdef USE_DAVINCI_TIMESTAMPS
  if (!this->TrackerTimeToSystemTimeComputed)
  {
    const double timeSystemSec = unfilteredTimestamp;
    const double timeTrackerSec = this->DaVinci->mtGetLatestFrameTime();
    this->TrackerTimeToSystemTimeSec = timeSystemSec - timeTrackerSec;
    this->TrackerTimeToSystemTimeComputed = true;
  }
  const double timeTrackerSec = this->DaVinci->mtGetLatestFrameTime();
  const double timeSystemSec = timeTrackerSec + this->TrackerTimeToSystemTimeSec;
#endif

  vtkNew<vtkMatrix4x4> transformMatrix;

  ISI_TRANSFORM* transform(NULL);
  ISI_STREAM_FIELD stream_data;
  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    std::string toolName = it->second->GetPortName();

    ISI_MANIP_INDEX manipIndex = getManipIndexFromName(toolName);

    if (manipIndex == -1)
    {
      continue;
    }

    bool inverseTransform(false);
    if (toolName.find("_TIP") != std::string::npos)
    {
      // Get the tip transform for this manipulator
      isi_get_stream_field(manipIndex, ISI_TIP_TRANSFORM, &stream_data);
      transform = (ISI_TRANSFORM*) stream_data.data;
    }
    else if (toolName.find("_EYE_FRAME") != std::string::npos)
    {
      // Get the eye_frame (current camera frame w.r.t to world coordinates)
      // It doesn't matter what the manipulator index is, the EYE FRAME will be the same fo
      // all manipulators.
      isi_get_reference_frame(ISI_PSM2, ISI_EYE_FRAME, transform);
      inverseTransform = true;  // This transform is the camera to world coordinates. We want our reference to be the inverse.
    }

    // If we really don't have data, keep on keeping on.
    if (transform == NULL)
    {
      continue;
    }

    setVtkMatrixFromISITransform(*transformMatrix, transform);

    if (inverseTransform)
    {
      vtkMatrix4x4::Invert(transformMatrix, transformMatrix);
    }

    std::ostringstream transformStream;
    transformMatrix->Print(transformStream);
    LOG_TRACE("Updating toolname: " << toolName << " with transform:\n\t" << transformStream << "\n");

#ifdef USE_DAVINCI_TIMESTAMPS
    this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetSourceId(), transformMatrix, TOOL_OK, timeSystemSec, timeSystemSec);
#else
    this->ToolTimeStampedUpdate(it->second->GetSourceId(), transformMatrix, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
#endif
  }

  LOG_TRACE("All subscribed fields from da Vinci have been updated");
}

//----------------------------------------------------------------------------
ISI_MANIP_INDEX vtkPlusIntuitiveDaVinciTracker::getManipIndexFromName(const std::string& toolName)
{
  ISI_MANIP_INDEX manipIndex = ISI_PSM1;

  toolName = toolName.substr(0, toolName.size() - 4);

  if (igsioCommon::IsEqualInsensitive("ISI_PSM1", toolName))
  {
    manipIndex = ISI_PSM1;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_PSM2", toolName))
  {
    manipIndex = ISI_PSM2;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_ECM", toolName))
  {
    manipIndex = ISI_ECM;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_MTML1", toolName))
  {
    manipIndex = ISI_MTML1;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_MTMR1", toolName))
  {
    manipIndex = ISI_MTMR1;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_PSM3", toolName))
  {
    manipIndex = ISI_PSM3;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_GANTRY", toolName))
  {
    manipIndex = ISI_GANTRY;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_MTML2", toolName))
  {
    manipIndex = ISI_MTML2;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_MTMR2", toolName))
  {
    manipIndex = ISI_MTMR2;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_CONSOLE1", toolName))
  {
    manipIndex = ISI_CONSOLE1;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_CONSOLE2", toolName))
  {
    manipIndex = ISI_CONSOLE2;
  }
  else if (igsioCommon::IsEqualInsensitive("ISI_CORE", toolName))
  {
    manipIndex = ISI_CORE;
  }
  else
  {
    LOG_DEBUG("Could not set manipIndex. Defaulting to PSM1");
  }

  return manipIndex;
}

//----------------------------------------------------------------------------
void vtkPlusIntuitiveDaVinciTracker::setVtkMatrixFromISITransform(vtkMatrix4x4& destVtkMatrix, ISI_TRANSFORM* srcIsiMatrix)
{
  destVtkMatrix->Identity();

  // Let's VERY EXPLCITLY copy over the values.
  destVtkMatrix.SetElement(0, 0, srcIsiMatrix->rot.row0.x);
  destVtkMatrix.SetElement(1, 0, srcIsiMatrix->rot.row0.y);
  destVtkMatrix.SetElement(2, 0, srcIsiMatrix->rot.row0.z);
  destVtkMatrix.SetElement(3, 0, 0);

  destVtkMatrix.SetElement(0, 1, srcIsiMatrix->rot.row1.x);
  destVtkMatrix.SetElement(1, 1, srcIsiMatrix->rot.row1.y);
  destVtkMatrix.SetElement(2, 1, srcIsiMatrix->rot.row1.z);
  destVtkMatrix.SetElement(3, 1, 0);

  destVtkMatrix.SetElement(0, 2, srcIsiMatrix->rot.row2.x);
  destVtkMatrix.SetElement(1, 2, srcIsiMatrix->rot.row2.y);
  destVtkMatrix.SetElement(2, 2, srcIsiMatrix->rot.row2.z);
  destVtkMatrix.SetElement(3, 2, 0);

  destVtkMatrix.SetElement(0, 3, srcIsiMatrix->pos.x);
  destVtkMatrix.SetElement(1, 3, srcIsiMatrix->pos.y);
  destVtkMatrix.SetElement(2, 3, srcIsiMatrix->pos.z);
  destVtkMatrix.SetElement(3, 3, 1);

  return;
}

namespace vtkPlusIntuitiveDaVinciTrackerUtilities
{
  //----------------------------------------------------------------------------
  void ISICALLBACK eventCB(ISI_MANIP_INDEX mid, ISI_EVENT_ID event_id, ISI_INT args[ISI_NUM_EVENT_ARGS], void* userdata)
  {
    LOG_INFO(isi_get_event_timestamp(mid, event_id) << " mid: " << isi_get_manip_name(mid) << ",\tevent: " << isi_get_event_name(event_id) << ", args: (" << args[0] << " " << args[1] << " " << args[2] << " " << args[3] << ")");
  }

  //----------------------------------------------------------------------------
  void ISICALLBACK streamCB(void* userData)
  {
    vtkPlusIntuitiveDaVinciTracker* trackerInstance = reinterpret_cast<vtkPlusIntuitiveDaVinciTracker*>(userData);
    if (trackerInstance)
    {
      trackerInstance->StreamCallback();
    }
  }
};

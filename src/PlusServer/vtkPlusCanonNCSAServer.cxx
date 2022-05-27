/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusCanonNCSAServer.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkIGSIOTransformRepository.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

// Canon NCSA includes


// STL includes
#include <fstream>
#include <streambuf>
#include <cstring>

namespace
{
  //----------------------------------------------------------------------------
 // If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
 // then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up.
 // This time should be long enough to comfortably retrieve a frame from the buffer.
  const double SAMPLING_SKIPPING_MARGIN_SEC = 0.1;
  const double DELAY_ON_SENDING_ERROR_SEC = 0.02;
  const double DELAY_ON_NO_NEW_FRAMES_SEC = 0.005;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusCanonNCSAServer);

//----------------------------------------------------------------------------
vtkPlusCanonNCSAServer::vtkPlusCanonNCSAServer()
  : TransformRepository(NULL)
  , DataCollector(NULL)
  , Threader(vtkSmartPointer<vtkMultiThreader>::New())
  , BroadcastChannel(NULL)
  , DataSenderThreadId(-1)
  , CompositionSpace(nullptr)
  , DataSource(nullptr)
{

}

//----------------------------------------------------------------------------
vtkPlusCanonNCSAServer::~vtkPlusCanonNCSAServer()
{
  this->Stop();
  this->SetTransformRepository(NULL);
  this->SetDataCollector(NULL);
  this->SetConfigFilename(NULL);
}

//----------------------------------------------------------------------------
bool vtkPlusCanonNCSAServer::JoinCompositionSpace()
{
  auto status = ncsa::CommStatus::SUCCESS;
  this->CompositionSpace = ncsa::CompositionSpace::join(status, *this, this->Config.getValue<std::string>(ConfigurationCommon::Option::COMPOSITION_SPACE).c_str());

  if (status != ncsa::CommStatus::SUCCESS)
  {
    std::string statusText;
    ncsa::CompositionSpace::getStatusString(status, statusText);
    LOG_ERROR("Unable to connect to composition space: " << statusText.c_str());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusCanonNCSAServer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::StartService()
{
  if (this->DataCollector == NULL)
  {
    LOG_WARNING("Tried to start Canon NCSA server without a vtkPlusDataCollector");
    return PLUS_FAIL;
  }

  if (this->JoinCompositionSpace() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to join composition space.");
  }

  if (this->DataSenderThreadId < 0)
  {
    this->DataSenderActive.Request = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    this->DataSenderThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&DataSenderThread, this);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::StopService()
{
  LOG_INFO("Plus Canon NCSA server stopping...");

  this->DataSenderActive.Request = false;
  unsigned int millis = 0;
  while (this->DataSenderActive.Respond != false && millis < 2000)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    millis += 100;
  }
  
  this->DataSource->destroy();
  this->DataSource = nullptr;

  this->CompositionSpace->leave();
  this->CompositionSpace = nullptr;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void* vtkPlusCanonNCSAServer::DataSenderThread(vtkMultiThreader::ThreadInfo* data)
{
  vtkPlusCanonNCSAServer* self = (vtkPlusCanonNCSAServer*)(data->UserData);

  vtkPlusDevice* aDevice(NULL);
  vtkPlusChannel* aChannel(NULL);

  DeviceCollection aCollection;
  if (self->DataCollector->GetDevices(aCollection) != PLUS_SUCCESS || aCollection.size() == 0)
  {
    LOG_ERROR("Unable to retrieve devices. Check configuration and connection.");
    return NULL;
  }

  // Find the requested channel ID in all the devices
  for (DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it)
  {
    aDevice = *it;
    if (aDevice->GetOutputChannelByName(aChannel, self->GetOutputChannelId()) == PLUS_SUCCESS)
    {
      break;
    }
  }

  if (aChannel == NULL)
  {
    // The requested channel ID is not found
    if (!self->GetOutputChannelId().empty())
    {
      // the user explicitly requested a specific channel, but none was found by that name
      // this is an error
      LOG_ERROR("Unable to start data sending. OutputChannelId not found: " << self->GetOutputChannelId());
      return NULL;
    }
    // the user did not specify any channel, so just use the first channel that can be found in any device
    for (DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it)
    {
      aDevice = *it;
      if (aDevice->OutputChannelCount() > 0)
      {
        aChannel = *(aDevice->GetOutputChannelsStart());
        break;
      }
    }
  }

  // If we didn't find any channel then return
  if (aChannel == NULL)
  {
    LOG_WARNING("There are no channels to broadcast. Only command processing is available.");
  }

  self->BroadcastChannel = aChannel;

  double elapsedTimeSinceLastPacketSentSec = 0;
  while (self->DataSenderActive.Request)
  {
    // Send image/tracking/string data
    SendLatestFramesToClients(*self, elapsedTimeSinceLastPacketSentSec);
  }

  // Close thread
  self->DataSenderThreadId = -1;
  self->DataSenderActive.Respond = false;
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::SendLatestFramesToClients(vtkPlusCanonNCSAServer& self, double& elapsedTimeSinceLastPacketSentSec)
{
  double startTimeSec = vtkIGSIOAccurateTimer::GetSystemTime();
  igsioTrackedFrame frame;

  if (self.BroadcastChannel != NULL)
  {
    if (self.BroadcastChannel->GetTrackedFrame(frame) != PLUS_SUCCESS)
    {
      vtkIGSIOAccurateTimer::Delay(DELAY_ON_NO_NEW_FRAMES_SEC);
      elapsedTimeSinceLastPacketSentSec += vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec;

      return PLUS_FAIL;
    }
  }

  // Send tracked frame
  self.SendTrackedFrame(frame);

  // Compute time spent with processing one frame in this round
  double computationTimeMs = (vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::SendTrackedFrame(igsioTrackedFrame& trackedFrame)
{
  int numberOfErrors = 0;

  // Update transform repository with the tracked frame
  if (this->TransformRepository != NULL)
  {
    if (this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set current transforms to transform repository");
      numberOfErrors++;
    }
  }

  // Convert relative timestamp to UTC
  double timestampSystem = trackedFrame.GetTimestamp(); // save original timestamp, we'll restore it later
  double timestampUniversal = vtkIGSIOAccurateTimer::GetUniversalTimeFromSystemTime(timestampSystem);
  trackedFrame.SetTimestamp(timestampUniversal);

  if (trackedFrame.GetImageData()->GetImage() == nullptr)
  {
    LOG_ERROR("No image in tracked frame. Skipping.");
    return PLUS_FAIL;
  }

  auto dataType = ncsa::DataSrc::RGB;
  if (trackedFrame.GetImageData()->GetImage()->GetNumberOfScalarComponents() == 1)
  {
    dataType = ncsa::DataSrc::GRAY;
  }

  // Get the canvas position from the UI.
  const ncsa::Position2D canvasPosition = { 0, 0 };

  // The canvas size will be the same as the image size.
  const ncsa::Dimensions canvasDimensions = { static_cast<unsigned int>(trackedFrame.GetImageData()->GetImage()->GetDimensions()[0]), 
    static_cast<unsigned int>(trackedFrame.GetImageData()->GetImage()->GetDimensions()[1]) };

  const ncsa::DataSrc::ArrayDimensions bufferDimensions = { static_cast<unsigned int>(trackedFrame.GetImageData()->GetImage()->GetDimensions()[0]),
                                                            static_cast<unsigned int>(trackedFrame.GetImageData()->GetImage()->GetDimensions()[1]),
                                                            static_cast<unsigned int>(trackedFrame.GetImageData()->GetImage()->GetNumberOfScalarComponents() * 8) };

  if (this->DataSource == nullptr)
  {
    ncsa::CommStatus status;
    this->DataSource = ncsa::DataSrc::create(status, dataType, bufferDimensions, this->CompositionSpace, this->GetOutputChannelId());
    if (ncsa::CommStatus::SUCCESS != status)
    {
      std::string statusText;
      ncsa::CompositionSpace::getStatusString(status, statusText);
      LOG_ERROR("Unable to create data source: " << statusText.c_str());
      return PLUS_FAIL;
    }
  }

  ncsa::CommStatus status;
  // Get the buffer from the DataSrc.
  auto dsBuffer = this->DataSource->getBuffer(status);
  if (ncsa::CommStatus::SUCCESS != status)
  {
    std::string statusText;
    ncsa::CompositionSpace::getStatusString(status, statusText);
    LOG_ERROR("Unable to get data buffer: " << statusText.c_str());
    return PLUS_FAIL;
  }

  // Copy the image into the buffer.
  std::memcpy(dsBuffer, trackedFrame.GetImageData()->GetImage()->GetScalarPointer(), bufferDimensions.numBytes());

  // Write the data in the buffer to Composition Space.
  status = this->CompositionSpace->update(*this->DataSource);
  if (ncsa::CommStatus::SUCCESS != status)
  {
    std::string statusText;
    ncsa::CompositionSpace::getStatusString(status, statusText);
    LOG_ERROR("Unable to update composition space with image: " << statusText.c_str());
    return PLUS_FAIL;
  }

  // restore original timestamp
  trackedFrame.SetTimestamp(timestampSystem);

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::ReadConfiguration(vtkXMLDataElement* serverElement, const std::string& aFilename)
{
  LOG_TRACE("vtkPlusCanonNCSAServer::ReadConfiguration");

  if (aFilename.empty())
  {
    LOG_ERROR("Unable to configure PlusServer without an acceptable config file submitted.");
    return PLUS_FAIL;
  }
  this->SetConfigFilename(aFilename);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(OutputChannelId, serverElement);

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::Start(vtkPlusDataCollector* dataCollector, vtkIGSIOTransformRepository* transformRepository, vtkXMLDataElement* serverElement, const std::string& configFilePath, int argc, char** argv)
{
  if (serverElement == NULL)
  {
    LOG_ERROR("NULL configuration sent to vtkPlusCanonNCSAServer::Start. Unable to start PlusServer.");
    return PLUS_FAIL;
  }

  Config.load(argc, argv);

  this->SetDataCollector(dataCollector);
  if (this->ReadConfiguration(serverElement, configFilePath.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read PlusCanonNCSAServer configuration");
    return PLUS_FAIL;
  }

  this->SetTransformRepository(transformRepository);
  if (this->StartService() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to start Plus Canon NCSA server");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCanonNCSAServer::Stop()
{
  PlusStatus status = PLUS_SUCCESS;

  if (StopService() != PLUS_SUCCESS)
  {
    status = PLUS_FAIL;
  }

  SetDataCollector(NULL);

  SetTransformRepository(NULL);

  return status;
}

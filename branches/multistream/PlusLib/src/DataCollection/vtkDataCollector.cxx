/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusStream.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkTrackedFrameList.h"
#include "vtkVirtualStreamMixer.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 2.0$");
vtkStandardNewMacro(vtkDataCollector);

//----------------------------------------------------------------------------

vtkDataCollector::vtkDataCollector()
: vtkObject()
, SelectedStreamMixer(NULL)
, Connected(false)
, Started(false)
, StartupDelaySec(0.0) 
{

}

//----------------------------------------------------------------------------

vtkDataCollector::~vtkDataCollector()
{
  if( Started )
  {
    this->Stop();
  }
  if( Connected )
  {
    this->Disconnect();
  }
  Devices.clear();
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  LOG_TRACE("vtkDataCollector::ReadConfiguration()");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* dataCollectionElement = aConfig->FindNestedElementWithName("DataCollection");

  if (dataCollectionElement == NULL)
  {
    LOG_ERROR("Unable to find data collection element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Read StartupDelaySec
  double startupDelaySec(0.0); 
  if ( dataCollectionElement->GetScalarAttribute("StartupDelaySec", startupDelaySec) )
  {
    this->SetStartupDelaySec(startupDelaySec); 
    LOG_DEBUG("StartupDelaySec: " << std::fixed << startupDelaySec ); 
  }

  vtkSmartPointer<vtkPlusDeviceFactory> factory = vtkSmartPointer<vtkPlusDeviceFactory>::New();

  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* deviceElement = dataCollectionElement->GetNestedElement(i); 

    if (STRCASECMP(deviceElement->GetName(), "Device") == 0 )
    {
      vtkPlusDevice* device = NULL;
      if( factory->CreateInstance(deviceElement->GetAttribute("Type"), device, deviceElement->GetAttribute("Id")) == PLUS_FAIL )
      {    
        LOG_ERROR("Unable to create device: " << deviceElement->GetAttribute("Type"));
      }
      else
      {
        device->ReadConfiguration(deviceElement);
        Devices.push_back(device);
      }
    }
  }

  // Connect any and all input streams to their corresponding output streams
  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* deviceElement = dataCollectionElement->GetNestedElement(i); 

    if (STRCASECMP(deviceElement->GetName(), "Device") == 0 )
    {
      vtkPlusDevice* thisDevice = NULL;
      if( this->GetDevice(thisDevice, deviceElement->GetAttribute("Id")) != PLUS_SUCCESS )
      {
        LOG_ERROR("Device " << deviceElement->GetAttribute("Id") << " doesn't exist.");
        continue;
      }

      for ( int i = 0; i < deviceElement->GetNumberOfNestedElements(); ++i )
      {
        vtkXMLDataElement* streamElement = deviceElement->GetNestedElement(i); 
        if( STRCASECMP(streamElement->GetName(), "InputStream") == 0 )
        {
          // We have an input stream, lets find it
          for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
          {
            vtkPlusDevice* device = (*it);
            vtkPlusStream* aStream = NULL;
            if( device->GetStreamByName(aStream, streamElement->GetAttribute("Id")) == PLUS_SUCCESS )
            {
              // Found it!
              device->AddInputStream(aStream);
              continue;
            }
          }
        }
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::WriteConfiguration( vtkXMLDataElement* aConfig )
{
  LOG_TRACE("vtkDataCollector::WriteConfiguration()");

  vtkXMLDataElement* dataCollectionConfig = PlusXmlUtils::GetNestedElementWithName(aConfig,"DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  dataCollectionConfig->SetDoubleAttribute("StartupDelaySec", GetStartupDelaySec());

  PlusStatus status = PLUS_SUCCESS;

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);

    if( device->WriteConfiguration(aConfig) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save device configuration " << device->GetDeviceId() );
      status = PLUS_FAIL;
    }
  }

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Start()
{
  LOG_TRACE("vtkDataCollector::Start()");

  PlusStatus status = PLUS_SUCCESS;

  const double startTime = vtkAccurateTimer::GetSystemTime(); 

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->StartRecording() != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to start data acquisition for device " << device->GetDeviceId() << ".");
      status = PLUS_FAIL;
    }
    device->SetStartTime(startTime);
  }

  LOG_DEBUG("vtkDataCollector::Start -- wait " << std::fixed << this->StartupDelaySec << " sec for buffer init..."); 
  vtkAccurateTimer::Delay(this->StartupDelaySec);

  Started = true;

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Stop()
{
  LOG_TRACE("vtkDataCollector::Stop()");

  Started = false;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Connect()
{
  LOG_TRACE("vtkDataCollector::Connect()");

  PlusStatus status = PLUS_SUCCESS;

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->Connect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to connect device: " << device->GetDeviceId() <<".");
      status = PLUS_FAIL;
    }
  }

  if( status != PLUS_SUCCESS )
  {
    this->Disconnect();
    return status;
  }

  Connected = true;
  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Disconnect()
{
  LOG_TRACE("vtkDataCollector::Disconnect()");

  PlusStatus status = PLUS_SUCCESS;

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->Disconnect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disconnect device: " << device->GetDeviceId() <<".");
      status = PLUS_FAIL;
    }
  }

  Connected = false;
  this->SelectedStreamMixer = NULL;

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetMostRecentTimestamp( double &ts ) const
{
  LOG_TRACE("vtkDataCollector::GetMostRecentTimestamp()");

  if( SelectedStreamMixer != NULL )
  {
    return SelectedStreamMixer->GetMostRecentTimestamp(ts);
  }

  LOG_ERROR("No selected tracked frame producer. No one to ask for most recent timestamp!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetTrackedFrameList( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd /*= -1*/ ) const
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameList(" << aTimestamp << ", aTrackedFrameList, " << aMaxNumberOfFramesToAdd << ")");

  if( SelectedStreamMixer != NULL )
  {
    return SelectedStreamMixer->GetTrackedFrameList(aTimestamp, aTrackedFrameList, aMaxNumberOfFramesToAdd);
  }

  LOG_ERROR("No selected tracked frame producer. No one to ask for a tracked frame list!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

void vtkDataCollector::PrintSelf( ostream& os, vtkIndent indent )
{
  LOG_TRACE("vtkDataCollector::PrintSelf()");

  this->Superclass::PrintSelf(os,indent);

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    os << indent << "Device: " << std::endl; 
    (*it)->PrintSelf(os, indent); 
  }
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetDevice( vtkPlusDevice* &aDevice, const std::string &aDeviceId ) const
{
  LOG_TRACE("vtkDataCollector::GetDevice( aDevice, " << aDeviceId << ")");

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);

    if( STRCASECMP(device->GetDeviceId(), aDeviceId.c_str()) == 0 )
    {
      aDevice = device;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetTrackingEnabled() const
{
  LOG_TRACE("vtkDataCollector::GetTrackingEnabled()");

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    if( (*it)->IsTracker() )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------
bool vtkDataCollector::GetVideoEnabled() const
{
  if( SelectedStreamMixer != NULL )
  {
    for( StreamBufferContainerConstIterator it = SelectedStreamMixer->GetStream()->GetBuffersStartConstIterator(); it != SelectedStreamMixer->GetStream()->GetBuffersEndConstIterator(); ++it)
    {
      vtkPlusStreamBuffer* buffer = *it;
      StreamBufferItem* item = NULL;
      if( buffer->GetLatestDataBufferItem(item) != ITEM_OK )
      {
        continue;
      }
      int frameSize[2];
      if( item->GetFrame().GetFrameSize(frameSize) != PLUS_SUCCESS )
      {
        continue;
      }
      if( frameSize[0] > 0 && frameSize[1] > 0 )
      {
        return true;
      }
    }
  }

  return false;
}


//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetTrackedFrame( TrackedFrame* trackedFrame )
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrame()");

  if( SelectedStreamMixer != NULL )
  {
    return SelectedStreamMixer->GetTrackedFrame(trackedFrame);
  }

  LOG_ERROR("No selected tracked frame producer. No one to ask for a tracked frame!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::SetSelectedStreamMixer( const std::string &aDeviceId )
{
  LOG_TRACE("vtkDataCollector::SetSelectedStreamMixer(" << aDeviceId << ")");

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);
    if( dynamic_cast<vtkVirtualStreamMixer*>(device) != NULL && aDeviceId.compare(device->GetDeviceId()) == 0 )
    {
      SelectedStreamMixer = dynamic_cast<vtkVirtualStreamMixer*>(device);
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetStreamMixers( std::vector<vtkVirtualStreamMixer*> &OutVector ) const
{
  LOG_TRACE("vtkDataCollector::GetStreamMixers()");

  OutVector.clear();

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);
    if( dynamic_cast<vtkVirtualStreamMixer*>(device) != NULL )
    {
      OutVector.push_back(dynamic_cast<vtkVirtualStreamMixer*>(device));
    }
  }

  return OutVector.size() > 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetSelectedStreamMixer( vtkVirtualStreamMixer* &aDevice )
{
  LOG_TRACE("vtkDataCollector::GetSelectedStreamMixer()");

  if( SelectedStreamMixer != NULL )
  {
    aDevice = SelectedStreamMixer;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetConnected() const
{
  return Connected;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetTrackingDataAvailable() const
{
  LOG_TRACE("vtkDataCollector::GetTrackingDataAvailable()");

  if( SelectedStreamMixer != NULL )
  {
    return SelectedStreamMixer->GetTrackingDataAvailable();
  }

  return false;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetVideoDataAvailable() const
{
  LOG_TRACE("vtkDataCollector::GetVideoDataAvailable()");

  if( SelectedStreamMixer != NULL )
  {
    return SelectedStreamMixer->GetVideoDataAvailable();
  }

  return false;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::DumpBuffersToDirectory( const char * aDirectory )
{
  LOG_TRACE("vtkDataCollector::DumpBuffersToDirectory(" << aDirectory << ")");

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = *it;

    std::string outputDeviceBufferSequenceFileName = "BufferDump_";
    outputDeviceBufferSequenceFileName.append(device->GetDeviceId());
    outputDeviceBufferSequenceFileName.append("_");
    outputDeviceBufferSequenceFileName.append(dateAndTime);

    LOG_INFO("Write device buffer to " << outputDeviceBufferSequenceFileName);
    device->GetBuffer()->WriteToMetafile( aDirectory, outputDeviceBufferSequenceFileName.c_str(), false); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
/*
void vtkDataCollector::SetTrackingOnly( bool aValue)
{
VideoEnabled = aValue == true ? false : true;
TrackingEnabled = aValue;
}

//----------------------------------------------------------------------------

void vtkDataCollector::SetVideoOnly( bool aValue )
{
TrackingEnabled = aValue == true ? false : true;
VideoEnabled = aValue;
}
*/

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime()");

  if( SelectedStreamMixer != NULL )
  {
    return SelectedStreamMixer->GetTrackedFrameByTime(time, trackedFrame);
  }

  LOG_ERROR("No selected stream mixer. Unable to get a tracked frame by time when no device available/selected.");
  return PLUS_FAIL;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollector::GetFrameSize");

  if( this->SelectedStreamMixer != NULL )
  {
    return this->SelectedStreamMixer->GetFrameSize(aDim);
  }

  LOG_ERROR("No selected stream mixer. Unable to GetFrameSize() when no device available/selected.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetBrightnessFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollector::GetFrameSize");

  if( this->SelectedStreamMixer != NULL )
  {
    return this->SelectedStreamMixer->GetBrightnessFrameSize(aDim);
  }

  LOG_ERROR("No selected stream mixer. Unable to GetBrightnessFrameSize() when no device available/selected.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
vtkImageData* vtkDataCollector::GetBrightnessOutput()
{
  LOG_TRACE("vtkDataCollector::GetBrightnessOutput");

  if( this->SelectedStreamMixer != NULL )
  {
    return this->SelectedStreamMixer->GetBrightnessOutput();
  }

  LOG_ERROR("No selected stream mixer. Unable to GetBrightnessOutput() when no device available/selected.");
  return NULL;
}

//----------------------------------------------------------------------------
DeviceCollectionConstIterator vtkDataCollector::GetDeviceConstIteratorBegin() const
{
  return this->Devices.begin();
}

//----------------------------------------------------------------------------
DeviceCollectionConstIterator vtkDataCollector::GetDeviceConstIteratorEnd() const
{
  return this->Devices.end();
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName)
{
  LOG_TRACE("vtkDataCollector::GetTrackerToolReferenceFrame");

  // If there is a physical tracker device then get the info from there
  if (this->SelectedStreamMixer != NULL)
  {
    aToolReferenceFrameName = std::string(this->SelectedStreamMixer->GetToolReferenceFrameName());
    return PLUS_SUCCESS;
  }

  // Try to find it out from the custom transforms that are stored in the tracked frame
  return GetTrackerToolReferenceFrameFromTrackedFrame(aToolReferenceFrameName);
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackerToolReferenceFrameFromTrackedFrame(std::string &aToolReferenceFrameName)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackerToolReferenceFrame");

  TrackedFrame trackedFrame;
  if (this->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get tracked frame!");
    return PLUS_FAIL;
  }

  std::vector<PlusTransformName> transformNames;
  trackedFrame.GetCustomFrameTransformNameList(transformNames);

  if (transformNames.size() == 0)
  {
    LOG_ERROR("No transforms found in tracked frame!");
    return PLUS_FAIL;
  }

  std::string frameName = "";
  for (std::vector<PlusTransformName>::iterator it = transformNames.begin(); it != transformNames.end(); ++it)
  {
    if (frameName == "")
    {
      frameName = it->To();
    }
    else if (frameName != it->To())
    {
      LOG_ERROR("Destination coordinate frame names are not the same!");
      return PLUS_FAIL;
    }
  }

  aToolReferenceFrameName = frameName;

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::SetLocalTimeOffsetSec( double trackerLagSec, double videoLagSec )
{
  if( SelectedStreamMixer == NULL )
  {
    LOG_ERROR("No selected stream mixer. Unable to set local time offset.");
    return PLUS_FAIL;
  }

  SelectedStreamMixer->SetLocalTimeOffsetSec(videoLagSec);
  SelectedStreamMixer->SetToolLocalTimeOffsetSec(trackerLagSec);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackingData(double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetTrackingData(" << aTimestampFrom); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( !this->GetTrackingEnabled() )
  {
    LOG_ERROR("Unable to get tracked frame list - Tracking is not enabled"); 
    return PLUS_FAIL; 
  }
  if ( this->SelectedStreamMixer == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - stream mixer is invalid"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  vtkPlusStreamTool* firstActiveTool = NULL; 
  vtkVirtualStreamMixer* aDevice = NULL;
  if ( this->GetSelectedStreamMixer(aDevice) != PLUS_SUCCESS )
  {
    LOG_ERROR("No selected stream mixer. Unable to search for tools.");
    return PLUS_FAIL;
  }
  if( aDevice->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get tracked frame list - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
  if ( trackerBuffer == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - Failed to get first active tool!"); 
    return PLUS_FAIL; 
  }

  if ( trackerBuffer->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetTrackingData: the tracking buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid=trackerBuffer->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid=trackerBuffer->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid=oldestItemUid; itemUid<=latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (trackerBuffer->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
    {
      // probably the buffer item is not available anymore
      continue;
    }
    if (itemTimestamp<=aTimestampFrom)
    {
      // this item has been acquired before the requested start time
      continue;
    }
    aTimestampFrom=itemTimestamp;
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 
    if ( aDevice->GetTrackedFrame(itemTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracking data by time: " << std::fixed << itemTimestamp ); 
      status=PLUS_FAIL;
    }
    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracking data to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetVideoData(double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetVideoData(" << aTimestampFrom); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( !this->GetVideoEnabled() )
  {
    LOG_ERROR("Unable to get tracked frame list - video is not enabled"); 
    return PLUS_FAIL; 
  }
  vtkVirtualStreamMixer* aDevice = NULL;
  if ( this->GetSelectedStreamMixer(aDevice) != PLUS_SUCCESS )
  {
    LOG_ERROR("Selected stream mixer is invalid. No video source present."); 
    return PLUS_FAIL; 
  }
  if ( aDevice->GetBuffer()->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetVideoData: the video buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid=aDevice->GetBuffer()->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid=aDevice->GetBuffer()->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid=oldestItemUid; itemUid<=latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (aDevice->GetBuffer()->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
    {
      // probably the buffer item is not available anymore
      continue;
    }
    if (itemTimestamp<=aTimestampFrom)
    {
      // this item has been acquired before the requested start time
      continue;
    }
    aTimestampFrom=itemTimestamp;
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 
    if ( aDevice->GetTrackedFrame(itemTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get video frame by time: " << std::fixed << itemTimestamp ); 
      status=PLUS_FAIL;
    }
    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add video data to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status; 
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameRate( double& frameRate ) const
{
  if( this->SelectedStreamMixer != NULL )
  {
    frameRate = this->SelectedStreamMixer->GetAcquisitionRate();
    return PLUS_SUCCESS;
  }

  LOG_ERROR("No selected mixer when requesting the framerate. Nothing to poll!");
  return PLUS_FAIL;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameListSampled( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec/*=-1*/ )
{
  if( this->SelectedStreamMixer != NULL )
  {
    return this->SelectedStreamMixer->GetTrackedFrameListSampled(aTimestamp, aTrackedFrameList, aSamplingRateSec, maxTimeLimitSec);
  }

  LOG_ERROR("No selected mixer when GetTrackedFrameListSampled(). Nothing to poll!");
  return PLUS_FAIL;
}

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
#include "vtkPlusStreamBuffer.h"
#include "vtkTrackedFrameList.h"
#include "vtkVirtualStreamMixer.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

// Temporary, to answer GetTrackingEnabled
#include "vtkPlusDevice.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 2.0$");
vtkStandardNewMacro(vtkDataCollector);

//----------------------------------------------------------------------------

vtkDataCollector::vtkDataCollector()
: vtkObject()
, SelectedStreamMixer(NULL)
, Connected(false)
, Started(false)
, TrackingEnabled(true)
, VideoEnabled(true)
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

  // TODO : connect these devices based on input/output streams

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

  for( vtkPlusDevice::DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
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

  for( vtkPlusDevice::DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
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
  
  for( vtkPlusDevice::DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
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

  for( vtkPlusDevice::DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->Disconnect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disconnect device: " << device->GetDeviceId() <<".");
      status = PLUS_FAIL;
    }
  }

  Connected = false;

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

  for( vtkPlusDevice::DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    os << indent << "Device: " << std::endl; 
    (*it)->PrintSelf(os, indent); 
  }
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetDevice( vtkPlusDevice* &aDevice, const std::string &aDeviceId ) const
{
  LOG_TRACE("vtkDataCollector::GetDevice( aDevice, " << aDeviceId << ")");

  for( vtkPlusDevice::DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
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

  // For now, scan the list of devices to see if we have a subtype of tracker
  // This will not work after vtkTracker is destroyed... perhaps each device can answer the question IsTracker... not sure what will be best
  for( vtkPlusDevice::DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    if( (*it)->IsTracker() )
      return true;
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

PlusStatus vtkDataCollector::SetSelectedTrackedFrameProducer( const std::string &aDeviceId )
{
  LOG_TRACE("vtkDataCollector::SetSelectedTrackedFrameProducer(" << aDeviceId << ")");

  for( vtkPlusDevice::DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
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

PlusStatus vtkDataCollector::GetTrackedFrameProducers( std::vector<vtkVirtualStreamMixer*> &OutVector ) const
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameProducers()");

  OutVector.clear();

  for( vtkPlusDevice::DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
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

PlusStatus vtkDataCollector::GetSelectedTrackedFrameProducer( vtkPlusDevice* &aDevice )
{
  LOG_TRACE("vtkDataCollector::GetSelectedTrackedFrameProducer()");

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

PlusStatus vtkDataCollector::Update()
{
  for( vtkPlusDevice::DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    (*it)->Update();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::DumpBuffersToDirectory( const char * aDirectory )
{
  LOG_TRACE("vtkDataCollector::DumpBuffersToDirectory(" << aDirectory << ")");

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");

  for( vtkPlusDevice::DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
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
/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusVirtualSwitcher.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusVirtualSwitcher);

const int FRAME_COUNT_BEFORE_INACTIVE = 25;

//----------------------------------------------------------------------------
vtkPlusVirtualSwitcher::vtkPlusVirtualSwitcher()
: vtkPlusDevice()
, CurrentActiveInputChannel(NULL)
, OutputChannel(NULL)
, FramesWhileInactive(0)
{
  // The data capture thread will be used to regularly check the input devices and generate and update the output
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
}

//----------------------------------------------------------------------------
vtkPlusVirtualSwitcher::~vtkPlusVirtualSwitcher()
{
}

//----------------------------------------------------------------------------
void vtkPlusVirtualSwitcher::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    os << indent << "Input channel: \n";
    (*it)->PrintSelf(os, indent);
  }

  os << indent << "Active input channel: \n";
  if( this->CurrentActiveInputChannel != NULL )
  {
    this->CurrentActiveInputChannel->PrintSelf(os, indent);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualSwitcher::GetChannel(vtkPlusChannel* &aChannel) const
{
  if( this->CurrentActiveInputChannel != NULL )
  {
    aChannel = this->CurrentActiveInputChannel;
    return PLUS_SUCCESS;
  }

  aChannel = NULL;
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualSwitcher::InternalUpdate()
{
  // if this device runs faster than input device, we might falsely detect that the device is not active because the latest timestamp hasn't changed
  //    correctly detect this situation and wait a few frames before switching
      // if timestamp not changed within 'FRAME_COUNT_BEFORE_INACTIVE' frames, then do new stream check

  double latestCurrentTimestamp(0);
  if( this->CurrentActiveInputChannel != NULL )
  {
    if( this->CurrentActiveInputChannel->GetLatestTimestamp(latestCurrentTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve timestamp from active stream.");
      return PLUS_FAIL;
    }
    if( this->LastRecordedTimestampMap[this->CurrentActiveInputChannel] == 0 )
    {
      this->LastRecordedTimestampMap[this->CurrentActiveInputChannel] = latestCurrentTimestamp;
      return PLUS_SUCCESS;
    }

    if( latestCurrentTimestamp > this->LastRecordedTimestampMap[this->CurrentActiveInputChannel] )
    {
      // Device is still active
      return PLUS_SUCCESS;
    }
    else
    {
      if( FramesWhileInactive >= FRAME_COUNT_BEFORE_INACTIVE )
      {
        this->FramesWhileInactive = 0;
        // Device is no longer active
        if( this->SelectActiveChannel() == PLUS_FAIL )
        {
          // No active devices, don't copy anything!
          return PLUS_SUCCESS;
        }
      }
      else
      {
        FramesWhileInactive++;
      }
    }

    this->CopyInputChannelToOutputChannel();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualSwitcher::SelectActiveChannel()
{
  std::vector<vtkPlusChannel*> ActiveChannels;

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aChannel = (*it);
    double latestTimestamp(0);
    if( aChannel->GetLatestTimestamp(latestTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve latest timestamp from stream.");
      continue;
    }
    if( latestTimestamp > this->LastRecordedTimestampMap[aChannel] )
    {
      // It's got new data... it's active
      this->LastRecordedTimestampMap[aChannel] = latestTimestamp;
      ActiveChannels.push_back(aChannel);
    }
  }

  if( ActiveChannels.size() > 0 )
  {
    // For now, just choose the first... maybe in the future make it more elegant
    this->SetCurrentActiveInputChannel(ActiveChannels[0]);
    this->CopyInputChannelToOutputChannel();

    // We will also now need to output the correct transform associated with the new stream
    // Is there any way to make this generic?
    // In config file, associate transform/image names to special prefix/postfixes?
    // scan stream name, if postfix matches, output transform(s) with that postfix? eg stream id -- Output_depth:5cm, transform -- ImageToProbeTransform_5cm, etc...
    //                                        have base transform name(s) in the config eg: ImageToProbeTransform

    return PLUS_SUCCESS;
  }
 
  this->SetCurrentActiveInputChannel(NULL);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
double vtkPlusVirtualSwitcher::GetAcquisitionRate() const
{
  vtkPlusChannel* aChannel = NULL;
  if( this->GetChannel(aChannel) == PLUS_SUCCESS )
  {
    return aChannel->GetOwnerDevice()->GetAcquisitionRate();
  }

  return this->AcquisitionRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualSwitcher::ReadConfiguration( vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];

  SetOutputChannel(outputChannel);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualSwitcher::NotifyConfigured()
{
  this->LastRecordedTimestampMap.clear();

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aChannel = (*it);
    this->LastRecordedTimestampMap[aChannel] = 0;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualSwitcher::CopyInputChannelToOutputChannel()
{
  // Only destroy if things have to change
  if( this->CurrentActiveInputChannel != NULL )
  {
    // no need to do a deep copy, iterators are used to access data anyways
    this->OutputChannel->ShallowCopy(*this->CurrentActiveInputChannel);
  }

  return PLUS_SUCCESS;
}
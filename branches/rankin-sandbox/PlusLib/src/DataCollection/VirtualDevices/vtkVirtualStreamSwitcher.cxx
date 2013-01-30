/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkVirtualStreamSwitcher.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamSwitcher, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamSwitcher);

const int FRAME_COUNT_BEFORE_INACTIVE = 25;

//----------------------------------------------------------------------------
vtkVirtualStreamSwitcher::vtkVirtualStreamSwitcher()
: vtkPlusDevice()
, CurrentActiveInputStream(NULL)
, OutputStream(NULL)
, FramesWhileInactive(0)
{
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
}

//----------------------------------------------------------------------------
vtkVirtualStreamSwitcher::~vtkVirtualStreamSwitcher()
{
}

//----------------------------------------------------------------------------
void vtkVirtualStreamSwitcher::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    os << indent << "Input stream: \n";
    (*it)->PrintSelf(os, indent);
  }

  os << indent << "Active input stream: \n";
  if( this->CurrentActiveInputStream != NULL )
  {
    this->CurrentActiveInputStream->PrintSelf(os, indent);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamSwitcher::GetStream(vtkPlusChannel* &aStream) const
{
  if( this->CurrentActiveInputStream != NULL )
  {
    aStream = this->CurrentActiveInputStream;
    return PLUS_SUCCESS;
  }

  aStream = NULL;
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamSwitcher::InternalUpdate()
{
  // if this device runs faster than input device, we might falsely detect that the device is not active because the latest timestamp hasn't changed
  //    correctly detect this situation and wait a few frames before switching
      // if timestamp not changed within 'FRAME_COUNT_BEFORE_INACTIVE' frames, then do new stream check

  double latestCurrentTimestamp(0);
  if( this->CurrentActiveInputStream != NULL )
  {
    if( this->CurrentActiveInputStream->GetLatestTimestamp(latestCurrentTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve timestamp from active stream.");
      return PLUS_FAIL;
    }
    if( this->LastRecordedTimestampMap[this->CurrentActiveInputStream] == 0 )
    {
      this->LastRecordedTimestampMap[this->CurrentActiveInputStream] = latestCurrentTimestamp;
      return PLUS_SUCCESS;
    }

    if( latestCurrentTimestamp > this->LastRecordedTimestampMap[this->CurrentActiveInputStream] )
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
        if( this->SelectActiveStream() == PLUS_FAIL )
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

    this->CopyInputStreamToOutputStream();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamSwitcher::SelectActiveStream()
{
  std::vector<vtkPlusChannel*> ActiveStreams;

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aStream = (*it);
    double latestTimestamp(0);
    if( aStream->GetLatestTimestamp(latestTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve latest timestamp from stream.");
      continue;
    }
    if( latestTimestamp > this->LastRecordedTimestampMap[aStream] )
    {
      // It's got new data... it's active
      this->LastRecordedTimestampMap[aStream] = latestTimestamp;
      ActiveStreams.push_back(aStream);
    }
  }

  if( ActiveStreams.size() > 0 )
  {
    // For now, just choose the first... maybe in the future make it more elegant
    this->SetCurrentActiveInputStream(ActiveStreams[0]);
    this->CopyInputStreamToOutputStream();

    // We will also now need to output the correct transform associated with the new stream
    // Is there any way to make this generic?
    // In config file, associate transform/image names to special prefix/postfixes?
    // scan stream name, if postfix matches, output transform(s) with that postfix? eg stream id -- Output_depth:5cm, transform -- ImageToProbeTransform_5cm, etc...
    //                                        have base transform name(s) in the config eg: ImageToProbeTransform

    return PLUS_SUCCESS;
  }
 
  this->SetCurrentActiveInputStream(NULL);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
double vtkVirtualStreamSwitcher::GetAcquisitionRate() const
{
  vtkPlusChannel* aStream = NULL;
  if( this->GetStream(aStream) == PLUS_SUCCESS )
  {
    return aStream->GetOwnerDevice()->GetAcquisitionRate();
  }

  return this->AcquisitionRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamSwitcher::ReadConfiguration( vtkXMLDataElement* element)
{
  if( Superclass::ReadConfiguration(element) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  SetOutputStream(this->OutputChannels[0]);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamSwitcher::NotifyConfigured()
{
  this->LastRecordedTimestampMap.clear();

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aStream = (*it);
    this->LastRecordedTimestampMap[aStream] = 0;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamSwitcher::CopyInputStreamToOutputStream()
{
  // Only destroy if things have to change
  if( this->CurrentActiveInputStream != NULL )
  {
    // no need to do a deep copy, iterators are used to access data anyways
    this->OutputStream->ShallowCopy(*this->CurrentActiveInputStream);
  }

  return PLUS_SUCCESS;
}
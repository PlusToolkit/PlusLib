/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusVirtualMixer.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusVirtualMixer);

//----------------------------------------------------------------------------
vtkPlusVirtualMixer::vtkPlusVirtualMixer()
  : vtkPlusDevice()
{
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  // No need for StartThreadForInternalUpdates, as capturing is performed in other devices, here we just collect references to buffers
}

//----------------------------------------------------------------------------
vtkPlusVirtualMixer::~vtkPlusVirtualMixer()
{
  // Clear reference to rf processor
  if (!this->OutputChannels.empty())
  {
    vtkPlusChannel* outputChannel = this->OutputChannels[0];
    if (outputChannel != NULL && outputChannel->GetRfProcessor() != NULL)
    {
      outputChannel->SetRfProcessor(NULL);
    }
  }

  // Mixer fakes an output channel to enable "GetTrackedFrame" functionality
  // We don't want the plus device destructor destroying sources in output channels it doesn't own
  // So clear it before it runs (see #756)
  this->OutputChannels.clear();
}

//----------------------------------------------------------------------------
void vtkPlusVirtualMixer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualMixer::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  if (this->OutputChannels.empty())
  {
    LOG_WARNING("vtkPlusVirtualMixer device " << this->GetDeviceId() << " does not have any output channels");
    vtkSmartPointer<vtkPlusChannel> aChannel = vtkSmartPointer<vtkPlusChannel>::New();
    std::ostringstream ss;
    ss << "aMixerId_" << vtkPlusAccurateTimer::GetSystemTime();
    aChannel->SetChannelId(ss.str().c_str());
    this->AddOutputChannel(aChannel);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkPlusVirtualMixer::GetAcquisitionRate() const
{
  // Determine frame rate from the video input device with the lowest frame rate
  bool lowestRateKnown = false;
  double lowestRate = 30; // just a usual value (FPS)

  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* anInputChannel = (*it);

    // Get the lowest rate from all image streams
    if (anInputChannel->HasVideoSource())
    {
      lowestRate = anInputChannel->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown = true;
    }

    if (anInputChannel->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
    {
      lowestRate = anInputChannel->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown = true;
    }
  }

  if (!lowestRateKnown)
  {
    // Couldn't determine the lowest acquisition rate, so just use the one that was set by default
    lowestRate = this->AcquisitionRate;
  }
  return lowestRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualMixer::NotifyConfigured()
{
  // First, empty whatever is there, because this can be called at any point after a configuration
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel = this->OutputChannels[0];

  outputChannel->RemoveTools();
  outputChannel->RemoveFieldDataSources();
  outputChannel->Clear();

  for (ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* anInputChannel = (*it);
    vtkPlusDataSource* aSource = NULL;

    if (anInputChannel->HasVideoSource() && anInputChannel->GetVideoSource(aSource) == PLUS_SUCCESS)
    {
      outputChannel->SetVideoSource(aSource);
      this->AddVideoSource(aSource);
    }

    for (DataSourceContainerConstIterator fieldSourceIter = anInputChannel->GetToolsStartConstIterator(); fieldSourceIter != anInputChannel->GetToolsEndConstIterator(); ++fieldSourceIter)
    {
      vtkPlusDataSource* anInputTool = fieldSourceIter->second;

      bool found = false;
      for (DataSourceContainerConstIterator outputToolIt = outputChannel->GetToolsStartConstIterator(); outputToolIt != outputChannel->GetToolsEndConstIterator(); ++outputToolIt)
      {
        vtkPlusDataSource* anOutputTool = outputToolIt->second;
        // Check for double adds or name conflicts
        if (anInputTool == anOutputTool)
        {
          found = true;
          LOG_ERROR("Tool already exists in the output stream. Somehow the same tool is part of two input streams. Consider using a virtual device to resolve them first.");
          break;
        }
        else if (anInputTool->GetId() == anOutputTool->GetId())
        {
          found = true;
          LOG_ERROR("Name collision! Two tools are outputting the same transform. Consider using a virtual device to resolve them first.");
          break;
        }
      }

      if (!found)
      {
        outputChannel->AddTool(anInputTool);
        if (this->AddTool(anInputTool, false) != PLUS_SUCCESS)
        {
          LOG_ERROR("Unable to add tool " << anInputTool->GetId() << " to device " << this->GetDeviceId());
        }
      }
    }

    for (DataSourceContainerConstIterator fieldSourceIter = anInputChannel->GetFieldDataSourcesStartConstIterator(); fieldSourceIter != anInputChannel->GetFieldDataSourcesEndConstIterator(); ++fieldSourceIter)
    {
      vtkPlusDataSource* inputFieldSource = fieldSourceIter->second;

      bool found = false;
      for (DataSourceContainerConstIterator outputFieldSourceIter = outputChannel->GetFieldDataSourcesStartConstIterator(); outputFieldSourceIter != outputChannel->GetFieldDataSourcesEndConstIterator(); ++outputFieldSourceIter)
      {
        vtkPlusDataSource* outputFieldSource = outputFieldSourceIter->second;
        // Check for double adds or name conflicts
        if (inputFieldSource == outputFieldSource)
        {
          found = true;
          LOG_ERROR("Field data source already exists in the output stream. Somehow the same data field is part of two input streams. Consider using a virtual device to resolve them first.");
          break;
        }
        else if (inputFieldSource->GetId() == outputFieldSource->GetId())
        {
          found = true;
          LOG_ERROR("Name collision! Two field sources are outputting the same data. Consider using a virtual device to resolve them first.");
          break;
        }
      }

      if (!found)
      {
        outputChannel->AddFieldDataSource(inputFieldSource);
        if (this->AddFieldDataSource(inputFieldSource) != PLUS_SUCCESS)
        {
          LOG_ERROR("Unable to add field data source " << inputFieldSource->GetId() << " to device " << this->GetDeviceId());
        }
      }
    }

    if (anInputChannel->GetRfProcessor() != NULL && outputChannel->GetRfProcessor() == NULL)
    {
      outputChannel->SetRfProcessor(anInputChannel->GetRfProcessor());
    }
    else if (anInputChannel->GetRfProcessor() != NULL && outputChannel->GetRfProcessor() != NULL)
    {
      LOG_WARNING("Multiple RfProcessors defined in InputChannels to mixer: " << this->GetDeviceId() << ". Check input configuration.");
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusVirtualMixer::IsTracker() const
{
  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aChannel = *it;
    if (aChannel->GetOwnerDevice()->IsTracker())
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualMixer::Reset()
{
  int numErrors(0);
  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aChannel = *it;
    if (aChannel->GetOwnerDevice()->Reset() != PLUS_SUCCESS)
    {
      numErrors++;
    }
  }

  return numErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusVirtualMixer::IsResettable()
{
  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aChannel = *it;
    if (aChannel->GetOwnerDevice()->IsResettable())
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
vtkPlusChannel* vtkPlusVirtualMixer::GetChannel() const
{
  return this->OutputChannels[0];
}

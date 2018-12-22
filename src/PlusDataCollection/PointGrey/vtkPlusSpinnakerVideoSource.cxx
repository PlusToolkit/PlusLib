/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusSpinnakerVideoSource.h"

// Local includes
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// Spinnaker API includes
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusSpinnakerVideoSource);

//----------------------------------------------------------------------------
class vtkPlusSpinnakerVideoSource::vtkInternal
{
public:
  vtkPlusSpinnakerVideoSource * External;

  vtkInternal(vtkPlusSpinnakerVideoSource* external)
    : External(external)
    , Align(nullptr)
  {
  }

  virtual ~vtkInternal()
  {
  }

};

//----------------------------------------------------------------------------
vtkPlusSpinnakerVideoSource::vtkPlusSpinnakerVideoSource()
  : Internal(new vtkInternal(this))
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::vtkPlusSpinnakerVideoSource()");
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = 30;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkPlusSpinnakerVideoSource::~vtkPlusSpinnakerVideoSource()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::~vtkPlusSpinnakerVideoSource()");

  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusSpinnakerVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::PrintSelf(ostream& os, vtkIndent indent)");

  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* dataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(dataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }

    if (dataElement->GetAttribute("Type") != NULL && STRCASECMP(dataElement->GetAttribute("Type"), "Video") == 0)
    {
      // this is a video element
      // get tool ID
      const char* toolId = dataElement->GetAttribute("Id");
      if (toolId == NULL)
      {
        // tool doesn't have ID needed to generate transform
        LOG_ERROR("Failed to initialize Spinnaker API VideoSource: Id is missing");
        continue;
      }

      // Get camera configuration specifics
  }
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalConnect()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalDisconnect()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalStartRecording()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalStartRecording()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalStopRecording()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalStopRecording()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::NotifyConfigured()
{
  // TODO: Implement some configuration checks here
  LOG_TRACE("vtkPlusSpinnakerVideoSource::NotifyConfigured()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalUpdate()");

  this->FrameNumber++; 
  return PLUS_FAIL;
}
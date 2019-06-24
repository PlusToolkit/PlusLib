/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

// Local includes
#include "vtkPlusDeckLinkVideoSource.h"
#include "vtkIGSIOAccurateTimer.h"

// VTK includes

// System includes
#include <string>

//----------------------------------------------------------------------------
// vtkPlusDeckLinkVideoSource::vtkInternal
//----------------------------------------------------------------------------

class vtkPlusDeckLinkVideoSource::vtkInternal
{
public:
  vtkPlusDeckLinkVideoSource* External;

  vtkInternal(vtkPlusDeckLinkVideoSource* external)
    : External(external)
  {
  }

  virtual ~vtkInternal()
  {
  }

};

//----------------------------------------------------------------------------
// vtkPlusDeckLinkVideoSource
//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDeckLinkVideoSource);

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource()");

  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = 30;
}

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::~vtkPlusDeckLinkVideoSource()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::~vtkPlusDeckLinkVideoSource()");

  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusDeckLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)");
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(toolDataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }
    if (toolDataElement->GetAttribute("Type") != NULL && STRCASECMP(toolDataElement->GetAttribute("Type"), "Video") != 0)
    {
      // if this is not a Tool element, skip it
      continue;
    }
    
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalConnect");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalDisconnect");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalStartRecording()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalStartRecording");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalStopRecording()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalStopRecording");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::Probe()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::Probe");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalUpdate");

  return PLUS_FAIL;
}
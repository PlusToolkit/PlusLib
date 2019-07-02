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
#include <vtkObject.h>

// System includes
#include <string>

//----------------------------------------------------------------------------
// vtkPlusDeckLinkVideoSource::vtkInternal
//----------------------------------------------------------------------------

class vtkPlusDeckLinkVideoSource::vtkInternal : public vtkObject
{
public:
  static vtkPlusDeckLinkVideoSource::vtkInternal* New(vtkPlusDeckLinkVideoSource*);
  vtkTypeMacro(vtkInternal, vtkObject);

public:
  vtkPlusDeckLinkVideoSource* External;

  vtkInternal(vtkPlusDeckLinkVideoSource* external)
    : External(external)
  {}

  virtual ~vtkInternal()
  {}
private:
  static vtkPlusDeckLinkVideoSource::vtkInternal* New();
  vtkInternal()
    : External(nullptr)
  {}
};

//----------------------------------------------------------------------------
// vtkPlusDeckLinkVideoSource
//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDeckLinkVideoSource);
vtkStandardNewMacro(vtkPlusDeckLinkVideoSource::vtkInternal);

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource::vtkInternal* vtkPlusDeckLinkVideoSource::vtkInternal::New(vtkPlusDeckLinkVideoSource* _arg)
{
  vtkPlusDeckLinkVideoSource::vtkInternal* result = new vtkPlusDeckLinkVideoSource::vtkInternal();
  result->InitializeObjectBase();
  result->External = _arg;
  return result;
}

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource()
  : vtkPlusDevice()
  , Internal(vtkInternal::New(this))
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

  this->Internal->Delete();
  this->Internal = nullptr;
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
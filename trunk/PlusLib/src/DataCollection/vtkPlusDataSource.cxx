/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkPlusDataSource);

//----------------------------------------------------------------------------
vtkPlusDataSource::vtkPlusDataSource()
: Device(NULL)
, PortName(NULL)
, Type(DATA_SOURCE_TYPE_NONE)
, FrameNumber(0)
, LED1(0)
, LED2(0)
, LED3(0)
, ToolRevision(NULL)
, ToolSerialNumber(NULL)
, ToolPartNumber(NULL)
, ToolManufacturer(NULL)
, SourceId(NULL)
, ReferenceCoordinateFrameName(NULL)
, Buffer(vtkPlusBuffer::New())
{
}

//----------------------------------------------------------------------------
vtkPlusDataSource::~vtkPlusDataSource()
{
  if ( this->SourceId != NULL )
  {
    delete [] this->SourceId; 
    this->SourceId = NULL; 
  }

  if ( this->ReferenceCoordinateFrameName != NULL )
  {
    delete [] this->ReferenceCoordinateFrameName; 
    this->ReferenceCoordinateFrameName = NULL; 
  }

  this->SetPortName(NULL); 
  this->SetToolRevision(NULL); 
  this->SetToolSerialNumber(NULL); 
  this->SetToolManufacturer(NULL); 

  if ( this->Buffer != NULL )
  {
    this->Buffer->Delete(); 
    this->Buffer = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  if ( this->Device )
  {
    os << indent << "Tracker: " << this->Device << "\n";
  }
  if ( this->SourceId )
  {
    os << indent << "SourceId: " << this->GetSourceId() << "\n";
  }
  if ( this->Type != DATA_SOURCE_TYPE_NONE )
  {
    os << indent << "Type: " << ((this->Type == DATA_SOURCE_TYPE_VIDEO) ? "Video" : "Tool") << "\n";
  }
  if ( this->ReferenceCoordinateFrameName )
  {
    os << indent << "ReferenceCoordinateFrameName: " << this->GetReferenceCoordinateFrameName() << "\n";
  }
  if ( this->PortName )
  {
    os << indent << "PortName: " << this->GetPortName() << "\n";
  }
  os << indent << "LED1: " << this->GetLED1() << "\n"; 
  os << indent << "LED2: " << this->GetLED2() << "\n"; 
  os << indent << "LED3: " << this->GetLED3() << "\n";

  if ( this->ToolRevision )
  {
    os << indent << "ToolRevision: " << this->GetToolRevision() << "\n";
  }
  if ( this->ToolManufacturer )
  {
    os << indent << "ToolManufacturer: " << this->GetToolManufacturer() << "\n";
  }
  if ( this->ToolPartNumber )
  {
    os << indent << "ToolPartNumber: " << this->GetToolPartNumber() << "\n";
  }
  if ( this->ToolSerialNumber )
  {
    os << indent << "ToolSerialNumber: " << this->GetToolSerialNumber() << "\n";
  }
  if ( this->Buffer )
  {
    os << indent << "Buffer: " << this->Buffer << "\n";
    this->Buffer->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetSourceId(const char* aSourceId)
{
  if ( this->SourceId == NULL && aSourceId == NULL) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->SourceId && aSourceId && ( STRCASECMP(this->SourceId, aSourceId) == 0 ) ) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->SourceId != NULL )
  {
    LOG_ERROR("SourceId change is not allowed for source '" << this->SourceId << "'" ); 
    return PLUS_FAIL; 
  }

  // Copy string 
  size_t n = strlen(aSourceId) + 1; 
  char *cp1 =  new char[n]; 
  const char *cp2 = (aSourceId); 
  this->SourceId = cp1;
  do { *cp1++ = *cp2++; } while ( --n ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetReferenceName(const char* referenceName)
{
  if ( this->ReferenceCoordinateFrameName == NULL && referenceName == NULL) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ReferenceCoordinateFrameName && referenceName && ( STRCASECMP(this->ReferenceCoordinateFrameName, referenceName) == 0 ) ) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ReferenceCoordinateFrameName != NULL )
  {
    LOG_ERROR("Reference frame name change is not allowed for tool '" << this->ReferenceCoordinateFrameName << "'" ); 
    return PLUS_FAIL; 
  }

  // Copy string 
  size_t n = strlen(referenceName) + 1; 
  char *cp1 =  new char[n]; 
  const char *cp2 = (referenceName); 
  this->ReferenceCoordinateFrameName = cp1;
  do { *cp1++ = *cp2++; } while ( --n ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetPortName(const char* portName)
{
  if ( this->PortName == NULL && portName == NULL) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->PortName && portName && ( STRCASECMP(this->PortName, portName) == 0 ) ) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->PortName != NULL )
  {
    LOG_ERROR("Port name change is not allowed on source port'" << this->PortName << "'" ); 
    return PLUS_FAIL; 
  }

  // Copy string 
  size_t n = strlen(portName) + 1; 
  char *cp1 =  new char[n]; 
  const char *cp2 = (portName); 
  this->PortName = cp1;
  do { *cp1++ = *cp2++; } while ( --n ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::SetLED1(int state)
{
  this->Device->SetToolLED(this->PortName,1,state);
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::SetLED2(int state)
{
  this->Device->SetToolLED(this->PortName,2,state);
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::SetLED3(int state)
{
  this->Device->SetToolLED(this->PortName,3,state);
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::DeepCopy(vtkPlusDataSource *aSource)
{
  LOG_TRACE("vtkPlusDataSource::DeepCopy"); 

  this->SetLED1( aSource->GetLED1() );
  this->SetLED2( aSource->GetLED2() );
  this->SetLED3( aSource->GetLED3() );

  this->SetToolRevision( aSource->GetToolRevision() );
  this->SetToolSerialNumber( aSource->GetToolSerialNumber() );
  this->SetToolPartNumber( aSource->GetToolPartNumber() );
  this->SetToolManufacturer( aSource->GetToolManufacturer() );
  this->SetSourceId( aSource->GetSourceId() ); 
  this->SetType( aSource->GetType() );
  this->SetReferenceName( aSource->GetReferenceCoordinateFrameName() );

  this->Buffer->DeepCopy( aSource->GetBuffer() );

  this->SetFrameNumber( aSource->GetFrameNumber() );
}


//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::ReadConfiguration(vtkXMLDataElement* sourceElement, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration, const char* aDescriptiveNameForBuffer)
{
  LOG_TRACE("vtkPlusDataSource::ReadConfiguration"); 

  if ( sourceElement == NULL )
  {
    LOG_ERROR("Unable to configure data source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  const char* sourceId = sourceElement->GetAttribute("Id"); 
  if ( sourceId != NULL ) 
  {
    this->SetSourceId(sourceId); 
  }
  else
  {
    LOG_ERROR("Unable to find attribute Id! Id attribute is mandatory in source definition."); 
    return PLUS_FAIL; 
  }

  const char* portName = sourceElement->GetAttribute("PortName"); 
  if ( portName != NULL ) 
  {
    this->SetPortName(portName); 
  }

  const char* type = sourceElement->GetAttribute("Type"); 
  if ( type != NULL && STRCASECMP(type, "Tool") == 0 ) 
  {
    this->SetType(DATA_SOURCE_TYPE_TOOL);
    
    if( portName == NULL )
    {
      LOG_ERROR("Unable to find PortName! This attribute is mandatory in tool definition."); 
      return PLUS_FAIL; 
    }
  }
  else if ( type != NULL && STRCASECMP(type, "Video") == 0 ) 
  {
    this->SetType(DATA_SOURCE_TYPE_VIDEO);
  }
  else
  {
    LOG_ERROR("Missing type element. It is required to define the source type.");
    return PLUS_FAIL;
  }

  int bufferSize = 0; 
  if ( sourceElement->GetScalarAttribute("BufferSize", bufferSize) ) 
  {
    this->GetBuffer()->SetBufferSize(bufferSize);
  }
  else
  {
    LOG_ERROR("Unable to find source \"" << this->GetSourceId() << "\" buffer size in source element when it is required.");
    return PLUS_FAIL;
  }

  int averagedItemsForFiltering = 0;
  if ( sourceElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
  }
  else if ( RequireAveragedItemsForFilteringInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find averaged items for filtering in source configuration when it is required.");
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG("Unable to find AveragedItemsForFiltering attribute in source element. Using default value.");
  }

  std::string descName;
  if( aDescriptiveNameForBuffer != NULL )
  {
    descName += aDescriptiveNameForBuffer;
    descName += "-";
    descName += this->GetSourceId();
  }
  else
  {
    descName += this->GetSourceId();
  }
  this->GetBuffer()->SetDescriptiveName(descName.c_str());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::WriteConfiguration( vtkXMLDataElement* aSourceElement )
{
  LOG_TRACE("vtkPlusDataSource::WriteConfiguration"); 

  if ( aSourceElement == NULL )
  {
    LOG_ERROR("Unable to configure data source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  aSourceElement->SetAttribute("Id", this->GetSourceId());
  aSourceElement->SetAttribute("PortName", this->GetPortName());
  aSourceElement->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize());

  if( aSourceElement->GetAttribute("AveragedItemsForFiltering") != NULL )
  {
    aSourceElement->SetIntAttribute("AveragedItemsForFiltering", this->GetBuffer()->GetAveragedItemsForFiltering());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::WriteCompactConfiguration( vtkXMLDataElement* aSourceElement )
{
  LOG_TRACE("vtkPlusDataSource::WriteConfiguration"); 

  if ( aSourceElement == NULL )
  {
    LOG_ERROR("Unable to configure source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  aSourceElement->SetAttribute("Id", this->GetSourceId());
  aSourceElement->SetAttribute("PortName", this->GetPortName());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

DataSourceType vtkPlusDataSource::GetType() const
{
  return this->Type;
}

//-----------------------------------------------------------------------------

void vtkPlusDataSource::SetType( DataSourceType aType )
{
  this->Type = aType;
}

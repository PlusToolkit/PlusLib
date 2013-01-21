/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkTransform.h"
//#include "vtkObjectFactory.h"
//#include "vtkXMLUtilities.h"

vtkStandardNewMacro(vtkPlusStreamTool);

//----------------------------------------------------------------------------
vtkPlusStreamTool::vtkPlusStreamTool()
: Device(NULL)
, PortName(NULL)
, FrameNumber(0)
, LED1(0)
, LED2(0)
, LED3(0)

, ToolRevision(NULL)
, ToolSerialNumber(NULL)
, ToolPartNumber(NULL)
, ToolManufacturer(NULL)
, ToolName(NULL)
, ReferenceCoordinateFrameName(NULL)
, Buffer(vtkPlusStreamBuffer::New())
{
}

//----------------------------------------------------------------------------
vtkPlusStreamTool::~vtkPlusStreamTool()
{
  if ( this->ToolName != NULL )
  {
    delete [] this->ToolName; 
    this->ToolName = NULL; 
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

  if ( this->Buffer )
  {
    this->Buffer->Delete(); 
    this->Buffer = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPlusStreamTool::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  if ( this->Device )
  {
    os << indent << "Tracker: " << this->Device << "\n";
  }
  if ( this->ToolName )
  {
    os << indent << "ToolName: " << this->GetToolName() << "\n";
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
PlusStatus vtkPlusStreamTool::SetToolName(const char* toolName)
{
  if ( this->ToolName == NULL && toolName == NULL) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ToolName && toolName && ( STRCASECMP(this->ToolName, toolName) == 0 ) ) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ToolName != NULL )
  {
    LOG_ERROR("Tool name change is not allowed for tool '" << this->ToolName << "'" ); 
    return PLUS_FAIL; 
  }

  // Copy string 
  size_t n = strlen(toolName) + 1; 
  char *cp1 =  new char[n]; 
  const char *cp2 = (toolName); 
  this->ToolName = cp1;
  do { *cp1++ = *cp2++; } while ( --n ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStreamTool::SetReferenceName(const char* referenceName)
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
    LOG_ERROR("Tool name change is not allowed for tool '" << this->ReferenceCoordinateFrameName << "'" ); 
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
PlusStatus vtkPlusStreamTool::SetPortName(const char* portName)
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
    LOG_ERROR("Port name change is not allowed on tool port'" << this->PortName << "'" ); 
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
void vtkPlusStreamTool::SetLED1(int state)
{
  this->Device->SetToolLED(this->PortName,1,state);
}

//----------------------------------------------------------------------------
void vtkPlusStreamTool::SetLED2(int state)
{
  this->Device->SetToolLED(this->PortName,2,state);
}

//----------------------------------------------------------------------------
void vtkPlusStreamTool::SetLED3(int state)
{
  this->Device->SetToolLED(this->PortName,3,state);
}

//----------------------------------------------------------------------------
void vtkPlusStreamTool::SetDevice(vtkPlusDevice *device)
{
  // The Tracker is not reference counted, since that would cause a reference loop
  if (device == this->Device)
  {
    return;
  }

  if (this->Device)
  {
    this->Device = NULL;
  }

  if (device)
  {
    this->Device = device;
  }
  else
  {
    this->Device = NULL;
  }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkPlusStreamTool::DeepCopy(vtkPlusStreamTool *tool)
{
  LOG_TRACE("vtkPlusStreamTool::DeepCopy"); 

  this->SetLED1( tool->GetLED1() );
  this->SetLED2( tool->GetLED2() );
  this->SetLED3( tool->GetLED3() );

  this->SetToolRevision( tool->GetToolRevision() );
  this->SetToolSerialNumber( tool->GetToolSerialNumber() );
  this->SetToolPartNumber( tool->GetToolPartNumber() );
  this->SetToolManufacturer( tool->GetToolManufacturer() );
  this->SetToolName( tool->GetToolName() ); 
  this->SetReferenceName( tool->GetReferenceCoordinateFrameName() );

  this->Buffer->DeepCopy( tool->GetBuffer() );

  this->SetFrameNumber( tool->GetFrameNumber() );
}


//-----------------------------------------------------------------------------
PlusStatus vtkPlusStreamTool::ReadConfiguration(vtkXMLDataElement* toolElement, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration)
{
  LOG_TRACE("vtkPlusStreamTool::ReadConfiguration"); 

  if ( toolElement == NULL )
  {
    LOG_ERROR("Unable to configure stream tool! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  const char* toolName = toolElement->GetAttribute("Name"); 
  if ( toolName != NULL ) 
  {
    this->SetToolName(toolName); 
  }
  else
  {
    LOG_ERROR("Unable to find tool name! Name attribute is mandatory in tool definition."); 
    return PLUS_FAIL; 
  }

  const char* portName = toolElement->GetAttribute("PortName"); 
  if ( portName != NULL ) 
  {
    this->SetPortName(portName); 
  }
  else
  {
    LOG_ERROR("Unable to find PortName! This attribute is mandatory in tool definition."); 
    return PLUS_FAIL; 
  }

  int bufferSize = 0; 
  if ( toolElement->GetScalarAttribute("BufferSize", bufferSize) ) 
  {
    this->GetBuffer()->SetBufferSize(bufferSize);
  }
  else
  {
    LOG_ERROR("Unable to find tool \"" << this->GetToolName() << "\" buffer size in device element when it is required.");
    return PLUS_FAIL;
  }

  int averagedItemsForFiltering = 0;
  if ( toolElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
  }
  else if ( RequireAveragedItemsForFilteringInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find averaged items for filtering in device configuration when it is required.");
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG("Unable to find AveragedItemsForFiltering attribute in device element. Using default value.");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusStreamTool::WriteConfiguration( vtkXMLDataElement* toolElement )
{
  LOG_TRACE("vtkPlusStreamTool::WriteConfiguration"); 

  if ( toolElement == NULL )
  {
    LOG_ERROR("Unable to configure stream tool! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  toolElement->SetAttribute("Name", this->GetToolName());
  toolElement->SetAttribute("PortName", this->GetPortName());
  toolElement->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize());

  if( toolElement->GetAttribute("AveragedItemsForFiltering") != NULL )
  {
    toolElement->SetIntAttribute("AveragedItemsForFiltering", this->GetBuffer()->GetAveragedItemsForFiltering());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusStreamTool::WriteCompactConfiguration( vtkXMLDataElement* toolElement )
{
  LOG_TRACE("vtkPlusStreamTool::WriteConfiguration"); 

  if ( toolElement == NULL )
  {
    LOG_ERROR("Unable to configure stream tool! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  toolElement->SetAttribute("Name", this->GetToolName());
  toolElement->SetAttribute("PortName", this->GetPortName());

  return PLUS_SUCCESS;
}
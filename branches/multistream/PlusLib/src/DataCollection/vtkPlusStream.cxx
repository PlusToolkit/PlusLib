/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStream.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusStream, "$Revision: 1.0$");
vtkStandardNewMacro(vtkPlusStream);

//----------------------------------------------------------------------------

vtkPlusStream::vtkPlusStream(void)
: OwnerDevice(NULL)
, StreamId(NULL)
{
  // Always have a default stream buffer
  vtkPlusStreamBuffer* aBuff = vtkPlusStreamBuffer::New();
  StreamBuffers.push_back(aBuff);
}

//----------------------------------------------------------------------------

vtkPlusStream::~vtkPlusStream(void)
{
  // A stream is only responsible for cleaning up non-tool streams
  StreamBuffers.clear();
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusStream::ReadConfiguration( vtkXMLDataElement* aStreamElement )
{
  // Read the stream element, build the stream
  // If there are references to tools, request them from the owner device and keep a reference to them here
  const char * id = aStreamElement->GetAttribute("Id");
  if( id == NULL )
  {
    LOG_ERROR("No stream id defined. It is required for all streams.");
    return PLUS_FAIL;
  }
  this->SetStreamId(id);

  for ( int i = 0; i < aStreamElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* toolElement = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(toolElement->GetName(), "Tool") != 0 )
    {
      // if this is not a tool element, skip it
      continue; 
    }

    const char* toolName = toolElement->GetAttribute("Id");
    if( toolName == NULL )
    {
      LOG_WARNING("No field \"Id\" defined in the OutputStream " << this->GetStreamId() << ". Unable to add it to the output stream.");
      continue;
    }
    vtkPlusStreamTool* tool = NULL;
    if( this->OwnerDevice == NULL || this->OwnerDevice->GetTool(toolName, tool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to retrieve tool from owner device.");
      continue;
    }
    this->Tools[toolName] = tool;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusStream::GetBuffer( vtkPlusStreamBuffer*& aBuffer, int port )
{
  if( StreamBuffers.size() < port )
  {
    LOG_ERROR("Invalid port selected.");
    return PLUS_FAIL;
  }

  aBuffer = StreamBuffers.at(port);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusStream::GetTool( vtkPlusStreamTool*& aTool, const char* toolName )
{
  if( OwnerDevice != NULL )
  {
    return OwnerDevice->GetTool(toolName, aTool);
  }

  LOG_ERROR("Owner device not set in stream.");
  return PLUS_FAIL;
}

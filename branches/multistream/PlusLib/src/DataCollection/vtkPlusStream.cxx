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

int vtkPlusStream::MAX_PORT = 500;

//----------------------------------------------------------------------------
vtkPlusStream::vtkPlusStream(void)
: OwnerDevice(NULL)
, StreamId(NULL)
{

}

//----------------------------------------------------------------------------
vtkPlusStream::~vtkPlusStream(void)
{
  // A stream is only responsible for cleaning up non-tool streams
  StreamBuffers.clear();

  // Don't touch the tools, the devices clean those up
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
  if( StreamBuffers.find(port) == StreamBuffers.end() )
  {
    LOG_ERROR("Invalid port selected.");
    return PLUS_FAIL;
  }

  aBuffer = StreamBuffers[port];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::GetTool( vtkPlusStreamTool*& aTool, const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Null toolname sent to stream tool request.");
    return PLUS_FAIL;
  }

  for( ToolContainerIteratorType it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(toolName, it->second->GetToolName()) == 0 )
    {
      aTool = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
StreamBufferMapContainerConstIterator vtkPlusStream::GetBuffersStartConstIterator() const
{
  return this->StreamBuffers.begin();
}

//----------------------------------------------------------------------------
StreamBufferMapContainerConstIterator vtkPlusStream::GetBuffersEndConstIterator() const
{
  return this->StreamBuffers.end();
}

//----------------------------------------------------------------------------
ToolContainerConstIteratorType vtkPlusStream::GetToolBuffersStartConstIterator() const
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
ToolContainerConstIteratorType vtkPlusStream::GetToolBuffersEndConstIterator() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
ToolContainerIteratorType vtkPlusStream::GetToolBuffersStartIterator()
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
ToolContainerIteratorType vtkPlusStream::GetToolBuffersEndIterator()
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::AddTool( vtkPlusStreamTool* aTool )
{
  if( aTool == NULL )
  {
    LOG_ERROR("Trying to add null tool to stream.");
    return PLUS_FAIL;
  }

  for( ToolContainerConstIteratorType it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( it->second == aTool )
    {
      // Yes, compare pointers
      return PLUS_SUCCESS;
    }
  }

  this->Tools[aTool->GetToolName()] = aTool;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::RemoveTool( const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Trying to remove null toolname from stream.");
    return PLUS_FAIL;
  }

  for( ToolContainerIteratorType it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(it->second->GetToolName(), toolName) == 0 )
    {
      this->Tools.erase(it);      
      return PLUS_SUCCESS;
    }
  }
  
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::AddBuffer( vtkPlusStreamBuffer* aBuffer, int& outNewPort )
{
  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    if( it->second == aBuffer )
    {
      return PLUS_SUCCESS;
    }
  }

  std::vector<int> usedPorts;
  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    usedPorts.push_back(it->first);
  }

  outNewPort = 0;
  while( outNewPort < MAX_PORT )
  {
    if( std::find(usedPorts.begin(), usedPorts.end(), outNewPort) == usedPorts.end() )
    {
      this->StreamBuffers[outNewPort] = aBuffer;
      return PLUS_SUCCESS;
    }
    outNewPort++;
  }

  LOG_ERROR("Unable to find a suitable port for a new buffer. How did you go over " << MAX_PORT << " ports!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::Clear()
{
  // TODO : Verify that this does not delete pointers, just empties vector
  this->Tools.clear();

  // TODO : Verify that this DOES delete buffers AND empties map
  for( StreamBufferMapContainerIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    it->second->Delete();
  }
  this->StreamBuffers.clear();

  return PLUS_SUCCESS;
}

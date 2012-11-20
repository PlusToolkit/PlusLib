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
const int vtkPlusStream::FIND_PORT = -1;

//----------------------------------------------------------------------------
vtkPlusStream::vtkPlusStream(void)
: OwnerDevice(NULL)
, StreamId(NULL)
{

}

//----------------------------------------------------------------------------
vtkPlusStream::~vtkPlusStream(void)
{
  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    it->second->Delete();
  }
  for( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    it->second->Delete();
  }
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
PlusStatus vtkPlusStream::GetBuffer( vtkSmartPointer<vtkPlusStreamBuffer>& aBuffer, int port )
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
PlusStatus vtkPlusStream::GetTool( vtkSmartPointer<vtkPlusStreamTool>& aTool, const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Null toolname sent to stream tool request.");
    return PLUS_FAIL;
  }

  for( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
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
ToolContainerConstIterator vtkPlusStream::GetToolBuffersStartConstIterator() const
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
ToolContainerConstIterator vtkPlusStream::GetToolBuffersEndConstIterator() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
ToolContainerIterator vtkPlusStream::GetToolBuffersStartIterator()
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
ToolContainerIterator vtkPlusStream::GetToolBuffersEndIterator()
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::AddTool( vtkSmartPointer<vtkPlusStreamTool> aTool )
{
  if( aTool == NULL )
  {
    LOG_ERROR("Trying to add null tool to stream.");
    return PLUS_FAIL;
  }

  for( ToolContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( it->second.GetPointer() == aTool.GetPointer() )
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

  for( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(it->second->GetToolName(), toolName) == 0 )
    {
      it->second->Delete();
      this->Tools.erase(it);      
      return PLUS_SUCCESS;
    }
  }
  
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::AddBuffer( vtkSmartPointer<vtkPlusStreamBuffer> aBuffer, int port )
{
  if( port != FIND_PORT )
  {
    if( this->StreamBuffers.find(port) != this->StreamBuffers.end() )
    {
      LOG_WARNING("Overwriting a buffer at port " << port );
    }
    this->StreamBuffers[port] = aBuffer;
    return PLUS_SUCCESS;
  }

  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    if( it->second.GetPointer() == aBuffer.GetPointer() && it->first == port )
    {
      return PLUS_SUCCESS;
    }
  }

  std::vector<int> usedPorts;
  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    usedPorts.push_back(it->first);
  }

  double newPort = 0;
  while( newPort < MAX_PORT )
  {
    if( std::find(usedPorts.begin(), usedPorts.end(), newPort) == usedPorts.end() )
    {
      this->StreamBuffers[newPort] = aBuffer;
      return PLUS_SUCCESS;
    }
    newPort++;
  }

  LOG_ERROR("Unable to find a suitable port for a new buffer. How did you go over " << MAX_PORT << " ports!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::Clear()
{
  this->Tools.clear();
  this->StreamBuffers.clear();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::GetLatestTimestamp(double& aTimestamp) const
{
  aTimestamp = 0;

  for( StreamBufferMapContainerConstIterator it = this->GetBuffersStartConstIterator(); it != this->GetBuffersEndConstIterator(); ++it )
  {
    vtkSmartPointer<vtkPlusStreamBuffer> aBuf = it->second;
    double timestamp;
    if( aBuf->GetLatestTimeStamp(timestamp) == ITEM_OK )
    {
      if( timestamp > aTimestamp )
      {
        aTimestamp = timestamp;
      }
    }
  }

  for( ToolContainerConstIterator it = this->GetToolBuffersStartConstIterator(); it != this->GetToolBuffersEndConstIterator(); ++it)
  {
    vtkSmartPointer<vtkPlusStreamTool> aTool = it->second;
    if( aTool->GetBuffer() != NULL )
    {
      double timestamp;
      if( aTool->GetBuffer()->GetLatestTimeStamp(timestamp) == ITEM_OK )
      {
        if( timestamp > aTimestamp )
        {
          aTimestamp = timestamp;
        }
      }
    }
  }
  
  return aTimestamp != 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusStream::DeepCopy( const vtkPlusStream& aStream )
{
  // Make this stream look like aStream
  this->Clear();
  for( StreamBufferMapContainerConstIterator it = aStream.GetBuffersStartConstIterator(); it != aStream.GetBuffersEndConstIterator(); ++it )
  {
    vtkSmartPointer<vtkPlusStreamBuffer> aBuff = vtkSmartPointer<vtkPlusStreamBuffer>::New();
    if( this->AddBuffer(aBuff, it->first) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a buffer to port " << it->first);
      continue;
    }
  }
  for( ToolContainerConstIterator it = aStream.GetToolBuffersStartConstIterator(); it != aStream.GetToolBuffersEndConstIterator(); ++it)
  {
    vtkSmartPointer<vtkPlusStreamTool> aTool = vtkSmartPointer<vtkPlusStreamTool>::New();
    if( this->AddTool(aTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a tool when deep copying a stream.");
      continue;
    }
  }

  // Now the streams are the same!
  for( StreamBufferMapContainerConstIterator it = aStream.GetBuffersStartConstIterator(); it != aStream.GetBuffersEndConstIterator(); ++it )
  {
    // Find the buffer in this with same port, copy it
    this->StreamBuffers[it->first]->DeepCopy(it->second);
  }
  for( ToolContainerConstIterator it = aStream.GetToolBuffersStartConstIterator(); it != aStream.GetToolBuffersEndConstIterator(); ++it)
  {
    // Find the tool with same name... copy it
    this->Tools[it->first]->DeepCopy(it->second);
  }
}

//----------------------------------------------------------------------------
void vtkPlusStream::ShallowCopy( const vtkPlusStream& aStream )
{
  this->Clear();
  for( StreamBufferMapContainerConstIterator it = aStream.GetBuffersStartConstIterator(); it != aStream.GetBuffersEndConstIterator(); ++it )
  {
    if( this->AddBuffer(it->second, it->first) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a buffer to port " << it->first);
      continue;
    }
  }
  for( ToolContainerConstIterator it = aStream.GetToolBuffersStartConstIterator(); it != aStream.GetToolBuffersEndConstIterator(); ++it)
  {
    if( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a tool when deep copying a stream.");
      continue;
    }
  }
}

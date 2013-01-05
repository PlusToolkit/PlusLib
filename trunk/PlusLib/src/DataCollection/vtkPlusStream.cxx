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
  StreamBuffers.clear();
  Tools.clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::ReadConfiguration( vtkXMLDataElement* aStreamElement, bool RequireFrameBufferSizeInDeviceSetConfiguration, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration )
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

  // TODO : come up with a way to define multiple buffers per stream... stereo image devices will require it
  int bufferSize = 0;
  if ( aStreamElement->GetScalarAttribute("BufferSize", bufferSize) )
  {
    vtkSmartPointer<vtkPlusStreamBuffer> aBuff = vtkSmartPointer<vtkPlusStreamBuffer>::New();
    if( this->AddBuffer(aBuff, vtkPlusStream::FIND_PORT) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a buffer to the stream. Can't set params of buffer.");
      return PLUS_FAIL;
    }
    // Set the buffer size
    if ( aBuff->SetBufferSize(bufferSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set buffer size!");
    }
  }
  else if( RequireFrameBufferSizeInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find main buffer size in device element when it is required.");
  }

  int averagedItemsForFiltering = 0;
  if ( aStreamElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    for( StreamBufferMapContainerIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
    {
      vtkPlusStreamBuffer* aBuff = it->second;
      aBuff->SetAveragedItemsForFiltering(averagedItemsForFiltering);
    }
  }
  else if ( RequireAveragedItemsForFilteringInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find averaged items for filtering in stream configuration when it is required.");
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG("Unable to find AveragedItemsForFiltering attribute in device element. Using default value.");
  }

  for ( int i = 0; i < aStreamElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* toolElement = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(toolElement->GetName(), "Tool") != 0 )
    {
      // if this is not a tool element, skip it
      continue; 
    }

    const char* toolName = toolElement->GetAttribute("Name");
    if( toolName == NULL )
    {
      LOG_WARNING("No field \"Name\" defined in the OutputStream " << this->GetStreamId() << ". Unable to add it to the output stream.");
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
PlusStatus vtkPlusStream::WriteConfiguration( vtkXMLDataElement* aStreamElement )
{
  aStreamElement->SetAttribute("Id", this->GetStreamId());

  if( this->StreamBuffers.size() > 0 )
  {
    // TODO : extend this to support multiple buffers per stream
    aStreamElement->SetIntAttribute("BufferSize", this->StreamBuffers[0]->GetBufferSize());
    if( aStreamElement->GetAttribute("AveragedItemsForFiltering") != NULL )
    {
      aStreamElement->SetIntAttribute("AveragedItemsForFiltering", this->StreamBuffers[0]->GetAveragedItemsForFiltering());
    }
  }

  for ( int i = 0; i < aStreamElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* toolElement = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(toolElement->GetName(), "Tool") != 0 )
    {
      // if this is not a tool element, skip it
      continue; 
    }
    vtkPlusStreamTool* aTool=NULL;
    if( toolElement->GetAttribute("Name") == NULL || this->GetTool(aTool, toolElement->GetAttribute("Name")) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve tool when saving config.");
      return PLUS_FAIL;
    }
    aTool->WriteCompactConfiguration(toolElement);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::WriteCompactConfiguration( vtkXMLDataElement* aStreamElement )
{
  aStreamElement->SetAttribute("Id", this->GetStreamId());

  for ( int i = 0; i < aStreamElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* toolElement = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(toolElement->GetName(), "Tool") != 0 )
    {
      // if this is not a tool element, skip it
      continue; 
    }
    vtkPlusStreamTool* aTool=NULL;
    if( toolElement->GetAttribute("Name") == NULL || this->GetTool(aTool, toolElement->GetAttribute("Name")) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve tool when saving config.");
      return PLUS_FAIL;
    }
    aTool->WriteCompactConfiguration(toolElement);
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
PlusStatus vtkPlusStream::GetTool(vtkPlusStreamTool*& aTool, const char* toolName )
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
PlusStatus vtkPlusStream::AddTool(vtkPlusStreamTool* aTool )
{
  if( aTool == NULL )
  {
    LOG_ERROR("Trying to add null tool to stream.");
    return PLUS_FAIL;
  }

  for( ToolContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( it->second == aTool )
    {
      // Yes, compare pointers
      return PLUS_SUCCESS;
    }
  }

  this->Tools[aTool->GetToolName()] = aTool;
  this->Tools[aTool->GetToolName()]->Register(this);

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
      it->second->UnRegister(this);
      this->Tools.erase(it);      
      return PLUS_SUCCESS;
    }
  }
  
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::AddBuffer(vtkPlusStreamBuffer* aBuffer, int port )
{
  if( port != FIND_PORT )
  {
    if( this->StreamBuffers.find(port) != this->StreamBuffers.end() )
    {
      LOG_WARNING("Overwriting a buffer at port " << port );
    }
    this->StreamBuffers[port] = aBuffer;
    this->StreamBuffers[port]->Register(this);
    return PLUS_SUCCESS;
  }

  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    if( it->second == aBuffer && it->first == port )
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
      this->StreamBuffers[newPort]->Register(this);
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
  for( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    (it->second)->UnRegister(this);
  }
  this->Tools.clear();
  
  for( StreamBufferMapContainerConstIterator it = this->StreamBuffers.begin(); it != this->StreamBuffers.end(); ++it)
  {
    (it->second)->UnRegister(this);
  }
  this->StreamBuffers.clear();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::GetLatestTimestamp(double& aTimestamp) const
{
  aTimestamp = 0;

  for( StreamBufferMapContainerConstIterator it = this->GetBuffersStartConstIterator(); it != this->GetBuffersEndConstIterator(); ++it )
  {
    vtkPlusStreamBuffer* aBuf = it->second;
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
    vtkPlusStreamTool* aTool = it->second;
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

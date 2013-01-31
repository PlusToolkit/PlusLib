/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDataSource.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusChannel, "$Revision: 1.0$");
vtkStandardNewMacro(vtkPlusChannel);

//----------------------------------------------------------------------------
vtkPlusChannel::vtkPlusChannel(void)
: VideoSource(NULL)
, OwnerDevice(NULL)
, ChannelId(NULL)
{

}

//----------------------------------------------------------------------------
vtkPlusChannel::~vtkPlusChannel(void)
{
  this->VideoSource = NULL;
  Tools.clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::ReadConfiguration( vtkXMLDataElement* aChannelElement )
{
  // Read the stream element, build the stream
  // If there are references to tools, request them from the owner device and keep a reference to them here
  const char * id = aChannelElement->GetAttribute("Id");
  if( id == NULL )
  {
    LOG_ERROR("No stream id defined. It is required for all streams.");
    return PLUS_FAIL;
  }
  this->SetChannelId(id);

  vtkPlusDataSource* aSource = NULL;
  for ( int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* aSourceElement = aChannelElement->GetNestedElement(i); 
    if ( STRCASECMP(aSourceElement->GetName(), "DataSource") != 0 )
    {
      // if this is not an image element, skip it
      continue; 
    }

    const char* id = aSourceElement->GetAttribute("Id");
    if( id == NULL )
    {
      LOG_WARNING("No field \"Id\" defined in the source element " << this->GetChannelId() << ". Unable to add it to the channel.");
      continue;
    }

    if( this->OwnerDevice != NULL && this->OwnerDevice->GetDataSource(id, aSource) == PLUS_SUCCESS)
    {
      if( aSource->GetType() == DATA_SOURCE_TYPE_VIDEO )
      {
        LOG_ERROR("Do not put Video sources as sub-tags of an output channel. Use the VideoDataSourceId attribute of the OutputChannel tag.");
      }
      else if( aSource->GetType() == DATA_SOURCE_TYPE_TOOL )
      {
        this->Tools[id] = aSource;
      }
      else
      {
        LOG_ERROR("Unknown source type when reading channel configuration. Id: " << id);
      }
    }
    else
    {
      LOG_ERROR("Unable to add tool data source \'" << id << "\'.");
      return PLUS_FAIL;
    }
  }

  if( aChannelElement->GetAttribute("VideoDataSourceId") != NULL && this->OwnerDevice->GetVideoSource(aChannelElement->GetAttribute("VideoDataSourceId"), aSource) == PLUS_SUCCESS )
  {
    this->VideoSource = aSource;
  }
  else if( aChannelElement->GetAttribute("VideoDataSourceId") != NULL )
  {
    LOG_ERROR("Unable to find video data source that matches Id: " << aChannelElement->GetAttribute("VideoDataSourceId"));
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::WriteConfiguration( vtkXMLDataElement* aChannelElement )
{
  aChannelElement->SetAttribute("Id", this->GetChannelId());

  for ( int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* element = aChannelElement->GetNestedElement(i); 
    if ( STRCASECMP(element->GetName(), "DataSource") != 0 )
    {
      continue; 
    }
    if( element->GetAttribute("Type") != NULL && STRCASECMP(element->GetAttribute("Type"), "Video") == 0 )
    {
      if( this->HasVideoSource() )
      {
        this->VideoSource->WriteConfiguration(element);
      }
    }
    else if( element->GetAttribute("Type") != NULL && STRCASECMP(element->GetAttribute("Type"), "Tool") == 0 )
    {
      vtkPlusDataSource* aTool = NULL;
      if( element->GetAttribute("Id") == NULL || this->GetTool(aTool, element->GetAttribute("Id")) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to retrieve tool when saving config.");
        return PLUS_FAIL;
      }
      aTool->WriteCompactConfiguration(element);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetVideoSource( vtkPlusDataSource*& aVideoSource ) const
{
  if( this->HasVideoSource() )
  {
    aVideoSource = this->VideoSource;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTool(vtkPlusDataSource*& aTool, const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Null toolname sent to stream tool request.");
    return PLUS_FAIL;
  }

  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(toolName, it->second->GetSourceId()) == 0 )
    {
      aTool = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusChannel::GetToolsStartConstIterator() const
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusChannel::GetToolsEndConstIterator() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
DataSourceContainerIterator vtkPlusChannel::GetToolsStartIterator()
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerIterator vtkPlusChannel::GetToolsEndIterator()
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::AddTool(vtkPlusDataSource* aTool )
{
  if( aTool == NULL )
  {
    LOG_ERROR("Trying to add null tool to stream.");
    return PLUS_FAIL;
  }

  for( DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( it->second == aTool )
    {
      // Yes, compare pointers
      return PLUS_SUCCESS;
    }
  }

  this->Tools[aTool->GetSourceId()] = aTool;
  this->Tools[aTool->GetSourceId()]->Register(this);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::RemoveTool( const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Trying to remove null toolname from stream.");
    return PLUS_FAIL;
  }

  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(it->second->GetSourceId(), toolName) == 0 )
    {
      it->second->UnRegister(this);
      this->Tools.erase(it);      
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::Clear()
{
  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    (it->second)->UnRegister(this);
  }
  this->Tools.clear();
  this->VideoSource = NULL;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetLatestTimestamp(double& aTimestamp) const
{
  aTimestamp = 0;

  if( this->HasVideoSource() )
  {
    if( this->VideoSource->GetBuffer()->GetLatestTimeStamp(aTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Unable to retrieve latest timestamp from the video source buffer.");
    }
  }

  for( DataSourceContainerConstIterator it = this->GetToolsStartConstIterator(); it != this->GetToolsEndConstIterator(); ++it)
  {
    vtkPlusDataSource* aTool = it->second;
    double timestamp;
    if( aTool->GetBuffer()->GetLatestTimeStamp(timestamp) == ITEM_OK )
    {
      if( timestamp > aTimestamp )
      {
        aTimestamp = timestamp;
      }
    }
  }

  return aTimestamp != 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusChannel::ShallowCopy( const vtkPlusChannel& aChannel )
{
  this->Clear();

  vtkPlusDataSource* aSource = NULL;
  if( aChannel.HasVideoSource() && aChannel.GetVideoSource(aSource) )
  {
    this->VideoSource = aSource;
  }
  for( DataSourceContainerConstIterator it = aChannel.GetToolsStartConstIterator(); it != aChannel.GetToolsEndConstIterator(); ++it)
  {
    if( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a tool when shallow copying a stream.");
      continue;
    }
  }
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::HasVideoSource() const
{
  return this->VideoSource != NULL;
}

//----------------------------------------------------------------------------
void vtkPlusChannel::SetVideoSource( vtkPlusDataSource* aSource )
{
  this->VideoSource = aSource;
}

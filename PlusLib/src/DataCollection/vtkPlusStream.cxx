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
#include "vtkPlusStreamImage.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusStream, "$Revision: 1.0$");
vtkStandardNewMacro(vtkPlusStream);

//----------------------------------------------------------------------------
vtkPlusStream::vtkPlusStream(void)
: OwnerDevice(NULL)
, StreamId(NULL)
{

}

//----------------------------------------------------------------------------
vtkPlusStream::~vtkPlusStream(void)
{
  Images.clear();
  Tools.clear();
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
    vtkXMLDataElement* imageElement = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(imageElement->GetName(), "Image") != 0 )
    {
      // if this is not an image element, skip it
      continue; 
    }

    const char* name = imageElement->GetAttribute("Id");
    if( name == NULL )
    {
      LOG_WARNING("No field \"Id\" defined in the image element " << this->GetStreamId() << ". Unable to add it to the output stream.");
      continue;
    }
    vtkPlusStreamImage* image = NULL;
    if( this->OwnerDevice == NULL || this->OwnerDevice->GetImage(name, image) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to retrieve image from owner device.");
      continue;
    }
    this->Images[name] = image;
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
      LOG_WARNING("No field \"Name\" defined in the tool element " << this->GetStreamId() << ". Unable to add it to the output stream.");
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

  for ( int i = 0; i < aStreamElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* element = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(element->GetName(), "Image") != 0 )
    {
      continue; 
    }
    vtkPlusStreamImage* anImage = NULL;
    if( element->GetAttribute("Name") == NULL || this->GetImage(anImage, element->GetAttribute("Name")) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve image when saving config.");
      return PLUS_FAIL;
    }
    anImage->WriteCompactConfiguration(element);
  }

  for ( int i = 0; i < aStreamElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* toolElement = aStreamElement->GetNestedElement(i); 
    if ( STRCASECMP(toolElement->GetName(), "Tool") != 0 )
    {
      continue; 
    }
    vtkPlusStreamTool* aTool = NULL;
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
PlusStatus vtkPlusStream::GetImage( vtkPlusStreamImage*& anImage, const char* name )
{
  if( name == NULL )
  {
    LOG_ERROR("Null name sent to stream image request.");
    return PLUS_FAIL;
  }

  for( ImageContainerIterator it = this->Images.begin(); it != this->Images.end(); ++it)
  {
    if( STRCASECMP(name, it->second->GetImageName()) == 0 )
    {
      anImage = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
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
ImageContainerConstIterator vtkPlusStream::GetImagesStartConstIterator() const
{
  return this->Images.begin();
}

//----------------------------------------------------------------------------
ImageContainerConstIterator vtkPlusStream::GetImagesEndConstIterator() const
{
  return this->Images.end();
}

//----------------------------------------------------------------------------
ImageContainerIterator vtkPlusStream::GetImagesStartIterator()
{
  return this->Images.begin();
}

//----------------------------------------------------------------------------
ImageContainerIterator vtkPlusStream::GetImagesEndIterator()
{
  return this->Images.end();
}

//----------------------------------------------------------------------------
ToolContainerConstIterator vtkPlusStream::GetToolsStartConstIterator() const
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
ToolContainerConstIterator vtkPlusStream::GetToolsEndConstIterator() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
ToolContainerIterator vtkPlusStream::GetToolsStartIterator()
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
ToolContainerIterator vtkPlusStream::GetToolsEndIterator()
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
PlusStatus vtkPlusStream::AddImage(vtkPlusStreamImage* anImage)
{
  if( anImage == NULL )
  {
    LOG_ERROR("Trying to add null image to stream.");
    return PLUS_FAIL;
  }

  for( ImageContainerConstIterator it = this->Images.begin(); it != this->Images.end(); ++it)
  {
    if( it->second == anImage )
    {
      return PLUS_SUCCESS;
    }
  }

  this->Images[anImage->GetImageName()] = anImage;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::Clear()
{
  for( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    (it->second)->UnRegister(this);
  }
  this->Tools.clear();
  this->Images.clear();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStream::GetLatestTimestamp(double& aTimestamp) const
{
  aTimestamp = 0;

  for( ImageContainerConstIterator it = this->GetImagesStartConstIterator(); it != this->GetImagesEndConstIterator(); ++it )
  {
    vtkPlusStreamImage* aBuf = it->second;
    double timestamp;
    if( it->second->GetBuffer()->GetLatestTimeStamp(timestamp) == ITEM_OK )
    {
      if( timestamp > aTimestamp )
      {
        aTimestamp = timestamp;
      }
    }
  }

  for( ToolContainerConstIterator it = this->GetToolsStartConstIterator(); it != this->GetToolsEndConstIterator(); ++it)
  {
    vtkPlusStreamTool* aTool = it->second;
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
void vtkPlusStream::DeepCopy( const vtkPlusStream& aStream )
{
  // Make this stream look like aStream
  this->ShallowCopy(aStream);

  // Now the streams are the same!
  for( ImageContainerConstIterator it = aStream.GetImagesStartConstIterator(); it != aStream.GetImagesEndConstIterator(); ++it )
  {
    // Find the buffer in this with same port, copy it
    this->Images[it->first]->DeepCopy(it->second);
  }
  for( ToolContainerConstIterator it = aStream.GetToolsStartConstIterator(); it != aStream.GetToolsEndConstIterator(); ++it)
  {
    // Find the tool with same name... copy it
    this->Tools[it->first]->DeepCopy(it->second);
  }
}

//----------------------------------------------------------------------------
void vtkPlusStream::ShallowCopy( const vtkPlusStream& aStream )
{
  this->Clear();
  for( ImageContainerConstIterator it = aStream.GetImagesStartConstIterator(); it != aStream.GetImagesEndConstIterator(); ++it)
  {
    vtkSmartPointer<vtkPlusStreamImage> anImage = vtkSmartPointer<vtkPlusStreamImage>::New();
    if( this->AddImage(anImage) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add an image when shallow copying a stream.");
      continue;
    }
  }
  for( ToolContainerConstIterator it = aStream.GetToolsStartConstIterator(); it != aStream.GetToolsEndConstIterator(); ++it)
  {
    if( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a tool when shallow copying a stream.");
      continue;
    }
  }
}

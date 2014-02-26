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
, PortImageOrientation(US_IMG_ORIENT_XX)
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

  if ( this->PortName != NULL )
  {
    delete [] this->PortName; 
    this->PortName=NULL; 
  }

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
    // Here we would normally delete SourceId and set it to NULL, but we just return with an error instead because modification of the value is not allowed
    LOG_ERROR("SourceId change is not allowed for source '" << this->SourceId << "'" ); 
    return PLUS_FAIL; 
  }

  if (aSourceId!=NULL)
  {
    // Copy string  (based on vtkSetStringMacro in vtkSetGet.h)
    size_t n = strlen(aSourceId) + 1; 
    char *cp1 =  new char[n]; 
    const char *cp2 = (aSourceId); 
    this->SourceId = cp1;
    do { *cp1++ = *cp2++; } while ( --n ); 
  }

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
    // Here we would normally delete ReferenceCoordinateFrame and set it to NULL, but we just return with an error instead because modification of the value is not allowed
    LOG_ERROR("Reference frame name change is not allowed for tool '" << this->ReferenceCoordinateFrameName << "'" ); 
    return PLUS_FAIL; 
  }

  if (referenceName!=NULL)
  {
    // Copy string  (based on vtkSetStringMacro in vtkSetGet.h)
    size_t n = strlen(referenceName) + 1; 
    char *cp1 =  new char[n]; 
    const char *cp2 = (referenceName); 
    this->ReferenceCoordinateFrameName = cp1;
    do { *cp1++ = *cp2++; } while ( --n ); 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetPortName(const char* portName)
{
  if ( this->PortName == NULL && portName == NULL) 
  { 
    // no change (current and requested name are both empty)
    return PLUS_SUCCESS;
  } 

  if ( this->PortName && portName && ( STRCASECMP(this->PortName, portName) == 0 ) ) 
  { 
    // no change (current and requested names are te same)
    return PLUS_SUCCESS;
  } 

  if ( this->PortName != NULL )
  {
    // Here we would normally delete PortName and set it to NULL, but we just return with an error instead because modification of the value is not allowed
    LOG_ERROR("Port name change is not allowed on source port'" << this->PortName << "'" ); 
    return PLUS_FAIL; 
  }

  if ( portName != NULL )
  {
    // Copy string (based on vtkSetStringMacro in vtkSetGet.h)
    size_t n = strlen(portName) + 1; 
    char *cp1 =  new char[n]; 
    const char *cp2 = (portName); 
    this->PortName = cp1;
    do { *cp1++ = *cp2++; } while ( --n ); 
  }

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

  this->CustomProperties=aSource->CustomProperties;
}


//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::ReadConfiguration(vtkXMLDataElement* sourceElement, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration, bool RequireImageOrientationInSourceConfiguration, const char* aDescriptiveNameForBuffer)
{
  LOG_TRACE("vtkPlusDataSource::ReadConfiguration"); 

  if ( sourceElement == NULL )
  {
    LOG_ERROR("Unable to configure data source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  const char* sourceId = sourceElement->GetAttribute("Id"); 
  if ( sourceId == NULL ) 
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
    PlusTransformName idName(sourceId, this->GetReferenceCoordinateFrameName());
    this->SetSourceId(idName.GetTransformName().c_str());
    this->SetType(DATA_SOURCE_TYPE_TOOL);
    
    if( portName == NULL )
    {
      LOG_ERROR("Unable to find PortName! This attribute is mandatory in tool definition."); 
      return PLUS_FAIL; 
    }
  }
  else if ( type != NULL && STRCASECMP(type, "Video") == 0 ) 
  {
    this->SetSourceId(sourceId); 
    this->SetType(DATA_SOURCE_TYPE_VIDEO);

    const char* usImageOrientation = sourceElement->GetAttribute("PortUsImageOrientation");
    if ( usImageOrientation != NULL )
    {
      LOG_INFO("Selected US image orientation: " << usImageOrientation );
      this->SetPortImageOrientation( PlusVideoFrame::GetUsImageOrientationFromString(usImageOrientation) );
      if ( this->GetPortImageOrientation() == US_IMG_ORIENT_XX )
      {
        LOG_ERROR("Video image orientation is undefined - please set PortUsImageOrientation in the source configuration");
      }
    }
    else if (RequireImageOrientationInSourceConfiguration)
    {
      LOG_ERROR("Video image orientation is not defined in the source \'" << this->GetSourceId() << "\' element - please set PortUsImageOrientation in the source configuration");
    }

    const char* imageType = sourceElement->GetAttribute("ImageType"); 
    if ( imageType != NULL && this->GetBuffer() != NULL ) 
    {
      if( STRCASECMP(imageType, "BRIGHTNESS") == 0 )
      {
        this->GetBuffer()->SetImageType(US_IMG_BRIGHTNESS);
      }
      else if( STRCASECMP(imageType, "RGB_COLOUR") == 0 )
      {
        this->GetBuffer()->SetImageType(US_IMG_RGB_COLOUR);
      }
      else if( STRCASECMP(imageType, "RF_I_LINE_Q_LINE") == 0 )
      {
        this->GetBuffer()->SetImageType(US_IMG_RF_I_LINE_Q_LINE);
      }
      else if( STRCASECMP(imageType, "RF_IQ_LINE") == 0 )
      {
        this->GetBuffer()->SetImageType(US_IMG_RF_IQ_LINE);
      }
      else if( STRCASECMP(imageType, "RF_REAL") == 0 )
      {
        this->GetBuffer()->SetImageType(US_IMG_RF_REAL);
      }
    }
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

  if( this->GetType() == DATA_SOURCE_TYPE_TOOL )
  {
    PlusTransformName sourceId(this->GetSourceId());
    aSourceElement->SetAttribute("Id", sourceId.From().c_str());
  }
  else
  {
    aSourceElement->SetAttribute("Id", this->GetSourceId());
  }
  aSourceElement->SetAttribute("PortName", this->GetPortName());
  aSourceElement->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize());

  if( aSourceElement->GetAttribute("AveragedItemsForFiltering") != NULL )
  {
    aSourceElement->SetIntAttribute("AveragedItemsForFiltering", this->GetBuffer()->GetAveragedItemsForFiltering());
  }

  // TODO: write custom properties

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

  if( this->GetType() == DATA_SOURCE_TYPE_TOOL )
  {
    PlusTransformName sourceId(this->GetSourceId());
    aSourceElement->SetAttribute("Id", sourceId.From().c_str());
  }
  else
  {
    aSourceElement->SetAttribute("Id", this->GetSourceId());
  }
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

//-----------------------------------------------------------------------------
std::string vtkPlusDataSource::GetTransformName() const
{
  std::stringstream ss;
  ss << this->SourceId << "To" << this->ReferenceCoordinateFrameName;
  return ss.str();
}

//-----------------------------------------------------------------------------
std::string vtkPlusDataSource::GetCustomProperty(const std::string& propertyName)
{
  std::map< std::string, std::string > :: iterator prop=this->CustomProperties.find(propertyName);
  std::string propValue;
  if (prop!=this->CustomProperties.end())
  {
    propValue=prop->second;
  }
  return propValue;
}

//-----------------------------------------------------------------------------
void vtkPlusDataSource::SetCustomProperty(const std::string& propertyName, const std::string& propertyValue)
{
  this->CustomProperties[propertyName]=propertyValue;
}

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkPlusDataSource);

//----------------------------------------------------------------------------
vtkPlusDataSource::vtkPlusDataSource()
: Device(NULL)
, PortName(NULL)
, InputImageOrientation(US_IMG_ORIENT_XX) // a.k.a. PortUsImageOrientation, PortImageOrientation
, Type(DATA_SOURCE_TYPE_NONE)
, FrameNumber(0)
, SourceId(NULL)
, ReferenceCoordinateFrameName(NULL)
, Buffer(vtkPlusBuffer::New())
{
  this->ClipRectangleOrigin[0] = PlusCommon::NO_CLIP;
  this->ClipRectangleOrigin[1] = PlusCommon::NO_CLIP;
  this->ClipRectangleOrigin[2] = PlusCommon::NO_CLIP;

  this->ClipRectangleSize[0] = PlusCommon::NO_CLIP;
  this->ClipRectangleSize[1] = PlusCommon::NO_CLIP;
  this->ClipRectangleSize[2] = PlusCommon::NO_CLIP;

  this->InputFrameSize[0] = 0;
  this->InputFrameSize[1] = 0;
  this->InputFrameSize[2] = 1;
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
PlusStatus vtkPlusDataSource::SetReferenceCoordinateFrameName(const char* referenceCoordinateName)
{
  if ( this->ReferenceCoordinateFrameName == NULL && referenceCoordinateName == NULL) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ReferenceCoordinateFrameName && referenceCoordinateName && ( STRCASECMP(this->ReferenceCoordinateFrameName, referenceCoordinateName) == 0 ) ) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ReferenceCoordinateFrameName != NULL )
  {
    // Here we would normally delete ReferenceCoordinateFrame and set it to NULL, but we just return with an error instead because modification of the value is not allowed
    LOG_ERROR("Reference frame name change is not allowed for tool '" << this->ReferenceCoordinateFrameName << "'" ); 
    return PLUS_FAIL; 
  }

  if (referenceCoordinateName!=NULL)
  {
    // Copy string  (based on vtkSetStringMacro in vtkSetGet.h)
    size_t n = strlen(referenceCoordinateName) + 1; 
    char *cp1 =  new char[n]; 
    const char *cp2 = (referenceCoordinateName); 
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
void vtkPlusDataSource::DeepCopy(vtkPlusDataSource *aSource)
{
  LOG_TRACE("vtkPlusDataSource::DeepCopy"); 

  this->SetSourceId( aSource->GetSourceId() ); 
  this->SetType( aSource->GetType() );
  this->SetReferenceCoordinateFrameName( aSource->GetReferenceCoordinateFrameName() );

  this->Buffer->DeepCopy( aSource->GetBuffer() );

  this->SetFrameNumber( aSource->GetFrameNumber() );

  this->CustomProperties=aSource->CustomProperties;
}


//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::ReadConfiguration(vtkXMLDataElement* sourceElement, bool requirePortNameInSourceConfiguration, bool requireImageOrientationInSourceConfiguration, const char* aDescriptiveNameForBuffer)
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
    
    if (requirePortNameInSourceConfiguration && portName == NULL )
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
      this->SetInputImageOrientation( PlusVideoFrame::GetUsImageOrientationFromString(usImageOrientation) );
      if ( this->GetInputImageOrientation() == US_IMG_ORIENT_XX )
      {
        LOG_ERROR("Video image orientation is undefined - please set PortUsImageOrientation in the source configuration");
      }
    }
    else if (requireImageOrientationInSourceConfiguration)
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
      else if( STRCASECMP(imageType, "RGB_COLOR") == 0 )
      {
        this->GetBuffer()->SetImageType(US_IMG_RGB_COLOR);
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

    // clipping parameters
    XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 3, ClipRectangleOrigin, sourceElement);
    XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 3, ClipRectangleSize, sourceElement);
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
    LOG_DEBUG("Buffer size is not defined in source element \"" << this->GetSourceId() << "\". Using default buffer size: "<<this->GetBuffer()->GetBufferSize());
  }

  int averagedItemsForFiltering = 0;
  if ( sourceElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
  }
  else
  {
    LOG_DEBUG("AveragedItemsForFiltering is not defined in source element \"" << this->GetSourceId() << "\". Using default value: "<<this->GetBuffer()->GetAveragedItemsForFiltering());
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

  // Read custom properties
  for( int i = 0; i < sourceElement->GetNumberOfNestedElements(); ++i )
  {
    if( STRCASECMP(sourceElement->GetNestedElement(i)->GetName(), "CustomProperties") == 0 )
    {
      vtkXMLDataElement* customPropertiesElement = sourceElement->GetNestedElement(i);

      for( int j = 0; j < customPropertiesElement->GetNumberOfNestedElements(); ++j )
      {
        vtkXMLDataElement* customPropertyElement = customPropertiesElement->GetNestedElement(j);
        this->CustomProperties[customPropertyElement->GetName()] = customPropertyElement->GetCharacterData();
      }
      break;
    }
  }

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

  // Write custom properties
  if( this->CustomProperties.size() > 0 )
  {
    vtkSmartPointer<vtkXMLDataElement> customPropertiesElement = vtkSmartPointer<vtkXMLDataElement>::New();
    customPropertiesElement->SetName("CustomProperties");

    for( CustomPropertyMapIterator it = this->CustomProperties.begin(); it != this->CustomProperties.end(); ++it )
    {
      vtkSmartPointer<vtkXMLDataElement> customPropertyElement = vtkSmartPointer<vtkXMLDataElement>::New();
      customPropertyElement->SetName(it->first.c_str());
      customPropertyElement->SetCharacterData(it->second.c_str(), it->second.length());
      customPropertiesElement->AddNestedElement(customPropertyElement);
    }

    aSourceElement->AddNestedElement(customPropertiesElement);
  }

  aSourceElement->SetVectorAttribute("ClipRectangleOrigin", 3, this->GetClipRectangleOrigin());
  aSourceElement->SetVectorAttribute("ClipRectangleSize", 3, this->GetClipRectangleSize());

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
  std::ostringstream ss;
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

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, US_IMAGE_TYPE imageType, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const TrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(frame, usImageOrientation, imageType, frameNumber, this->ClipRectangleOrigin, this->ClipRectangleSize, unfilteredTimestamp, filteredTimestamp, customFields);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(const PlusVideoFrame* frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const TrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(frame, frameNumber, this->ClipRectangleOrigin, this->ClipRectangleSize, unfilteredTimestamp, filteredTimestamp, customFields);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const int frameSizeInPx[3], PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const TrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, this->ClipRectangleOrigin, this->ClipRectangleSize, unfilteredTimestamp, filteredTimestamp, customFields);
}

//-----------------------------------------------------------------------------
US_IMAGE_TYPE vtkPlusDataSource::GetImageType()
{
  return this->GetBuffer()->GetImageType();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetImageType(US_IMAGE_TYPE imageType)
{
  return this->GetBuffer()->SetImageType(imageType);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetInputFrameSize(int x, int y, int z)
{
  this->InputFrameSize[0] = x;
  this->InputFrameSize[1] = y;
  this->InputFrameSize[2] = z;

  int outputFrameSizeInPx[3] = {x,y,z};

  int extents[6] = {0, x-1, 0, y-1, 0, z-1};
  if(PlusCommon::IsClippingRequested(this->ClipRectangleOrigin, this->ClipRectangleSize))
  {
    if (PlusCommon::IsClippingWithinExtents(this->ClipRectangleOrigin, this->ClipRectangleSize, extents))
    {
      outputFrameSizeInPx[0] = this->ClipRectangleSize[0];
      outputFrameSizeInPx[1] = this->ClipRectangleSize[1];
      outputFrameSizeInPx[2] = this->ClipRectangleSize[2];
    }
    else
    {
      LOG_WARNING("Clipping information cannot fit within the original image extents ["<<extents[0]<<","<<extents[1]<<","
        <<extents[2]<<","<<extents[3]<<","<<extents[4]<<","<<extents[5]<<"]. No clipping will be performed."
        <<" Origin=[" << this->ClipRectangleOrigin[0] << "," << this->ClipRectangleOrigin[1] << "," << this->ClipRectangleOrigin[2] << "]."
        <<" Size=[" << this->ClipRectangleSize[0] << "," << this->ClipRectangleSize[1] << "," << this->ClipRectangleSize[2] << "].");
      this->ClipRectangleOrigin[0] = PlusCommon::NO_CLIP;
      this->ClipRectangleOrigin[1] = PlusCommon::NO_CLIP;
      this->ClipRectangleOrigin[2] = PlusCommon::NO_CLIP;
      this->ClipRectangleSize[0] = PlusCommon::NO_CLIP;
      this->ClipRectangleSize[1] = PlusCommon::NO_CLIP;
      this->ClipRectangleSize[2] = PlusCommon::NO_CLIP;
    }
  }

  PlusVideoFrame::FlipInfoType flipInfo;
  if ( PlusVideoFrame::GetFlipAxes(this->InputImageOrientation, this->GetBuffer()->GetImageType(), this->GetBuffer()->GetImageOrientation(), flipInfo) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from " << PlusVideoFrame::GetStringFromUsImageOrientation(this->InputImageOrientation) << 
      " to " << PlusVideoFrame::GetStringFromUsImageOrientation(this->GetBuffer()->GetImageOrientation()) <<
      " for a buffer of type " << PlusVideoFrame::GetStringFromUsImageType(this->GetBuffer()->GetImageType()));
    return PLUS_FAIL;
  }

  if( flipInfo.tranpose == PlusVideoFrame::TRANSPOSE_IJKtoKIJ )
  {
    int temp = outputFrameSizeInPx[0];
    outputFrameSizeInPx[0] = outputFrameSizeInPx[2];
    outputFrameSizeInPx[2] = outputFrameSizeInPx[1];
    outputFrameSizeInPx[1] = temp;
  }

  return this->GetBuffer()->SetFrameSize(outputFrameSizeInPx[0], outputFrameSizeInPx[1], outputFrameSizeInPx[2]);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetInputFrameSize(int frameSize[3])
{
  return this->SetInputFrameSize(frameSize[0], frameSize[1], frameSize[2]);
}

//-----------------------------------------------------------------------------
int* vtkPlusDataSource::GetOutputFrameSize()
{
  return this->GetBuffer()->GetFrameSize();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::GetOutputFrameSize(int &_arg1, int &_arg2, int &_arg3)
{
  return this->GetBuffer()->GetFrameSize(_arg1, _arg2, _arg3);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::GetOutputFrameSize(int _arg[3])
{
  return this->GetBuffer()->GetFrameSize(_arg);
}

//-----------------------------------------------------------------------------
vtkPlusBuffer* vtkPlusDataSource::GetBuffer()
{
  return this->Buffer;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetInputImageOrientation(US_IMAGE_ORIENTATION imageOrientation)
{
  this->InputImageOrientation = imageOrientation;
  // Orientation of the images in the buffer is standardized (MF(A) for B-mode images, FM for RF-mode images).
  // We set up the this standard image orientation based on the orientation of the input data.
  if( imageOrientation <= US_IMG_ORIENT_XX || imageOrientation >= US_IMG_ORIENT_LAST)
  {
    LOG_ERROR("vtkPlusDataSource::SetInputImageOrientation failed: invalid image orientation received");
    return PLUS_FAIL;
  }
  if( imageOrientation <= US_IMG_ORIENT_FU )
  {
    // B-mode or non-ultrasound
    return SetOutputImageOrientation(US_IMG_ORIENT_MF);
  }
  else
  {
    // RF-mode
    return SetOutputImageOrientation(US_IMG_ORIENT_FM);
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetOutputImageOrientation(US_IMAGE_ORIENTATION imageOrientation)
{
  if (imageOrientation!=US_IMG_ORIENT_MF && imageOrientation!=US_IMG_ORIENT_FM)
  {
    LOG_ERROR("vtkPlusDataSource::SetOutputImageOrientation failed: only standard MF and FM orientations are allowed, got "
      << PlusVideoFrame::GetStringFromUsImageOrientation(imageOrientation));
    return PLUS_FAIL;
  }
  return this->GetBuffer()->SetImageOrientation(imageOrientation);
}

//-----------------------------------------------------------------------------
US_IMAGE_ORIENTATION vtkPlusDataSource::GetOutputImageOrientation()
{
  return this->GetBuffer()->GetImageOrientation();
}

//-----------------------------------------------------------------------------
US_IMAGE_ORIENTATION vtkPlusDataSource::GetInputImageOrientation()
{
  return this->InputImageOrientation;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetNumberOfScalarComponents(int numberOfScalarComponents)
{
  return this->GetBuffer()->SetNumberOfScalarComponents(numberOfScalarComponents);
}

//-----------------------------------------------------------------------------
int vtkPlusDataSource::GetNumberOfScalarComponents()
{
  return this->GetBuffer()->GetNumberOfScalarComponents();
}

//-----------------------------------------------------------------------------
PlusCommon::VTKScalarPixelType vtkPlusDataSource::GetPixelType()
{
  return this->GetBuffer()->GetPixelType();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetPixelType(PlusCommon::VTKScalarPixelType pixelType)
{
  return this->GetBuffer()->SetPixelType(pixelType);
}

//-----------------------------------------------------------------------------
void vtkPlusDataSource::SetStartTime(double startTime)
{
  return this->GetBuffer()->SetStartTime(startTime);
}

//-----------------------------------------------------------------------------
double vtkPlusDataSource::GetStartTime()
{
  return this->GetBuffer()->GetStartTime();
}

//-----------------------------------------------------------------------------
int vtkPlusDataSource::GetNumberOfItems()
{
  return this->GetBuffer()->GetNumberOfItems();
}

//-----------------------------------------------------------------------------
BufferItemUidType vtkPlusDataSource::GetOldestItemUidInBuffer()
{
  return this->GetBuffer()->GetOldestItemUidInBuffer();
}

//-----------------------------------------------------------------------------
BufferItemUidType vtkPlusDataSource::GetLatestItemUidInBuffer()
{
  return this->GetBuffer()->GetLatestItemUidInBuffer();
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetItemUidFromTime(double time, BufferItemUidType& uid)
{
  return this->GetBuffer()->GetItemUidFromTime(time, uid);
}

//-----------------------------------------------------------------------------
bool vtkPlusDataSource::GetLatestItemHasValidVideoData()
{
  return this->GetBuffer()->GetLatestItemHasValidVideoData();
}

//-----------------------------------------------------------------------------
bool vtkPlusDataSource::GetLatestItemHasValidTransformData()
{
  return this->GetBuffer()->GetLatestItemHasValidTransformData();
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetStreamBufferItem(BufferItemUidType uid, StreamBufferItem* bufferItem)
{
  return this->GetBuffer()->GetStreamBufferItem(uid, bufferItem);
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetLatestStreamBufferItem(StreamBufferItem* bufferItem)
{
  return this->GetBuffer()->GetLatestStreamBufferItem(bufferItem);
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetOldestStreamBufferItem(StreamBufferItem* bufferItem)
{
  return this->GetBuffer()->GetOldestStreamBufferItem(bufferItem);
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetStreamBufferItemFromTime(double time, StreamBufferItem* bufferItem, vtkPlusBuffer::DataItemTemporalInterpolationType interpolation)
{
  return this->GetBuffer()->GetStreamBufferItemFromTime(time, bufferItem, interpolation);
}

//-----------------------------------------------------------------------------
void vtkPlusDataSource::Clear()
{
  return this->GetBuffer()->Clear();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetBufferSize(int n)
{
  return this->GetBuffer()->SetBufferSize(n);
}

//-----------------------------------------------------------------------------
int vtkPlusDataSource::GetBufferSize()
{
  return this->GetBuffer()->GetBufferSize();
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetLatestTimeStamp(double& latestTimestamp)
{
  return this->GetBuffer()->GetLatestTimeStamp(latestTimestamp);
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetOldestTimeStamp(double& oldestTimestamp)
{
  return this->GetBuffer()->GetOldestTimeStamp(oldestTimestamp);
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetTimeStamp(BufferItemUidType uid, double& timestamp)
{
  return this->GetBuffer()->GetTimeStamp(uid, timestamp);
}

//-----------------------------------------------------------------------------
void vtkPlusDataSource::SetLocalTimeOffsetSec(double offsetSec)
{
  return this->GetBuffer()->SetLocalTimeOffsetSec(offsetSec);
}

//-----------------------------------------------------------------------------
double vtkPlusDataSource::GetLocalTimeOffsetSec()
{
  return this->GetBuffer()->GetLocalTimeOffsetSec();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::GetTimeStampReportTable(vtkTable* timeStampReportTable)
{
  return this->GetBuffer()->GetTimeStampReportTable(timeStampReportTable);
}

//-----------------------------------------------------------------------------
void vtkPlusDataSource::SetTimeStampReporting(bool enable)
{
  return this->GetBuffer()->SetTimeStampReporting(enable);
}

//-----------------------------------------------------------------------------
bool vtkPlusDataSource::GetTimeStampReporting()
{
  return this->GetBuffer()->GetTimeStampReporting();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::WriteToMetafile(const char* filename, bool useCompression /*= false */)
{
  return this->GetBuffer()->WriteToMetafile(filename, useCompression);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::DeepCopyBufferTo(vtkPlusBuffer& bufferToFill)
{
  bufferToFill.DeepCopy(this->GetBuffer());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddTimeStampedItem(vtkMatrix4x4 *matrix, ToolStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/)
{
  return this->GetBuffer()->AddTimeStampedItem(matrix, status, frameNumber, unfilteredTimestamp, filteredTimestamp);
}

//-----------------------------------------------------------------------------
int vtkPlusDataSource::GetNumberOfBytesPerPixel()
{
  return this->GetBuffer()->GetNumberOfBytesPerPixel();
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetIndex(const BufferItemUidType uid, unsigned long &index)
{
  return this->GetBuffer()->GetIndex(uid, index);
}

//-----------------------------------------------------------------------------
double vtkPlusDataSource::GetFrameRate(bool ideal /*= false*/, double *framePeriodStdevSecPtr/*=NULL*/)
{
  return this->GetBuffer()->GetFrameRate(ideal, framePeriodStdevSecPtr);
}

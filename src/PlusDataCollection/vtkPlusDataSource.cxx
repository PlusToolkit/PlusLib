/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
//#include "igsioVideoFrame.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDataSource);

//----------------------------------------------------------------------------

std::string vtkPlusDataSource::DATA_SOURCE_TYPE_TOOL_TAG = "Tool";
std::string vtkPlusDataSource::DATA_SOURCE_TYPE_VIDEO_TAG = "Video";
std::string vtkPlusDataSource::DATA_SOURCE_TYPE_FIELDDATA_TAG = "FieldData";

//----------------------------------------------------------------------------
vtkPlusDataSource::vtkPlusDataSource()
  : Device(NULL)
  , PortName("")
  , InputImageOrientation(US_IMG_ORIENT_XX)   // a.k.a. PortUsImageOrientation, PortImageOrientation
  , Type(DATA_SOURCE_TYPE_NONE)
  , FrameNumber(0)
  , Id("")
  , ReferenceCoordinateFrameName("")
  , Buffer(vtkPlusBuffer::New())
{
  this->ClipRectangleOrigin[0] = igsioCommon::NO_CLIP;
  this->ClipRectangleOrigin[1] = igsioCommon::NO_CLIP;
  this->ClipRectangleOrigin[2] = igsioCommon::NO_CLIP;

  this->ClipRectangleSize[0] = igsioCommon::NO_CLIP;
  this->ClipRectangleSize[1] = igsioCommon::NO_CLIP;
  this->ClipRectangleSize[2] = igsioCommon::NO_CLIP;

  this->InputFrameSize[0] = 0;
  this->InputFrameSize[1] = 0;
  this->InputFrameSize[2] = 1;
}

//----------------------------------------------------------------------------
vtkPlusDataSource::~vtkPlusDataSource()
{
  if (this->Buffer != NULL)
  {
    this->Buffer->Delete();
    this->Buffer = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os, indent);

  if (this->Device)
  {
    os << indent << "Tracker: " << this->Device << std::endl;
  }
  os << indent << "Id: " << this->GetId() << std::endl;
  if (this->Type != DATA_SOURCE_TYPE_NONE)
  {
    switch (this->Type)
    {
      case DATA_SOURCE_TYPE_TOOL:
        os << indent << "Type: Tool" << std::endl;
        break;
      case DATA_SOURCE_TYPE_VIDEO:
        os << indent << "Type: Video" << std::endl;
        break;
      case DATA_SOURCE_TYPE_FIELDDATA:
        os << indent << "Type: Fields" << std::endl;
        break;
      case DATA_SOURCE_TYPE_NONE:
        break;
    }
  }
  os << indent << "ReferenceCoordinateFrameName: " << this->GetReferenceCoordinateFrameName() << std::endl;

  if (!this->PortName.empty())
  {
    os << indent << "PortName: " << this->GetPortName() << std::endl;
  }

  if (this->Buffer)
  {
    os << indent << "Buffer: " << this->Buffer << std::endl;
    this->Buffer->PrintSelf(os, indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetId(const char* aSourceId)
{
  if (aSourceId == NULL && !this->Id.empty())
  {
    return PLUS_FAIL;
  }

  return this->SetId(std::string(aSourceId));
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetId(const std::string& aSourceId)
{
  if (this->Id == aSourceId)
  {
    return PLUS_SUCCESS;
  }

  if (!this->Id.empty())
  {
    // Here we would normally delete SourceId and set it to NULL, but we just return with an error instead because modification of the value is not allowed
    LOG_ERROR("SourceId change is not allowed for source '" << this->Id << "'");
    return PLUS_FAIL;
  }

  this->Id = aSourceId;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetSourceId(const std::string& aSourceId)
{
  return this->SetId(aSourceId);
}

//----------------------------------------------------------------------------
std::string vtkPlusDataSource::GetSourceId() const
{
  return this->GetId();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetReferenceCoordinateFrameName(const std::string& referenceCoordinateName)
{
  if (this->ReferenceCoordinateFrameName == referenceCoordinateName)
  {
    return PLUS_SUCCESS;
  }

  if (!this->ReferenceCoordinateFrameName.empty())
  {
    // Here we would normally delete ReferenceCoordinateFrame and set it to NULL, but we just return with an error instead because modification of the value is not allowed
    LOG_ERROR("Reference frame name change is not allowed for tool '" << this->ReferenceCoordinateFrameName << "'");
    return PLUS_FAIL;
  }

  this->ReferenceCoordinateFrameName = referenceCoordinateName;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetReferenceCoordinateFrameName(const char* referenceName)
{
  if (referenceName == NULL && !this->ReferenceCoordinateFrameName.empty())
  {
    return PLUS_FAIL;
  }

  return this->SetReferenceCoordinateFrameName(std::string(referenceName));
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetPortName(const char* portName)
{
  if (portName == NULL)
  {
    LOG_ERROR("NULL portName sent to SetPortName.");
    return PLUS_FAIL;
  }

  return this->SetPortName(std::string(portName));
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetPortName(const std::string& portName)
{
  if (this->PortName == portName)
  {
    // no change (current and requested name are the same)
    return PLUS_SUCCESS;
  }

  if (!this->PortName.empty())
  {
    LOG_ERROR("Port name change is not allowed on source port'" << this->PortName << "'");
    return PLUS_FAIL;
  }

  if (!portName.empty())
  {
    this->PortName = portName;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::DeepCopy(const vtkPlusDataSource& aSource)
{
  LOG_TRACE("vtkPlusDataSource::DeepCopy");

  this->SetId(aSource.GetId());
  this->SetType(aSource.GetType());
  this->SetReferenceCoordinateFrameName(aSource.GetReferenceCoordinateFrameName());

  this->Buffer->DeepCopy(aSource.Buffer);

  this->SetFrameNumber(aSource.GetFrameNumber());

  this->CustomProperties = aSource.CustomProperties;
}

//----------------------------------------------------------------------------
std::array<int, 3> vtkPlusDataSource::GetClipRectangleSize() const
{
  return this->ClipRectangleSize;
}

//----------------------------------------------------------------------------
std::array<int, 3> vtkPlusDataSource::GetClipRectangleOrigin() const
{
  return this->ClipRectangleOrigin;
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::SetClipRectangleSize(const std::array<int, 3> _arg)
{
  this->ClipRectangleSize = _arg;
}

//----------------------------------------------------------------------------
void vtkPlusDataSource::SetClipRectangleOrigin(const std::array<int, 3> _arg)
{
  this->ClipRectangleOrigin = _arg;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::ReadConfiguration(vtkXMLDataElement* sourceElement, bool requirePortNameInSourceConfiguration, bool requireImageOrientationInSourceConfiguration, const std::string& aDescriptiveNameForBuffer)
{
  LOG_TRACE("vtkPlusDataSource::ReadConfiguration");

  if (sourceElement == NULL)
  {
    LOG_ERROR("Unable to configure data source! (XML data element is NULL)");
    return PLUS_FAIL;
  }

  std::string sourceId = sourceElement->GetAttribute("Id") != NULL ? sourceElement->GetAttribute("Id") : "";
  if (sourceId.empty())
  {
    LOG_ERROR("Unable to find attribute \"Id\"! Id attribute is mandatory in source definition.");
    return PLUS_FAIL;
  }

  const char* portName = sourceElement->GetAttribute("PortName");
  if (portName != NULL)
  {
    this->SetPortName(portName);
  }

  const char* type = sourceElement->GetAttribute("Type");
  if (type != NULL && STRCASECMP(type, DATA_SOURCE_TYPE_TOOL_TAG.c_str()) == 0)
  {
    igsioTransformName idName(sourceId, this->GetReferenceCoordinateFrameName());
    this->SetId(idName.GetTransformName());
    this->SetType(DATA_SOURCE_TYPE_TOOL);

    if (requirePortNameInSourceConfiguration && portName == NULL)
    {
      LOG_ERROR("Unable to find PortName! This attribute is mandatory in tool definition.");
      return PLUS_FAIL;
    }
  }
  else if (type != NULL && STRCASECMP(type, DATA_SOURCE_TYPE_FIELDDATA_TAG.c_str()) == 0)
  {
    this->SetId(sourceId);
    this->SetType(DATA_SOURCE_TYPE_FIELDDATA);
  }
  else if (type != NULL && STRCASECMP(type, DATA_SOURCE_TYPE_VIDEO_TAG.c_str()) == 0)
  {
    this->SetId(sourceId);
    this->SetType(DATA_SOURCE_TYPE_VIDEO);

    const char* usImageOrientation = sourceElement->GetAttribute("PortUsImageOrientation");
    if (usImageOrientation != NULL)
    {
      LOG_INFO("Selected US image orientation: " << usImageOrientation);
      this->SetInputImageOrientation(igsioVideoFrame::GetUsImageOrientationFromString(usImageOrientation));
      if (this->GetInputImageOrientation() == US_IMG_ORIENT_XX)
      {
        LOG_ERROR("Video image orientation is undefined - please set PortUsImageOrientation in the source configuration");
      }
    }
    else if (requireImageOrientationInSourceConfiguration)
    {
      LOG_ERROR("Video image orientation is not defined in the source \'" << this->GetId() << "\' element - please set PortUsImageOrientation in the source configuration");
    }

    const char* imageType = sourceElement->GetAttribute("ImageType");
    if (imageType != NULL && this->GetBuffer() != NULL)
    {
      if (STRCASECMP(imageType, "BRIGHTNESS") == 0)
      {
        this->GetBuffer()->SetImageType(US_IMG_BRIGHTNESS);
      }
      else if (STRCASECMP(imageType, "RGB_COLOR") == 0)
      {
        this->GetBuffer()->SetImageType(US_IMG_RGB_COLOR);
      }
      else if (STRCASECMP(imageType, "RF_I_LINE_Q_LINE") == 0)
      {
        this->GetBuffer()->SetImageType(US_IMG_RF_I_LINE_Q_LINE);
      }
      else if (STRCASECMP(imageType, "RF_IQ_LINE") == 0)
      {
        this->GetBuffer()->SetImageType(US_IMG_RF_IQ_LINE);
      }
      else if (STRCASECMP(imageType, "RF_REAL") == 0)
      {
        this->GetBuffer()->SetImageType(US_IMG_RF_REAL);
      }
    }

    // Clipping parameters:
    // Users may forget that images are 3D and provide clipping coordinates and size in 2D only.
    // Detect this and set correct values in the third component.
    int tmpValue[3] = {0}; // 3 so that we can see if only 2 components could be successfully read
    int clipRectangleOriginComponents = sourceElement->GetVectorAttribute("ClipRectangleOrigin", 3, tmpValue);
    if (clipRectangleOriginComponents == 2)
    {
      // Only 2D data is provided
      XML_READ_STD_ARRAY_ATTRIBUTE_EXACT_OPTIONAL(int, 2, ClipRectangleOrigin, sourceElement);
      if (this->ClipRectangleOrigin[0] == igsioCommon::NO_CLIP || this->ClipRectangleOrigin[1] == igsioCommon::NO_CLIP)
      {
        this->ClipRectangleOrigin[2] = igsioCommon::NO_CLIP;
      }
      else
      {
        this->ClipRectangleOrigin[2] = 0;
      }
    }
    else
    {
      XML_READ_STD_ARRAY_ATTRIBUTE_OPTIONAL(int, 3, ClipRectangleOrigin, sourceElement);
    }
    int clipRectangleSizeComponents = sourceElement->GetVectorAttribute("ClipRectangleSize", 3, tmpValue);
    if (clipRectangleSizeComponents == 2)
    {
      // Only 2D data is provided
      XML_READ_STD_ARRAY_ATTRIBUTE_EXACT_OPTIONAL(int, 2, ClipRectangleSize, sourceElement);
      if (this->ClipRectangleSize[0] == igsioCommon::NO_CLIP || this->ClipRectangleSize[1] == igsioCommon::NO_CLIP)
      {
        this->ClipRectangleSize[2] = igsioCommon::NO_CLIP;
      }
      else
      {
        this->ClipRectangleSize[2] = 1;
      }
    }
    else
    {
      XML_READ_STD_ARRAY_ATTRIBUTE_OPTIONAL(int, 3, ClipRectangleSize, sourceElement);
    }
  }
  else
  {
    LOG_ERROR("Missing type element. It is required to define the source type.");
    return PLUS_FAIL;
  }

  int bufferSize = 0;
  if (sourceElement->GetScalarAttribute("BufferSize", bufferSize))
  {
    this->GetBuffer()->SetBufferSize(bufferSize);
  }
  else
  {
    LOG_DEBUG("Buffer size is not defined in source element \"" << this->GetId() << "\". Using default buffer size: " << this->GetBuffer()->GetBufferSize());
  }

  int averagedItemsForFiltering = 0;
  if (sourceElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering))
  {
    this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
  }
  else
  {
    LOG_DEBUG("AveragedItemsForFiltering is not defined in source element \"" << this->GetId() << "\". Using default value: " << this->GetBuffer()->GetAveragedItemsForFiltering());
  }

  std::string descName;
  if (!aDescriptiveNameForBuffer.empty())
  {
    descName += aDescriptiveNameForBuffer;
    descName += "-";
    descName += this->GetId();
  }
  else
  {
    descName += this->GetId();
  }
  this->GetBuffer()->SetDescriptiveName(descName.c_str());

  // Read custom properties
  for (int i = 0; i < sourceElement->GetNumberOfNestedElements(); ++i)
  {
    if (STRCASECMP(sourceElement->GetNestedElement(i)->GetName(), "CustomProperties") == 0)
    {
      vtkXMLDataElement* customPropertiesElement = sourceElement->GetNestedElement(i);

      for (int j = 0; j < customPropertiesElement->GetNumberOfNestedElements(); ++j)
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
PlusStatus vtkPlusDataSource::WriteConfiguration(vtkXMLDataElement* aSourceElement)
{
  LOG_TRACE("vtkPlusDataSource::WriteConfiguration");

  if (aSourceElement == NULL)
  {
    LOG_ERROR("Unable to configure data source! (XML data element is NULL)");
    return PLUS_FAIL;
  }

  if (this->GetType() == DATA_SOURCE_TYPE_TOOL)
  {
    igsioTransformName sourceId(this->GetId());
    aSourceElement->SetAttribute("Id", sourceId.From().c_str());
  }
  else
  {
    XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(Id, aSourceElement);
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(PortName, aSourceElement);
  aSourceElement->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize());

  if (aSourceElement->GetAttribute("AveragedItemsForFiltering") != NULL)
  {
    aSourceElement->SetIntAttribute("AveragedItemsForFiltering", this->GetBuffer()->GetAveragedItemsForFiltering());
  }

  // Write custom properties
  if (this->CustomProperties.size() > 0)
  {
    vtkSmartPointer<vtkXMLDataElement> customPropertiesElement = vtkSmartPointer<vtkXMLDataElement>::New();
    customPropertiesElement->SetName("CustomProperties");

    for (CustomPropertyMapIterator it = this->CustomProperties.begin(); it != this->CustomProperties.end(); ++it)
    {
      vtkSmartPointer<vtkXMLDataElement> customPropertyElement = vtkSmartPointer<vtkXMLDataElement>::New();
      customPropertyElement->SetName(it->first.c_str());
      customPropertyElement->SetCharacterData(it->second.c_str(), it->second.length());
      customPropertiesElement->AddNestedElement(customPropertyElement);
    }

    aSourceElement->AddNestedElement(customPropertiesElement);
  }

  if (igsioCommon::IsClippingRequested(this->ClipRectangleOrigin, this->ClipRectangleSize))
  {
    {
      int tmpValue[3] = { this->ClipRectangleOrigin[0], this->ClipRectangleOrigin[1], this->ClipRectangleOrigin[2] };
      aSourceElement->SetVectorAttribute("ClipRectangleOrigin", 3, tmpValue);
    }
    {
      int tmpValue[3] = { this->ClipRectangleSize[0], this->ClipRectangleSize[1], this->ClipRectangleSize[2] };
      aSourceElement->SetVectorAttribute("ClipRectangleSize", 3, tmpValue);
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::WriteCompactConfiguration(vtkXMLDataElement* aSourceElement)
{
  LOG_TRACE("vtkPlusDataSource::WriteConfiguration");

  if (aSourceElement == NULL)
  {
    LOG_ERROR("Unable to configure source! (XML data element is NULL)");
    return PLUS_FAIL;
  }

  if (this->GetType() == DATA_SOURCE_TYPE_TOOL)
  {
    igsioTransformName sourceId(this->GetId());
    aSourceElement->SetAttribute("Id", sourceId.From().c_str());
  }
  else
  {
    XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(Id, aSourceElement);
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(PortName, aSourceElement);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkPlusDataSource::GetTransformName() const
{
  std::ostringstream ss;
  ss << this->Id << "To" << this->ReferenceCoordinateFrameName;
  return ss.str();
}

//-----------------------------------------------------------------------------
std::string vtkPlusDataSource::GetCustomProperty(const std::string& propertyName)
{
  std::map< std::string, std::string > :: iterator prop = this->CustomProperties.find(propertyName);
  std::string propValue;
  if (prop != this->CustomProperties.end())
  {
    propValue = prop->second;
  }
  return propValue;
}

//-----------------------------------------------------------------------------
void vtkPlusDataSource::SetCustomProperty(const std::string& propertyName, const std::string& propertyValue)
{
  this->CustomProperties[propertyName] = propertyValue;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, US_IMAGE_TYPE imageType, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(frame, usImageOrientation, imageType, frameNumber, this->ClipRectangleOrigin, this->ClipRectangleSize, unfilteredTimestamp, filteredTimestamp, customFields);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(const igsioVideoFrame* frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(frame, frameNumber, this->ClipRectangleOrigin, this->ClipRectangleSize, unfilteredTimestamp, filteredTimestamp, customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(const igsioTrackedFrame::FieldMapType& customFields, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
                                      double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/)
{
  return this->GetBuffer()->AddItem(customFields, frameNumber, unfilteredTimestamp, filteredTimestamp);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const FrameSizeType& frameSizeInPx, igsioCommon::VTKScalarPixelType pixelType,
                                      unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/,
                                      double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber,
                                    this->ClipRectangleOrigin, this->ClipRectangleSize, unfilteredTimestamp, filteredTimestamp, customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddItem(void* imageDataPtr, const FrameSizeType& frameSize, unsigned int frameSizeInBytes, US_IMAGE_TYPE imageType, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddItem(imageDataPtr, frameSize, frameSizeInBytes, imageType, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields);
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
PlusStatus vtkPlusDataSource::SetInputFrameSize(unsigned int x, unsigned int y, unsigned int z)
{
  this->InputFrameSize[0] = x;
  this->InputFrameSize[1] = y;
  this->InputFrameSize[2] = z;

  FrameSizeType outputFrameSizeInPx = {x, y, z};

  if (x > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
      y > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
      z > static_cast<unsigned int>(std::numeric_limits<int>::max()))
  {
    LOG_ERROR("Unable to determine clipping details in vtkPlusDataSource::SetInputFrameSize. Cannot continue.");
    return PLUS_FAIL;
  }

  int extents[6] = {0, static_cast<int>(x) - 1, 0, static_cast<int>(y) - 1, 0, static_cast<int>(z) - 1};
  if (igsioCommon::IsClippingRequested(this->ClipRectangleOrigin, this->ClipRectangleSize))
  {
    if (igsioCommon::IsClippingWithinExtents(this->ClipRectangleOrigin, this->ClipRectangleSize, extents))
    {
      outputFrameSizeInPx[0] = static_cast<unsigned int>(this->ClipRectangleSize[0]);
      outputFrameSizeInPx[1] = static_cast<unsigned int>(this->ClipRectangleSize[1]);
      outputFrameSizeInPx[2] = static_cast<unsigned int>(this->ClipRectangleSize[2]);
    }
    else
    {
      LOG_WARNING("Clipping information cannot fit within the original image extents [" << extents[0] << "," << extents[1] << ","
                  << extents[2] << "," << extents[3] << "," << extents[4] << "," << extents[5] << "]. No clipping will be performed."
                  << " Origin=[" << this->ClipRectangleOrigin[0] << "," << this->ClipRectangleOrigin[1] << "," << this->ClipRectangleOrigin[2] << "]."
                  << " Size=[" << this->ClipRectangleSize[0] << "," << this->ClipRectangleSize[1] << "," << this->ClipRectangleSize[2] << "].");
      this->ClipRectangleOrigin[0] = igsioCommon::NO_CLIP;
      this->ClipRectangleOrigin[1] = igsioCommon::NO_CLIP;
      this->ClipRectangleOrigin[2] = igsioCommon::NO_CLIP;
      this->ClipRectangleSize[0] = igsioCommon::NO_CLIP;
      this->ClipRectangleSize[1] = igsioCommon::NO_CLIP;
      this->ClipRectangleSize[2] = igsioCommon::NO_CLIP;
    }
  }

  igsioVideoFrame::FlipInfoType flipInfo;
  if (igsioVideoFrame::GetFlipAxes(this->InputImageOrientation, this->GetBuffer()->GetImageType(), this->GetBuffer()->GetImageOrientation(), flipInfo) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from " << igsioVideoFrame::GetStringFromUsImageOrientation(this->InputImageOrientation) <<
              " to " << igsioVideoFrame::GetStringFromUsImageOrientation(this->GetBuffer()->GetImageOrientation()) <<
              " for a buffer of type " << igsioVideoFrame::GetStringFromUsImageType(this->GetBuffer()->GetImageType()));
    return PLUS_FAIL;
  }

  if (flipInfo.tranpose == igsioVideoFrame::TRANSPOSE_IJKtoKIJ)
  {
    unsigned int temp = outputFrameSizeInPx[0];
    outputFrameSizeInPx[0] = outputFrameSizeInPx[2];
    outputFrameSizeInPx[2] = outputFrameSizeInPx[1];
    outputFrameSizeInPx[1] = temp;
  }

  return this->GetBuffer()->SetFrameSize(outputFrameSizeInPx[0], outputFrameSizeInPx[1], outputFrameSizeInPx[2]);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetInputFrameSize(const FrameSizeType& frameSize)
{
  return this->SetInputFrameSize(frameSize[0], frameSize[1], frameSize[2]);
}

//----------------------------------------------------------------------------
FrameSizeType vtkPlusDataSource::GetInputFrameSize() const
{
  return this->InputFrameSize;
}

//-----------------------------------------------------------------------------
FrameSizeType vtkPlusDataSource::GetOutputFrameSize() const
{
  return this->GetBuffer()->GetFrameSize();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::GetOutputFrameSize(unsigned int& _arg1, unsigned int& _arg2, unsigned int& _arg3) const
{
  return this->GetBuffer()->GetFrameSize(_arg1, _arg2, _arg3);
}

//-----------------------------------------------------------------------------
vtkPlusBuffer* vtkPlusDataSource::GetBuffer() const
{
  return this->Buffer;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetInputImageOrientation(US_IMAGE_ORIENTATION imageOrientation)
{
  this->InputImageOrientation = imageOrientation;
  // Orientation of the images in the buffer is standardized (MF(A) for B-mode images, FM for RF-mode images).
  // We set up the this standard image orientation based on the orientation of the input data.
  if (imageOrientation <= US_IMG_ORIENT_XX || imageOrientation >= US_IMG_ORIENT_LAST)
  {
    LOG_ERROR("vtkPlusDataSource::SetInputImageOrientation failed: invalid image orientation received");
    return PLUS_FAIL;
  }
  if (imageOrientation <= US_IMG_ORIENT_FU)
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
  if (imageOrientation != US_IMG_ORIENT_MF && imageOrientation != US_IMG_ORIENT_FM)
  {
    LOG_ERROR("vtkPlusDataSource::SetOutputImageOrientation failed: only standard MF and FM orientations are allowed, got "
              << igsioVideoFrame::GetStringFromUsImageOrientation(imageOrientation));
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
PlusStatus vtkPlusDataSource::SetNumberOfScalarComponents(unsigned int numberOfScalarComponents)
{
  return this->GetBuffer()->SetNumberOfScalarComponents(numberOfScalarComponents);
}

//-----------------------------------------------------------------------------
unsigned int vtkPlusDataSource::GetNumberOfScalarComponents()
{
  return this->GetBuffer()->GetNumberOfScalarComponents();
}

//-----------------------------------------------------------------------------
igsioCommon::VTKScalarPixelType vtkPlusDataSource::GetPixelType()
{
  return this->GetBuffer()->GetPixelType();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::SetPixelType(igsioCommon::VTKScalarPixelType pixelType)
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

//----------------------------------------------------------------------------
bool vtkPlusDataSource::GetLatestItemHasValidFieldData()
{
  return this->GetBuffer()->GetLatestItemHasValidFieldData();
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

//----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::ModifyBufferItemFrameField(BufferItemUidType uid, const std::string& key, const std::string& value)
{
  return this->GetBuffer()->ModifyBufferItemFrameField(uid, key, value);
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
PlusStatus vtkPlusDataSource::WriteToSequenceFile(const char* filename, bool useCompression /*= false */)
{
  return this->GetBuffer()->WriteToSequenceFile(filename, useCompression);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::DeepCopyBufferTo(vtkPlusBuffer& bufferToFill)
{
  bufferToFill.DeepCopy(this->GetBuffer());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDataSource::AddTimeStampedItem(vtkMatrix4x4* matrix, ToolStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  return this->GetBuffer()->AddTimeStampedItem(matrix, status, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields);
}

//-----------------------------------------------------------------------------
int vtkPlusDataSource::GetNumberOfBytesPerPixel()
{
  return this->GetBuffer()->GetNumberOfBytesPerPixel();
}

//-----------------------------------------------------------------------------
ItemStatus vtkPlusDataSource::GetIndex(const BufferItemUidType uid, unsigned long& index)
{
  return this->GetBuffer()->GetIndex(uid, index);
}

//-----------------------------------------------------------------------------
double vtkPlusDataSource::GetFrameRate(bool ideal /*= false*/, double* framePeriodStdevSecPtr/*=NULL*/)
{
  return this->GetBuffer()->GetFrameRate(ideal, framePeriodStdevSecPtr);
}

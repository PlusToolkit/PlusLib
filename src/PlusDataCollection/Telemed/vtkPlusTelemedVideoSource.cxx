/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PixelCodec.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusTelemedVideoSource.h"
#include "vtkPlusUsImagingParameters.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkObjectFactory.h>

// STL includes
#include <cstdlib>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusTelemedVideoSource); // Corresponds to the implementation of : static vtkPlusTelemedVideoSource *New();  (in .h file)

const char* vtkPlusTelemedVideoSource::KEY_SPECKLE_REDUCTION_ENABLED = "SpeckleReductionEnabled";
const char* vtkPlusTelemedVideoSource::KEY_SPECKLE_REDUCTION_METHOD = "SpeckleReductionMethod";

namespace
{
bool SpeckleReductionMethodFromXmlString(const std::string& text, int& outMethod);

//----------------------------------------------------------------------------
bool TryParseInteger(const std::string& text, int& value)
{
  char* endPtr = nullptr;
  long parsed = std::strtol(text.c_str(), &endPtr, 10);
  if (endPtr == text.c_str() || *endPtr != '\0')
  {
    return false;
  }
  value = static_cast<int>(parsed);
  return true;
}

//----------------------------------------------------------------------------
bool SpeckleReductionMethodToEnumString(int method, std::string& outEnumName)
{
  std::string familyPrefix;
  int level = -1;
  if (method >= 1 && method <= 16)
  {
    familyPrefix = "SRF_CV";
    level = method;
  }
  else if (method >= 101 && method <= 116)
  {
    familyPrefix = "SRF_PV";
    level = method - 100;
  }
  else if (method >= 201 && method <= 216)
  {
    familyPrefix = "SRF_NV";
    level = method - 200;
  }
  else if (method >= 301 && method <= 316)
  {
    familyPrefix = "SRF_NVC";
    level = method - 300;
  }
  else if (method >= 401 && method <= 416)
  {
    familyPrefix = "SRF_QV";
    level = method - 400;
  }
  else if (method >= 501 && method <= 516)
  {
    familyPrefix = "SRF_NVO";
    level = method - 500;
  }
  else
  {
    return false;
  }

  std::ostringstream enumName;
  enumName << familyPrefix << level;
  outEnumName = enumName.str();
  return true;
}

//----------------------------------------------------------------------------
bool SpeckleReductionMethodFromString(const std::string& text, int& outMethod)
{
  if (TryParseInteger(text, outMethod))
  {
    return true;
  }

  return SpeckleReductionMethodFromXmlString(text, outMethod);
}

//----------------------------------------------------------------------------
bool SpeckleReductionMethodFromXmlString(const std::string& text, int& outMethod)
{
  std::string enumName = igsioCommon::ToUpper(text);
  struct FamilyInfo
  {
    const char* Prefix;
    int BaseValue;
  };

  static const FamilyInfo families[] =
  {
    { "SRF_CV", 0 },
    { "SRF_PV", 100 },
    { "SRF_NV", 200 },
    { "SRF_NVC", 300 },
    { "SRF_QV", 400 },
    { "SRF_NVO", 500 }
  };

  for (const FamilyInfo& family : families)
  {
    std::string prefix = family.Prefix;
    if (enumName.size() <= prefix.size())
    {
      continue;
    }
    if (enumName.rfind(prefix, 0) != 0)
    {
      continue;
    }

    int level = 0;
    if (!TryParseInteger(enumName.substr(prefix.size()), level) || level < 1 || level > 16)
    {
      return false;
    }

    outMethod = family.BaseValue + level;
    return true;
  }

  return false;
}
}

//----------------------------------------------------------------------------
vtkPlusTelemedVideoSource::vtkPlusTelemedVideoSource()
  : ProbeId(0)
  , FrequencyMhz(-1)
  , DepthMm(-1)
  , GainPercent(-1)
  , DynRangeDb(-1)
  , PowerDb(-1)
  , FocusDepthPercent(-1)
  , SpeckleReductionEnabled(false)
  , SpeckleReductionMethod(-1)
  , ConnectedToDevice(false)
{
  this->FrameSize[0] = 512;
  this->FrameSize[1] = 512;
  this->FrameSize[2] = 1; // just in case if the frame size is passed to a method that expects a 3D frame size

  this->Device = NULL;
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusTelemedVideoSource::~vtkPlusTelemedVideoSource()
{
  delete this->Device;
  this->Device = NULL;
}

//----------------------------------------------------------------------------
void vtkPlusTelemedVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusTelemedVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STD_ARRAY_ATTRIBUTE_OPTIONAL(int, 3, FrameSize, deviceConfig);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ProbeId, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DepthMm, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FrequencyMhz, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DynRangeDb, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, GainPercent, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, PowerDb, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FocusDepthPercent, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SpeckleReductionEnabled, deviceConfig);

  std::string speckleReductionMethodStr;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(SpeckleReductionMethod, speckleReductionMethodStr, deviceConfig);
  if (!speckleReductionMethodStr.empty())
  {
    int parsedMethod = -1;
    if (SpeckleReductionMethodFromXmlString(speckleReductionMethodStr, parsedMethod))
    {
      this->SpeckleReductionMethod = parsedMethod;
    }
    else
    {
      LOG_ERROR("Unable to parse SpeckleReductionMethod='" << speckleReductionMethodStr << "'. Use a CLEAR_VIEW enum name (for example SRF_NVC6).");
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  int frameSize[3] = { this->FrameSize[0], this->FrameSize[1], this->FrameSize[2] };
  deviceConfig->SetVectorAttribute("FrameSize", 3, frameSize);

  deviceConfig->SetIntAttribute("ProbeId", this->ProbeId);
  deviceConfig->SetDoubleAttribute("DepthMm", this->DepthMm);
  deviceConfig->SetDoubleAttribute("FrequencyMhz", this->FrequencyMhz);
  deviceConfig->SetDoubleAttribute("DynRangeDb", this->DynRangeDb);
  deviceConfig->SetDoubleAttribute("GainPercent", this->GainPercent);
  deviceConfig->SetDoubleAttribute("PowerDb", this->PowerDb);
  deviceConfig->SetDoubleAttribute("FocusDepthPercent", this->FocusDepthPercent);
  deviceConfig->SetAttribute("SpeckleReductionEnabled", this->SpeckleReductionEnabled ? "TRUE" : "FALSE");
  std::string speckleReductionMethodAsEnum;
  if (SpeckleReductionMethodToEnumString(this->SpeckleReductionMethod, speckleReductionMethodAsEnum))
  {
    deviceConfig->SetAttribute("SpeckleReductionMethod", speckleReductionMethodAsEnum.c_str());
  }
  else
  {
    deviceConfig->SetIntAttribute("SpeckleReductionMethod", this->SpeckleReductionMethod);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::FreezeDevice(bool freeze)
{
  this->Device->FreezeDevice(freeze);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::InternalConnect()
{
  if (this->Device == NULL)
  {
    this->Device = new TelemedUltrasound();
  }
  this->Device->SetMaximumFrameSize(this->FrameSize);
  if (this->Device->Connect(this->ProbeId) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource device initialization failed");
    this->ConnectedToDevice = false;
    return PLUS_FAIL;
  }
  this->ConnectedToDevice = true;

  // Updating imaging parameters from config file.
  // We should consider removing the parameters from this class, and rely on
  // vtkPlusUsImagingParameters to read parameters from config file.
  if (this->FrequencyMhz > 0)
  {
    this->ImagingParameters->SetFrequencyMhz(this->FrequencyMhz);
  }
  if (this->DepthMm > 0)
  {
    this->ImagingParameters->SetDepthMm(this->DepthMm);
  }
  if (this->GainPercent >= 0)
  {
    this->ImagingParameters->SetGainPercent(this->GainPercent);
  }
  if (this->DynRangeDb > 0)
  {
    this->ImagingParameters->SetDynRangeDb(this->DynRangeDb);
  }
  if (this->PowerDb >= 0)
  {
    this->ImagingParameters->SetPowerDb(this->PowerDb);
  }
  if (this->FocusDepthPercent >= 0 && this->FocusDepthPercent <= 1)
  {
    this->ImagingParameters->SetFocusDepthPercent(this->FocusDepthPercent);
  }

  // For the parameters not set from the config file or in the imaging parameters, we should try to read them from the current device settings
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FREQUENCY))
  {
    double deviceFrequencyMhz;
    this->GetFrequencyMhz(deviceFrequencyMhz);
    this->ImagingParameters->SetFrequencyMhz(deviceFrequencyMhz);
  }
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DEPTH))
  {
    double deviceDepthMM;
    this->GetDepthMm(deviceDepthMM);
    this->ImagingParameters->SetDepthMm(deviceDepthMM);
  }
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_GAIN))
  {
    double deviceGainPercent;
    this->GetGainPercent(deviceGainPercent);
    this->ImagingParameters->SetGainPercent(deviceGainPercent);
  }
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DYNRANGE))
  {
    double deviceDynRangeDb;
    this->GetDynRangeDb(deviceDynRangeDb);
    this->ImagingParameters->SetDynRangeDb(deviceDynRangeDb);
  }
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_POWER))
  {
    double devicePowerDb;
    this->GetPowerDb(devicePowerDb);
    this->ImagingParameters->SetPowerDb(devicePowerDb);
  }
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FOCUS_DEPTH))
  {
    double deviceFocusDepthPercent;
    this->GetFocusDepthPercent(deviceFocusDepthPercent);
    this->ImagingParameters->SetFocusDepthPercent(deviceFocusDepthPercent);
  }
  if (!this->ImagingParameters->IsSet(KEY_SPECKLE_REDUCTION_ENABLED))
  {
    bool deviceSpeckleReductionEnabled;
    if (this->GetSpeckleReductionEnabled(deviceSpeckleReductionEnabled) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<bool>(KEY_SPECKLE_REDUCTION_ENABLED, deviceSpeckleReductionEnabled);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_SPECKLE_REDUCTION_METHOD))
  {
    int deviceSpeckleReductionMethod;
    if (this->GetSpeckleReductionMethod(deviceSpeckleReductionMethod) == PLUS_SUCCESS)
    {
      std::string speckleReductionMethodAsEnum;
      if (SpeckleReductionMethodToEnumString(deviceSpeckleReductionMethod, speckleReductionMethodAsEnum))
      {
        this->ImagingParameters->SetValue<std::string>(KEY_SPECKLE_REDUCTION_METHOD, speckleReductionMethodAsEnum);
      }
      else
      {
        this->ImagingParameters->SetValue<int>(KEY_SPECKLE_REDUCTION_METHOD, deviceSpeckleReductionMethod);
      }
    }
  }

  // Apply all of the parameters
  this->InternalApplyImagingParameterChange();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Telemed");
  this->Device->Disconnect();
  delete this->Device;
  this->Device = NULL;
  this->ConnectedToDevice = false;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusTelemedVideoSource::InternalUpdate");
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  // Capture one frame from the Telemed device
  unsigned char* bufferData = this->Device->CaptureFrame();
  if (bufferData == NULL)
  {
    LOG_ERROR("No frame received by the device");
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  FrameSizeType frameSizeInPix = { 0, 0, 1 };
  this->Device->GetFrameSize(frameSizeInPix);
  int bufferSize = this->Device->GetBufferSize();
  if (frameSizeInPix[0] * frameSizeInPix[1] == 0)
  {
    LOG_ERROR("Failed to retrieve valid frame size (got " << frameSizeInPix[0] << "x" << frameSizeInPix[1]);
    return PLUS_FAIL;
  }
  unsigned int numberOfScalarComponents = bufferSize / (frameSizeInPix[0] * frameSizeInPix[1]);

  PixelCodec::PixelEncoding encoding = PixelCodec::PixelEncoding_BGR24;
  PixelCodec::ComponentOrdering componentOrdering = PixelCodec::ComponentOrder_RGB;
  if (numberOfScalarComponents == 3)
  {
    encoding = PixelCodec::PixelEncoding_BGR24;
    componentOrdering = PixelCodec::ComponentOrder_RGB;
  }
  else if (numberOfScalarComponents == 4)
  {
    encoding = PixelCodec::PixelEncoding_RGBA32;
    componentOrdering = PixelCodec::ComponentOrder_RGBA;
  }
  else
  {
    LOG_ERROR("Unexpected number of scalar components: " << numberOfScalarComponents);
    return PLUS_FAIL;
  }

  // Retrieve the video source in Telemed device
  vtkPlusDataSource* aSource = NULL;
  if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the Telemed device.");
    return PLUS_FAIL;
  }

  // If the buffer is empty, set the pixel type and frame size to the first received properties
  if (aSource->GetNumberOfItems() == 0)
  {
    LOG_DEBUG("Set up image buffer for Telemed");
    aSource->SetPixelType(VTK_UNSIGNED_CHAR);
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetInputFrameSize(frameSizeInPix);
    LOG_DEBUG("Frame size: " << frameSizeInPix[0] << "x" << frameSizeInPix[1]
      << ", pixel type: " << vtkImageScalarTypeNameMacro(aSource->GetPixelType())
      << ", buffer image orientation: " << igsioCommon::GetStringFromUsImageOrientation(aSource->GetInputImageOrientation()));
    this->UncompressedVideoFrame.SetImageType(aSource->GetImageType());
    this->UncompressedVideoFrame.SetImageOrientation(aSource->GetInputImageOrientation());
  }

  PlusStatus decodingStatus = PLUS_FAIL;
  if (aSource->GetImageType() == US_IMG_RGB_COLOR)
  {
    this->UncompressedVideoFrame.AllocateFrame(frameSizeInPix, VTK_UNSIGNED_CHAR, 3);
    decodingStatus = PixelCodec::ConvertToBGR24(componentOrdering, encoding, frameSizeInPix[0], frameSizeInPix[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
  }
  else
  {
    this->UncompressedVideoFrame.AllocateFrame(frameSizeInPix, VTK_UNSIGNED_CHAR, 1);
    decodingStatus = PixelCodec::ConvertToGray(encoding, frameSizeInPix[0], frameSizeInPix[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
  }
  if (decodingStatus != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while decoding the grabbed image");
    return PLUS_FAIL;
  }

  // Add the frame to the stream buffer
  PlusStatus status = aSource->AddItem(&this->UncompressedVideoFrame, this->FrameNumber);
  this->Modified();

  return status;
}


/*********** PARAMETERS *************/

#define IMAGING_PARAMETER_SET(parameterName) \
PlusStatus vtkPlusTelemedVideoSource::Set##parameterName(double a##parameterName) \
{ \
  LOG_INFO("Setting US parameter "<<#parameterName<<"="<<a##parameterName); \
  if (this->Device==NULL) \
  { \
    /* Connection has not been established yet. Parameter value will be set upon connection. */ \
    this->parameterName=a##parameterName; \
    return PLUS_SUCCESS; \
  } \
  int oldParamValue = this->parameterName; \
  this->parameterName=a##parameterName; \
  if (this->Device->Set##parameterName(this->parameterName)!=PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: "<<#parameterName<<"="<<a##parameterName); \
    this->parameterName=oldParamValue; \
    return PLUS_FAIL; \
  } \
  return PLUS_SUCCESS; \
}

#define IMAGING_PARAMETER_GET(parameterName) \
PlusStatus vtkPlusTelemedVideoSource::Get##parameterName(double &a##parameterName) \
{ \
  if (this->Device==NULL) \
  { \
    /* Connection has not been established yet. Return cached parameter value. */ \
    a##parameterName=this->parameterName; \
    return PLUS_SUCCESS; \
  } \
  if (this->Device->Get##parameterName(this->parameterName)!=PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: "<<parameterName<<"="<<a##parameterName); \
    return PLUS_FAIL; \
  } \
  a##parameterName=this->parameterName; \
  return PLUS_SUCCESS; \
}

IMAGING_PARAMETER_GET(FrequencyMhz);
IMAGING_PARAMETER_GET(DepthMm);
IMAGING_PARAMETER_GET(GainPercent);
IMAGING_PARAMETER_GET(DynRangeDb);
IMAGING_PARAMETER_GET(PowerDb);
IMAGING_PARAMETER_GET(FocusDepthPercent);

IMAGING_PARAMETER_SET(FrequencyMhz);
IMAGING_PARAMETER_SET(DepthMm);
IMAGING_PARAMETER_SET(GainPercent);
IMAGING_PARAMETER_SET(DynRangeDb);
IMAGING_PARAMETER_SET(PowerDb);
IMAGING_PARAMETER_SET(FocusDepthPercent);

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::SetSpeckleReductionEnabled(bool aEnabled)
{
  LOG_INFO("Setting US parameter SpeckleReductionEnabled=" << (aEnabled ? "TRUE" : "FALSE"));
  this->ImagingParameters->SetValue<bool>(KEY_SPECKLE_REDUCTION_ENABLED, aEnabled);
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->SpeckleReductionEnabled = aEnabled;
    return PLUS_SUCCESS;
  }
  bool oldEnabled = this->SpeckleReductionEnabled;
  this->SpeckleReductionEnabled = aEnabled;
  if (this->Device->SetSpeckleReductionEnabled(aEnabled) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: SpeckleReductionEnabled=" << (aEnabled ? "TRUE" : "FALSE"));
    this->SpeckleReductionEnabled = oldEnabled;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::GetSpeckleReductionEnabled(bool& aEnabled)
{
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Return cached parameter value.
    aEnabled = this->SpeckleReductionEnabled;
    return PLUS_SUCCESS;
  }
  if (this->Device->GetSpeckleReductionEnabled(this->SpeckleReductionEnabled) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter retrieval failed: SpeckleReductionEnabled");
    return PLUS_FAIL;
  }
  aEnabled = this->SpeckleReductionEnabled;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::SetSpeckleReductionMethod(int aMethod)
{
  if (aMethod < 0)
  {
    // Negative value means that the parameter is not specified; keep the current device setting
    return PLUS_SUCCESS;
  }
  LOG_INFO("Setting US parameter SpeckleReductionMethod=" << aMethod);
  std::string speckleReductionMethodAsEnum;
  if (SpeckleReductionMethodToEnumString(aMethod, speckleReductionMethodAsEnum))
  {
    this->ImagingParameters->SetValue<std::string>(KEY_SPECKLE_REDUCTION_METHOD, speckleReductionMethodAsEnum);
  }
  else
  {
    this->ImagingParameters->SetValue<int>(KEY_SPECKLE_REDUCTION_METHOD, aMethod);
  }
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->SpeckleReductionMethod = aMethod;
    return PLUS_SUCCESS;
  }
  int oldMethod = this->SpeckleReductionMethod;
  this->SpeckleReductionMethod = aMethod;
  if (this->Device->SetSpeckleReductionMethod(aMethod) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: SpeckleReductionMethod=" << aMethod);
    this->SpeckleReductionMethod = oldMethod;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::GetSpeckleReductionMethod(int& aMethod)
{
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Return cached parameter value.
    aMethod = this->SpeckleReductionMethod;
    return PLUS_SUCCESS;
  }
  if (this->Device->GetSpeckleReductionMethod(this->SpeckleReductionMethod) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter retrieval failed: SpeckleReductionMethod");
    return PLUS_FAIL;
  }
  aMethod = this->SpeckleReductionMethod;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusTelemedVideoSource::IsKnownKey(const std::string& queryKey) const
{
  if (igsioCommon::IsEqualInsensitive(queryKey, KEY_SPECKLE_REDUCTION_ENABLED) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_SPECKLE_REDUCTION_METHOD))
  {
    return true;
  }
  return Superclass::IsKnownKey(queryKey);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusTelemedVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusTelemedVideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusTelemedVideoSource::GetSdkVersion()
{
  std::ostringstream versionString;
  versionString << "Telemed version unknown" << std::ends;
  return versionString.str();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::SetFrameSize(const FrameSizeType& frameSize)
{
  this->FrameSize = frameSize;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::InternalApplyImagingParameterChange()
{
  PlusStatus status = PLUS_SUCCESS;

  // FREQUENCY
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FREQUENCY)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_FREQUENCY))
  {
    if (this->SetFrequencyMhz(this->ImagingParameters->GetFrequencyMhz()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set frequency imaging parameter");
      status = PLUS_FAIL;
    }
  }

  // DEPTH
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DEPTH)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DEPTH))
  {
    if (this->SetDepthMm(this->ImagingParameters->GetDepthMm()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set depth imaging parameter");
      status = PLUS_FAIL;
    }
  }

  // GAIN
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_GAIN)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_GAIN))
  {
    if (this->SetGainPercent(this->ImagingParameters->GetGainPercent()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set gain imaging parameter");
      status = PLUS_FAIL;
    }
  }

  // DYNAMIC RANGE
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DYNRANGE)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DYNRANGE))
  {
    if (this->SetDynRangeDb(this->ImagingParameters->GetDynRangeDb()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set dynamic range imaging parameter");
      status = PLUS_FAIL;
    }
  }

  // POWER
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_POWER)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_POWER))
  {
    if (this->SetPowerDb(this->ImagingParameters->GetPowerDb()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set power imaging parameter");
      status = PLUS_FAIL;
    }
  }

  // FOCUS DEPTH
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FOCUS_DEPTH)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_FOCUS_DEPTH))
  {
    if (this->SetFocusDepthPercent(this->ImagingParameters->GetFocusDepthPercent()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set focus depth percent imaging parameter");
      status = PLUS_FAIL;
    }
  }

  // SPECKLE REDUCTION (ClearView) METHOD
  // Set the method before the enabled state so that the requested filter is active when the filtering is turned on
  if (this->ImagingParameters->IsSet(KEY_SPECKLE_REDUCTION_METHOD)
    && this->ImagingParameters->IsPending(KEY_SPECKLE_REDUCTION_METHOD))
  {
    std::string speckleReductionMethodStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_SPECKLE_REDUCTION_METHOD, speckleReductionMethodStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read speckle reduction method imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int speckleReductionMethod = 0;
      if (!SpeckleReductionMethodFromString(speckleReductionMethodStr, speckleReductionMethod))
      {
        LOG_ERROR("Failed to parse speckle reduction method '" << speckleReductionMethodStr << "'. Use an integer value or CLEAR_VIEW enum name (for example SRF_NVC6).");
        status = PLUS_FAIL;
      }
      else if (this->SetSpeckleReductionMethod(speckleReductionMethod) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_SPECKLE_REDUCTION_METHOD, false);
      }
      else
      {
        LOG_ERROR("Failed to set speckle reduction method imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  // SPECKLE REDUCTION (ClearView) ENABLED
  if (this->ImagingParameters->IsSet(KEY_SPECKLE_REDUCTION_ENABLED)
    && this->ImagingParameters->IsPending(KEY_SPECKLE_REDUCTION_ENABLED))
  {
    std::string speckleReductionEnabledStr;
    this->ImagingParameters->GetValue<std::string>(KEY_SPECKLE_REDUCTION_ENABLED, speckleReductionEnabledStr);
    bool speckleReductionEnabled = igsioCommon::IsEqualInsensitive(speckleReductionEnabledStr, "TRUE") || speckleReductionEnabledStr == "1";
    if (this->SetSpeckleReductionEnabled(speckleReductionEnabled) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetPending(KEY_SPECKLE_REDUCTION_ENABLED, false);
    }
    else
    {
      LOG_ERROR("Failed to set speckle reduction enabled imaging parameter");
      status = PLUS_FAIL;
    }
  }

  return status;
}

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
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <sstream>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusTelemedVideoSource); // Corresponds to the implementation of : static vtkPlusTelemedVideoSource *New();  (in .h file)

const char* vtkPlusTelemedVideoSource::KEY_SPECKLE_REDUCTION_ENABLED = "SpeckleReductionEnabled";
const char* vtkPlusTelemedVideoSource::KEY_SPECKLE_REDUCTION_METHOD = "SpeckleReductionMethod";
const char* vtkPlusTelemedVideoSource::KEY_DYNAMIC_FOCUS_ENABLED = "DynamicFocusEnabled";
const char* vtkPlusTelemedVideoSource::KEY_FOCUSES_NUMBER = "FocusesNumber";
const char* vtkPlusTelemedVideoSource::KEY_FOCUS_SET = "FocusSet";
const char* vtkPlusTelemedVideoSource::KEY_FRAME_AVERAGING = "FrameAveraging";
const char* vtkPlusTelemedVideoSource::KEY_VIEW_AREA_PERCENT = "ViewAreaPercent";
const char* vtkPlusTelemedVideoSource::KEY_LINE_DENSITY = "LineDensity";
const char* vtkPlusTelemedVideoSource::KEY_IMAGE_ENHANCEMENT_ENABLED = "ImageEnhancementEnabled";
const char* vtkPlusTelemedVideoSource::KEY_IMAGE_ENHANCEMENT_METHOD = "ImageEnhancementMethod";
const char* vtkPlusTelemedVideoSource::KEY_REJECTION = "Rejection";
const char* vtkPlusTelemedVideoSource::KEY_NEGATIVE = "Negative";
const char* vtkPlusTelemedVideoSource::KEY_CHANGE_SCAN_DIRECTION = "ChangeScanDirection";
const char* vtkPlusTelemedVideoSource::KEY_ROTATE_IMAGE = "RotateImage";

namespace
{
bool SpeckleReductionMethodFromXmlString(const std::string& text, int& outMethod);
bool ImageEnhancementMethodFromXmlString(const std::string& text, int& outMethod);

//----------------------------------------------------------------------------
bool TryParseBool(const std::string& text, bool& value)
{
  if (igsioCommon::IsEqualInsensitive(text, "TRUE") || text == "1")
  {
    value = true;
    return true;
  }
  if (igsioCommon::IsEqualInsensitive(text, "FALSE") || text == "0")
  {
    value = false;
    return true;
  }
  return false;
}

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
bool ImageEnhancementMethodToEnumString(int method, std::string& outEnumName)
{
  switch (method)
  {
    case IMAGE_ENHANC_SHARPEN:
      outEnumName = "IMAGE_ENHANC_SHARPEN";
      return true;
    case IMAGE_ENHANC_SHARPENMORE:
      outEnumName = "IMAGE_ENHANC_SHARPENMORE";
      return true;
    case IMAGE_ENHANC_SMOOTH:
      outEnumName = "IMAGE_ENHANC_SMOOTH";
      return true;
    case IMAGE_ENHANC_SMOOTHMORE:
      outEnumName = "IMAGE_ENHANC_SMOOTHMORE";
      return true;
    default:
      return false;
  }
}

//----------------------------------------------------------------------------
bool ImageEnhancementMethodFromString(const std::string& text, int& outMethod)
{
  if (TryParseInteger(text, outMethod))
  {
    return true;
  }

  return ImageEnhancementMethodFromXmlString(text, outMethod);
}

//----------------------------------------------------------------------------
bool ImageEnhancementMethodFromXmlString(const std::string& text, int& outMethod)
{
  if (igsioCommon::IsEqualInsensitive(text, "IMAGE_ENHANC_SHARPEN"))
  {
    outMethod = IMAGE_ENHANC_SHARPEN;
    return true;
  }
  if (igsioCommon::IsEqualInsensitive(text, "IMAGE_ENHANC_SHARPENMORE"))
  {
    outMethod = IMAGE_ENHANC_SHARPENMORE;
    return true;
  }
  if (igsioCommon::IsEqualInsensitive(text, "IMAGE_ENHANC_SMOOTH"))
  {
    outMethod = IMAGE_ENHANC_SMOOTH;
    return true;
  }
  if (igsioCommon::IsEqualInsensitive(text, "IMAGE_ENHANC_SMOOTHMORE"))
  {
    outMethod = IMAGE_ENHANC_SMOOTHMORE;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool SpeckleReductionMethodFromXmlString(const std::string& text, int& outMethod)
{
  std::string enumName = text;
  std::transform(enumName.begin(), enumName.end(), enumName.begin(), ::toupper);
  struct FamilyInfo
  {
    const char* Prefix;
    int BaseValue;
  };

  // Longer, more specific prefixes (SRF_NVC, SRF_NVO) must be checked before the
  // shorter SRF_NV prefix they would otherwise be mistakenly matched against.
  static const FamilyInfo families[] =
  {
    { "SRF_CV", 0 },
    { "SRF_PV", 100 },
    { "SRF_NVC", 300 },
    { "SRF_QV", 400 },
    { "SRF_NVO", 500 },
    { "SRF_NV", 200 }
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
  , TimeGainCompensation(3, -1.0)
  , SpeckleReductionEnabled(-1)
  , SpeckleReductionMethod(-1)
  , DynamicFocusEnabled(-1)
  , FocusesNumber(-1)
  , FocusSet(-1)
  , FrameAveraging(-1)
  , ViewAreaPercent(-1)
  , LineDensity(-1)
  , ImageEnhancementEnabled(-1)
  , ImageEnhancementMethod(-1)
  , Rejection(-1)
  , Negative(-1)
  , ChangeScanDirection(-1)
  , RotateImage(-1)
  , ConnectedToDevice(false)
{
  this->FrameSize[0] = 512;
  this->FrameSize[1] = 512;
  this->FrameSize[2] = 1; // just in case if the frame size is passed to a method that expects a 3D frame size

  this->Device = NULL;
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;

  // Register the device-specific keys up front so that IsSet/IsPending can be safely queried on
  // them (e.g. by InternalApplyImagingParameterChange, which the base class may call before this
  // device has had a chance to populate any of its custom parameters) without logging spurious
  // "Invalid key request" errors for keys that simply haven't been set yet.
  this->ImagingParameters->DeclareParameter(KEY_SPECKLE_REDUCTION_ENABLED);
  this->ImagingParameters->DeclareParameter(KEY_SPECKLE_REDUCTION_METHOD);
  this->ImagingParameters->DeclareParameter(KEY_DYNAMIC_FOCUS_ENABLED);
  this->ImagingParameters->DeclareParameter(KEY_FOCUSES_NUMBER);
  this->ImagingParameters->DeclareParameter(KEY_FOCUS_SET);
  this->ImagingParameters->DeclareParameter(KEY_FRAME_AVERAGING);
  this->ImagingParameters->DeclareParameter(KEY_VIEW_AREA_PERCENT);
  this->ImagingParameters->DeclareParameter(KEY_LINE_DENSITY);
  this->ImagingParameters->DeclareParameter(KEY_IMAGE_ENHANCEMENT_ENABLED);
  this->ImagingParameters->DeclareParameter(KEY_IMAGE_ENHANCEMENT_METHOD);
  this->ImagingParameters->DeclareParameter(KEY_REJECTION);
  this->ImagingParameters->DeclareParameter(KEY_NEGATIVE);
  this->ImagingParameters->DeclareParameter(KEY_CHANGE_SCAN_DIRECTION);
  this->ImagingParameters->DeclareParameter(KEY_ROTATE_IMAGE);
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

  const char* tgcAttr = deviceConfig->GetAttribute("TimeGainCompensation");
  if (tgcAttr != NULL)
  {
    std::stringstream ss(tgcAttr);
    std::vector<double> parsedTgc((std::istream_iterator<double>(ss)), std::istream_iterator<double>());
    if (parsedTgc.size() >= 2)
    {
      this->TimeGainCompensation = parsedTgc;
    }
    else
    {
      LOG_ERROR("Unable to parse TimeGainCompensation='" << tgcAttr << "'. Provide at least 2 space-separated gain values.");
    }
  }

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

  std::string intAttr;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(DynamicFocusEnabled, intAttr, deviceConfig);
  if (!intAttr.empty())
  {
    bool parsed = false;
    bool boolValue = false;
    if (TryParseBool(intAttr, boolValue))
    {
      this->DynamicFocusEnabled = boolValue ? 1 : 0;
      parsed = true;
    }
    if (!parsed)
    {
      int value = -1;
      if (TryParseInteger(intAttr, value))
      {
        this->DynamicFocusEnabled = (value != 0) ? 1 : 0;
        parsed = true;
      }
    }
    if (!parsed)
    {
      LOG_ERROR("Unable to parse DynamicFocusEnabled='" << intAttr << "'. Use TRUE/FALSE or 0/1.");
    }
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(FocusesNumber, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->FocusesNumber))
  {
    LOG_ERROR("Unable to parse FocusesNumber='" << intAttr << "'. Use an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(FocusSet, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->FocusSet))
  {
    LOG_ERROR("Unable to parse FocusSet='" << intAttr << "'. Use an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(FrameAveraging, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->FrameAveraging))
  {
    LOG_ERROR("Unable to parse FrameAveraging='" << intAttr << "'. Use an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(ViewAreaPercent, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->ViewAreaPercent))
  {
    LOG_ERROR("Unable to parse ViewAreaPercent='" << intAttr << "'. Use an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(LineDensity, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->LineDensity))
  {
    LOG_ERROR("Unable to parse LineDensity='" << intAttr << "'. Use an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(ImageEnhancementEnabled, intAttr, deviceConfig);
  if (!intAttr.empty())
  {
    bool parsed = false;
    bool boolValue = false;
    if (TryParseBool(intAttr, boolValue))
    {
      this->ImageEnhancementEnabled = boolValue ? 1 : 0;
      parsed = true;
    }
    if (!parsed)
    {
      int value = -1;
      if (TryParseInteger(intAttr, value))
      {
        this->ImageEnhancementEnabled = (value != 0) ? 1 : 0;
        parsed = true;
      }
    }
    if (!parsed)
    {
      LOG_ERROR("Unable to parse ImageEnhancementEnabled='" << intAttr << "'. Use TRUE/FALSE or 0/1.");
    }
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(ImageEnhancementMethod, intAttr, deviceConfig);
  if (!intAttr.empty() && !ImageEnhancementMethodFromString(intAttr, this->ImageEnhancementMethod))
  {
    LOG_ERROR("Unable to parse ImageEnhancementMethod='" << intAttr << "'. Use IMAGE_ENHANC_SHARPEN, IMAGE_ENHANC_SHARPENMORE, IMAGE_ENHANC_SMOOTH, IMAGE_ENHANC_SMOOTHMORE, or an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(Rejection, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->Rejection))
  {
    LOG_ERROR("Unable to parse Rejection='" << intAttr << "'. Use an integer value.");
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(Negative, intAttr, deviceConfig);
  if (!intAttr.empty())
  {
    bool parsed = false;
    bool boolValue = false;
    if (TryParseBool(intAttr, boolValue))
    {
      this->Negative = boolValue ? 1 : 0;
      parsed = true;
    }
    if (!parsed)
    {
      int value = -1;
      if (TryParseInteger(intAttr, value))
      {
        this->Negative = (value != 0) ? 1 : 0;
        parsed = true;
      }
    }
    if (!parsed)
    {
      LOG_ERROR("Unable to parse Negative='" << intAttr << "'. Use TRUE/FALSE or 0/1.");
    }
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(ChangeScanDirection, intAttr, deviceConfig);
  if (!intAttr.empty())
  {
    bool parsed = false;
    bool boolValue = false;
    if (TryParseBool(intAttr, boolValue))
    {
      this->ChangeScanDirection = boolValue ? 1 : 0;
      parsed = true;
    }
    if (!parsed)
    {
      int value = -1;
      if (TryParseInteger(intAttr, value))
      {
        this->ChangeScanDirection = (value != 0) ? 1 : 0;
        parsed = true;
      }
    }
    if (!parsed)
    {
      LOG_ERROR("Unable to parse ChangeScanDirection='" << intAttr << "'. Use TRUE/FALSE or 0/1.");
    }
  }

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(RotateImage, intAttr, deviceConfig);
  if (!intAttr.empty() && !TryParseInteger(intAttr, this->RotateImage))
  {
    LOG_ERROR("Unable to parse RotateImage='" << intAttr << "'. Use 0/90/180/270 (or enum 0..3).");
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
  std::stringstream tgcStream;
  std::copy(this->TimeGainCompensation.begin(), this->TimeGainCompensation.end(), std::ostream_iterator<double>(tgcStream, " "));
  deviceConfig->SetAttribute("TimeGainCompensation", tgcStream.str().c_str());
  if (this->SpeckleReductionEnabled >= 0)
  {
    deviceConfig->SetAttribute("SpeckleReductionEnabled", this->SpeckleReductionEnabled != 0 ? "TRUE" : "FALSE");
  }
  std::string speckleReductionMethodAsEnum;
  if (SpeckleReductionMethodToEnumString(this->SpeckleReductionMethod, speckleReductionMethodAsEnum))
  {
    deviceConfig->SetAttribute("SpeckleReductionMethod", speckleReductionMethodAsEnum.c_str());
  }
  else
  {
    deviceConfig->SetIntAttribute("SpeckleReductionMethod", this->SpeckleReductionMethod);
  }
  deviceConfig->SetIntAttribute("DynamicFocusEnabled", this->DynamicFocusEnabled);
  deviceConfig->SetIntAttribute("FocusesNumber", this->FocusesNumber);
  deviceConfig->SetIntAttribute("FocusSet", this->FocusSet);
  deviceConfig->SetIntAttribute("FrameAveraging", this->FrameAveraging);
  deviceConfig->SetIntAttribute("ViewAreaPercent", this->ViewAreaPercent);
  deviceConfig->SetIntAttribute("LineDensity", this->LineDensity);
  deviceConfig->SetIntAttribute("ImageEnhancementEnabled", this->ImageEnhancementEnabled);
  std::string imageEnhancementMethodAsEnum;
  if (ImageEnhancementMethodToEnumString(this->ImageEnhancementMethod, imageEnhancementMethodAsEnum))
  {
    deviceConfig->SetAttribute("ImageEnhancementMethod", imageEnhancementMethodAsEnum.c_str());
  }
  else
  {
    deviceConfig->SetIntAttribute("ImageEnhancementMethod", this->ImageEnhancementMethod);
  }
  deviceConfig->SetIntAttribute("Rejection", this->Rejection);
  deviceConfig->SetIntAttribute("Negative", this->Negative);
  deviceConfig->SetIntAttribute("ChangeScanDirection", this->ChangeScanDirection);
  deviceConfig->SetIntAttribute("RotateImage", this->RotateImage);
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
  if (this->TimeGainCompensation.size() >= 2 &&
      std::all_of(this->TimeGainCompensation.begin(), this->TimeGainCompensation.end(), [](double v) { return v >= 0; }))
  {
    this->ImagingParameters->SetTimeGainCompensation(this->TimeGainCompensation);
  }
  if (this->SpeckleReductionEnabled >= 0)
  {
    this->ImagingParameters->SetValue<bool>(KEY_SPECKLE_REDUCTION_ENABLED, this->SpeckleReductionEnabled != 0);
  }
  if (this->SpeckleReductionMethod >= 0)
  {
    std::string speckleReductionMethodAsEnum;
    if (SpeckleReductionMethodToEnumString(this->SpeckleReductionMethod, speckleReductionMethodAsEnum))
    {
      this->ImagingParameters->SetValue<std::string>(KEY_SPECKLE_REDUCTION_METHOD, speckleReductionMethodAsEnum);
    }
    else
    {
      this->ImagingParameters->SetValue<int>(KEY_SPECKLE_REDUCTION_METHOD, this->SpeckleReductionMethod);
    }
  }
  if (this->DynamicFocusEnabled >= 0)
  {
    this->ImagingParameters->SetValue<bool>(KEY_DYNAMIC_FOCUS_ENABLED, this->DynamicFocusEnabled != 0);
  }
  if (this->FocusesNumber >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_FOCUSES_NUMBER, this->FocusesNumber);
  }
  if (this->FocusSet >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_FOCUS_SET, this->FocusSet);
  }
  if (this->FrameAveraging >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_FRAME_AVERAGING, this->FrameAveraging);
  }
  if (this->ViewAreaPercent >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_VIEW_AREA_PERCENT, this->ViewAreaPercent);
  }
  if (this->LineDensity >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_LINE_DENSITY, this->LineDensity);
  }
  if (this->ImageEnhancementEnabled >= 0)
  {
    this->ImagingParameters->SetValue<bool>(KEY_IMAGE_ENHANCEMENT_ENABLED, this->ImageEnhancementEnabled != 0);
  }
  if (this->ImageEnhancementMethod >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_IMAGE_ENHANCEMENT_METHOD, this->ImageEnhancementMethod);
  }
  if (this->Rejection >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_REJECTION, this->Rejection);
  }
  if (this->Negative >= 0)
  {
    this->ImagingParameters->SetValue<bool>(KEY_NEGATIVE, this->Negative != 0);
  }
  if (this->ChangeScanDirection >= 0)
  {
    this->ImagingParameters->SetValue<bool>(KEY_CHANGE_SCAN_DIRECTION, this->ChangeScanDirection != 0);
  }
  if (this->RotateImage >= 0)
  {
    this->ImagingParameters->SetValue<int>(KEY_ROTATE_IMAGE, this->RotateImage);
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
  if (!this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_TGC))
  {
    std::vector<double> deviceTimeGainCompensation;
    if (this->GetTimeGainCompensation(deviceTimeGainCompensation) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetTimeGainCompensation(deviceTimeGainCompensation);
    }
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
  if (!this->ImagingParameters->IsSet(KEY_DYNAMIC_FOCUS_ENABLED))
  {
    bool value = false;
    if (this->GetDynamicFocusEnabled(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<bool>(KEY_DYNAMIC_FOCUS_ENABLED, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_FOCUSES_NUMBER))
  {
    int value = 0;
    if (this->GetFocusesNumber(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_FOCUSES_NUMBER, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_FOCUS_SET))
  {
    int value = 0;
    if (this->GetFocusSet(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_FOCUS_SET, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_FRAME_AVERAGING))
  {
    int value = 0;
    if (this->GetFrameAveraging(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_FRAME_AVERAGING, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_VIEW_AREA_PERCENT))
  {
    int value = 0;
    if (this->GetViewAreaPercent(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_VIEW_AREA_PERCENT, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_LINE_DENSITY))
  {
    int value = 0;
    if (this->GetLineDensity(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_LINE_DENSITY, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_IMAGE_ENHANCEMENT_ENABLED))
  {
    bool value = false;
    if (this->GetImageEnhancementEnabled(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<bool>(KEY_IMAGE_ENHANCEMENT_ENABLED, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_IMAGE_ENHANCEMENT_METHOD))
  {
    int value = 0;
    if (this->GetImageEnhancementMethod(value) == PLUS_SUCCESS)
    {
      std::string imageEnhancementMethodAsEnum;
      if (ImageEnhancementMethodToEnumString(value, imageEnhancementMethodAsEnum))
      {
        this->ImagingParameters->SetValue<std::string>(KEY_IMAGE_ENHANCEMENT_METHOD, imageEnhancementMethodAsEnum);
      }
      else
      {
        this->ImagingParameters->SetValue<int>(KEY_IMAGE_ENHANCEMENT_METHOD, value);
      }
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_REJECTION))
  {
    int value = 0;
    if (this->GetRejection(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_REJECTION, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_NEGATIVE))
  {
    bool value = false;
    if (this->GetNegative(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<bool>(KEY_NEGATIVE, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_CHANGE_SCAN_DIRECTION))
  {
    bool value = false;
    if (this->GetChangeScanDirection(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<bool>(KEY_CHANGE_SCAN_DIRECTION, value);
    }
  }
  if (!this->ImagingParameters->IsSet(KEY_ROTATE_IMAGE))
  {
    int value = 0;
    if (this->GetRotateImage(value) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetValue<int>(KEY_ROTATE_IMAGE, value);
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
  double oldParamValue = this->parameterName; \
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
PlusStatus vtkPlusTelemedVideoSource::SetTimeGainCompensation(const std::vector<double>& aTimeGainCompensation)
{
  LOG_INFO("Setting US parameter TimeGainCompensation");
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->TimeGainCompensation = aTimeGainCompensation;
    return PLUS_SUCCESS;
  }
  std::vector<double> oldTimeGainCompensation = this->TimeGainCompensation;
  this->TimeGainCompensation = aTimeGainCompensation;
  if (this->Device->SetTimeGainCompensation(this->TimeGainCompensation) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: TimeGainCompensation");
    this->TimeGainCompensation = oldTimeGainCompensation;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::GetTimeGainCompensation(std::vector<double>& aTimeGainCompensation)
{
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Return cached parameter value.
    aTimeGainCompensation = this->TimeGainCompensation;
    return PLUS_SUCCESS;
  }
  if (this->Device->GetTimeGainCompensation(this->TimeGainCompensation) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter getting failed: TimeGainCompensation");
    return PLUS_FAIL;
  }
  aTimeGainCompensation = this->TimeGainCompensation;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::SetSpeckleReductionEnabled(bool aEnabled)
{
  LOG_INFO("Setting US parameter SpeckleReductionEnabled=" << (aEnabled ? "TRUE" : "FALSE"));
  this->ImagingParameters->SetValue<bool>(KEY_SPECKLE_REDUCTION_ENABLED, aEnabled);
  if (this->Device == NULL)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->SpeckleReductionEnabled = aEnabled ? 1 : 0;
    return PLUS_SUCCESS;
  }
  int oldEnabled = this->SpeckleReductionEnabled;
  this->SpeckleReductionEnabled = aEnabled ? 1 : 0;
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
    aEnabled = (this->SpeckleReductionEnabled != 0);
    return PLUS_SUCCESS;
  }
  bool deviceEnabled = false;
  if (this->Device->GetSpeckleReductionEnabled(deviceEnabled) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter retrieval failed: SpeckleReductionEnabled");
    return PLUS_FAIL;
  }
  this->SpeckleReductionEnabled = deviceEnabled ? 1 : 0;
  aEnabled = deviceEnabled;
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

#define IMAGING_PARAMETER_SET_INT(parameterName) \
PlusStatus vtkPlusTelemedVideoSource::Set##parameterName(int a##parameterName) \
{ \
  LOG_INFO("Setting US parameter " << #parameterName << "=" << a##parameterName); \
  if (this->Device == NULL) \
  { \
    this->parameterName = a##parameterName; \
    return PLUS_SUCCESS; \
  } \
  int oldParamValue = this->parameterName; \
  this->parameterName = a##parameterName; \
  if (this->Device->Set##parameterName(this->parameterName) != PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: " << #parameterName << "=" << a##parameterName); \
    this->parameterName = oldParamValue; \
    return PLUS_FAIL; \
  } \
  return PLUS_SUCCESS; \
}

#define IMAGING_PARAMETER_GET_INT(parameterName) \
PlusStatus vtkPlusTelemedVideoSource::Get##parameterName(int& a##parameterName) \
{ \
  if (this->Device == NULL) \
  { \
    a##parameterName = this->parameterName; \
    return PLUS_SUCCESS; \
  } \
  if (this->Device->Get##parameterName(this->parameterName) != PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkPlusTelemedVideoSource parameter retrieval failed: " << #parameterName); \
    return PLUS_FAIL; \
  } \
  a##parameterName = this->parameterName; \
  return PLUS_SUCCESS; \
}

#define IMAGING_PARAMETER_SET_BOOL(parameterName) \
PlusStatus vtkPlusTelemedVideoSource::Set##parameterName(bool a##parameterName) \
{ \
  LOG_INFO("Setting US parameter " << #parameterName << "=" << (a##parameterName ? "TRUE" : "FALSE")); \
  if (this->Device == NULL) \
  { \
    this->parameterName = a##parameterName ? 1 : 0; \
    return PLUS_SUCCESS; \
  } \
  int oldParamValue = this->parameterName; \
  this->parameterName = a##parameterName ? 1 : 0; \
  if (this->Device->Set##parameterName(a##parameterName) != PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: " << #parameterName << "=" << (a##parameterName ? "TRUE" : "FALSE")); \
    this->parameterName = oldParamValue; \
    return PLUS_FAIL; \
  } \
  return PLUS_SUCCESS; \
}

#define IMAGING_PARAMETER_GET_BOOL(parameterName) \
PlusStatus vtkPlusTelemedVideoSource::Get##parameterName(bool& a##parameterName) \
{ \
  if (this->Device == NULL) \
  { \
    a##parameterName = (this->parameterName != 0); \
    return PLUS_SUCCESS; \
  } \
  bool paramValue = false; \
  if (this->Device->Get##parameterName(paramValue) != PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkPlusTelemedVideoSource parameter retrieval failed: " << #parameterName); \
    return PLUS_FAIL; \
  } \
  this->parameterName = paramValue ? 1 : 0; \
  a##parameterName = paramValue; \
  return PLUS_SUCCESS; \
}

IMAGING_PARAMETER_SET_BOOL(DynamicFocusEnabled);
IMAGING_PARAMETER_GET_BOOL(DynamicFocusEnabled);
IMAGING_PARAMETER_SET_INT(FocusesNumber);
IMAGING_PARAMETER_GET_INT(FocusesNumber);
IMAGING_PARAMETER_SET_INT(FocusSet);
IMAGING_PARAMETER_GET_INT(FocusSet);
IMAGING_PARAMETER_SET_INT(FrameAveraging);
IMAGING_PARAMETER_GET_INT(FrameAveraging);
IMAGING_PARAMETER_SET_INT(ViewAreaPercent);
IMAGING_PARAMETER_GET_INT(ViewAreaPercent);
IMAGING_PARAMETER_SET_INT(LineDensity);
IMAGING_PARAMETER_GET_INT(LineDensity);
IMAGING_PARAMETER_SET_BOOL(ImageEnhancementEnabled);
IMAGING_PARAMETER_GET_BOOL(ImageEnhancementEnabled);
IMAGING_PARAMETER_SET_INT(ImageEnhancementMethod);
IMAGING_PARAMETER_GET_INT(ImageEnhancementMethod);
IMAGING_PARAMETER_SET_INT(Rejection);
IMAGING_PARAMETER_GET_INT(Rejection);
IMAGING_PARAMETER_SET_BOOL(Negative);
IMAGING_PARAMETER_GET_BOOL(Negative);
IMAGING_PARAMETER_SET_INT(RotateImage);
IMAGING_PARAMETER_GET_INT(RotateImage);

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::SetChangeScanDirection(bool aEnabled)
{
  LOG_INFO("Setting US parameter ChangeScanDirection=" << (aEnabled ? "TRUE" : "FALSE"));
  if (this->Device == NULL)
  {
    this->ChangeScanDirection = aEnabled ? 1 : 0;
    return PLUS_SUCCESS;
  }
  int oldValue = this->ChangeScanDirection;
  this->ChangeScanDirection = aEnabled ? 1 : 0;
  if (this->Device->SetScanDirection(aEnabled) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter setting failed: ChangeScanDirection=" << (aEnabled ? "TRUE" : "FALSE"));
    this->ChangeScanDirection = oldValue;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTelemedVideoSource::GetChangeScanDirection(bool& aEnabled)
{
  if (this->Device == NULL)
  {
    aEnabled = (this->ChangeScanDirection != 0);
    return PLUS_SUCCESS;
  }
  bool current = false;
  if (this->Device->GetScanDirection(current) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusTelemedVideoSource parameter retrieval failed: ChangeScanDirection");
    return PLUS_FAIL;
  }
  this->ChangeScanDirection = current ? 1 : 0;
  aEnabled = current;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusTelemedVideoSource::IsKnownKey(const std::string& queryKey) const
{
  if (igsioCommon::IsEqualInsensitive(queryKey, KEY_SPECKLE_REDUCTION_ENABLED) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_SPECKLE_REDUCTION_METHOD) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_DYNAMIC_FOCUS_ENABLED) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_FOCUSES_NUMBER) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_FOCUS_SET) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_FRAME_AVERAGING) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_VIEW_AREA_PERCENT) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_LINE_DENSITY) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_IMAGE_ENHANCEMENT_ENABLED) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_IMAGE_ENHANCEMENT_METHOD) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_REJECTION) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_NEGATIVE) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_CHANGE_SCAN_DIRECTION) ||
      igsioCommon::IsEqualInsensitive(queryKey, KEY_ROTATE_IMAGE))
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

  // TIME GAIN COMPENSATION
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_TGC)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_TGC))
  {
    if (this->SetTimeGainCompensation(this->ImagingParameters->GetTimeGainCompensation()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set time gain compensation imaging parameter");
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

  if (this->ImagingParameters->IsSet(KEY_DYNAMIC_FOCUS_ENABLED)
    && this->ImagingParameters->IsPending(KEY_DYNAMIC_FOCUS_ENABLED))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_DYNAMIC_FOCUS_ENABLED, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read dynamic focus enabled imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      bool value = false;
      if (!TryParseBool(valueStr, value))
      {
        LOG_ERROR("Failed to parse DynamicFocusEnabled='" << valueStr << "'. Use TRUE/FALSE or 0/1.");
        status = PLUS_FAIL;
      }
      else if (this->SetDynamicFocusEnabled(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_DYNAMIC_FOCUS_ENABLED, false);
      }
      else
      {
        LOG_ERROR("Failed to set dynamic focus enabled imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_FOCUSES_NUMBER)
    && this->ImagingParameters->IsPending(KEY_FOCUSES_NUMBER))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_FOCUSES_NUMBER, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read focuses number imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse FocusesNumber='" << valueStr << "'. Use an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetFocusesNumber(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_FOCUSES_NUMBER, false);
      }
      else
      {
        LOG_ERROR("Failed to set focuses number imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_FOCUS_SET)
    && this->ImagingParameters->IsPending(KEY_FOCUS_SET))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_FOCUS_SET, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read focus set imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse FocusSet='" << valueStr << "'. Use an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetFocusSet(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_FOCUS_SET, false);
      }
      else
      {
        LOG_ERROR("Failed to set focus set imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_FRAME_AVERAGING)
    && this->ImagingParameters->IsPending(KEY_FRAME_AVERAGING))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_FRAME_AVERAGING, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read frame averaging imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse FrameAveraging='" << valueStr << "'. Use an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetFrameAveraging(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_FRAME_AVERAGING, false);
      }
      else
      {
        LOG_ERROR("Failed to set frame averaging imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_VIEW_AREA_PERCENT)
    && this->ImagingParameters->IsPending(KEY_VIEW_AREA_PERCENT))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_VIEW_AREA_PERCENT, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read view area imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse ViewAreaPercent='" << valueStr << "'. Use an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetViewAreaPercent(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_VIEW_AREA_PERCENT, false);
      }
      else
      {
        LOG_ERROR("Failed to set view area imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_LINE_DENSITY)
    && this->ImagingParameters->IsPending(KEY_LINE_DENSITY))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_LINE_DENSITY, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read line density imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse LineDensity='" << valueStr << "'. Use an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetLineDensity(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_LINE_DENSITY, false);
      }
      else
      {
        LOG_ERROR("Failed to set line density imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_IMAGE_ENHANCEMENT_ENABLED)
    && this->ImagingParameters->IsPending(KEY_IMAGE_ENHANCEMENT_ENABLED))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_IMAGE_ENHANCEMENT_ENABLED, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read image enhancement enabled imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      bool value = false;
      if (!TryParseBool(valueStr, value))
      {
        LOG_ERROR("Failed to parse ImageEnhancementEnabled='" << valueStr << "'. Use TRUE/FALSE or 0/1.");
        status = PLUS_FAIL;
      }
      else if (this->SetImageEnhancementEnabled(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_IMAGE_ENHANCEMENT_ENABLED, false);
      }
      else
      {
        LOG_ERROR("Failed to set image enhancement enabled imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_IMAGE_ENHANCEMENT_METHOD)
    && this->ImagingParameters->IsPending(KEY_IMAGE_ENHANCEMENT_METHOD))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_IMAGE_ENHANCEMENT_METHOD, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read image enhancement method imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!ImageEnhancementMethodFromString(valueStr, value))
      {
        LOG_ERROR("Failed to parse ImageEnhancementMethod='" << valueStr << "'. Use IMAGE_ENHANC_SHARPEN, IMAGE_ENHANC_SHARPENMORE, IMAGE_ENHANC_SMOOTH, IMAGE_ENHANC_SMOOTHMORE, or an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetImageEnhancementMethod(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_IMAGE_ENHANCEMENT_METHOD, false);
      }
      else
      {
        LOG_ERROR("Failed to set image enhancement method imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_REJECTION)
    && this->ImagingParameters->IsPending(KEY_REJECTION))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_REJECTION, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read rejection imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse Rejection='" << valueStr << "'. Use an integer value.");
        status = PLUS_FAIL;
      }
      else if (this->SetRejection(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_REJECTION, false);
      }
      else
      {
        LOG_ERROR("Failed to set rejection imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_NEGATIVE)
    && this->ImagingParameters->IsPending(KEY_NEGATIVE))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_NEGATIVE, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read negative imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      bool value = false;
      if (!TryParseBool(valueStr, value))
      {
        LOG_ERROR("Failed to parse Negative='" << valueStr << "'. Use TRUE/FALSE or 0/1.");
        status = PLUS_FAIL;
      }
      else if (this->SetNegative(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_NEGATIVE, false);
      }
      else
      {
        LOG_ERROR("Failed to set negative imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_CHANGE_SCAN_DIRECTION)
    && this->ImagingParameters->IsPending(KEY_CHANGE_SCAN_DIRECTION))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_CHANGE_SCAN_DIRECTION, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read change scan direction imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      bool value = false;
      if (!TryParseBool(valueStr, value))
      {
        LOG_ERROR("Failed to parse ChangeScanDirection='" << valueStr << "'. Use TRUE/FALSE or 0/1.");
        status = PLUS_FAIL;
      }
      else if (this->SetChangeScanDirection(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_CHANGE_SCAN_DIRECTION, false);
      }
      else
      {
        LOG_ERROR("Failed to set change scan direction imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  if (this->ImagingParameters->IsSet(KEY_ROTATE_IMAGE)
    && this->ImagingParameters->IsPending(KEY_ROTATE_IMAGE))
  {
    std::string valueStr;
    if (this->ImagingParameters->GetValue<std::string>(KEY_ROTATE_IMAGE, valueStr) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read rotate image imaging parameter value");
      status = PLUS_FAIL;
    }
    else
    {
      int value = 0;
      if (!TryParseInteger(valueStr, value))
      {
        LOG_ERROR("Failed to parse RotateImage='" << valueStr << "'. Use 0/90/180/270 (or enum 0..3).");
        status = PLUS_FAIL;
      }
      else if (this->SetRotateImage(value) == PLUS_SUCCESS)
      {
        this->ImagingParameters->SetPending(KEY_ROTATE_IMAGE, false);
      }
      else
      {
        LOG_ERROR("Failed to set rotate image imaging parameter");
        status = PLUS_FAIL;
      }
    }
  }

  return status;
}

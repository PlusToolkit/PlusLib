/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusCommon.h"
#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkVirtualTextRecognizer.h"

#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>
#include <allheaders.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkVirtualTextRecognizer);

//----------------------------------------------------------------------------

namespace
{
  static const double SAMPLING_SKIPPING_MARGIN_SEC = 0.1;
  static const double DELAY_ON_SENDING_ERROR_SEC = 0.02;
  static const char* PARAMETER_LIST_TAG_NAME = "TextFields";
  static const char* PARAMETER_TAG_NAME = "Field";
  static const char* PARAMETER_NAME_ATTRIBUTE = "Name";
  static const char* PARAMETER_CHANNEL_ATTRIBUTE = "Channel";
  static const char* PARAMETER_ORIGIN_ATTRIBUTE = "InputRegionOrigin";
  static const char* PARAMETER_SIZE_ATTRIBUTE = "InputRegionSize";
  static const int PARAMETER_DEPTH_BITS = 8;
  static const char* DEFAULT_LANGUAGE = "eng";
  static const int TEXT_RECOGNIZER_MISSING_INPUT_DEFAULT = 1;
}

//----------------------------------------------------------------------------
vtkVirtualTextRecognizer::vtkVirtualTextRecognizer()
  : vtkPlusDevice()
  , Language(NULL)
  , TrackedFrames(vtkTrackedFrameList::New())
{
  // The data capture thread will be used to regularly check the input devices and generate and update the output
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
}

//----------------------------------------------------------------------------
void vtkVirtualTextRecognizer::ClearConfiguration()
{
  for( ChannelFieldListMapIterator it = this->RecognitionFields.begin(); it != this->RecognitionFields.end(); ++it )
  {
    for( FieldListIterator fieldIt = it->second.begin(); fieldIt != it->second.end(); ++fieldIt )
    {
      TextFieldParameter* parameter = *fieldIt;
      delete parameter;
    }
    it->second.clear();
  }
  this->RecognitionFields.clear();
}

//----------------------------------------------------------------------------
vtkVirtualTextRecognizer::~vtkVirtualTextRecognizer()
{
  TrackedFrames->Delete();
  TrackedFrames = NULL;
}

//----------------------------------------------------------------------------
void vtkVirtualTextRecognizer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

#ifdef PLUS_TEST_tesseract
//----------------------------------------------------------------------------
vtkVirtualTextRecognizer::ChannelFieldListMap& vtkVirtualTextRecognizer::GetRecognitionFields()
{
  return this->RecognitionFields;
}
#endif

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::InternalUpdate()
{
  std::map<double, int> queriedFramesIndexes;
  std::vector<TrackedFrame*> queriedFrames;

  if( !this->HasGracePeriodExpired() )
  {
    return PLUS_SUCCESS;
  }

  for( ChannelFieldListMapIterator it = this->RecognitionFields.begin(); it != this->RecognitionFields.end(); ++it )
  {
    vtkPlusChannel* channel = it->first;

    for( FieldListIterator fieldIt = it->second.begin(); fieldIt != it->second.end(); ++fieldIt )
    {
      TextFieldParameter* parameter = *fieldIt;
      TrackedFrame frame;

      // Attempt to find the frame already retrieved
      PlusStatus result = FindOrQueryFrame(frame, queriedFramesIndexes, parameter, queriedFrames);

      if( result != PLUS_SUCCESS || frame.GetImageData()->GetImage() == NULL)
      {
        continue;
      }

      // We have a frame, let's parse it      
      vtkImageDataToPix(frame, parameter);

      this->TesseractAPI->SetImage(parameter->ReceivedFrame);
      char* text_out = this->TesseractAPI->GetUTF8Text();
      std::string textStr(text_out);
      parameter->LatestParameterValue = PlusCommon::Trim(textStr);
      delete [] text_out;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkVirtualTextRecognizer::vtkImageDataToPix(TrackedFrame& frame, TextFieldParameter* parameter)
{
  PlusVideoFrame::GetOrientedClippedImage(frame.GetImageData()->GetImage(), PlusVideoFrame::FlipInfoType(), 
    frame.GetImageData()->GetImageType(), parameter->ScreenRegion, parameter->Origin, parameter->Size);

  unsigned int *data = pixGetData(parameter->ReceivedFrame);
  int wpl = pixGetWpl(parameter->ReceivedFrame);
  int bpl = ( (8*parameter->Size[0]) + 7) / 8;
  unsigned int *line;
  unsigned char val8;

  int extents[6];
  parameter->ScreenRegion->GetExtent(extents);
  int ySize = extents[3]-extents[2];
  int xSize = extents[1]-extents[0];

  static int someVal = 0;
  int coords[3] = {0,0,0};
  for(int y = 0; y < ySize; y++)
  {
    coords[1] = ySize - y - 1 + extents[2];
    line = data + y * wpl;
    for(int x = 0; x < bpl; x++)
    {
      coords[0] = x + extents[0];
      val8 = (*(unsigned char*)parameter->ScreenRegion->GetScalarPointer(coords));
      SET_DATA_BYTE(line,x,val8);
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::FindOrQueryFrame(TrackedFrame& frame, std::map<double, int>& QueriedFramesIndexes, TextFieldParameter* parameter, std::vector<TrackedFrame*>& QueriedFrames)
{
  double mostRecent(-1);

  if ( !parameter->SourceChannel->GetVideoDataAvailable() )
  {
    LOG_WARNING("Processed data is not generated, as no video data is available yet. Device ID: " << this->GetDeviceId()); 
    return PLUS_FAIL;
  }

  if( parameter->SourceChannel->GetMostRecentTimestamp(mostRecent) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve most recent timestamp for parameter " << parameter->ParameterName);
    return PLUS_FAIL;
  }

  std::map<double, int>::iterator frameIt = QueriedFramesIndexes.find(mostRecent);
  if( frameIt == QueriedFramesIndexes.end() )
  {
    this->TrackedFrames->Clear();
    double aTimestamp(UNDEFINED_TIMESTAMP);
    if ( parameter->SourceChannel->GetTrackedFrameList(aTimestamp, this->TrackedFrames, 1) != PLUS_SUCCESS )
    {
      LOG_INFO("Failed to get tracked frame list from data collector."); 
      return PLUS_FAIL;
    }
    double timestamp = TrackedFrames->GetTrackedFrame(0)->GetTimestamp();

    // Copy the frame so it isn't lost when the tracked frame list is cleared
    frame = (*TrackedFrames->GetTrackedFrame(0));

    // Record the index of this timestamp
    QueriedFramesIndexes[timestamp] = QueriedFrames.size();
    QueriedFrames.push_back(&frame);
  }
  else
  {
    frame = (*QueriedFrames[frameIt->second]);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::InternalConnect()
{
  this->TesseractAPI = new tesseract::TessBaseAPI(); 
  this->TesseractAPI->Init(NULL, Language, tesseract::OEM_TESSERACT_CUBE_COMBINED);
  this->TesseractAPI->SetPageSegMode(tesseract::PSM_SINGLE_LINE);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::InternalDisconnect()
{
  delete this->TesseractAPI;
  this->TesseractAPI = NULL;

  ClearConfiguration();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::ReadConfiguration( vtkXMLDataElement* rootConfigElement)
{
  ClearConfiguration();

  Superclass::ReadConfiguration(rootConfigElement);

  if( this->MissingInputGracePeriodSec < TEXT_RECOGNIZER_MISSING_INPUT_DEFAULT )
  {
    LOG_WARNING("MissingInputGracePeriodSec must be set to a value > 1s to allow input to arrive and be processed.");
    this->MissingInputGracePeriodSec = TEXT_RECOGNIZER_MISSING_INPUT_DEFAULT;
  }

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  this->SetLanguage(DEFAULT_LANGUAGE);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(Language, deviceConfig);

  XML_FIND_NESTED_ELEMENT_OPTIONAL(screenFields, deviceConfig, PARAMETER_LIST_TAG_NAME);

  for( int i = 0; i < screenFields->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* fieldElement = screenFields->GetNestedElement(i);

    if( STRCASECMP(fieldElement->GetName(), PARAMETER_TAG_NAME) != 0 )
    {
      continue;
    }

    const char* channelName = fieldElement->GetAttribute(PARAMETER_CHANNEL_ATTRIBUTE);
    vtkPlusChannel* aChannel;
    if( channelName == NULL || this->GetDataCollector()->GetChannel(aChannel, channelName) != PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot build field scanner. Input " << PARAMETER_CHANNEL_ATTRIBUTE << " is not defined or invalid " << PARAMETER_CHANNEL_ATTRIBUTE << " name specified.");
      continue;
    }

    if( fieldElement->GetAttribute(PARAMETER_NAME_ATTRIBUTE) == NULL )
    {
      LOG_ERROR("Parameter " << PARAMETER_NAME_ATTRIBUTE << " not defined. Unable to build field scanner.");
      continue;
    }

    int origin[2] = {-1,-1};
    int size[2] = {-1,-1};
    fieldElement->GetVectorAttribute(PARAMETER_ORIGIN_ATTRIBUTE, 2, origin);
    fieldElement->GetVectorAttribute(PARAMETER_SIZE_ATTRIBUTE, 2, size);
    if( origin[0] < 0 || origin[1] < 0 || size[0] < 0 || size[1] < 0 )
    {
      LOG_ERROR("Invalid definition for " << PARAMETER_ORIGIN_ATTRIBUTE << " and " << PARAMETER_SIZE_ATTRIBUTE << ". Unable to build field scanner.");
      continue;
    }

    TextFieldParameter* parameter = new TextFieldParameter();
    parameter->ParameterName = std::string(fieldElement->GetAttribute(PARAMETER_NAME_ATTRIBUTE));
    parameter->SourceChannel = aChannel;
    parameter->Origin[0] = origin[0];
    parameter->Origin[1] = origin[1];
    parameter->Size[0] = size[0];
    parameter->Size[1] = size[1];
    parameter->ReceivedFrame = pixCreate(parameter->Size[0], parameter->Size[1], PARAMETER_DEPTH_BITS);
    parameter->ScreenRegion = vtkSmartPointer<vtkImageData>::New();
    parameter->ScreenRegion->SetExtent(0, size[0]-1, 0, size[1]-1, 0, 0);
    parameter->ScreenRegion->AllocateScalars(VTK_UNSIGNED_CHAR, 1); // Black and white images for now

    this->RecognitionFields[parameter->SourceChannel].push_back(parameter);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  if( STRCASECMP(this->Language, DEFAULT_LANGUAGE) != 0 )
  {
    XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(Language, deviceConfig);
  }

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(screenFields, deviceConfig, PARAMETER_LIST_TAG_NAME);

  for( ChannelFieldListMapIterator it = this->RecognitionFields.begin(); it != this->RecognitionFields.end(); ++it )
  {
    for( FieldListIterator fieldIt = it->second.begin(); fieldIt != it->second.end(); ++fieldIt )
    {
      TextFieldParameter* parameter = *fieldIt;

      XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(fieldElement, screenFields, PARAMETER_TAG_NAME);

      fieldElement->SetAttribute(PARAMETER_CHANNEL_ATTRIBUTE, parameter->SourceChannel->GetChannelId());
      fieldElement->SetAttribute(PARAMETER_NAME_ATTRIBUTE, parameter->ParameterName.c_str());
      fieldElement->SetVectorAttribute(PARAMETER_ORIGIN_ATTRIBUTE, 2, parameter->Origin);
      fieldElement->SetVectorAttribute(PARAMETER_SIZE_ATTRIBUTE, 2, parameter->Size);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualTextRecognizer::NotifyConfigured()
{
  if( this->InputChannels.size() < 1 )
  {
    LOG_ERROR("Screen reader needs at least one input image to analyze. Please add an input channel with video data.");
    return PLUS_FAIL;
  }

  if( this->RecognitionFields.size() < 1 )
  {
    LOG_ERROR("Screen reader has no fields defined. There's nothing for me to do!");
  }

  return PLUS_SUCCESS;
}
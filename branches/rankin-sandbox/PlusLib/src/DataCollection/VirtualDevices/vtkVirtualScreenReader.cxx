/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkVirtualScreenReader.h"

#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>
#include <allheaders.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkVirtualScreenReader);

//----------------------------------------------------------------------------

namespace
{
  const double SAMPLING_SKIPPING_MARGIN_SEC = 0.1;
  const double DELAY_ON_SENDING_ERROR_SEC = 0.02; 
}

//----------------------------------------------------------------------------
vtkVirtualScreenReader::vtkVirtualScreenReader()
: vtkPlusDevice()
, Language(NULL)
, InputChannel(NULL)
, TrackedFrames(vtkTrackedFrameList::New())
, ReceivedFrame(NULL)
, LastTimestampEvaluated(UNDEFINED_TIMESTAMP)
{
  // The data capture thread will be used to regularly check the input devices and generate and update the output
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
}

//----------------------------------------------------------------------------
vtkVirtualScreenReader::~vtkVirtualScreenReader()
{
  TrackedFrames->Delete();
  TrackedFrames = NULL;
}

//----------------------------------------------------------------------------
void vtkVirtualScreenReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::InternalUpdate()
{
  double oldestDataTimestamp(0);
  if (this->GetInputChannel()->GetOldestTimestamp(oldestDataTimestamp) == PLUS_SUCCESS)
  {
    if (this->LastTimestampEvaluated < oldestDataTimestamp)
    {
      this->LastTimestampEvaluated = oldestDataTimestamp + SAMPLING_SKIPPING_MARGIN_SEC;
    }
    this->TrackedFrames->Clear();
    if ( this->GetInputChannel()->GetTrackedFrameList(this->LastTimestampEvaluated, this->TrackedFrames, 3) != PLUS_SUCCESS )
    {
      LOG_INFO("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << this->LastTimestampEvaluated ); 
      vtkAccurateTimer::Delay(DELAY_ON_SENDING_ERROR_SEC);
    }
  }

  if( this->TrackedFrames->GetNumberOfTrackedFrames() > 1 )
  {
    LOG_DEBUG("Processing cannot keep up with acquisition. Dropping " << this->TrackedFrames->GetNumberOfTrackedFrames() - 1 << " frames.");
    TrackedFrame* frame = this->TrackedFrames->GetTrackedFrame(0);
    this->LastTimestampEvaluated = frame->GetTimestamp();

    int w = frame->GetFrameSize()[0];
    int h = frame->GetFrameSize()[1];
    if( this->ReceivedFrame == NULL )
    {
      this->ReceivedFrame = pixCreate(w, h, 8);
      this->TesseractAPI->SetImage(this->ReceivedFrame);
    }

    unsigned int *data = pixGetData(this->ReceivedFrame);
    int wpl = pixGetWpl(this->ReceivedFrame);
    int bpl = ( (8*w) + 7) / 8;
    unsigned int *line;
    unsigned char val8;

    static int someVal = 0;
    int coords[3] = {0,0,0};
    for(int y = 0; y < h; y++)
    {
      coords[1] = h-y-1;
      line = data + y * wpl;
      for(int x = 0; x < bpl; x++)
      {
        coords[0] = x;
        val8 = (*(unsigned char*)frame->GetImageData()->GetImage()->GetScalarPointer(coords));
        SET_DATA_BYTE(line,x,val8);
      }
    }

    char* text_out;
    text_out = this->TesseractAPI->GetUTF8Text();
    delete [] text_out;
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::InternalConnect()
{
  this->TesseractAPI = new tesseract::TessBaseAPI();
  this->TesseractAPI->Init(NULL, Language, tesseract::OEM_DEFAULT);
  this->TesseractAPI->SetPageSegMode(tesseract::PSM_AUTO);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::InternalDisconnect()
{
  delete this->TesseractAPI;
  this->TesseractAPI = NULL;

  this->LastTimestampEvaluated = UNDEFINED_TIMESTAMP;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::ReadConfiguration( vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(Language, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::NotifyConfigured()
{
  if( this->InputChannels.size() != 1 )
  {
    LOG_ERROR("Screen reader needs an input image to analyze. Please add an input channel with video data.");
    return PLUS_FAIL;
  }

  this->SetInputChannel(this->InputChannels[0]);
  return PLUS_SUCCESS;
}
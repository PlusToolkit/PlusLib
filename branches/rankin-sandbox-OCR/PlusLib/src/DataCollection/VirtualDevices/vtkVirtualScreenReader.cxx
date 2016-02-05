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
vtkVirtualScreenReader::vtkVirtualScreenReader()
: vtkPlusDevice()
, Language(NULL)
, InputChannel(NULL)
, TrackedFrames(vtkTrackedFrameList::New())
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
  if( this->GetInputChannel()->GetTrackedFrameList(this->LastTimestampEvaluated, this->TrackedFrames, 50) == PLUS_FAIL )
  {
    // No data to process
    return PLUS_SUCCESS;
  }

  for( int i = 0; i < this->TrackedFrames->GetNumberOfTrackedFrames(); ++i )
  {
    TrackedFrame* frame = this->TrackedFrames->GetTrackedFrame(i);
    const int depth = 8;
    PIX* pix = pixCreate(frame->GetFrameSize()[0], frame->GetFrameSize()[1], depth);

    unsigned int *data = pixGetData(pix);
    int wpl = pixGetWpl(pix);
    int bpl = (depth * frame->GetFrameSize()[0] + 7) / 8;
    unsigned int *line;
    unsigned char val8;

    int extent[6] = {0,0,0,0,0,0};
    for(int y = 0; y < frame->GetFrameSize()[1]; y++)
    {
      extent[3] = frame->GetFrameSize()[1]-y-1;
      line = data + y * wpl;
      for(int x = 0; x < bpl; x++)
      {
        extent[1] = x;
        val8 = (*(unsigned char*)frame->GetImageData()->GetImage()->GetScalarPointerForExtent(extent));
        SET_DATA_BYTE(line,x,val8);
      }
    }

    pixWritePng("c:\\users\\arankin\\Downloads\\a.png", pix, 1.0);

    char* text_out;
    this->TesseractAPI->SetImage(pix);
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
  this->TesseractAPI->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);

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
/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusVirtualDeinterlacer.h"

// IGSIO includes
#include <igsioVideoFrame.h>
#include <vtkIGSIOTrackedFrameList.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

namespace
{
  //----------------------------------------------------------------------------
  std::string ModeToString(vtkPlusVirtualDeinterlacer::StereoMode mode)
  {
    switch (mode)
    {
      case vtkPlusVirtualDeinterlacer::Stereo_HorizontalInterlace:
        return "HorizontalInterlace";
      case vtkPlusVirtualDeinterlacer::Stereo_VerticalInterlace:
        return "VerticalInterlace";
      default:
        return "Unknown";
    }
  }

  //----------------------------------------------------------------------------
  vtkPlusVirtualDeinterlacer::StereoMode StringToMode(const std::string& mode)
  {
    if (igsioCommon::IsEqualInsensitive(mode, "HorizontalInterlace"))
    {
      return vtkPlusVirtualDeinterlacer::Stereo_HorizontalInterlace;
    }
    else if (igsioCommon::IsEqualInsensitive(mode, "VerticalInterlace"))
    {
      return vtkPlusVirtualDeinterlacer::Stereo_VerticalInterlace;
    }
    else
    {
      return vtkPlusVirtualDeinterlacer::Stereo_Unknown;
    }
  }
}
//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusVirtualDeinterlacer);

//----------------------------------------------------------------------------
vtkPlusVirtualDeinterlacer::vtkPlusVirtualDeinterlacer()
  : vtkPlusDevice()
  , Mode(Stereo_Unknown)
  , Initialized(false)
  , LastInputTimestamp(UNDEFINED_TIMESTAMP)
  , FrameList(vtkIGSIOTrackedFrameList::New())
  , InputSource(nullptr)
  , LeftImage(nullptr)
  , RightImage(nullptr)
  , SwitchInterlaceOrdering(false)
{
  this->AcquisitionRate = 400; // Super fast!
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusVirtualDeinterlacer::~vtkPlusVirtualDeinterlacer()
{
  if (this->LeftImage != nullptr)
  {
    this->LeftImage->Delete();
    this->LeftImage = nullptr;
  }
  if (this->RightImage != nullptr)
  {
    this->RightImage->Delete();
    this->RightImage = nullptr;
  }
  this->FrameList->Delete();
}

//----------------------------------------------------------------------------
void vtkPlusVirtualDeinterlacer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualDeinterlacer::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  std::string modeStr;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(StereoMode, modeStr, deviceConfig);
  StereoMode mode = StringToMode(modeStr);
  if (mode == Stereo_Unknown)
  {
    LOG_ERROR("Unknown stereo mode in configuration. Please double check.");
    return PLUS_FAIL;
  }
  this->Mode = mode;

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SwitchInterlaceOrdering, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualDeinterlacer::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetAttribute("StereoMode", ModeToString(this->Mode).c_str());

  XML_WRITE_BOOL_ATTRIBUTE(SwitchInterlaceOrdering, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualDeinterlacer::InternalUpdate()
{
  if (!this->Initialized && this->InputChannels[0]->GetVideoDataAvailable())
  {
    FrameSizeType size = this->InputSource->GetOutputFrameSize();
    if (this->Mode == Stereo_HorizontalInterlace)
    {
      if (size[1] % 2 == 1)
      {
        LOG_WARNING("Odd sized Y dimension, extra row will be added.");
      }
      // horizontal rows, Y dim is halved
      size[1] = std::ceil(size[1] / 2.0);
    }
    else if (this->Mode == Stereo_VerticalInterlace)
    {
      if (size[01] % 2 == 1)
      {
        LOG_WARNING("Odd sized X dimension, extra column will be lost.");
      }
      // vertical rows, X dim is halved
      size[0] = std::ceil(size[0] / 2.0);
    }
    this->LeftSource->SetInputImageOrientation(US_IMG_ORIENT_MFA);
    this->RightSource->SetInputImageOrientation(US_IMG_ORIENT_MFA);
    this->LeftSource->SetInputFrameSize(size);
    this->RightSource->SetInputFrameSize(size);
    this->LeftSource->SetPixelType(this->InputSource->GetPixelType());
    this->RightSource->SetPixelType(this->InputSource->GetPixelType());
    this->LeftSource->SetNumberOfScalarComponents(this->InputSource->GetNumberOfScalarComponents());
    this->RightSource->SetNumberOfScalarComponents(this->InputSource->GetNumberOfScalarComponents());
    this->LeftSource->SetImageType(this->InputSource->GetImageType());
    this->RightSource->SetImageType(this->InputSource->GetImageType());

    this->LeftImage = vtkImageData::New();
    this->RightImage = vtkImageData::New();
    this->LeftImage->SetDimensions(size[0], size[1], size[2]);
    this->RightImage->SetDimensions(size[0], size[1], size[2]);
    this->LeftImage->AllocateScalars(this->InputSource->GetPixelType(), this->InputSource->GetNumberOfScalarComponents());
    this->RightImage->AllocateScalars(this->InputSource->GetPixelType(), this->InputSource->GetNumberOfScalarComponents());

    this->Initialized = true;
  }

  this->FrameList->Clear();
  if (this->InputChannels[0]->GetTrackedFrameList(this->LastInputTimestamp, this->FrameList, 100) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  for (auto frame : *this->FrameList)
  {
    if (this->Mode == Stereo_HorizontalInterlace)
    {
      this->SplitFrameHorizontal(frame);
    }
    else if (this->Mode == Stereo_VerticalInterlace)
    {
      this->SplitFrameVertical(frame);
    }

    this->LeftSource->AddItem(this->LeftImage, this->LeftSource->GetInputImageOrientation(), this->LeftSource->GetImageType(), this->FrameNumber);
    this->RightSource->AddItem(this->RightImage, this->RightSource->GetInputImageOrientation(), this->RightSource->GetImageType(), this->FrameNumber);
    this->FrameNumber++;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusVirtualDeinterlacer::SplitFrameHorizontal(igsioTrackedFrame* frame)
{
  vtkImageData* inputImage = frame->GetImageData()->GetImage();
  vtkIdType* inputIncrements = inputImage->GetIncrements();
  unsigned char* inputPtr = (unsigned char*)inputImage->GetScalarPointer();
  unsigned char* leftPtr = (unsigned char*)this->LeftImage->GetScalarPointer();
  unsigned char* rightPtr = (unsigned char*)this->RightImage->GetScalarPointer();
  vtkIdType* outputIncrements = this->LeftImage->GetIncrements();

  // for each input row
  for (vtkIdType row = 0; row < inputImage->GetDimensions()[1]; row++)
  {
    // %2 = 0 left, %2 = 1 right
    bool left(false);
    if (row % 2 == 0)
    {
      left = true;
    }
    if (this->SwitchInterlaceOrdering)
    {
      left = !left;
    }

    // finished with this row,
    // increment output image pointer for this row's image
    if (left)
    {
      // Copy row to left image, increment left image pointer
      memcpy(leftPtr, inputPtr, inputIncrements[1]);
      leftPtr += outputIncrements[1];
    }
    else
    {
      memcpy(rightPtr, inputPtr, inputIncrements[1]);
      rightPtr += outputIncrements[1];
    }
    inputPtr += inputIncrements[1];
  }
}

//----------------------------------------------------------------------------
void vtkPlusVirtualDeinterlacer::SplitFrameVertical(igsioTrackedFrame* frame)
{

}

//----------------------------------------------------------------------------
double vtkPlusVirtualDeinterlacer::GetAcquisitionRate() const
{
  // Determine frame rate from the video input
  if (this->InputSource == nullptr)
  {
    return -1.;
  }
  return this->InputSource->GetDevice()->GetAcquisitionRate();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualDeinterlacer::NotifyConfigured()
{
  if (this->InputChannels.size() != 1)
  {
    LOG_ERROR("Deinterlacer requires exactly 1 input channel");
    return PLUS_FAIL;
  }
  this->InputChannels[0]->GetVideoSource(this->InputSource);
  if (this->InputSource == nullptr)
  {
    LOG_ERROR("Input channel does not have a video source. It is required.");
    return PLUS_FAIL;
  }

  if (this->OutputChannels.size() < 2)
  {
    LOG_ERROR("Deinterlacer requires at least 2 output channels.");
    return PLUS_FAIL;
  }

  if (this->VideoSources.size() != 2)
  {
    LOG_ERROR("Deinterlacer requires exactly 2 video sources named \"Left\" and \"Right\"");
    return PLUS_FAIL;
  }
  if (this->VideoSources.find("Left") == this->VideoSources.end())
  {
    LOG_ERROR("No video source with ID \"Left\" found.");
    return PLUS_FAIL;
  }
  this->LeftSource = this->VideoSources.find("Left")->second;
  if (this->VideoSources.find("Right") == this->VideoSources.end())
  {
    LOG_ERROR("No video source with ID \"Right\" found.");
    return PLUS_FAIL;
  }
  this->RightSource = this->VideoSources.find("Right")->second;

  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    if (!(*it)->HasVideoSource())
    {
      LOG_ERROR("All output channels require a video source.");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

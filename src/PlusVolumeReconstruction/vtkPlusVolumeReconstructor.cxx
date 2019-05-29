/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusSequenceIO.h"
#include "vtkPlusVolumeReconstructor.h"

// VTK includes
#include <vtkImageFlip.h>
#include <vtkObjectFactory.h>
#include <vtkPNGReader.h>

vtkStandardNewMacro(vtkPlusVolumeReconstructor);

//----------------------------------------------------------------------------
vtkPlusVolumeReconstructor::vtkPlusVolumeReconstructor()
{
}

//----------------------------------------------------------------------------
vtkPlusVolumeReconstructor::~vtkPlusVolumeReconstructor()
{
}

//----------------------------------------------------------------------------
void vtkPlusVolumeReconstructor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
igsioStatus vtkPlusVolumeReconstructor::SaveReconstructedVolumeToFile(const std::string& filename, bool accumulation/*=false*/, bool useCompression/*=true*/)
{
  vtkSmartPointer<vtkImageData> volumeToSave = vtkSmartPointer<vtkImageData>::New();
  if (accumulation)
  {
    if (this->ExtractAccumulation(volumeToSave) != PLUS_SUCCESS)
    {
      LOG_ERROR("Extracting accumulation buffer failed!");
      return PLUS_FAIL;
    }
  }
  else
  {
    if (this->ExtractGrayLevels(volumeToSave) != PLUS_SUCCESS)
    {
      LOG_ERROR("Extracting gray channel failed!");
      return PLUS_FAIL;
    }
  }
  return vtkPlusVolumeReconstructor::SaveReconstructedVolumeToFile(volumeToSave, filename, useCompression);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVolumeReconstructor::SaveReconstructedVolumeToFile(vtkImageData* volumeToSave, const std::string& filename, bool useCompression/*=true*/)
{
  if (volumeToSave == NULL)
  {
    LOG_ERROR("vtkPlusVolumeReconstructor::SaveReconstructedVolumeToMetafile: invalid input image");
    return PLUS_FAIL;
  }

  int dims[3];
  volumeToSave->GetDimensions(dims);
  FrameSizeType frameSize = { static_cast<unsigned int>(dims[0]), static_cast<unsigned int>(dims[1]), static_cast<unsigned int>(dims[2]) };

  vtkNew<vtkIGSIOTrackedFrameList> list;
  igsioTrackedFrame frame;
  igsioVideoFrame image;
  image.AllocateFrame(frameSize, volumeToSave->GetScalarType(), volumeToSave->GetNumberOfScalarComponents());
  image.GetImage()->DeepCopy(volumeToSave);
  image.SetImageOrientation(US_IMG_ORIENT_MFA);
  image.SetImageType(US_IMG_BRIGHTNESS);
  frame.SetImageData(image);
  list->AddTrackedFrame(&frame);
  if (vtkPlusSequenceIO::Write(filename, list.GetPointer(), US_IMG_ORIENT_MF, useCompression) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to save reconstructed volume in sequence metafile!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVolumeReconstructor::UpdateImportanceMask()
{
  if (this->ImportanceMaskFilename == this->ImportanceMaskFilenameInReconstructor)
  {
    // no change
    return PLUS_SUCCESS;
  }
  this->ImportanceMaskFilenameInReconstructor = this->ImportanceMaskFilename;

  if (!this->ImportanceMaskFilename.empty())
  {
    std::string importanceMaskFilePath;
    if (vtkPlusConfig::GetInstance()->FindImagePath(this->ImportanceMaskFilename, importanceMaskFilePath) == PLUS_FAIL)
    {
      LOG_ERROR("Cannot get importance mask from file: " << this->ImportanceMaskFilename);
      return PLUS_FAIL;
    }
    vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
    reader->SetFileName(importanceMaskFilePath.c_str());
    reader->Update();
    if (reader->GetOutput() == NULL)
    {
      LOG_ERROR("Failed to read importance image from file: " << importanceMaskFilePath);
      return PLUS_FAIL;
    }
    vtkSmartPointer<vtkImageFlip> flipYFilter = vtkSmartPointer<vtkImageFlip>::New();
    flipYFilter->SetFilteredAxis(1); // flip y axis
    flipYFilter->SetInputConnection(reader->GetOutputPort());
    flipYFilter->Update();
    this->Reconstructor->SetImportanceMask(flipYFilter->GetOutput());
  }
  else
  {
    this->Reconstructor->SetImportanceMask(NULL);
  }
  return PLUS_SUCCESS;
}

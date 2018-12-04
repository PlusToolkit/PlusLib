/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusSequenceIO.h"

#include <vtkIGSIOSequenceIO.h>

/// VTK includes
#include <vtkNew.h>

//----------------------------------------------------------------------------
igsioStatus vtkPlusSequenceIO::Write(const std::string& filename, vtkIGSIOTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile/*=US_IMG_ORIENT_MF*/, bool useCompression/*=true*/, bool enableImageDataWrite/*=true*/)
{
  return vtkIGSIOSequenceIO::Write(filename, vtkPlusConfig::GetInstance()->GetOutputDirectory(), frameList, orientationInFile, useCompression, enableImageDataWrite);
}

//----------------------------------------------------------------------------
igsioStatus vtkPlusSequenceIO::Write(const std::string& filename, igsioTrackedFrame* frame, US_IMAGE_ORIENTATION orientationInFile /*= US_IMG_ORIENT_MF*/, bool useCompression /*= true*/, bool enableImageDataWrite /*=true*/)
{
  return vtkIGSIOSequenceIO::Write(filename, vtkPlusConfig::GetInstance()->GetOutputDirectory(), frame, orientationInFile, useCompression, enableImageDataWrite);
}

//----------------------------------------------------------------------------
igsioStatus vtkPlusSequenceIO::Read(const std::string& trackedSequenceDataFileName, vtkIGSIOTrackedFrameList* frameList)
{
  std::string trackedSequenceDataFilePath = trackedSequenceDataFileName;

  // If file is not found in the current directory then try to find it in the image directory, too
  if (!vtksys::SystemTools::FileExists(trackedSequenceDataFilePath.c_str(), true))
  {
    if (vtkPlusConfig::GetInstance()->FindImagePath(trackedSequenceDataFileName, trackedSequenceDataFilePath) == PLUS_FAIL)
    {
      LOG_ERROR("Cannot find sequence metafile: " << trackedSequenceDataFileName);
      return PLUS_FAIL;
    }
  }
  return vtkIGSIOSequenceIO::Read(trackedSequenceDataFilePath, frameList);
}

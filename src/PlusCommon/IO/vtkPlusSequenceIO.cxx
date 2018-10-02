/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusMetaImageSequenceIO.h"
#include "vtkPlusNrrdSequenceIO.h"
#include "vtkPlusSequenceIO.h"
#include "vtkPlusTrackedFrameList.h"

#ifdef PLUS_USE_VTKVIDEOIO_MKV
#include "vtkPlusMkvSequenceIO.h"
#endif

//----------------------------------------------------------------------------
PlusStatus vtkPlusSequenceIO::Write(const std::string& filename, vtkPlusTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile/*=US_IMG_ORIENT_MF*/, bool useCompression/*=true*/, bool enableImageDataWrite/*=true*/)
{
  // Convert local filename to plus output filename
  if (vtksys::SystemTools::FileExists(filename.c_str()))
  {
    // Remove the file before replacing it
    vtksys::SystemTools::RemoveFile(filename.c_str());
  }

  // Parse sequence filename to determine if it's metafile or NRRD
  if (vtkPlusMetaImageSequenceIO::CanWriteFile(filename))
  {
    if (frameList->SaveToSequenceMetafile(filename, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to save file: " << filename << " as sequence metafile.");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
  else if (vtkPlusNrrdSequenceIO::CanWriteFile(filename))
  {
    if (frameList->SaveToNrrdFile(filename, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to save file: " << filename << " as Nrrd file.");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
#ifdef PLUS_USE_VTKVIDEOIO_MKV
  else if (vtkPlusMkvSequenceIO::CanWriteFile(filename))
  {
    if (frameList->SaveToMatroskaFile(filename, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to save file: " << filename << " as MKV file.");
      return PLUS_FAIL;
    }
  }
#endif

  LOG_ERROR("No writer for file: " << filename);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSequenceIO::Read(const std::string& filename, vtkPlusTrackedFrameList* frameList)
{
  if (!vtksys::SystemTools::FileExists(filename.c_str()))
  {
    LOG_ERROR("File: " << filename << " does not exist.");
    return PLUS_FAIL;
  }

  if (vtkPlusMetaImageSequenceIO::CanReadFile(filename))
  {
    // Attempt metafile read
    if (frameList->ReadFromSequenceMetafile(filename) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read video buffer from sequence metafile: " << filename);
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
  // Parse sequence filename to determine if it's metafile or NRRD
  else if (vtkPlusNrrdSequenceIO::CanReadFile(filename))
  {
    // Attempt Nrrd read
    if (frameList->ReadFromNrrdFile(filename.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read video buffer from Nrrd file: " << filename);
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
#ifdef PLUS_USE_VTKVIDEOIO_MKV
  else if (vtkPlusMkvSequenceIO::CanReadFile(filename))
  {
    // Attempt MKV read
    if (frameList->ReadFromMatroskaFile(filename.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read video buffer from MKV file: " << filename);
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
#endif

  LOG_ERROR("No reader for file: " << filename);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
vtkPlusSequenceIOBase* vtkPlusSequenceIO::CreateSequenceHandlerForFile(const std::string& filename)
{
  // Parse sequence filename to determine if it's metafile or NRRD
  if( vtkPlusMetaImageSequenceIO::CanWriteFile(filename) )
  {
    return vtkPlusMetaImageSequenceIO::New();
  }
  else if( vtkPlusNrrdSequenceIO::CanWriteFile(filename) )
  {
    return vtkPlusNrrdSequenceIO::New();
  }
#ifdef PLUS_USE_VTKVIDEOIO_MKV
  else if (vtkPlusMkvSequenceIO::CanReadFile(filename))
  {
    return vtkPlusMkvSequenceIO::New();
  }
#endif

  LOG_ERROR("No writer for file: " << filename);
  return NULL;
}

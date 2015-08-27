/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkMetaImageSequenceIO.h"
#include "vtkNrrdSequenceIO.h"
#include "vtkSequenceIO.h"
#include "vtkTrackedFrameList.h"

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIO::Write(const std::string& filename, vtkTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile/*=US_IMG_ORIENT_MF*/, bool useCompression/*=true*/, bool enableImageDataWrite/*=true*/)
{
  if( vtksys::SystemTools::FileExists(filename) )
  {
    // Remove the file before replacing it
    vtksys::SystemTools::RemoveFile(filename);
  }

  // Parse sequence filename to determine if it's metafile or NRRD
  if( vtkMetaImageSequenceIO::CanWriteFile(filename) )
  {
    if( frameList->SaveToSequenceMetafile(filename, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to save file: " << filename << " as sequence metafile.");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
  else if( vtkNrrdSequenceIO::CanWriteFile(filename) )
  {
    if( frameList->SaveToNrrdFile(filename, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to save file: " << filename << " as Nrrd file.");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  LOG_ERROR("No writer for file: " << filename);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIO::Read(const std::string& filename, vtkTrackedFrameList* frameList)
{
  if( !vtksys::SystemTools::FileExists(filename) )
  {
    LOG_ERROR("File: " << filename << " does not exist.");
    return PLUS_FAIL;
  }

  // Parse sequence filename to determine if it's metafile or NRRD
  if( vtkNrrdSequenceIO::CanReadFile(filename) )
  {
    // Attempt Nrrd read
    if( frameList->ReadFromNrrdFile(filename.c_str()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read video buffer from Nrrd file: " << filename); 
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
  else if( vtkMetaImageSequenceIO::CanReadFile(filename) )
  {
    // Attempt metafile read
    if ( frameList->ReadFromSequenceMetafile(filename) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read video buffer from sequence metafile: " << filename); 
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  LOG_ERROR("No reader for file: " << filename);
  return PLUS_FAIL;
}

vtkSequenceIOBase* vtkSequenceIO::CreateSequenceHandlerForFile(const std::string& filename)
{
  // Parse sequence filename to determine if it's metafile or NRRD
  if( vtkNrrdSequenceIO::CanWriteFile(filename) )
  {
    return vtkNrrdSequenceIO::New();
  }
  else if( vtkMetaImageSequenceIO::CanWriteFile(filename) )
  {
    return vtkMetaImageSequenceIO::New();
  }

  LOG_ERROR("No writer for file: " << filename);
  return NULL;
}

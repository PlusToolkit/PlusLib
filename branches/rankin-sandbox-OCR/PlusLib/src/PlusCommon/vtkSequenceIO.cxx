/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkMetaImageSequenceIO.h"
#if VTK_MAJOR_VERSION > 5
#include "vtkNrrdSequenceIO.h"
#endif
#include "vtkSequenceIO.h"
#include "vtkTrackedFrameList.h"

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIO::Write(const std::string& filename, vtkTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile/*=US_IMG_ORIENT_MF*/, bool useCompression/*=true*/, bool enableImageDataWrite/*=true*/)
{
  if( vtksys::SystemTools::FileExists(filename.c_str()) )
  {
    // Remove the file before replacing it
    vtksys::SystemTools::RemoveFile(filename.c_str());
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
#if VTK_MAJOR_VERSION > 5
  else if( vtkNrrdSequenceIO::CanWriteFile(filename) )
  {
    if( frameList->SaveToNrrdFile(filename, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to save file: " << filename << " as Nrrd file.");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
#else
  else if( filename.find(".nrrd") != std::string::npos || filename.find(".nhdr") != std::string::npos )
  {
    LOG_WARNING("NRRD support is disabled in PLUS that is built on VTK5. Please consider building against VTK6. Outputting in MHA format.");
    std::vector<std::string> pathElems;
    vtksys::SystemTools::SplitPath(filename.c_str(), pathElems);
    std::string noExt = vtksys::SystemTools::GetFilenameWithoutExtension(pathElems[pathElems.size()-1]);
    pathElems.pop_back();
    pathElems.push_back(noExt+".mha");
    std::string file = vtksys::SystemTools::JoinPath(pathElems.begin(), pathElems.end());
    if( frameList->SaveToSequenceMetafile(file, orientationInFile, useCompression, enableImageDataWrite) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to save file: " << file << " as sequence metafile.");
      return PLUS_FAIL;
    }
  }
#endif
  LOG_ERROR("No writer for file: " << filename);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIO::Read(const std::string& filename, vtkTrackedFrameList* frameList)
{
  if( !vtksys::SystemTools::FileExists(filename.c_str()) )
  {
    LOG_ERROR("File: " << filename << " does not exist.");
    return PLUS_FAIL;
  }

  if( vtkMetaImageSequenceIO::CanReadFile(filename) )
  {
    // Attempt metafile read
    if ( frameList->ReadFromSequenceMetafile(filename) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read video buffer from sequence metafile: " << filename); 
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
#if VTK_MAJOR_VERSION > 5
  // Parse sequence filename to determine if it's metafile or NRRD
  else if( vtkNrrdSequenceIO::CanReadFile(filename) )
  {
    // Attempt Nrrd read
    if( frameList->ReadFromNrrdFile(filename.c_str()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read video buffer from Nrrd file: " << filename); 
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }
#else
  else if( filename.find(".nrrd") != std::string::npos || filename.find(".nhdr") != std::string::npos )
  {
    LOG_ERROR("NRRD support is disabled in PLUS that is built on VTK5. Unable to read NRRD format.");
    return PLUS_FAIL;
  }
#endif

  LOG_ERROR("No reader for file: " << filename);
  return PLUS_FAIL;
}

vtkSequenceIOBase* vtkSequenceIO::CreateSequenceHandlerForFile(const std::string& filename)
{
  // Parse sequence filename to determine if it's metafile or NRRD
  if( vtkMetaImageSequenceIO::CanWriteFile(filename) )
  {
    return vtkMetaImageSequenceIO::New();
  }
#if VTK_MAJOR_VERSION > 5
  else if( vtkNrrdSequenceIO::CanWriteFile(filename) )
  {
    return vtkNrrdSequenceIO::New();
  }
#endif

  LOG_ERROR("No writer for file: " << filename);
  return NULL;
}

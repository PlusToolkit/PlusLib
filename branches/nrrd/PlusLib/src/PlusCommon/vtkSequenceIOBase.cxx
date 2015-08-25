/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkSequenceIOBase.h"
#include "vtkTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"
#include "TrackedFrame.h"

#if _WIN32
#include <errno.h>
#endif

//----------------------------------------------------------------------------

vtkCxxSetObjectMacro(vtkSequenceIOBase, TrackedFrameList, vtkTrackedFrameList);

//----------------------------------------------------------------------------
vtkSequenceIOBase::vtkSequenceIOBase()
  : TrackedFrameList(vtkTrackedFrameList::New())
  , UseCompression(false)
  , EnableImageDataWrite(true)
  , PixelType(VTK_VOID)
  , NumberOfScalarComponents(1)
  , NumberOfDimensions(4)
  , CurrentFrameOffset(0)
  , TotalBytesWritten(0)
  , ImageOrientationInFile(US_IMG_ORIENT_XX)
  , ImageOrientationInMemory(US_IMG_ORIENT_XX)
  , ImageType(US_IMG_TYPE_XX)
  , PixelDataFileOffset(0)
  , PixelDataFileName("")
{
  this->Dimensions[0]=1;
  this->Dimensions[1]=1;
  this->Dimensions[2]=1;
  this->Dimensions[3]=1;
}

//----------------------------------------------------------------------------
vtkSequenceIOBase::~vtkSequenceIOBase()
{
  SetTrackedFrameList(NULL);
}

//----------------------------------------------------------------------------
void vtkSequenceIOBase::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Frame List User Fields:" << std::endl;
  this->TrackedFrameList->PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::Read()
{
  this->TrackedFrameList->Clear();

  if ( this->ReadImageHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Could not load header from file: " << this->FileName);
    return PLUS_FAIL;
  }

  if ( this->ReadImagePixels() != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::DeleteCustomFrameString(int frameNumber, const char* fieldName)
{
  TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);
  if (trackedFrame==NULL)
  {
    LOG_ERROR("Cannot access frame " << frameNumber);
    return PLUS_FAIL;
  }

  return trackedFrame->DeleteCustomFrameField(fieldName); 
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue)
{
  if (fieldName==NULL || fieldValue==NULL)
  {
    LOG_ERROR("Invalid field name or value");
    return PLUS_FAIL;
  }
  this->CreateTrackedFrameIfNonExisting(frameNumber);
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  if (trackedFrame==NULL)
  {
    LOG_ERROR("Cannot access frame "<<frameNumber);
    return PLUS_FAIL;
  }
  trackedFrame->SetCustomFrameField( fieldName, fieldValue );     
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
bool vtkSequenceIOBase::SetCustomString(const char* fieldName, const char* fieldValue)
{
  if (fieldName==NULL)
  {
    LOG_ERROR("Invalid field name");
    return PLUS_FAIL;
  }
  this->TrackedFrameList->SetCustomString(fieldName, fieldValue); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
const char* vtkSequenceIOBase::GetCustomString(const char* fieldName)
{
  if (fieldName==NULL)
  {
    LOG_ERROR("Invalid field name or value");
    return NULL;
  }
  return this->TrackedFrameList->GetCustomString(fieldName); 
}

//----------------------------------------------------------------------------
void vtkSequenceIOBase::CreateTrackedFrameIfNonExisting(unsigned int frameNumber)
{
  if ( frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames() )
  {
    // frame is already created
    return;
  }
  TrackedFrame emptyFrame;
  for (unsigned int i=this->TrackedFrameList->GetNumberOfTrackedFrames(); i<frameNumber+1; i++)
  {
    this->TrackedFrameList->AddTrackedFrame(&emptyFrame, vtkTrackedFrameList::ADD_INVALID_FRAME);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::Write()
{
  if( this->PrepareHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to prepare the header.");
    return PLUS_FAIL;
  }
  if( this->AppendImagesToHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to append images to the header.");
    return PLUS_FAIL;
  }
  if( this->FinalizeHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to finalize the header.");
    return PLUS_FAIL;
  }

  if ( this->WriteImagePixels(this->TempImageFileName, false) != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }

  this->Close();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::AppendImages()
{
  if( UseCompression )
  {
    LOG_ERROR("Unable to append images if compression is selected.");
    return PLUS_FAIL;
  }

  if (this->WriteImagePixels(this->TempImageFileName, true) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::Close()
{
  std::string headerFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(this->FileName);

  // Rename header to final filename
  RenameFile(this->TempHeaderFileName.c_str(), headerFullPath.c_str());

  if( this->PixelDataFileName.empty() )
  {
    // Append image to final file (single file)
    AppendFile(this->TempImageFileName, headerFullPath.c_str());
  }
  else
  {
    // Rename image to final filename (header+data file)
    std::string pixFullPath;
    if( !vtksys::SystemTools::FileIsFullPath(this->PixelDataFileName) )
    {
      std::vector<std::string> pathElements;
      vtksys::SystemTools::SplitPath(headerFullPath, pathElements);
      pathElements.erase(pathElements.end()-1);
      pathElements.push_back(this->PixelDataFileName);
      pixFullPath = vtksys::SystemTools::JoinPath(pathElements);
    }
    else
    {
      pixFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(this->PixelDataFileName);
    }
     
    RenameFile(this->TempImageFileName.c_str(), pixFullPath.c_str());
  }

  this->TempHeaderFileName.clear();
  this->TempImageFileName.clear();

  CurrentFrameOffset = 0;
  TotalBytesWritten = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::RenameFile(const char* oldname, const char* newname)
{
  // Adopted from CMake's cmSystemTools.cxx
  bool success = false;
#ifdef _WIN32
  // On Windows the move functions will not replace existing files. Check if the destination exists.
  if (vtksys::SystemTools::FileExists( newname, true))
  {
    // The destination exists.  We have to replace it carefully.  The
    // MoveFileEx function does what we need but is not available on
    // Win9x.
    OSVERSIONINFO osv;
    DWORD attrs;

    // Make sure the destination is not read only.
    attrs = GetFileAttributes(newname);
    if(attrs & FILE_ATTRIBUTE_READONLY)
    {
      SetFileAttributes(newname, attrs & ~FILE_ATTRIBUTE_READONLY);
    }

    // Check the windows version number.
    osv.dwOSVersionInfoSize = sizeof(osv);
    GetVersionEx(&osv);
    if(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
      // This is Win9x.  There is no MoveFileEx implementation.  We
      // cannot quite rename the file atomically.  Just delete the
      // destination and then move the file.
      DeleteFile(newname);
      success = (MoveFile(oldname, newname)!=0);
    }
    else
    {
      // This is not Win9x.  Use the MoveFileEx implementation.
      success = (MoveFileEx(oldname, newname, MOVEFILE_REPLACE_EXISTING)!=0);
    }
  }
  else
  {
    // The destination does not exist.  Just move the file.
    success = (MoveFile(oldname, newname)!=0);
  }
#else
  /* On UNIX we have an OS-provided call to do this atomically.  */
  success = (rename(oldname, newname) == 0);
#endif
  return success ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::Discard()
{
  vtksys::SystemTools::RemoveFile(this->TempHeaderFileName.c_str());
  vtksys::SystemTools::RemoveFile(this->TempImageFileName.c_str());

  this->TempHeaderFileName.clear();
  this->TempImageFileName.clear();

  CurrentFrameOffset = 0;
  TotalBytesWritten = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
TrackedFrame* vtkSequenceIOBase::GetTrackedFrame(int frameNumber)
{
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  return trackedFrame;
}

//----------------------------------------------------------------------------
void vtkSequenceIOBase::GetMaximumImageDimensions(int maxFrameSize[3])
{
  maxFrameSize[0]=0;
  maxFrameSize[1]=0;
  maxFrameSize[2]=0;

  for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    int * currFrameSize = this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameSize(); 
    if ( maxFrameSize[0] < currFrameSize[0] )
    {
      maxFrameSize[0] = currFrameSize[0]; 
    }

    if ( maxFrameSize[1] < currFrameSize[1] )
    {
      maxFrameSize[1] = currFrameSize[1]; 
    }

    if( maxFrameSize[2] < currFrameSize[2] )
    {
      maxFrameSize[2] = currFrameSize[2];
    }
  }
}

//----------------------------------------------------------------------------
std::string vtkSequenceIOBase::GetPixelDataFilePath()
{
  if ( this->PixelDataFileName.empty() )
  {
    // LOCAL => data is stored in one file
    return this->FileName;
  }

  std::string dir=vtksys::SystemTools::GetFilenamePath(this->FileName);
  if (!dir.empty())
  {
    dir+="/";
  }
  std::string path=dir+this->PixelDataFileName;
  return path;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::FileOpen(FILE **stream, const char* filename, const char* flags)
{
#ifdef _WIN32
  if (fopen_s(stream, filename, flags)!=0)
  {
    (*stream)=NULL;
  }
#else
  (*stream)=fopen(filename, flags);
#endif
  if ((*stream)==0)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSequenceIOBase::AppendFile(const std::string& sourceFilename, const std::string& destFilename)
{
#if _WIN32
  FILE* in;
  errno_t inErr = fopen_s( &in, sourceFilename.c_str(), "rb" ) ;
#else
  FILE* in = fopen( sourceFilename.c_str(), "rb" ) ;
#endif

#if _WIN32
  FILE* out;
  errno_t outErr = fopen_s( &out, destFilename.c_str(), "ab+");
#else
  FILE* out = fopen( destFilename.c_str(), "ab+");
#endif

#if _WIN32
  if( inErr != 0 || outErr != 0 )
  {
    char inErrStr[3000];
    char outErrStr[3000];
    strerror_s(inErrStr, inErr);
    strerror_s(outErrStr, outErr);
    LOG_ERROR( "An error occurred while appending data from " << sourceFilename << " to " << destFilename << ": " << inErrStr << "::" << outErrStr) ;
#else
  if( in == NULL || out == NULL )
  {
    LOG_ERROR( "An error occurred while appending data from " << sourceFilename << " to " << destFilename) ;
#endif
    return PLUS_FAIL;
  }
  else
  {
    const int BUFFER_SIZE = 32000;
    char *buffer = new char[BUFFER_SIZE];
    size_t len = 0 ;
    while( (len = fread( buffer, 1, BUFFER_SIZE, in)) > 0 )
    {
      fwrite( buffer, 1, len, out ) ;
      memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(in);
    fclose(out);
    if( !vtksys::SystemTools::RemoveFile(sourceFilename.c_str()) )
    {
      LOG_WARNING("Unable to remove the file " << sourceFilename << " after append is completed");
    }
    delete[] buffer;
  }
  return PLUS_SUCCESS;
}
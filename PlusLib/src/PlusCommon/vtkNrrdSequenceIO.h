/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkNrrdSequenceIO_h
#define __vtkNrrdSequenceIO_h

#include "vtkPlusCommonExport.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif 

#include "vtkSequenceIOBase.h"
#include "PlusVideoFrame.h" // for US_IMAGE_ORIENTATION

class vtkTrackedFrameList;
class TrackedFrame;

/*!
  \class vtkNrrdSequenceIO
  \brief Read and write Nrrd file with a sequence of frames, with additional information for each frame. File definition can be found at http://teem.sourceforge.net/nrrd/format.html
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkNrrdSequenceIO : public vtkSequenceIOBase
{
  enum NrrdEncoding
  {
    NRRD_ENCODING_RAW = 0,
    NRRD_ENCODING_TXT,
    NRRD_ENCODING_TEXT = NRRD_ENCODING_TXT,
    NRRD_ENCODING_ASCII = NRRD_ENCODING_TXT,
    NRRD_ENCODING_HEX,
    NRRD_ENCODING_GZ, 
    NRRD_ENCODING_GZIP = NRRD_ENCODING_GZ,
    NRRD_ENCODING_BZ2,
    NRRD_ENCODING_BZIP2 = NRRD_ENCODING_BZ2
  };

#ifdef _WIN32
  typedef __int64 FilePositionOffsetType;
#elif defined __APPLE__
  typedef off_t FilePositionOffsetType;
#else
  typedef off64_t FilePositionOffsetType;
#endif

public:
  static vtkNrrdSequenceIO *New();
  vtkTypeMacro(vtkNrrdSequenceIO, vtkSequenceIOBase);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Set the TrackedFrameList where the images are stored */
  virtual void SetTrackedFrameList(vtkTrackedFrameList *trackedFrameList);
  /*! Get the TrackedFrameList where the images are stored */
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList);

  /*!
    Set/get the ultrasound image orientation for file storage (as the result of writing).
    Note that the B-mode image data shall be always stored in MF orientation in the TrackedFrameList object in memory.
    The ultrasound image axes are defined as follows:
    * x axis: points towards the x coordinate increase direction
    * y axis: points towards the y coordinate increase direction
  */  
  vtkSetMacro(ImageOrientationInFile, US_IMAGE_ORIENTATION);

  /*!
    Set/get the ultrasound image orientation for memory storage (as the result of reading).
    B-mode image data shall be always stored in MF orientation in the TrackedFrameList object in memory.
    The ultrasound image axes are defined as follows:
    * x axis: points towards the x coordinate increase direction
    * y axis: points towards the y coordinate increase direction
  */  
  vtkSetMacro(ImageOrientationInMemory, US_IMAGE_ORIENTATION);

  /*! Write object contents into file */
  virtual PlusStatus Write();

  /*! Read file contents into the object */
  virtual PlusStatus Read();

  /*! Prepare the sequence for writing */
  virtual PlusStatus PrepareHeader();

  /*! Update the number of frames in the header 
      This is used primarily by vtkVirtualDiscCapture to update the final tally of frames, as it continually appends new frames to the file
      /param numberOfFrames the new number of frames to write
      /param addPadding this should only be true if this is the first time this function is called, which typically happens in OpenImageHeader
  */
  virtual PlusStatus OverwriteNumberOfFramesInHeader(int numberOfFrames, bool addPadding=false);

  /*! 
    Append the frames in tracked frame list to the header, if the onlyTrackerData flag is true it will not save
    in the header the image data related fields. 
  */
  virtual PlusStatus AppendImagesToHeader();

  /*! Finalize the header */
  virtual PlusStatus FinalizeHeader();

  /*! Write images to disc, compression allowed */
  virtual PlusStatus WriteImages();

  /*! Append image data to the sequence, compression not allowed */
  virtual PlusStatus AppendImages();

  /*! Close the sequence */
  virtual PlusStatus Close();

  /*! Close the sequence without saving anything (temporary files are deleted) */
  virtual PlusStatus Discard();

  /*! Check if this class can read the specified file */
  static bool CanReadFile(const std::string& filename);

  /*! Check if this class can write the specified file */
  static bool CanWriteFile(const std::string& filename);

  /*! Returns a pointer to a single frame */
  virtual TrackedFrame* GetTrackedFrame(int frameNumber);

  /*! Update a field in the image header with its current value */
  virtual PlusStatus UpdateFieldInImageHeader(const char* fieldName);

  /*!
    Set input/output file name. The file contains only the image header in case of
    nhdr images and the full image (including pixel data) in case of nrrd images.
  */
  virtual PlusStatus SetFileName(const std::string& aFilename);

  /*! Flag to enable/disable compression of image data */
  vtkGetMacro(UseCompression, bool);
  /*! Flag to enable/disable compression of image data */
  vtkSetMacro(UseCompression, bool);
  /*! Flag to enable/disable compression of image data */
  vtkBooleanMacro(UseCompression, bool);

  /*! Return the dimensions of the sequence */
  vtkGetMacro(Dimensions, int*);

  /*! Flag to enable/disable writing of image data */
  vtkGetMacro(EnableImageDataWrite, bool);
  /*! Flag to enable/disable writing of image data */
  vtkSetMacro(EnableImageDataWrite, bool);
  /*! Flag to enable/disable writing of image data */
  vtkBooleanMacro(EnableImageDataWrite, bool);


protected:
  vtkNrrdSequenceIO();
  virtual ~vtkNrrdSequenceIO();

  /*! Opens a file. Doesn't log error if it fails because it may be expected. */
  static PlusStatus FileOpen(FILE **stream, const char* filename, const char* flags);

  /*! Set a custom string field value for a specific frame */
  PlusStatus SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue);

  /*! Delete custom frame field from tracked frame */
  PlusStatus DeleteCustomFrameString(int frameNumber, const char* fieldName); 
  
  /*! Get a custom string field value for a specific frame */
  bool SetCustomString(const char* fieldName, const char* fieldValue);

  /*! Get a custom string field value (global, not for a specific frame) */
  const char* GetCustomString(const char* fieldName);

  /*! Read all the fields in the image file header */
  virtual PlusStatus ReadImageHeader();

  /*! Read pixel data from the image */
  virtual PlusStatus ReadImagePixels();

  /*! Write all the fields to the image file header */
  virtual PlusStatus OpenImageHeader();

  /*! Write pixel data to the image */
  virtual PlusStatus WriteImagePixels(const std::string& aFilename, bool forceAppend = false);

  /*! 
    Convenience function that extends the tracked frame list (if needed) to make sure
    that the requested frame is included in the list
  */
  virtual void CreateTrackedFrameIfNonExisting(unsigned int frameNumber);
  
  /*! Get the largest possible image size in the tracked frame list */
  virtual void GetMaximumImageDimensions(int maxFrameSize[3]); 

  /*! Get full path to the file for storing the pixel data */
  std::string GetPixelDataFilePath();
  /*! Conversion between ITK and METAIO pixel types */
  PlusStatus ConvertNrrdTypeToVtkPixelType(const std::string &elementTypeStr, PlusCommon::VTKScalarPixelType &vtkPixelType);
  /*! Conversion between ITK and METAIO pixel types */
  PlusStatus ConvertVtkPixelTypeToNrrdType(PlusCommon::VTKScalarPixelType vtkPixelType, std::string &elementTypeStr);

  /*! 
    Writes the compressed pixel data directly into file. 
    The compression is performed in chunks, so no excessive memory is used for the compression.
    \param outputFileStream the file stream where the compressed pixel data will be written to
    \param compressedDataSize returns the size of the total compressed data that is written to the file.
  */
  virtual PlusStatus WriteCompressedImagePixelsToFile(FILE *outputFileStream, int &compressedDataSize);

  /*! Append content of source file to the end of destination file and then delete source file */
  virtual PlusStatus AppendFile(const std::string& sourceFilename, const std::string& destFilename);

  /*! Rename file */
  virtual PlusStatus RenameFile(const char* oldname, const char* newname);

  /*! Return the size of a file */
  static FilePositionOffsetType GetFileSize(const std::string& filename);

  /*! Convert a string to an encoding */
  static NrrdEncoding StringToNrrdEncoding(const std::string& encoding);

  /*! Convert an encoding to a string*/
  static std::string NrrdEncodingToString(NrrdEncoding encoding);

private:    
  /*! Custom frame fields and image data are stored in the TrackedFrameList for each frame */
  vtkTrackedFrameList* TrackedFrameList;

  /*! Name of the file that contains the image header (*.nrrd or *.nhdr) */
  std::string FileName;
  /*! Name of the temporary file used to build up the header */
  std::string TempHeaderFileName;
  /*! Name of the temporary file used to build up the image data */
  std::string TempImageFileName;
  /*! Enable/disable zlib compression of pixel data */
  bool UseCompression;
  /*! Whether to enable pixel writing */
  bool EnableImageDataWrite;
  /*! Nrrd encoding type */
  NrrdEncoding Encoding;
  /*! Integer/float, short/long, signed/unsigned */
  PlusCommon::VTKScalarPixelType PixelType;
  /*! Number of components (or channels) */
  int NumberOfScalarComponents;
  /*! Number of image dimensions. Only 2 (single frame) or 3 (sequence of frames) or 4 (sequence of volumes) are supported. */
  int NumberOfDimensions;
  /*! Frame size (first three elements) and number of frames (last element) */
  int Dimensions[4];
  /*! Current frame offset, this is used to build up frames one addition at a time */
  int CurrentFrameOffset;
  /*! Total bytes written */
  unsigned long long TotalBytesWritten;

  /*! 
    Image orientation in memory is always MF for B-mode, but when reading/writing a file then
    any orientation can be used.
  */
  US_IMAGE_ORIENTATION ImageOrientationInFile;

  /*! 
    Image orientation for reading into memory.
  */
  US_IMAGE_ORIENTATION ImageOrientationInMemory;

  /*! 
    Image type (B-mode, RF, ...)
  */
  US_IMAGE_TYPE ImageType;

  /*! Position of the first pixel of the image data within the pixel data file */
  FilePositionOffsetType PixelDataFileOffset;
  /*! File name where the pixel data is stored */
  std::string PixelDataFileName;
  
  vtkNrrdSequenceIO(const vtkNrrdSequenceIO&); //purposely not implemented
  void operator=(const vtkNrrdSequenceIO&); //purposely not implemented
};

#endif // __vtkNrrdSequenceIO_h 

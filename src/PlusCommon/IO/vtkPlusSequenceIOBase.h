/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSequenceIOBase_h
#define __vtkPlusSequenceIOBase_h

#include "PlusCommon.h"
#include "vtkPlusCommonExport.h"
#include "PlusVideoFrame.h"
#include "vtkObject.h"

class vtkPlusTrackedFrameList;
class PlusTrackedFrame;

#ifndef Z_BUFSIZE
  #ifdef MAXSEG_64K
    #define Z_BUFSIZE 4096 /* minimize memory usage for 16-bit DOS */
  #else
    #define Z_BUFSIZE 16384
  #endif
#endif

/*!
  \class vtkPlusSequenceIOBase
  \brief Class to abstract away specific sequence file read/write details
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusSequenceIOBase : public vtkObject
{
public:
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Write object contents into file */
  virtual PlusStatus Write();

  /*! Read file contents into the object */
  virtual PlusStatus Read();

  /*! Write images to disc, compression allowed */
  virtual PlusStatus WriteImages();

  /*!
    Append the frames in tracked frame list to the header, if the onlyTrackerData flag is true it will not save
    in the header the image data related fields.
  */
  virtual PlusStatus AppendImagesToHeader() = 0;

  /*! Set the TrackedFrameList where the images are stored */
  virtual void SetTrackedFrameList(vtkPlusTrackedFrameList* trackedFrameList);
  /*! Get the TrackedFrameList where the images are stored */
  vtkGetObjectMacro(TrackedFrameList, vtkPlusTrackedFrameList);

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

  /*!
    Set input/output file name. The file contains only the image header in case of
    MHD images and the full image (including pixel data) in case of MHA images.
  */
  virtual PlusStatus SetFileName(const std::string& aFilename) = 0;
  vtkGetStdStringMacro(FileName);

  /*! Generate frame size related custom strings and store them
      This is used by vtkPlusVirtualCapture to update the final tally of frames, as it continually appends new frames to the file
      and this class to initialize the values when opening the image header
      /param numberOfFrames the new number of frames to write
      /param isData3D is the data 3D or 2D?
      */
  virtual PlusStatus UpdateDimensionsCustomStrings(int numberOfFrames, bool isData3D) = 0;

  /*! Update a field in the image header with its current value */
  virtual PlusStatus UpdateFieldInImageHeader(const char* fieldName) = 0;

  /*! Finalize the header */
  virtual PlusStatus FinalizeHeader() = 0;

  /*! Returns a pointer to a single frame */
  virtual PlusTrackedFrame* GetTrackedFrame(int frameNumber);

  /*! Close the sequence */
  virtual PlusStatus Close();

  /*! Close the sequence without saving anything (temporary files are deleted) */
  virtual PlusStatus Discard();

  /*! Prepare the sequence for writing */
  virtual PlusStatus PrepareHeader();

  /*! Return the string that represents the dimensional sizes */
  virtual const char* GetDimensionSizeString() = 0;

  /*! Return the string that represents the dimensional sizes
    This only actually exists for the nrrd writer, so in the MHA writer we return NULL
    */
  virtual const char* GetDimensionKindsString() = 0;

  /*! Flag to enable/disable compression of image data */
  vtkGetMacro(UseCompression, bool);
  /*! Flag to enable/disable compression of image data */
  vtkSetMacro(UseCompression, bool);
  /*! Flag to enable/disable compression of image data */
  vtkBooleanMacro(UseCompression, bool);

  /*! Flag to indicate that there is a time dimension */
  vtkGetMacro(IsDataTimeSeries, bool);
  /*! Flag to indicate that there is a time dimension */
  vtkSetMacro(IsDataTimeSeries, bool);
  /*! Flag to indicate that there is a time dimension */
  vtkBooleanMacro(IsDataTimeSeries, bool);

  /*! Return the dimensions of the sequence */
  std::array<unsigned int, 4> GetDimensions() const;

  /*! Flag to enable/disable writing of image data */
  vtkGetMacro(EnableImageDataWrite, bool);
  /*! Flag to enable/disable writing of image data */
  vtkSetMacro(EnableImageDataWrite, bool);
  /*! Flag to enable/disable writing of image data */
  vtkBooleanMacro(EnableImageDataWrite, bool);

protected:
  /*! Read all the fields in the image file header */
  virtual PlusStatus ReadImageHeader() = 0;

  /*! Read pixel data from the image */
  virtual PlusStatus ReadImagePixels() = 0;

  /*! Write all the fields to the sequence file header */
  virtual PlusStatus WriteInitialImageHeader() = 0;

  /*! Prepare the image file for writing */
  virtual PlusStatus PrepareImageFile() = 0;

  /*! Move file */
  virtual PlusStatus MoveFileInternal(const char* oldname, const char* newname);

  /*! Append content of source file to the end of destination file and then delete source file */
  virtual PlusStatus AppendFile(const std::string& sourceFilename, const std::string& destFilename);

  /*!
    Writes the compressed pixel data directly into file.
    The compression is performed in chunks, so no excessive memory is used for the compression.
    \param compressedDataSize returns the size of the total compressed data that is written to the file.
  */
  virtual PlusStatus WriteCompressedImagePixelsToFile(int& compressedDataSize) = 0;

  /*! Opens a file. Doesn't log error if it fails because it may be expected. */
  static PlusStatus FileOpen(FILE** stream, const char* filename, const char* flags);

  /*! Get full path to the file for storing the pixel data */
  std::string GetPixelDataFilePath();

  /*! Get the largest possible image size in the tracked frame list */
  virtual FrameSizeType GetMaximumImageDimensions();

  /*! Set a custom string field value for a specific frame */
  PlusStatus SetFrameString(int frameNumber, const char* fieldName,  const char* fieldValue);

  /*! Delete frame field from tracked frame */
  PlusStatus DeleteFrameString(int frameNumber, const char* fieldName);

  /*! Get a custom string field value for a specific frame */
  bool SetCustomString(const char* fieldName, const char* fieldValue);
  bool SetCustomString(const std::string& fieldName, const std::string& fieldValue);
  bool SetCustomString(const std::string& fieldName, int fieldValue);

  /*! Get a custom string field value (global, not for a specific frame) */
  const char* GetCustomString(const char* fieldName);
  std::string GetCustomString(const std::string& fieldName);

  /*!
    Convenience function that extends the tracked frame list (if needed) to make sure
    that the requested frame is included in the list
  */
  virtual void CreateTrackedFrameIfNonExisting(unsigned int frameNumber);

protected:
#ifdef _WIN32
  typedef __int64 FilePositionOffsetType;
#elif defined __APPLE__
  typedef off_t FilePositionOffsetType;
#else
  typedef off64_t FilePositionOffsetType;
#endif

  /*! Custom frame fields and image data are stored in the TrackedFrameList for each frame */
  vtkPlusTrackedFrameList* TrackedFrameList;

  /*! Name of the file that contains the image header (*.nrrd or *.nhdr) */
  std::string FileName;
  /*! Name of the temporary file used to build up the header */
  std::string TempHeaderFileName;
  /*! Name of the temporary file used to build up the image data */
  std::string TempImageFileName;
  /*! Enable/disable zlib compression of pixel data */
  bool UseCompression;
  /*! Buffered compressed data size */
  unsigned long long CompressedBytesWritten;
  /*! Whether to enable pixel writing */
  bool EnableImageDataWrite;
  /*! Integer/float, short/long, signed/unsigned */
  PlusCommon::VTKScalarPixelType PixelType;
  /*! Number of components (or channels) */
  unsigned int NumberOfScalarComponents;
  /*! True if there is a time dimension */
  bool IsDataTimeSeries;
  /*! Number of image dimensions. Only 2 (single frame) or 3 (sequence of frames) or 4 (sequence of volumes) are supported. */
  int NumberOfDimensions;
  /*! Frame size (first three elements) and number of frames (last element) */
  std::array<unsigned int, 4> Dimensions;
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
  /*! file handle for image output */
  FILE* OutputImageFileHandle;

protected:
  vtkPlusSequenceIOBase();
  virtual ~vtkPlusSequenceIOBase();
};

#endif // __vtkPlusSequenceIOBase_h 

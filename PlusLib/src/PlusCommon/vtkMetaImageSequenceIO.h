/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkMetaImageSequenceIO_h
#define __vtkMetaImageSequenceIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif 

#define PLUS_EXPORTS

#include "vtkPlusWin32Header.h"
#include "PlusVideoFrame.h"

#include "itkImageIOBase.h"
#include "vtkMatrix4x4.h"

class vtkTrackedFrameList;
class TrackedFrame;

/*!
  \class vtkMetaImageSequenceIO
  \brief Read and write MetaImage file with a sequence of frames, with additional information for each frame
  \ingroup PlusLibCommon
*/
class PLUS_EXPORT vtkMetaImageSequenceIO : public vtkObject
{
public:

  static vtkMetaImageSequenceIO *New();
  vtkTypeRevisionMacro(vtkMetaImageSequenceIO, vtkObject);
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

  /*! Append the frames in tracked frame list to the header */
  virtual PlusStatus AppendImagesToHeader();

  /*! Finalize the header */
  virtual PlusStatus FinalizeHeader();

  /*! Write images to disc, compression allowed */
  virtual PlusStatus WriteImages();

  /*! Append image data to the sequence, compression not allowed */
  virtual PlusStatus AppendImages();

  /*! Close the sequence */
  virtual PlusStatus Close();

  /*! Check if this class can read the specified file */
  virtual bool CanReadFile(const char*);

  /*! Returns a pointer to a single frame */
  virtual TrackedFrame* GetTrackedFrame(int frameNumber);

  /*! Update a field in the image header with its current value */
  PlusStatus UpdateFieldInImageHeader(const char* fieldName);

  /*!
    Set input/output file name. The file contains only the image header in case of
    MHD images and the full image (including pixel data) in case of MHA images.
  */
  virtual PlusStatus SetFileName(const char* aFilename);
  /*! Get input/output file name. */
  vtkGetStringMacro(FileName); 

  /*! Flag to enable/disable compression of image data */
  vtkGetMacro(UseCompression, bool);
  /*! Flag to enable/disable compression of image data */
  vtkSetMacro(UseCompression, bool);
  /*! Flag to enable/disable compression of image data */
  vtkBooleanMacro(UseCompression, bool);

  /*! Return the dimensions of the sequence */
  vtkGetMacro(Dimensions, int*);

protected:
  vtkMetaImageSequenceIO();
  virtual ~vtkMetaImageSequenceIO();

  /*! Opens a file. Doesn't log error if it fails because it may be expected. */
  PlusStatus FileOpen(FILE **stream, const char* filename, const char* flags);

  /*! Set a custom string field value for a specific frame */
  PlusStatus SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue);

  /*! Delete custom frame field from tracked frame */
  PlusStatus DeleteCustomFrameString(int frameNumber, const char* fieldName); 
  
  /*! Get a custom string field value for a specific frame */
  bool SetCustomString(const char* fieldName, const char* fieldValue);

  /*! Get a custom string field value (global, not for a specific frame) */
  const char* GetCustomString(const char* fieldName);

  /*! Read all the fields in the metaimage file header */
  virtual PlusStatus ReadImageHeader();

  /*! Read pixel data from the metaimage */
  virtual PlusStatus ReadImagePixels();

  /*! Write all the fields to the metaimage file header */
  virtual PlusStatus OpenImageHeader();

  /*! Write pixel data to the metaimage */
  virtual PlusStatus WriteImagePixels(char* aFilename, bool forceAppend = false);

  /*! 
    Convenience function that extends the tracked frame list (if needed) to make sure
    that the requested frame is included in the list
  */
  virtual void CreateTrackedFrameIfNonExisting(unsigned int frameNumber);
  
  /*! Get the largest possible image size in the tracked frame list */
  virtual void GetMaximumImageDimensions(int maxFrameSize[2]); 

  /*! Set file name for storing the pixel data */
  vtkSetStringMacro(PixelDataFileName); 
  /*! Get file name for storing the pixel data */
  vtkGetStringMacro(PixelDataFileName); 

  vtkSetStringMacro(TempHeaderFileName); 
  vtkGetStringMacro(TempHeaderFileName); 

  vtkSetStringMacro(TempImageFileName); 
  vtkGetStringMacro(TempImageFileName); 

  /*! Get full path to the file for storing the pixel data */
  std::string GetPixelDataFilePath();
  /*! Conversion between ITK and METAIO pixel types */
  PlusStatus ConvertMetaElementTypeToItkPixelType(const std::string &elementTypeStr, PlusCommon::ITKScalarPixelType &itkPixelType);
  /*! Conversion between ITK and METAIO pixel types */
  PlusStatus ConvertItkPixelTypeToMetaElementType(PlusCommon::ITKScalarPixelType itkPixelType, std::string &elementTypeStr);

  /*! 
    Writes the compressed pixel data directly into file. 
    The compression is performed in chunks, so no excessive memory is used for the compression.
    \param outputFileStream the file stream where the compressed pixel data will be written to
    \param compressedDataSize returns the size of the total compressed data that is written to the file.
  */
  virtual PlusStatus WriteCompressedImagePixelsToFile(FILE *outputFileStream, int &compressedDataSize);

  /*! Copy from file A to B */
  virtual PlusStatus MoveDataInFiles(const char* sourceFilename, const char* destFilename, bool append);
private:

#ifdef _WIN32
  typedef __int64 FilePositionOffsetType;
#elif defined __APPLE__
  typedef off_t FilePositionOffsetType;
#else
  typedef off64_t FilePositionOffsetType;
#endif
    
  /*! Custom frame fields and image data are stored in the m_FrameList for each frame */
  vtkTrackedFrameList* TrackedFrameList;

  /*! Name of the file that contains the image header (*.MHA or *.MHD) */
  char* FileName;
  /*! Name of the temporary file used to build up the header */
  char* TempHeaderFileName;
  /*! Name of the temporary file used to build up the image data */
  char* TempImageFileName;
  /*! Enable/disable zlib compression of pixel data */
  bool UseCompression;
  /*! ASCII or binary */
  itk::ImageIOBase::FileType FileType;
  /*! Integer/float, short/long, signed/unsigned */
  PlusCommon::ITKScalarPixelType PixelType;
  /*! Number of components (or channels). Only single-component images are supported. */
  int NumberOfComponents;
  /*! Number of image dimensions. Only 2 (single frame) or 3 (sequence of frames) are supported. */
  int NumberOfDimensions;
  /*! Frame size (first two elements) and number of frames (last element) */
  int Dimensions[3];
  /*! Current frame offset, this is used to build up frames one addition at a time */
  int m_CurrentFrameOffset;
  /*! Total bytes written */
  unsigned long long m_TotalBytesWritten;

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
  char* PixelDataFileName;
  
  vtkMetaImageSequenceIO(const vtkMetaImageSequenceIO&); //purposely not implemented
  void operator=(const vtkMetaImageSequenceIO&); //purposely not implemented
};

#endif // __vtkMetaImageSequenceIO_h 

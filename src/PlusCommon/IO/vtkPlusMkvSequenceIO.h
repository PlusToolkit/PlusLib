/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusMkvSequenceIO_h
#define __vtkPlusMkvSequenceIO_h

#include "vtkPlusCommonExport.h"

// PlusLib includes
#include "vtkPlusSequenceIOBase.h"

class vtkPlusTrackedFrameList;

/*!
  \class vtkPlusMkvSequenceIO
  \brief Read and write a matroska file with a sequence of frames, with additional information for each frame, stored in subtitle tracks
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusMkvSequenceIO : public vtkPlusSequenceIOBase
{
public:
  static vtkPlusMkvSequenceIO* New();
  vtkTypeMacro(vtkPlusMkvSequenceIO, vtkPlusSequenceIOBase);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
    Update the number of frames in the header
    This is used primarily by vtkPlusVirtualCapture to update the final tally of frames, as it continually appends new frames to the file
    /param numberOfFrames the new number of frames to write
    /param isData3D is the data 3D or 2D?
  */
  virtual PlusStatus UpdateDimensionsCustomStrings(int numberOfFrames, bool isData3D);

  /*!
    Append the frames in tracked frame list to the header, if the onlyTrackerData flag is true it will not save
    in the header the image data related fields.
  */
  virtual PlusStatus AppendImagesToHeader();

  /*! Finalize the header */
  virtual PlusStatus FinalizeHeader() { return PLUS_SUCCESS; }

  /*! Close the sequence */
  virtual PlusStatus Close();

  /*! Check if this class can read the specified file */
  static bool CanReadFile(const std::string& filename);

  /*! Check if this class can write the specified file */
  static bool CanWriteFile(const std::string& filename);

  /*! Update a field in the image header with its current value */
  virtual PlusStatus UpdateFieldInImageHeader(const char* fieldName) { return PLUS_SUCCESS; }

  /*! Return the string that represents the dimensional sizes */
  virtual const char* GetDimensionSizeString() { return ""; }

  /*! Return the string that represents the dimensional kinds */
  virtual const char* GetDimensionKindsString() { return ""; }

  /*!
    Set input/output file name. The file contains only the image header in case of
    MHD images and the full image (including pixel data) in case of MHA images.
  */
  virtual PlusStatus SetFileName(const std::string& aFilename);

protected:
  vtkPlusMkvSequenceIO();
  virtual ~vtkPlusMkvSequenceIO();

  /*! Read all the fields in the metaimage file header */
  virtual PlusStatus ReadImageHeader();

  /*! Read pixel data from the metaimage */
  virtual PlusStatus ReadImagePixels();

  /*! Prepare the image file for writing */
  virtual PlusStatus PrepareImageFile();

  /*! Write all the fields to the metaimage file header */
  virtual PlusStatus WriteInitialImageHeader();

  virtual PlusStatus WriteImages();

  virtual PlusStatus PrepareHeader() { return PLUS_SUCCESS; };

  /*!
    Writes the compressed pixel data directly into file.
    The compression is performed in chunks, so no excessive memory is used for the compression.
    \param aFilename the file where the compressed pixel data will be written to
    \param compressedDataSize returns the size of the total compressed data that is written to the file.
  */
  virtual PlusStatus WriteCompressedImagePixelsToFile(int& compressedDataSize);

public:
  void SetEncodingFourCC(std::string encodingFourCC);
  std::string GetEncodingFourCC();

protected:
  vtkPlusMkvSequenceIO(const vtkPlusMkvSequenceIO&); //purposely not implemented
  void operator=(const vtkPlusMkvSequenceIO&); //purposely not implemented

  class vtkInternal;
  vtkInternal* Internal;
};

#endif // __vtkPlusMkvSequenceIO_h 

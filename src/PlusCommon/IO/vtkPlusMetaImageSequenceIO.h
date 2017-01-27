/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusMetaImageSequenceIO_h
#define __vtkPlusMetaImageSequenceIO_h

#include "vtkPlusCommonExport.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif 

#include "vtkPlusSequenceIOBase.h"
#include "itk_zlib.h"

class vtkPlusTrackedFrameList;

/*!
  \class vtkPlusMetaImageSequenceIO
  \brief Read and write MetaImage file with a sequence of frames, with additional information for each frame
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusMetaImageSequenceIO : public vtkPlusSequenceIOBase
{
public:
  static vtkPlusMetaImageSequenceIO *New();
  vtkTypeMacro(vtkPlusMetaImageSequenceIO, vtkPlusSequenceIOBase);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Accessors to control 2D Dims output */
  vtkSetMacro(Output2DDataWithZDimensionIncluded, bool);
  vtkGetMacro(Output2DDataWithZDimensionIncluded, bool);

  /*! Update the number of frames in the header 
      This is used primarily by vtkPlusVirtualDiscCapture to update the final tally of frames, as it continually appends new frames to the file
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
  virtual PlusStatus FinalizeHeader();

  /*! Close the sequence */
  virtual PlusStatus Close();

  /*! Check if this class can read the specified file */
  static bool CanReadFile(const std::string& filename);

  /*! Check if this class can write the specified file */
  static bool CanWriteFile(const std::string& filename);

  /*! Update a field in the image header with its current value */
  virtual PlusStatus UpdateFieldInImageHeader(const char* fieldName);

  /*! Return the string that represents the dimensional sizes */
  virtual const char* GetDimensionSizeString();

    /*! Return the string that represents the dimensional kinds */
  virtual const char* GetDimensionKindsString();

  /*!
    Set input/output file name. The file contains only the image header in case of
    MHD images and the full image (including pixel data) in case of MHA images.
  */
  virtual PlusStatus SetFileName(const std::string& aFilename);

protected:
  vtkPlusMetaImageSequenceIO();
  virtual ~vtkPlusMetaImageSequenceIO();

  /*! Read all the fields in the metaimage file header */
  virtual PlusStatus ReadImageHeader();

  /*! Read pixel data from the metaimage */
  virtual PlusStatus ReadImagePixels();

  /*! Prepare the image file for writing */
  virtual PlusStatus PrepareImageFile();

  /*! Write all the fields to the metaimage file header */
  virtual PlusStatus WriteInitialImageHeader();

  /*! 
    Writes the compressed pixel data directly into file. 
    The compression is performed in chunks, so no excessive memory is used for the compression.
    \param aFilename the file where the compressed pixel data will be written to
    \param compressedDataSize returns the size of the total compressed data that is written to the file.
  */
  virtual PlusStatus WriteCompressedImagePixelsToFile(int &compressedDataSize);

  /*! Conversion between ITK and METAIO pixel types */
  PlusStatus ConvertMetaElementTypeToVtkPixelType(const std::string &elementTypeStr, PlusCommon::VTKScalarPixelType &vtkPixelType);
  /*! Conversion between ITK and METAIO pixel types */
  PlusStatus ConvertVtkPixelTypeToMetaElementType(PlusCommon::VTKScalarPixelType vtkPixelType, std::string &elementTypeStr);

private:
  /*! ASCII or binary */
  bool IsPixelDataBinary;
  /*! If 2D data, boolean to determine if we should write out in the form X Y Nfr (false) or X Y 1 Nfr (true) */
  bool Output2DDataWithZDimensionIncluded;
  /*! compression stream handle for compression streaming */
  z_stream CompressionStream;

protected:
  vtkPlusMetaImageSequenceIO(const vtkPlusMetaImageSequenceIO&); //purposely not implemented
  void operator=(const vtkPlusMetaImageSequenceIO&); //purposely not implemented
};

#endif // __vtkPlusMetaImageSequenceIO_h 

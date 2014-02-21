/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/


#ifndef __PlusVideoFrame_h
#define __PlusVideoFrame_h

#include "PlusCommon.h"
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "vtkImageData.h"
#include "vtkImageExport.h"
#include "vtkSmartPointer.h"

/*!
\enum US_IMAGE_ORIENTATION
\brief Defines constant values for ultrasound image orientation
The ultrasound image axes are defined as follows:
\li x axis: points towards the x coordinate increase direction
\li y axis: points towards the y coordinate increase direction
The image orientation can be defined by specifying which transducer axis corresponds to the x and y image axes, respectively.
\ingroup PlusLibCommon
*/
enum US_IMAGE_ORIENTATION
{
  US_IMG_ORIENT_XX,  /*!< undefined */
  US_IMG_ORIENT_UF, /*!< image x axis = unmarked transducer axis, image y axis = far transducer axis */
  US_IMG_ORIENT_UN, /*!< image x axis = unmarked transducer axis, image y axis = near transducer axis */
  US_IMG_ORIENT_MF, /*!< image x axis = marked transducer axis, image y axis = far transducer axis */
  US_IMG_ORIENT_MN, /*!< image x axis = marked transducer axis, image y axis = near transducer axis */
  US_IMG_ORIENT_FU, /*!< image x axis = far transducer axis, image y axis = unmarked transducer axis (usually for RF frames)*/
  US_IMG_ORIENT_NU, /*!< image x axis = near transducer axis, image y axis = unmarked transducer axis (usually for RF frames)*/
  US_IMG_ORIENT_FM, /*!< image x axis = far transducer axis, image y axis = marked transducer axis (usually for RF frames)*/
  US_IMG_ORIENT_NM, /*!< image x axis = near transducer axis, image y axis = marked transducer axis (usually for RF frames)*/  
  US_IMG_ORIENT_LAST   /*!< just a placeholder for range checking, this must be the last defined orientation item */  
}; 

/*!
\enum US_IMAGE_TYPE
\brief Defines constant values for ultrasound image type  
\ingroup PlusLibCommon
*/
enum US_IMAGE_TYPE
{
  US_IMG_TYPE_XX,    /*!< undefined */
  US_IMG_BRIGHTNESS, /*!< B-mode image */
  US_IMG_RF_REAL,    /*!< RF-mode image, signal is stored as a series of real values */
  US_IMG_RF_IQ_LINE, /*!< RF-mode image, signal is stored as a series of I and Q samples in a line (I1, Q1, I2, Q2, ...) */  
  US_IMG_RF_I_LINE_Q_LINE, /*!< RF-mode image, signal is stored as a series of I samples in a line, then Q samples in the next line (I1, I2, ..., Q1, Q2, ...) */  
  US_IMG_TYPE_LAST   /*!< just a placeholder for range checking, this must be the last defined image type */  
}; 

/*!
\class PlusVideoFrame 
\brief Store images in a variety of pixel formats
An ITK image can only store a certain pixel type.
This class can store an ITK image with any pixel type and has convenient
functions to get/set its content from ITK and VTK images and byte arrays.
\ingroup PlusLibPlusCommon
\sa vtkPlusDataBufferItem
*/
class VTK_EXPORT PlusVideoFrame
{
public:
  struct FlipInfoType
  {
    FlipInfoType() : hFlip(false), vFlip(false), doubleColumn(false), doubleRow(false) {};
    bool hFlip; // flip the image horizontally (pixel columns are reordered)
    bool vFlip; // flip the image vertically (pixel rows are reordered)
    bool doubleColumn; // keep pairs of pixel columns together (for RF_IQ_LINE encoded images)
    bool doubleRow; // keep pairs of pixel rows together (for RF_I_LINE_Q_LINE encoded images)
  };

  /*! Constructor */
  PlusVideoFrame();

  /*! Destructor */
  virtual ~PlusVideoFrame();

  /*! Copy constructor */
  PlusVideoFrame(const PlusVideoFrame& DataBufferItem);

  /*! Equality operator */
  PlusVideoFrame& operator=(PlusVideoFrame const&videoItem); 

  /*! Allocate memory for the image. The image object must be already created. */
  static PlusStatus AllocateFrame(vtkImageData* image, const int imageSize[2], PlusCommon::VTKScalarPixelType vtkScalarPixelType); 
  PlusStatus AllocateFrame(const int imageSize[2], PlusCommon::VTKScalarPixelType vtkScalarPixelType); 

  /*! Return the pixel type using VTK enums. */
  PlusCommon::VTKScalarPixelType GetVTKScalarPixelType() const;

  /*! Convert between ITK and VTK scalar pixel types */  
  static PlusCommon::VTKScalarPixelType GetVTKScalarPixelType(PlusCommon::ITKScalarPixelType pixelType);

  /* Return a string version of the VTK type */
  static std::string GetStringFromVTKPixelType(PlusCommon::VTKScalarPixelType vtkScalarPixelType);

#ifdef PLUS_USE_OpenIGTLink
  /*! Convert between ITK and IGTL scalar pixel types */
  static PlusCommon::IGTLScalarPixelType GetIGTLScalarPixelTypeFromVTK(PlusCommon::VTKScalarPixelType vtkScalarPixelType); 
#endif

#ifdef PLUS_USE_OpenIGTLink
  /*! Convert between IGTL and ITK scalar pixel types */
  static PlusCommon::VTKScalarPixelType GetVTKScalarPixelTypeFromIGTL(PlusCommon::IGTLScalarPixelType igtlPixelType);
#endif

  /*! Return the image orientation */
  US_IMAGE_ORIENTATION GetImageOrientation() const;

  /*! Set the image orientation. Does not reorder the pixels. */
  void SetImageOrientation(US_IMAGE_ORIENTATION imgOrientation);

  /*! Return the image type */
  US_IMAGE_TYPE GetImageType() const;

  /*! Set the image orientation. Does not do any conversion. */
  void SetImageType(US_IMAGE_TYPE imgType);

  /*! Get the size of the pixel in bytes (e.g., returns 1 for 8-bit images) */
  int GetNumberOfBytesPerPixel() const;

  /*! Get the size of a scalar pixel in bytes */
  static int GetNumberOfBytesPerPixel(PlusCommon::VTKScalarPixelType pixelType);

  /*! Get the dimensions of the frame in pixels */
  PlusStatus GetFrameSize(int frameSize[2]) const;

  /*! Get the pointer to the pixel buffer */
  void* GetScalarPointer() const;

  /*! Get the pixel buffer size in bytes */
  unsigned long GetFrameSizeInBytes() const; 

  /*! Get the VTK image, does not copy the pixel buffer */
  vtkImageData* GetImage() const;

  /*! Copy pixel data from another PlusVideoFrame object, same as operator= */
  PlusStatus DeepCopy(PlusVideoFrame* DataBufferItem); 

  /*! Sets the pixel buffer content by copying pixel data from a vtkImageData object.*/
  PlusStatus DeepCopyFrom(vtkImageData* frame);

  /*! Get US_IMAGE_ORIENTATION enum value from string */
  static US_IMAGE_ORIENTATION GetUsImageOrientationFromString( const char* imgOrientationStr ); 

  /*! Get US_IMAGE_ORIENTATION string value from enum */
  static const char* GetStringFromUsImageOrientation(US_IMAGE_ORIENTATION imgOrientation);

  /*! Get US_IMAGE_TYPE enum value from string */
  static US_IMAGE_TYPE GetUsImageTypeFromString( const char* imgTypeStr );

  /*! Get US_IMAGE_ORIENTATION string value from enum */
  static const char* GetStringFromUsImageType(US_IMAGE_TYPE imgType);

  /*! Read unsigned char type image file to PlusVideoFrame */
  static PlusStatus ReadImageFromFile( PlusVideoFrame &frame, const char* fileName); 

  /*! Save PlusVideoFrame to image file */
  //static PlusStatus SaveImageToFile( PlusVideoFrame &frame, const char* fileName); 

  /*! Save unsigned char itk image to image file */
  //static PlusStatus SaveImageToFile(const itk::Image< unsigned char, 2 >::Pointer image, const char* fileName); 

  /*! Save unsigned char vtk image to image file */
  //static PlusStatus SaveImageToFile(vtkImageData* frame, const char* fileName); 

  /*! Save image file */
  //static PlusStatus SaveImageToFile(unsigned char* imageDataPtr, const int frameSizeInPx[2], int numberOfBitsPerPixel, const char* fileName); 

  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION outUsImageOrientation, vtkImageData* outUsOrientedImage ); 

  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, PlusCommon::VTKScalarPixelType inUsImagePixelType, const int frameSizeInPx[2], US_IMAGE_ORIENTATION outUsImageOrientation, vtkImageData* outUsOrientedImage); 

  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, PlusCommon::VTKScalarPixelType inUsImagePixelType, const int frameSizeInPx[2], US_IMAGE_ORIENTATION outUsImageOrientation, PlusVideoFrame &outBufferItem);

  static PlusStatus GetFlipAxes(US_IMAGE_ORIENTATION usImageOrientation1, US_IMAGE_TYPE usImageType1, US_IMAGE_ORIENTATION usImageOrientation2, FlipInfoType& flipInfo);

  /*! 
  Flip a 2D image along one or two axes. This is a performance optimized version of flipping that does not use ITK filters 
  \param doubleRow If this flag is set to true then pairs of rows are kept together. This is needed for flipping RF images with US_IMG_RF_I_LINE_Q_LINE encoding, where one scanline is encoded as a pair of I and Q lines in the image. 
  */
  static PlusStatus FlipImage(vtkImageData* inUsImage, const FlipInfoType& flipInfo, vtkImageData* outUsOrientedImage);

  /*! Return true if the image data is valid (e.g. not NULL) */
  bool IsImageValid() const
  {
    return this->Image != NULL;
  }

  /*! Fill the actual image data with black pixels (0) */
  PlusStatus FillBlank();

  /*! Convert vtkImageData to itkImage */
  template<typename PixelType> static PlusStatus DeepCopyVtkImageToItkImage(vtkImageData* inFrame, typename itk::Image< PixelType, 2 >::Pointer outFrame);

protected:
  
  /*! Get the image width and height in pixels */
  static PlusStatus GetImageSize(vtkImageData* imageData, int* imageSize);
  
  void SetImageData(vtkImageData* imageData);

  vtkImageData* Image;
  US_IMAGE_TYPE ImageType;
  US_IMAGE_ORIENTATION ImageOrientation;
};

#include "PlusVideoFrame.txx"

#endif

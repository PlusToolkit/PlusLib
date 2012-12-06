/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/


#ifndef __PlusVideoFrame_h
#define __PlusVideoFrame_h

#include "itkImage.h"
#include "itkImageIOBase.h"
#include "PlusCommon.h"

class vtkImageData;
class vtkImageImport;

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
  typedef itk::ImageBase< 2 >                    ImageBaseType;
  typedef ImageBaseType::Pointer                 ImageBasePointer;
  typedef ImageBaseType::ConstPointer            ImageBaseConstPointer;  

  /*! Utility function to get the pixel type of an ImageBaseType object */
  static PlusCommon::ITKScalarPixelType GetITKScalarPixelType(ImageBaseType* itkImageBase);

  /*! Constructor */
  PlusVideoFrame();

  /*! Destructor */
  virtual ~PlusVideoFrame();

  /*! Copy constructor */
  PlusVideoFrame(const PlusVideoFrame& DataBufferItem);

  /*! Equality operator */
  PlusVideoFrame& operator=(PlusVideoFrame const&videoItem); 

  /*! Allocate memory for the image */
  PlusStatus AllocateFrame(int imageSize[2], PlusCommon::ITKScalarPixelType pixType); 

  /*! Return the pixel type using VTK enums. */
  PlusCommon::VTKScalarPixelType GetVTKScalarPixelType() const;

  /*! Return the pixel type using ITK enums. */
  PlusCommon::ITKScalarPixelType GetITKScalarPixelType() const;

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

  /*! Get the dimensions of the frame in pixels */
  PlusStatus GetFrameSize(int frameSize[2]) const;

  /*! Get the pointer to the pixel buffer */
  void* GetBufferPointer() const;

  /*! Get the pixel buffer size in bytes */
  unsigned long GetFrameSizeInBytes() const; 

  /*! 
    Get frame that can be displayed on the screen (with 8-bit unsigned char pixels)
    If the stored image pixel type is unsigned char (such as B-mode post beamforming image) then it just returns the stored image.
    If the stored image pixel type is short (such as RF image) then it returns an image that is obtained by rescaling the short image
    into an unsigned char range. An alternative implementation could be to apply a Hilbert transform on 16-bit images (that would transform RF images to B-mode images)
    If the stored image pixel type is any othar than (unsigned char) and (short) then it returns a NULL pointer.
  */
  itk::Image<unsigned char, 2>::Pointer GetDisplayableImage() const;

  /*! Get frame in an ITK image, does not copy the pixel buffer */
  template <class OutputPixelType> 
  typename itk::Image<OutputPixelType, 2>::Pointer GetImage() const
  {  
    return dynamic_cast< itk::Image<OutputPixelType, 2> * >( this->ItkImage.GetPointer() );
  }

  /*! Get frame in an ITK image, does not copy the pixel buffer */
  template <class OutputImageType> 
  PlusStatus GetImage(typename OutputImageType::Pointer &frame) const
  {
    ImageBaseType * itkImageBase = this->ItkImage.GetPointer();
    typename OutputImageType::Pointer requestedItkImage = dynamic_cast< OutputImageType * >( itkImageBase );
    if (requestedItkImage.IsNull())
    {
      LOG_ERROR("Cannot convert the frame data to the requested displayable frame data");
      return PLUS_FAIL;
    }
    frame=requestedItkImage;
    return PLUS_SUCCESS;  
  }

  /*! Return the ITK image base. This is independent of the pixel type */
  const ImageBaseType* GetITKImageBase() const;

  /*! 
    Gets a VTK image pointer to the image buffer
    Note that the same pixel buffer is used as for the ITK image,
    therefore no image flip and no pixel copying is performed.
  */
  vtkImageData* GetVtkImage();

  /*! Copy pixel data from another PlusVideoFrame object, same as operator= */
  PlusStatus DeepCopy(PlusVideoFrame* DataBufferItem); 

  /*! Sets the pixel buffer content by copying pixel data from a vtkImageData object.*/
  PlusStatus DeepCopyFrom(vtkImageData* frame); 

  /*! Sets the pixel buffer content from an ITK image, does not copy the pixel buffer */
  void SetITKImageBase( ImageBaseType * );

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
  static PlusStatus SaveImageToFile( PlusVideoFrame &frame, const char* fileName); 
  
  /*! Save unsigned char itk image to image file */
  static PlusStatus SaveImageToFile(const itk::Image< unsigned char, 2 >::Pointer image, const char* fileName); 

  /*! Save unsigned char vtk image to image file */
  static PlusStatus SaveImageToFile(vtkImageData* frame, const char* fileName); 
  
  /*! Save image file */
  static PlusStatus SaveImageToFile(unsigned char* imageDataPtr, const int frameSizeInPx[2], int numberOfBitsPerPixel, const char* fileName); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, vtkImageData* outUsOrientedImage ); 

  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image< unsigned char, 2 >::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<unsigned char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned char, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<char, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<unsigned short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned short, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<short, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<unsigned int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned int, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<int, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<unsigned long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned long, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<long, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<float, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<float, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( const itk::Image<double, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<double, 2>::Pointer outUsOrientedImage ); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  template<class OutputImageType> 
  static PlusStatus GetOrientedImageGeneric( const typename OutputImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_ORIENTATION outUsImageOrientation, typename OutputImageType::Pointer outUsOrientedImage ); 

  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned char, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<char, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned short, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<short, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned int, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<int, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<unsigned long, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<long, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<float, 2>::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, itk::Image<double, 2>::Pointer outUsOrientedImage); 
    
  /*! Convert oriented image to MF oriented ultrasound image */
  template<class OutputImageType> 
  static PlusStatus GetOrientedImageGeneric( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION outUsImageOrientation, typename OutputImageType::Pointer outUsOrientedImage); 
  
  /*! Convert oriented image to MF oriented ultrasound image */
  static PlusStatus GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, US_IMAGE_ORIENTATION outUsImageOrientation, PlusVideoFrame &outBufferItem);

  /*! Convert vtkImageData to itkImage */
  static PlusStatus ConvertVtkImageToItkImage(vtkImageData* inFrame, itk::Image< unsigned char, 2 >::Pointer outFrame); 

  static PlusStatus GetFlipAxes(US_IMAGE_ORIENTATION usImageOrientation1, US_IMAGE_ORIENTATION usImageOrientation2, itk::FixedArray<bool, 2> &flipAxes);

  /*! Flip a 2D image along one or two axes. This is a performance optimized version of flipping that does not use ITK filters */
  template<class OutputImageType> 
  static PlusStatus FlipImage(const typename OutputImageType::Pointer inUsImage, const itk::FixedArray<bool, 2> &flipAxes, typename OutputImageType::Pointer outUsOrientedImage);

  /*! Convert between VTK and ITK scalar pixel types */
  static PlusCommon::ITKScalarPixelType GetITKScalarPixelType(PlusCommon::VTKScalarPixelType vtkScalarPixelType);

  /*! Convert between ITK and VTK scalar pixel types */  
  static PlusCommon::VTKScalarPixelType GetVTKScalarPixelType(PlusCommon::ITKScalarPixelType pixelType);

#ifdef PLUS_USE_OpenIGTLink
  /*! Convert between ITK and IGTL scalar pixel types */
  static PlusCommon::IGTLScalarPixelType GetIGTLScalarPixelType(PlusCommon::ITKScalarPixelType pixelType); 
#endif

#ifdef PLUS_USE_OpenIGTLink
  /*! Convert between IGTL and ITK scalar pixel types */
  static PlusCommon::ITKScalarPixelType GetITKScalarPixelTypeFromIGTL(PlusCommon::IGTLScalarPixelType igtlPixelType);
#endif

  /*! Get the size of a scalar pixel in bytes */
  static int GetNumberOfBytesPerPixel(PlusCommon::ITKScalarPixelType pixelType);

  /*! Return true if the image data is valid (e.g. not NULL) */
  bool IsImageValid()
  {
    return (!this->ItkImage.IsNull()); 
  }

  /*! Fill the actual image data with black pixels (0) */
  PlusStatus FillBlank(); 

protected:
  /*! 
    Connect ITK and VTK pipelines 
    \sa PipelineCreator
  */
  void UpdateVtkImage();

private:
  ImageBasePointer ItkImage;
  US_IMAGE_TYPE ImageType;
  US_IMAGE_ORIENTATION ImageOrientation;

  itk::ProcessObject::Pointer Exporter;
  vtkImageImport* Importer;
};

//#include "PlusVideoFrame.cxx"

#endif

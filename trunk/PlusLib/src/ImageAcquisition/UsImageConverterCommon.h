#ifndef __USIMAGECONVERTERCOMMON_H
#define __USIMAGECONVERTERCOMMON_H

#include "PlusConfigure.h"
#include "itkImage.h" 
#include "itkImageFileWriter.h"
#include "PlusVideoFrame.h"

class vtkImageData; 
class PlusVideoFrame;

// The ultrasound image axes are defined as follows:
// - x axis: points towards the x coordinate increase direction
// - y axis: points towards the y coordinate increase direction
// The image orientation can be defined by specifying which transducer axis corresponds to the x and y image axes, respectively.
enum US_IMAGE_ORIENTATION
{
  US_IMG_ORIENT_UF, // image x axis = unmarked transducer axis, image y axis = far transducer axis
  US_IMG_ORIENT_UN, // image x axis = unmarked transducer axis, image y axis = near transducer axis
  US_IMG_ORIENT_MF, // image x axis = marked transducer axis, image y axis = far transducer axis
  US_IMG_ORIENT_MN, // image x axis = marked transducer axis, image y axis = near transducer axis
  US_IMG_ORIENT_XX  // undefined
}; 

class VTK_EXPORT UsImageConverterCommon
{
public:

  //! Operation: 
  // Get US_IMAGE_ORIENTATION enum value from string 
  static US_IMAGE_ORIENTATION GetUsImageOrientationFromString( const char* usImgOrientation ); 

  //! Operation: 
  // Get US_IMAGE_ORIENTATION enum value from string 
  static const char* GetStringFromUsImageOrientation(US_IMAGE_ORIENTATION orientation);

  //! Operation: 
  // Save image to file
  static PlusStatus SaveImageToFile(const itk::Image< unsigned char, 2 >::Pointer image, const char* fileName); 
  static PlusStatus SaveImageToFile(unsigned char* imageDataPtr, const int frameSizeInPx[2], int numberOfBitsPerPixel, const char* fileName); 
  static PlusStatus SaveImageToJpeg(vtkImageData* image, const char* fileName); 

  //! Operation: 
  // Convert oriented image to MF oriented ultrasound image 
  static PlusStatus GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, vtkImageData* outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image< unsigned char, 2 >::Pointer& outUsOrintedImage ); 
  
  static PlusStatus GetMFOrientedImage( const itk::Image<unsigned char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned char, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<char, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<unsigned short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned short, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<short, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<unsigned int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned int, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<int, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<unsigned long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned long, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<long, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<float, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<float, 2>::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const itk::Image<double, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<double, 2>::Pointer& outUsOrintedImage ); 
  template<class OutputImageType> 
  static PlusStatus GetMFOrientedImageGeneric( typename const OutputImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, typename OutputImageType::Pointer& outUsOrintedImage ); 

  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned char, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<char, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned short, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<short, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned int, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<int, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned long, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<long, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<float, 2>::Pointer& outUsOrintedImage); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<double, 2>::Pointer& outUsOrintedImage); 
  template<class OutputImageType> 
  static PlusStatus GetMFOrientedImageGeneric( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, typename OutputImageType::Pointer& outUsOrintedImage); 

  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, PlusVideoFrame &outBufferItem);

  // Description:
  // Convert vtkImageData to itkImage 
  // Flipping is necessary, because vtk stores the image data in bottom up format, while itk in top down format. 
  static PlusStatus ConvertVtkImageToItkImage(vtkImageData* inFrame, itk::Image< unsigned char, 2 >::Pointer& outFrame); 

  // Description:
  // Convert itkImage to vtkImageData 
  // Flipping is necessary, because vtk stores the image data in bottom up format, while itk in top down format. 
  static PlusStatus ConvertItkImageToVtkImage(const itk::Image< unsigned char, 2 >::Pointer& inFrame, vtkImageData* outFrame); 

  // Description:
  // Flip a 2D image along one or two axes
  // This is a performance optimized version of flipping that does not use ITK filters 
  template<class OutputImageType> 
  static PlusStatus FlipImage(typename const OutputImageType::Pointer inUsImage, const itk::FixedArray<bool, 2> &flipAxes, typename OutputImageType::Pointer& outUsOrintedImage);

  // Description:
  // Convert between ITK and VTK scalar pixel types
  // This is a performance optimized version of flipping that does not use ITK filters 
  static PlusCommon::ITKScalarPixelType GetITKScalarPixelType(PlusCommon::VTKScalarPixelType vtkScalarPixelType);
  static PlusCommon::VTKScalarPixelType GetVTKScalarPixelType(PlusCommon::ITKScalarPixelType pixelType);

  // Description:
  // Get the size of a scalar pixel in bytes
  static int GetNumberOfBytesPerPixel(PlusCommon::ITKScalarPixelType pixelType);

protected:
  UsImageConverterCommon(); 
  ~UsImageConverterCommon();

private: 
  UsImageConverterCommon(UsImageConverterCommon const&);
  UsImageConverterCommon& operator=(UsImageConverterCommon const&);

};

#endif
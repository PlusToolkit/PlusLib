#ifndef __USIMAGECONVERTERCOMMON_H
#define __USIMAGECONVERTERCOMMON_H

#include "PlusConfigure.h"
#include "itkImage.h" 
#include "itkImageFileWriter.h"

class vtkImageData; 

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

  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, 2 > ImageType;

  //! Operation: 
  // Get US_IMAGE_ORIENTATION enum value from string 
  static US_IMAGE_ORIENTATION GetUsImageOrientationFromString( const char* usImgOrientation ); 

  //! Operation: 
  // Save image to file
  static PlusStatus SaveImageToFile(const ImageType::Pointer image, const char* fileName); 
  static PlusStatus SaveImageToFile(unsigned char* imageDataPtr, const int frameSizeInPx[2], int numberOfBitsPerPixel, const char* fileName); 
  static PlusStatus SaveImageToJpeg(vtkImageData* image, const char* fileName); 

  //! Operation: 
  // Convert oriented image to MF oriented ultrasound image 
  static PlusStatus GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, vtkImageData* outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, ImageType::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( const ImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, ImageType::Pointer& outUsOrintedImage ); 
  static PlusStatus GetMFOrientedImage( unsigned char* imageDataPtr,                               
    US_IMAGE_ORIENTATION  inUsImageOrientation, 
    const int frameSizeInPx[2],
    int numberOfBitsPerPixel, 
    ImageType::Pointer& outUsOrintedImage); 

  /*static PlusStatus GetMFOrientedImage( const unsigned char* inImageDataPtr, 
  US_IMAGE_ORIENTATION inUsImageOrientation,
  const int    frameSizeInPx[3],
  const int    numberOfBitsPerPixel, 
  const int	numberOfBytesToSkip,
  vtkImageData* outUsOrintedImage ); */

  // Description:
  // Convert vtkImageData to itkImage 
  // Flipping is necessary, because vtk stores the image data in bottom up format, while itk in top down format. 
  static PlusStatus ConvertVtkImageToItkImage(vtkImageData* inFrame, ImageType::Pointer& outFrame); 

  // Description:
  // Convert itkImage to vtkImageData 
  // Flipping is necessary, because vtk stores the image data in bottom up format, while itk in top down format. 
  static PlusStatus ConvertItkImageToVtkImage(const ImageType::Pointer& inFrame, vtkImageData* outFrame); 

  // Description:
  // Flip a 2D image along one or two axes
  // This is a performance optimized version of flipping that does not use ITK filters 
  static PlusStatus FlipImage(const ImageType::Pointer inUsImage, const itk::FixedArray<bool, 2> &flipAxes, ImageType::Pointer& outUsOrintedImage);

protected:
  UsImageConverterCommon(); 
  ~UsImageConverterCommon();

private: 
  UsImageConverterCommon(UsImageConverterCommon const&);
  UsImageConverterCommon& operator=(UsImageConverterCommon const&);

};

#endif
/*========================================================================*/
// .NAME vtkPlusVideoFrame - Store images in a variety of pixel formats
// .SECTION Description
// vtkPlusVideoFrame is a structure for storing images in formats.
// An ITK image can only store a certain pixel type.
// This class can store an ITK image with any pixel type and has convenient
// functions to get/set its content from ITK and VTK images and byte arrays.
// .SECTION See Also
// vtkPlusVideoBufferItem

#ifndef __PlusVideoFrame_h
#define __PlusVideoFrame_h

#include "itkImage.h"
#include "itkImageIOBase.h"

class vtkImageData;

class VTK_EXPORT PlusVideoFrame
{
public:
  typedef itk::ImageBase< 2 >                    ImageBaseType;
  typedef ImageBaseType::Pointer                 ImageBasePointer;
  typedef ImageBaseType::ConstPointer            ImageBaseConstPointer;  

  // Description:
  // Utility function to get the pixel type of an ImageBaseType object
  static PlusCommon::ITKScalarPixelType GetITKScalarPixelType(ImageBaseType* itkImageBase);

  PlusVideoFrame();
  virtual ~PlusVideoFrame();

  PlusVideoFrame(const PlusVideoFrame& videoBufferItem); 
  PlusVideoFrame& operator=(PlusVideoFrame const&videoItem); 

  // Description:
  // Allocate memory for the image
  PlusStatus AllocateFrame(int imageSize[2], PlusCommon::ITKScalarPixelType pixType); 

  // Description:
  // Return the pixel type using VTK enums.
  PlusCommon::VTKScalarPixelType GetVTKScalarPixelType();

  // Description:
  // Return the pixel type using ITK enums.
  PlusCommon::ITKScalarPixelType GetITKScalarPixelType() const;

  // Description:
  // Get the size of the pixel in bytes (e.g., returns 1 for 8-bit images)
  int GetNumberOfBytesPerPixel() const;

  // Description:
  // Get the dimensions of the frame in pixels
  PlusStatus GetFrameSize(int frameSize[2]) const;

  // Description:
  // Get the pointer to the pixel buffer
  void* GetBufferPointer() const;

  // Description:
  // Get the pixel buffer size in bytes
  unsigned long GetFrameSizeInBytes() const; 

  // Description:
  // Get frame that can be displayed on the screen (with 8-bit unsigned char pixels)
  // If the stored image pixel type is unsigned char (such as B-mode post beamforming image) then it just returns the stored image.
  // If the stored image pixel type is short (such as RF image) then it returns an image that is obtained by rescaling the short image
  // into an unsigned char range. An alternative implementation could be to apply a Hilbert transform on 16-bit images (that would transform RF images to B-mode images)
  // If the stored image pixel type is any othar than (unsigned char) and (short) then it returns a NULL pointer.
  itk::Image<unsigned char, 2>::Pointer GetDisplayableImage() const;

  // Description:
  // Get frame in an ITK image, does not copy the pixel buffer
  template <class OutputPixelType> 
  typename itk::Image<OutputPixelType, 2>::Pointer GetImage() const
  {  
    return dynamic_cast< itk::Image<OutputPixelType, 2> * >( this->ItkImage.GetPointer() );
  }

  // Description:
  // Get frame in an ITK image, does not copy the pixel buffer
  template <class OutputImageType> 
  PlusStatus PlusVideoFrame::GetImage(typename OutputImageType::Pointer &frame) const
  {
    ImageBaseType * itkImageBase = this->ItkImage.GetPointer();
    OutputImageType::Pointer requestedItkImage = dynamic_cast< OutputImageType * >( itkImageBase );
    if (requestedItkImage.IsNull())
    {
      LOG_ERROR("Cannot convert the frame data to the requested displayable frame data");
      return PLUS_FAIL;
    }
    frame=requestedItkImage;
    return PLUS_SUCCESS;  
  }

  // Return the ITK image base. This is independent of the pixel type
  const ImageBaseType* GetITKImageBase() const;

  // Description:
  // Copy pixel data to a VTK image, copies the pixel buffer
  PlusStatus CopyToVtkImage( vtkImageData* targetFrame );

  // Description:
  // Copy pixel data from another PlusVideoFrame object, same as operator=
  PlusStatus DeepCopy(PlusVideoFrame* videoBufferItem); 

  // Description:
  // Sets the pixel buffer content by copying pixel data from a vtkImageData object
  PlusStatus SetFrame(vtkImageData* frame); 

  // Description:
  // Sets the pixel buffer content by copying pixel data from a pixel buffer
  PlusStatus SetFrame(unsigned char *imageDataPtr, const int frameSizeInPx[3], int numberOfBitsPerPixel, int	numberOfBytesToSkip ); 

  // Description:
  // Sets the pixel buffer content from an ITK image, does not copy the pixel buffer
  void SetITKImageBase( ImageBaseType * );

private:
  ImageBasePointer ItkImage;

};

#endif

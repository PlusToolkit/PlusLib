#include "PlusConfigure.h"

#include "UsImageConverterCommon.h"
#include "vtkPlusLogger.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkImageExport.h"
#include "vtkImageImport.h"
#include "vtkJPEGWriter.h"

#include "itkFlipImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkImportImageContainer.h"


//----------------------------------------------------------------------------
UsImageConverterCommon::UsImageConverterCommon()
{

}

//----------------------------------------------------------------------------
UsImageConverterCommon::~UsImageConverterCommon()
{

}

//----------------------------------------------------------------------------
US_IMAGE_ORIENTATION UsImageConverterCommon::GetUsImageOrientationFromString( const char* usImgOrientation )
{
  US_IMAGE_ORIENTATION imageorientation = US_IMG_ORIENT_XX; 
  if ( usImgOrientation == NULL )
  {
    return imageorientation; 
  }
  else if ( STRCASECMP(usImgOrientation, "UF" ) == 0 )
  {
    imageorientation = US_IMG_ORIENT_UF; 
  }
  else if ( STRCASECMP(usImgOrientation, "UN" ) == 0 )
  {
    imageorientation = US_IMG_ORIENT_UN; 
  }
  else if ( STRCASECMP(usImgOrientation, "MF" ) == 0 )
  {
    imageorientation = US_IMG_ORIENT_MF; 
  }
  else if ( STRCASECMP(usImgOrientation, "MN" ) == 0 )
  {
    imageorientation = US_IMG_ORIENT_MN; 
  }

  return imageorientation; 
}

const char* UsImageConverterCommon::GetStringFromUsImageOrientation(US_IMAGE_ORIENTATION orientation)
{
  switch (orientation)
  {
  case US_IMG_ORIENT_MF: return "MF";
  case US_IMG_ORIENT_MN: return "MN";
  case US_IMG_ORIENT_UF: return "UF";
  case US_IMG_ORIENT_UN: return "UN";
  default:
    return "XX";
  }
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, vtkImageData* outUsOrintedImage )
{
  if ( inUsImage == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrintedImage == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  if ( inUsImageOrientation == US_IMG_ORIENT_XX ) 
  {
    LOG_WARNING("Failed to convert image data to MF orientation - unknown input image orientation, return identical copy!"); 
    outUsOrintedImage->ShallowCopy( inUsImage ); 
    return PLUS_SUCCESS; 
  }

  if ( inUsImageOrientation == US_IMG_ORIENT_MF )
  {
    outUsOrintedImage->ShallowCopy( inUsImage ); 
    return PLUS_SUCCESS; 
  }

  vtkSmartPointer<vtkImageFlip> imageFlipX = vtkSmartPointer<vtkImageFlip>::New(); 
  imageFlipX->FlipAboutOriginOff();
  imageFlipX->SetFilteredAxis(0); 

  vtkSmartPointer<vtkImageFlip> imageFlipY = vtkSmartPointer<vtkImageFlip>::New(); 
  imageFlipY->FlipAboutOriginOff();
  imageFlipY->SetFilteredAxis(1); 

  switch( inUsImageOrientation )
  {
  case US_IMG_ORIENT_UF: 
    {
      imageFlipX->SetInput(inUsImage); 
      imageFlipX->Update(); 
      outUsOrintedImage->ShallowCopy(imageFlipX->GetOutput());
    }
    break; 
  case US_IMG_ORIENT_UN: 
    {
      imageFlipX->SetInput(inUsImage); 
      imageFlipX->Update(); 

      imageFlipY->SetInput(imageFlipX->GetOutput()); 
      imageFlipY->Update(); 

      outUsOrintedImage->ShallowCopy(imageFlipY->GetOutput());
    }
    break; 
  case US_IMG_ORIENT_MN: 
    {
      imageFlipY->SetInput(inUsImage); 
      imageFlipY->Update(); 
      outUsOrintedImage->ShallowCopy(imageFlipY->GetOutput());
    }
    break; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image< unsigned char, 2 >::Pointer& outUsOrintedImage )
{
  itk::Image< unsigned char, 2 >::Pointer itkimage = itk::Image< unsigned char, 2 >::New(); 
  if ( UsImageConverterCommon::ConvertVtkImageToItkImage(inUsImage, itkimage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get MF oriented image from vtk image data!"); 
    return PLUS_FAIL;
  }

  return UsImageConverterCommon::GetMFOrientedImage(itkimage, inUsImageOrientation, outUsOrintedImage); 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned char, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned char, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<char, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<char, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned short, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned short, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<short, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<short, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned int, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned int, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<int, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<int, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned long, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned long, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<long, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<long, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<float, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<float, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<double, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<double, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus UsImageConverterCommon::GetMFOrientedImageGeneric( unsigned char* imageDataPtr,                               
                                                      US_IMAGE_ORIENTATION  inUsImageOrientation, 
                                                      const int    frameSizeInPx[2],
                                                      int    numberOfBitsPerPixel, 
                                                      typename OutputImageType::Pointer& outUsOrintedImage
                                                      )
{

  if ( imageDataPtr == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrintedImage.IsNull() )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  if ( numberOfBitsPerPixel != sizeof(OutputImageType::PixelType)*8 )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - pixel size mismatch (input: " << numberOfBitsPerPixel << " bits, output: " << sizeof(OutputImageType::PixelType)*8 << " bits)!"); 
    return PLUS_FAIL; 
  }

  OutputImageType::Pointer inUsImage = OutputImageType::New(); 
  OutputImageType::SizeType size = {frameSizeInPx[0], frameSizeInPx[1]};
  OutputImageType::IndexType start = {0,0};
  OutputImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  inUsImage->SetRegions(region);
  OutputImageType::PixelContainer::Pointer pixelContainer = OutputImageType::PixelContainer::New(); 
  pixelContainer->SetImportPointer(reinterpret_cast<OutputImageType::PixelType*>(imageDataPtr), frameSizeInPx[0]*frameSizeInPx[1], false); 
  inUsImage->SetPixelContainer(pixelContainer); 

  return UsImageConverterCommon::GetMFOrientedImage(inUsImage, inUsImageOrientation, outUsOrintedImage); 
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus UsImageConverterCommon::FlipImage(typename const OutputImageType::Pointer inUsImage, const itk::FixedArray<bool, 2> &flipAxes, typename OutputImageType::Pointer& outUsOrintedImage)
{
  outUsOrintedImage->SetOrigin(inUsImage->GetOrigin());
  outUsOrintedImage->SetSpacing(inUsImage->GetSpacing());
  outUsOrintedImage->SetDirection(inUsImage->GetDirection());
  outUsOrintedImage->SetLargestPossibleRegion(inUsImage->GetLargestPossibleRegion());
  outUsOrintedImage->SetRequestedRegion(inUsImage->GetRequestedRegion());
  outUsOrintedImage->SetBufferedRegion(inUsImage->GetBufferedRegion());

  try 
  {
    outUsOrintedImage->Allocate(); 
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  OutputImageType::SizeType imageSize=inUsImage->GetLargestPossibleRegion().GetSize();
  int width=imageSize[0];
  int height=imageSize[1];

  if (!flipAxes[0] && flipAxes[1])
  {
    // flip Y    
    OutputImageType::PixelType *inputPixel=inUsImage->GetBufferPointer();
    // Set the target position pointer to the first pixel of the last row
    OutputImageType::PixelType *outputPixel=outUsOrintedImage->GetBufferPointer()+width*(height-1);
    // Copy the image row-by-row, reversing the row order
    for (int y=height; y>0; y--)
    {
      memcpy(outputPixel, inputPixel, width);
      inputPixel+=width;
      outputPixel-=width;
    }
  }
  else if (flipAxes[0] && !flipAxes[1])
  {
    // flip X    
    OutputImageType::PixelType *inputPixel=inUsImage->GetBufferPointer();
    // Set the target position pointer to the last pixel of the first row
    OutputImageType::PixelType *outputPixel=outUsOrintedImage->GetBufferPointer()+width-1;
    // Copy the image row-by-row, reversing the pixel order in each row
    for (int y=height; y>0; y--)
    {
      for (int x=width; x>0; x--)
      {
        *outputPixel=*inputPixel;
        inputPixel++;
        outputPixel--;
      }
      outputPixel+=2*width;
    }
  }
  else if (flipAxes[0] && flipAxes[1])
  {
    // flip X and Y
    OutputImageType::PixelType *inputPixel=inUsImage->GetBufferPointer();
    // Set the target position pointer to the last pixel
    OutputImageType::PixelType *outputPixel=outUsOrintedImage->GetBufferPointer()+height*width-1;
    // Copy the image pixel-by-pixel, reversing the pixel order
    for (int p=width*height; p>0; p--)
    {
      *outputPixel=*inputPixel;
      inputPixel++;
      outputPixel--;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
static PlusStatus SaveImageToFile(unsigned char* imageDataPtr, const int frameSizeInPx[2], int numberOfBitsPerPixel, const char* fileName)
{

  if ( numberOfBitsPerPixel != sizeof(unsigned char)*8 )
  {
    LOG_ERROR("Failed to save image to file - pixel size mismatch (expected: " <<  sizeof(unsigned char)*8 << " bits, current: " << numberOfBitsPerPixel << " bits)!)"); 
    return PLUS_FAIL; 
  }

  itk::Image< unsigned char, 2 >::Pointer inUsImage = itk::Image< unsigned char, 2 >::New(); 
  itk::Image< unsigned char, 2 >::SizeType size = {frameSizeInPx[0], frameSizeInPx[1]};
  itk::Image< unsigned char, 2 >::IndexType start = {0,0};
  itk::Image< unsigned char, 2 >::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  inUsImage->SetRegions(region);
  itk::Image< unsigned char, 2 >::PixelContainer::Pointer pixelContainer = itk::Image< unsigned char, 2 >::PixelContainer::New(); 
  pixelContainer->SetImportPointer(imageDataPtr, frameSizeInPx[0]*frameSizeInPx[1], false); 
  inUsImage->SetPixelContainer(pixelContainer); 

  return UsImageConverterCommon::SaveImageToFile(inUsImage, "_IncomingFrame.jpg"); 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::SaveImageToFile(const itk::Image< unsigned char, 2 >::Pointer image, const char* fileName)
{
  if ( image.IsNull() )
  {
    LOG_ERROR("Failed to save image to file, input image is NULL!"); 
    return PLUS_FAIL; 
  }

  typedef itk::ImageFileWriter< itk::Image< unsigned char, 2 > > ImgWriterType;
  ImgWriterType::Pointer writer = ImgWriterType::New();
  writer->SetFileName( fileName ); 
  writer->SetInput( image ); 

  try    
  {
    writer->Update();
  }	
  catch (itk::ExceptionObject & e)
  {
    LOG_ERROR(e.GetDescription());
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 

}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::SaveImageToJpeg(vtkImageData* image, const char* fileName)
{
  if ( image == NULL )
  {
    LOG_ERROR("Failed to save image to file, input image is NULL!"); 
    return PLUS_FAIL; 
  }
  
  vtkSmartPointer<vtkJPEGWriter> writer = vtkSmartPointer<vtkJPEGWriter>::New(); 
  writer->SetInput(image); 
  writer->SetFileName(fileName); 
  writer->Write(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<unsigned char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned char, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<unsigned char, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<char, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<char, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<unsigned short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned short, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<unsigned short, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<short, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<short, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<unsigned int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned int, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<unsigned int, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<int, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<int, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<unsigned long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned long, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<unsigned long, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<long, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<long, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<float, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<float, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<float, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const itk::Image<double, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<double, 2>::Pointer& outUsOrintedImage )
{
  return UsImageConverterCommon::GetMFOrientedImageGeneric<itk::Image<double, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}

//----------------------------------------------------------------------------
template<class OutputImageType>
static PlusStatus UsImageConverterCommon::GetMFOrientedImageGeneric( typename const OutputImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, typename OutputImageType::Pointer& outUsOrintedImage )
{
  if ( inUsImage.IsNull() )
  {
    LOG_ERROR("Failed to convert image data MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrintedImage.IsNull() )
  {
    LOG_ERROR("Failed to convert image data MF orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  outUsOrintedImage->SetOrigin(inUsImage->GetOrigin());
  outUsOrintedImage->SetSpacing(inUsImage->GetSpacing());
  outUsOrintedImage->SetDirection(inUsImage->GetDirection());
  outUsOrintedImage->SetLargestPossibleRegion(inUsImage->GetLargestPossibleRegion());
  outUsOrintedImage->SetRequestedRegion(inUsImage->GetRequestedRegion());
  outUsOrintedImage->SetBufferedRegion(inUsImage->GetBufferedRegion());

  try 
  {
    outUsOrintedImage->Allocate(); 
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }


  if ( inUsImageOrientation == US_IMG_ORIENT_XX ) 
  {
    LOG_WARNING("Failed to convert image data MF orientation - unknown input image orientation, return identical copy!"); 
    // We need to copy the raw data since we're using the image array as an OutputImageType::PixelContainer
    long bufferSize = inUsImage->GetLargestPossibleRegion().GetSize()[0]*inUsImage->GetLargestPossibleRegion().GetSize()[1]*sizeof(OutputImageType::PixelType); 
    memcpy(outUsOrintedImage->GetBufferPointer(), inUsImage->GetBufferPointer(), bufferSize); 
    return PLUS_SUCCESS; 
  }

  if ( inUsImageOrientation == US_IMG_ORIENT_MF )
  {
    // We need to copy the raw data since we're using the image array as an OutputImageType::PixelContainer
    long bufferSize = inUsImage->GetLargestPossibleRegion().GetSize()[0]*inUsImage->GetLargestPossibleRegion().GetSize()[1]*sizeof(OutputImageType::PixelType); 
    memcpy(outUsOrintedImage->GetBufferPointer(), inUsImage->GetBufferPointer(), bufferSize); 
    return PLUS_SUCCESS; 
  }

  itk::FixedArray<bool, 2> flipAxes;
  switch( inUsImageOrientation )
  {
  case US_IMG_ORIENT_UF: 
    {
      flipAxes[0] = true;
      flipAxes[1] = false;
    }
    break; 
  case US_IMG_ORIENT_UN: 
    {
      flipAxes[0] = true;
      flipAxes[1] = true;
    }
    break; 
  case US_IMG_ORIENT_MN: 
    {
      flipAxes[0] = false;
      flipAxes[1] = true;
    }
    break; 
  }

  // Performance profiling showed that flip image filter is very slow,
  // therefore, an alternative implementation was tried, which does not use this filter.
  // Execution time of the alternative implementation in releaes mode does not seem to be
  // much faster, so for now keep using the flip image filter.
  const bool useItkFlipImageFilter=false;

  if (useItkFlipImageFilter)
  {
    typedef itk::FlipImageFilter <OutputImageType> FlipImageFilterType;
    FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New();
    flipFilter->SetInput(inUsImage);
    flipFilter->FlipAboutOriginOff(); 
    flipFilter->SetFlipAxes(flipAxes);
    flipFilter->Update();
    // We need to copy the raw data since we're using the image array as an OutputImageType::PixelContainer
    long bufferSize = flipFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]*flipFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1]*sizeof(OutputImageType::PixelType); 
    memcpy(outUsOrintedImage->GetBufferPointer(), flipFilter->GetOutput()->GetBufferPointer(), bufferSize); 
    return PLUS_SUCCESS; 
  }
  else
  {
    return FlipImage<OutputImageType>(inUsImage, flipAxes, outUsOrintedImage);
  }   
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::ConvertItkImageToVtkImage(const itk::Image< unsigned char, 2 >::Pointer& inFrame, vtkImageData* outFrame)
{
  if ( inFrame.IsNull() )
  {
    LOG_ERROR("Failed to convert itk image to vtk image - input image is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( outFrame == NULL )
  {
    LOG_ERROR("Failed to convert itk image to vtk image - output image is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetWholeExtent(0, inFrame->GetLargestPossibleRegion().GetSize()[0] - 1, 0, inFrame->GetLargestPossibleRegion().GetSize()[1] - 1, 0, 0);
  importer->SetDataExtentToWholeExtent(); 
  importer->SetDataScalarTypeToUnsignedChar(); 
  importer->SetImportVoidPointer(inFrame->GetBufferPointer()); 
  importer->SetNumberOfScalarComponents(1); 
  importer->Update(); 

  // VTK convention is for the image voxel index (0,0,0) to be the lower-left corner of the image, so we need to flip it
  vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New(); 
  flipY->SetFilteredAxis(1); 
  flipY->SetInput( importer->GetOutput() ); 
  flipY->Update(); 

  outFrame->DeepCopy( flipY->GetOutput() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::ConvertVtkImageToItkImage(vtkImageData* inFrame, itk::Image< unsigned char, 2 >::Pointer& outFrame)
{
  LOG_TRACE("UsImageConverterCommon::ConvertVtkImageToItkImage"); 

  if ( inFrame == NULL )
  {
    LOG_ERROR("Failed to convert vtk image to itk image - input image is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( outFrame.IsNull() )
  {
    LOG_ERROR("Failed to convert vtk image to itk image - output image is NULL!"); 
    return PLUS_FAIL; 
  }

  // convert vtkImageData to itkImage 
  vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
  imageExport->ImageLowerLeftOff();
  imageExport->SetInput(inFrame); 
  imageExport->Update(); 

  int extent[6]= {0}; 
  inFrame->GetExtent(extent); 

  double width = extent[1] - extent[0] + 1; 
  double height = extent[3] - extent[2] + 1; 
  itk::Image< unsigned char, 2 >::SizeType size = { width, height };
  itk::Image< unsigned char, 2 >::IndexType start = {0,0};
  itk::Image< unsigned char, 2 >::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  outFrame->SetRegions(region);
  try 
  {
    outFrame->Allocate();
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  imageExport->Export( outFrame->GetBufferPointer() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, PlusVideoFrame &outBufferItem)
{
  PlusStatus status=PLUS_FAIL;
  int bitsPerPixel=UsImageConverterCommon::GetNumberOfBytesPerPixel(pixelType)*8;
  switch (pixelType)
  {
  case itk::ImageIOBase::UCHAR:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned char>());
    break;
  case itk::ImageIOBase::CHAR:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<char>());
    break;
  case itk::ImageIOBase::USHORT:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned short>());
    break;
  case itk::ImageIOBase::SHORT:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<short>());
    break;
  case itk::ImageIOBase::UINT:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned int>());
    break;
  case itk::ImageIOBase::INT:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<int>());
    break;
  case itk::ImageIOBase::ULONG:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned long>());
    break;
  case itk::ImageIOBase::LONG:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<long>());
    break;
  case itk::ImageIOBase::FLOAT:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<float>());
    break;
  case itk::ImageIOBase::DOUBLE:
    status=UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<double>());
    break;
  default:
    {
      LOG_ERROR("Unsupported pixel type: "<<pixelType); 
      return PLUS_FAIL; 
    }
  }
  return status;
}

//----------------------------------------------------------------------------
int UsImageConverterCommon::GetNumberOfBytesPerPixel(PlusCommon::ITKScalarPixelType pixelType)
{
  switch (pixelType)
  {
    case itk::ImageIOBase::UCHAR: return sizeof(unsigned char);
    case itk::ImageIOBase::CHAR: return sizeof(char);
    case itk::ImageIOBase::USHORT: return sizeof(unsigned short);
    case itk::ImageIOBase::SHORT: return sizeof(short);
    case itk::ImageIOBase::UINT: return sizeof(unsigned int);
    case itk::ImageIOBase::INT: return sizeof(int);
    case itk::ImageIOBase::ULONG: return sizeof(unsigned long);
    case itk::ImageIOBase::LONG: return sizeof(long);
    case itk::ImageIOBase::FLOAT: return sizeof(float);
    case itk::ImageIOBase::DOUBLE: return sizeof(double);
    default:
      LOG_ERROR("GetNumberOfBytesPerPixel: unknown pixel type "<<pixelType);
      return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  }
}

//----------------------------------------------------------------------------
int UsImageConverterCommon::GetVTKScalarPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  switch (pixelType)
  {
  case itk::ImageIOBase::UCHAR: return VTK_UNSIGNED_CHAR;
  case itk::ImageIOBase::CHAR: return VTK_CHAR;
  case itk::ImageIOBase::USHORT: return VTK_UNSIGNED_SHORT;
  case itk::ImageIOBase::SHORT: return VTK_SHORT;
  case itk::ImageIOBase::UINT: return VTK_UNSIGNED_INT;
  case itk::ImageIOBase::INT: return VTK_INT;
  case itk::ImageIOBase::ULONG: return VTK_UNSIGNED_LONG;
  case itk::ImageIOBase::LONG: return VTK_LONG;
  case itk::ImageIOBase::FLOAT: return VTK_FLOAT;
  case itk::ImageIOBase::DOUBLE: return VTK_DOUBLE;
  case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
  default:
    return VTK_VOID;      
  }
}


//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType UsImageConverterCommon::GetITKScalarPixelType(int vtkScalarPixelType)
{
  switch (vtkScalarPixelType)
  {
  case VTK_UNSIGNED_CHAR: return itk::ImageIOBase::UCHAR;
  case VTK_CHAR: return itk::ImageIOBase::CHAR;
  case VTK_UNSIGNED_SHORT: return itk::ImageIOBase::USHORT;
  case VTK_SHORT: return itk::ImageIOBase::SHORT;
  case VTK_UNSIGNED_INT: return itk::ImageIOBase::UINT;
  case VTK_INT: return itk::ImageIOBase::INT;
  case VTK_UNSIGNED_LONG: return itk::ImageIOBase::ULONG;
  case VTK_LONG: return itk::ImageIOBase::LONG;
  case VTK_FLOAT: return itk::ImageIOBase::FLOAT;
  case VTK_DOUBLE: return itk::ImageIOBase::DOUBLE;
  default:
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;      
  }
}


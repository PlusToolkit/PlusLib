#include "UsImageConverterCommon.h"

#include "PlusLogger.h"
#include "PlusConfigure.h"
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
PlusStatus UsImageConverterCommon::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, ImageType::Pointer& outUsOrintedImage )
{
  ImageType::Pointer itkimage = ImageType::New(); 
  if ( UsImageConverterCommon::ConvertVtkImageToItkImage(inUsImage, itkimage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get MF oriented image from vtk image data!"); 
    return PLUS_FAIL;
  }

  return UsImageConverterCommon::GetMFOrientedImage(itkimage, inUsImageOrientation, outUsOrintedImage); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( unsigned char* imageDataPtr,                               
                                                      US_IMAGE_ORIENTATION  inUsImageOrientation, 
                                                      const int    frameSizeInPx[2],
                                                      int    numberOfBitsPerPixel )
{

  if ( imageDataPtr == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

   if ( inUsImageOrientation == US_IMG_ORIENT_XX ) 
  {
    LOG_WARNING("Failed to convert image data MF orientation - unknown input image orientation, return identical copy!"); 
    return PLUS_SUCCESS; 
  }

  if ( inUsImageOrientation == US_IMG_ORIENT_MF )
  {
    return PLUS_SUCCESS; 
  }

  long bufferSize = frameSizeInPx[0]*frameSizeInPx[1]*(numberOfBitsPerPixel/8.0); 

  ImageType::Pointer image = ImageType::New(); 
  ImageType::SizeType size = {frameSizeInPx[0], frameSizeInPx[1]};
  ImageType::IndexType start = {0,0};
  ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  image->SetRegions(region);

  try 
  {
    image->Allocate(); 
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  memcpy(image->GetBufferPointer(), imageDataPtr, bufferSize); 

  ImageType::Pointer orientedImage = ImageType::New(); 
  ImageType::PixelContainer::Pointer pixelContainer = ImageType::PixelContainer::New(); 
  pixelContainer->SetImportPointer(imageDataPtr, frameSizeInPx[0]*frameSizeInPx[1], false); 
  orientedImage->SetPixelContainer(pixelContainer); 

  return UsImageConverterCommon::GetMFOrientedImage(image, inUsImageOrientation, orientedImage); 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::FlipImage(const UsImageConverterCommon::ImageType::Pointer inUsImage, const itk::FixedArray<bool, 2> &flipAxes, UsImageConverterCommon::ImageType::Pointer& outUsOrintedImage)
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

  ImageType::SizeType imageSize=inUsImage->GetLargestPossibleRegion().GetSize();
  int width=imageSize[0];
  int height=imageSize[1];

  if (!flipAxes[0] && flipAxes[1])
  {
    // flip Y    
    ImageType::PixelType *inputPixel=inUsImage->GetBufferPointer();
    // Set the target position pointer to the first pixel of the last row
    ImageType::PixelType *outputPixel=outUsOrintedImage->GetBufferPointer()+width*(height-1);
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
    ImageType::PixelType *inputPixel=inUsImage->GetBufferPointer();
    // Set the target position pointer to the last pixel of the first row
    ImageType::PixelType *outputPixel=outUsOrintedImage->GetBufferPointer()+width-1;
    // Copy the image row-by-row, reversing the pixel order in each row
    for (int y=height; y>0; y--)
    {
      for (int x=width; x>0; x--)
      {
        *outputPixel=*inputPixel;
        inputPixel++;
        outputPixel--;
      }
      outputPixel+=2*width-1;
    }
  }
  else if (flipAxes[0] && flipAxes[1])
  {
    // flip X and Y
    ImageType::PixelType *inputPixel=inUsImage->GetBufferPointer();
    // Set the target position pointer to the last pixel
    ImageType::PixelType *outputPixel=outUsOrintedImage->GetBufferPointer()+height*width-1;
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
PlusStatus UsImageConverterCommon::SaveImageToFile(const ImageType::Pointer image, const char* fileName)
{
  if ( image.IsNull() )
  {
    LOG_ERROR("Failed to save image to file, input image is NULL!"); 
    return PLUS_FAIL; 
  }

  typedef itk::ImageFileWriter< ImageType > ImgWriterType;
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
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const ImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, ImageType::Pointer& outUsOrintedImage )
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
    outUsOrintedImage = inUsImage; 
    return PLUS_SUCCESS; 
  }

  if ( inUsImageOrientation == US_IMG_ORIENT_MF )
  {
    outUsOrintedImage = inUsImage; 
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
  const bool useItkFlipImageFilter=true;

  if (useItkFlipImageFilter)
  {
    typedef itk::FlipImageFilter <ImageType> FlipImageFilterType;
    FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New();
    flipFilter->SetInput(inUsImage);
    flipFilter->FlipAboutOriginOff(); 
    flipFilter->SetFlipAxes(flipAxes);
    flipFilter->Update();
    // We need to copy the raw data since we're using the image array as an ImageType::PixelContainer
    long bufferSize = flipFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]*flipFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1]*sizeof(PixelType); 
    memcpy(outUsOrintedImage->GetBufferPointer(), flipFilter->GetOutput()->GetBufferPointer(), bufferSize); 
    return PLUS_SUCCESS; 
  }
  else
  {
    return FlipImage(inUsImage, flipAxes, outUsOrintedImage);
  }   
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::ConvertItkImageToVtkImage(const ImageType::Pointer& inFrame, vtkImageData* outFrame)
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
PlusStatus UsImageConverterCommon::ConvertVtkImageToItkImage(vtkImageData* inFrame, ImageType::Pointer& outFrame)
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
  ImageType::SizeType size = { width, height };
  ImageType::IndexType start = {0,0};
  ImageType::RegionType region;
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


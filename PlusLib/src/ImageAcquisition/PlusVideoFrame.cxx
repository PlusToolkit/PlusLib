#include "PlusConfigure.h"
#include "PlusVideoFrame.h"

#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "itkNormalizeImageFilter.h"
#include "itkVTKImageExport.h"

#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "itkImageBase.h"
#include "UsImageConverterCommon.h"

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame()
{
}

//----------------------------------------------------------------------------
PlusVideoFrame::~PlusVideoFrame()
{
}

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame(const PlusVideoFrame &videoItem)
{
  *this = videoItem;
}

//----------------------------------------------------------------------------
PlusVideoFrame& PlusVideoFrame::operator=(PlusVideoFrame const&videoItem)
{
  // Handle self-assignment
  if (this == &videoItem)
  {
    return *this;
  }

  // Copy the pixels. Don't use image duplicator, because that wouldn't reuse the existing buffer
  if ( videoItem.GetFrameSizeInBytes()>0)
  {
    int frameSize[2] = {0,0};
    videoItem.GetFrameSize(frameSize);

    if ( this->AllocateFrame(frameSize, videoItem.GetITKScalarPixelType()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to allocate memory for the new frame in the buffer!"); 
    }
    else
    {
      memcpy(this->GetBufferPointer(), videoItem.GetBufferPointer(), this->GetFrameSizeInBytes() ); 
    }
  }
  else
  {
    itk::Image<unsigned char,2>::Pointer emptyImage;
    SetITKImageBase(emptyImage);
  }

  return *this;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::DeepCopy(PlusVideoFrame* videoItem)
{
  if ( videoItem == NULL )
  {
    LOG_ERROR("Failed to deep copy video buffer item - buffer item NULL!"); 
    return PLUS_FAIL; 
  }

  (*this)=(*videoItem);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// Helper method, duplicates the image base if the specified pixel type matches the actual pixel type 
template <class TPixel >
PlusStatus CreateAndAllocateImageGeneric(PlusVideoFrame::ImageBasePointer &resultImage, int imageSize[2])
{
  typedef itk::Image< TPixel, 2> ImageType;

  ImageType::Pointer image = ImageType::New();

  PlusVideoFrame::ImageBaseType::SizeType size = {imageSize[0], imageSize[1]};
  PlusVideoFrame::ImageBaseType::IndexType start = {0,0};
  PlusVideoFrame::ImageBaseType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  image->SetRegions(region);

  try
  {
    image->Allocate();
  }
  catch (itk::ExceptionObject & err) 
  {		
    LOG_ERROR("Unable to allocate memory for image: " << err.GetDescription() );
    return PLUS_FAIL;
  }	

  resultImage=image;

  return PLUS_SUCCESS;
};


//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::AllocateFrame(int imageSize[2], PlusCommon::ITKScalarPixelType pixType)
{  
  if ( !this->ItkImage.IsNull() )
  {
    ImageBaseType::RegionType region=this->ItkImage->GetLargestPossibleRegion();
    if (imageSize[0] == region.GetSize()[0] &&
        imageSize[1] == region.GetSize()[1] &&
        GetITKScalarPixelType() == pixType)
    {
      // already allocated, no change
      return PLUS_SUCCESS;
    }        
  }

  // delete the old image
  this->ItkImage=NULL;

  PlusStatus status=PLUS_SUCCESS;
  switch (pixType)
  {
    case itk::ImageIOBase::UCHAR: status=CreateAndAllocateImageGeneric<unsigned char>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::CHAR: status=CreateAndAllocateImageGeneric<char>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::USHORT: status=CreateAndAllocateImageGeneric<unsigned short>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::SHORT: status=CreateAndAllocateImageGeneric<short>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::UINT: status=CreateAndAllocateImageGeneric<unsigned int>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::INT: status=CreateAndAllocateImageGeneric<int>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::ULONG: status=CreateAndAllocateImageGeneric<unsigned long>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::LONG: status=CreateAndAllocateImageGeneric<long>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::FLOAT: status=CreateAndAllocateImageGeneric<float>(this->ItkImage, imageSize); break;
    case itk::ImageIOBase::DOUBLE: status=CreateAndAllocateImageGeneric<double>(this->ItkImage, imageSize); break;
    default:
      LOG_ERROR("AllocateFrame: unknown pixel type "<<pixType);
      return PLUS_FAIL;
  }

  return status; 
}
//----------------------------------------------------------------------------
unsigned long PlusVideoFrame::GetFrameSizeInBytes() const
{
  if (this->ItkImage.IsNull())
  {
    return 0;
  }
  int frameSize[2] = {0,0};
  GetFrameSize(frameSize);

  int bytesPerPixel = GetNumberOfBytesPerPixel(); 
  if (bytesPerPixel!=1 && bytesPerPixel!=2)
  {
    LOG_ERROR("Unsupported pixel size: "<<bytesPerPixel<<" bytes/pixel");
  }
  unsigned long frameSizeInBytes = frameSize[0] * frameSize[1] * bytesPerPixel;
  return frameSizeInBytes; 
}

//----------------------------------------------------------------------------
itk::Image<unsigned char, 2>::Pointer PlusVideoFrame::GetDisplayableImage() const
{ 
  ImageBaseType * itkImageBase = this->ItkImage.GetPointer();
  itk::Image<unsigned char, 2>::Pointer requestedItkImage = dynamic_cast< itk::Image<unsigned char, 2> * >( itkImageBase );
  if (requestedItkImage.IsNull())
  {
    // Try to convert it from 16-bit data (RF data format)
    typedef short PixelType16;
    typedef itk::Image< PixelType16, 2 > ImageType16;
    ImageType16::Pointer requestedItkImage16;
    requestedItkImage16 = dynamic_cast< ImageType16 * >( itkImageBase );
    if (requestedItkImage16.IsNotNull())
    {
      typedef itk::NormalizeImageFilter<ImageType16, itk::Image<unsigned char, 2> > NormalizeFilterType;
      static NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New(); // :TODO: revise this, it's just for testing
      normalizeFilter->SetInput(requestedItkImage16);
      normalizeFilter->Update();
      requestedItkImage=normalizeFilter->GetOutput();
      return requestedItkImage;
    }
    
    LOG_ERROR("Cannot convert the frame data to the requested displayable frame data");
    return NULL;
  }

  return requestedItkImage;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::SetFrame(vtkImageData* frame)
{
  if ( this->ItkImage.IsNull() )
  {
    LOG_ERROR("Failed to add frame into video buffer item - frame is NULL!"); 
    return PLUS_FAIL; 
  }

  int frameSize[2] = {0,0};
  GetFrameSize(frameSize);

  int* frameExtent = frame->GetExtent(); 

  if ( ( frameExtent[1] - frameExtent[0] + 1 ) != frameSize[0] ||
    ( frameExtent[3] - frameExtent[2] + 1 ) != frameSize[1] )
  {
    LOG_ERROR("Input image size doesn't match buffer image size!"); 
    return PLUS_FAIL; 
  }

  if (frame->GetScalarType()!=VTK_UNSIGNED_CHAR)
  {
    LOG_ERROR("PlusVideoFrame::SetFrame supports only unsigned char pixel type");
    return PLUS_FAIL;
  }

  itk::Image<unsigned char, 2>::Pointer newImage=itk::Image<unsigned char, 2>::New();
  if (UsImageConverterCommon::ConvertVtkImageToItkImage(frame, newImage)!=PLUS_SUCCESS)
  {
    LOG_ERROR("PlusVideoFrame::SetFrame failed");
    return PLUS_FAIL;
  }

  this->ItkImage=newImage;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::SetFrame(unsigned char *imageDataPtr, 
                                     const int frameSizeInPx[3],
                                     int numberOfBitsPerPixel, 
                                     int numberOfBytesToSkip )
{
  if ( imageDataPtr == NULL )
  {
    LOG_ERROR( "Failed to add NULL frame to video buffer!"); 
    return PLUS_FAIL;
  }

  if ( this->ItkImage.IsNull() )
  {
    LOG_ERROR( "Unable to add frame to video buffer - need to allocate frame first!"); 
    return PLUS_FAIL; 
  }

  int frameSize[2] = {0,0};
  GetFrameSize(frameSize);

  if ( frameSizeInPx[0] != frameSize[0] 
    || frameSizeInPx[1] != frameSize[1] )
  {
    LOG_ERROR("Input frame size is different from buffer frame size (input: " << frameSizeInPx[0] << "x" << frameSizeInPx[1]
      << ",   buffer: " << frameSize[0] << "x" << frameSize[1] << ")!"); 
    return PLUS_FAIL; 
  }

  // Skip the numberOfBytesToSkip bytes, e.g. header size
  imageDataPtr += numberOfBytesToSkip; 

  memcpy(GetBufferPointer(), imageDataPtr, this->GetFrameSizeInBytes() );

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
const PlusVideoFrame::ImageBaseType * PlusVideoFrame::GetITKImageBase() const
{
  return this->ItkImage;
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType PlusVideoFrame::GetITKScalarPixelType() const
{
  return PlusVideoFrame::GetITKScalarPixelType(this->ItkImage.GetPointer());
}

//----------------------------------------------------------------------------
int PlusVideoFrame::GetNumberOfBytesPerPixel() const
{
  return UsImageConverterCommon::GetNumberOfBytesPerPixel(GetITKScalarPixelType());
}

//----------------------------------------------------------------------------
// Helper method, performs the operation if the specified pixel type matches the actual pixel type 
template <class TPixel >
bool GetBufferPointerGeneric(PlusVideoFrame::ImageBaseType* inputImage, void* &bufferPointer)
{
  typedef itk::Image< TPixel, 2> ImageType;
  ImageType * image = dynamic_cast< ImageType* >( inputImage );
  if( image )
  {
    bufferPointer=image->GetBufferPointer();
    return true;
  }
  return false;
};

//----------------------------------------------------------------------------
void* PlusVideoFrame::GetBufferPointer() const
{
  if (this->ItkImage.IsNull())
  {
    LOG_ERROR("Cannot get buffer pointer, the buffer hasn't been created yet");
    return NULL;
  }

  void* bufferPointer=NULL;

  ImageBaseType * itkImageBase = this->ItkImage;
  if (GetBufferPointerGeneric<unsigned char>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<char>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<unsigned short>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<short>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<unsigned int>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<int>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<unsigned long>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<long>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<float>(itkImageBase, bufferPointer)) {}
  else if (GetBufferPointerGeneric<double>(itkImageBase, bufferPointer)) {}
  else
  {
    LOG_ERROR("Unknown pixel type, cannot get buffer pointer");
    return NULL;
  }

  return bufferPointer;
}

//----------------------------------------------------------------------------
// Helper method, performs the operation if the specified pixel type matches the actual pixel type 
template <class TPixel >
bool GetFrameSizeGeneric(PlusVideoFrame::ImageBaseType* inputImage, int frameSize[2])
{
  typedef itk::Image< TPixel, 2> ImageType;
  ImageType * image = dynamic_cast< ImageType* >( inputImage );
  if( image )
  {
    PlusVideoFrame::ImageBaseType::RegionType region=image->GetLargestPossibleRegion();
    frameSize[0]=region.GetSize()[0];
    frameSize[1]=region.GetSize()[1];
    return true;
  }
  return false;
};

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetFrameSize(int frameSize[2]) const
{
  if (this->ItkImage.IsNull())
  {
    //LOG_ERROR("Cannot get frame size, the buffer hasn't been created yet");
    return PLUS_FAIL;
  }

  ImageBaseType * itkImageBase = this->ItkImage;
  if (GetFrameSizeGeneric<unsigned char>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<char>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<unsigned short>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<short>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<unsigned int>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<int>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<unsigned long>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<long>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<float>(itkImageBase, frameSize)) {}
  else if (GetFrameSizeGeneric<double>(itkImageBase, frameSize)) {}
  else
  {
    LOG_ERROR("Unknown pixel type, cannot get frame size");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::CopyToVtkImage( vtkImageData* targetFrame )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::CopyToVtkImage"); 

  if ( this->ItkImage.IsNull() )
  {
    LOG_ERROR("PlusVideoFrame: Failed to copy NULL video frame!"); 
    return PLUS_FAIL; 
  }

  if (targetFrame==NULL)
  {
    LOG_ERROR("PlusVideoFrame: Failed to copy video frame to NULL image!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  int frameSize[2]={0,0};
  GetFrameSize(frameSize);
  importer->SetWholeExtent(0, frameSize[0] - 1, 0, frameSize[1] - 1, 0, 0);
  importer->SetDataExtentToWholeExtent(); 
  importer->SetDataScalarType(GetVTKScalarPixelType()); 
  importer->SetImportVoidPointer(GetBufferPointer()); 
  importer->SetNumberOfScalarComponents(1); 
  importer->Update(); 

  // VTK convention is for the image voxel index (0,0,0) to be the lower-left corner of the image, so we need to flip it
  vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New(); 
  flipY->SetFilteredAxis(1); 
  flipY->SetInput( importer->GetOutput() ); 
  flipY->Update(); 

  targetFrame->DeepCopy( flipY->GetOutput() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int PlusVideoFrame::GetVTKScalarPixelType()
{
  return UsImageConverterCommon::GetVTKScalarPixelType(GetITKScalarPixelType());
}

//----------------------------------------------------------------------------
void PlusVideoFrame::SetITKImageBase( ImageBaseType * image )
{
  if( this->ItkImage.GetPointer() == image )
    {
    return;
    }

  this->ItkImage = image;
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType PlusVideoFrame::GetITKScalarPixelType(ImageBaseType* itkImageBase)
{
  PlusCommon::ITKScalarPixelType pixelType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;

  if( dynamic_cast< itk::Image< unsigned char, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::UCHAR; }
  else if( dynamic_cast< itk::Image< char, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::CHAR; }
  else if( dynamic_cast< itk::Image< short, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::SHORT; }
  else if( dynamic_cast< itk::Image< unsigned short, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::USHORT; }
  else if( dynamic_cast< itk::Image< int, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::INT; }
  else if( dynamic_cast< itk::Image< unsigned int, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::UINT; }
  else if( dynamic_cast< itk::Image< long, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::LONG; }
  else if( dynamic_cast< itk::Image< unsigned long, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::ULONG; }
  else if( dynamic_cast< itk::Image< float, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::FLOAT; }
  else if( dynamic_cast< itk::Image< double, 2> * >( itkImageBase ) ) { pixelType = itk::ImageIOBase::DOUBLE; }
  else
  {
    LOG_ERROR("GetITKScalarPixelType: Unknown pixel type");
  }
  return pixelType;
}

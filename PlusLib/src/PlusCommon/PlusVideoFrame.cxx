/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"

#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "itkNormalizeImageFilter.h"
#include "vtkImageExport.h"
#include "itkVTKImageExport.h"

#include "itkFlipImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkImportImageContainer.h"

#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "itkImageBase.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"


/*! 
  \class PipelineCreator
  This helper class will take care of instantiating the appropriate
  ITK Export class corresponding to the actual pixel type of the 
  input image. 
*/
template <class TPixel >
class PipelineCreator
{
public:
  
  typedef itk::ImageBase<2>           ImageBaseType;
  typedef ImageBaseType::Pointer      ImageBasePointer;
  typedef itk::ProcessObject          ExporterBaseType;
  typedef itk::ProcessObject::Pointer ExporterBasePointer;
  typedef itk::Image< TPixel, 2 >     ImageType;

  static void 
  CreateExporter( ImageBasePointer    & imageBase, 
                  ExporterBasePointer & exporter,
                  vtkImageImport      * importer  )
    {
    ImageType * image = 
      dynamic_cast< ImageType * >( imageBase.GetPointer() );

    if( image )
      {
      typedef itk::VTKImageExport< ImageType >   ExportFilterType;
      typedef typename ExportFilterType::Pointer ExportFilterPointer;
      ExportFilterPointer itkExporter = ExportFilterType::New();
      itkExporter->SetInput( image );

      exporter = itkExporter;

      importer->SetUpdateInformationCallback(
        itkExporter->GetUpdateInformationCallback());
      importer->SetPipelineModifiedCallback(
        itkExporter->GetPipelineModifiedCallback());
      importer->SetWholeExtentCallback(
        itkExporter->GetWholeExtentCallback());
      importer->SetSpacingCallback(
        itkExporter->GetSpacingCallback());
      importer->SetOriginCallback(
        itkExporter->GetOriginCallback());
      importer->SetScalarTypeCallback(
        itkExporter->GetScalarTypeCallback());
      importer->SetNumberOfComponentsCallback(
        itkExporter->GetNumberOfComponentsCallback());
      importer->SetPropagateUpdateExtentCallback(
        itkExporter->GetPropagateUpdateExtentCallback());
      importer->SetUpdateDataCallback(
        itkExporter->GetUpdateDataCallback());
      importer->SetDataExtentCallback(
        itkExporter->GetDataExtentCallback());
      importer->SetBufferPointerCallback(
        itkExporter->GetBufferPointerCallback());
      importer->SetCallbackUserData(
        itkExporter->GetCallbackUserData());
      }
    }
};


/** This helper macro will instantiate the pipeline creator for a particular
 * pixel type */
#define CreatePipelineMacro( PixelType ) \
  PipelineCreator< PixelType >::CreateExporter( \
      this->ItkImage, this->Exporter, this->Importer );

//----------------------------------------------------------------------------
vtkImageData * PlusVideoFrame::GetVtkImage()
{
  return this->Importer->GetOutput();
}

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame()
{
  this->Importer = vtkImageImport::New();
}

//----------------------------------------------------------------------------
PlusVideoFrame::~PlusVideoFrame()
{
  if( this->Importer )
  { 
    this->Importer->Delete();
    this->Importer=NULL;
  }
}

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame(const PlusVideoFrame &videoItem)
{
  // don't copy the importer, just create a new one
  // it will be updated whenever the image is updated
  this->Importer = vtkImageImport::New();
  
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
      UpdateVtkImage();
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
  UpdateVtkImage();
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
PlusStatus PlusVideoFrame::DeepCopyFrom(vtkImageData* frame)
{
  if ( frame == NULL )
  {
    LOG_ERROR("Failed to deep copy from vtk image data - input frame is NULL!"); 
    return PLUS_FAIL; 
  }

  PlusCommon::ITKScalarPixelType scalarPixelType = PlusVideoFrame::GetITKScalarPixelType(frame->GetScalarType()); 
  int* frameExtent = frame->GetExtent(); 
  int frameSize[2] = {( frameExtent[1] - frameExtent[0] + 1 ), ( frameExtent[3] - frameExtent[2] + 1 ) }; 

  if ( this->AllocateFrame(frameSize, scalarPixelType) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to allocate memory for plus video frame!"); 
    return PLUS_FAIL; 
  }

  memcpy(this->GetBufferPointer(), frame->GetScalarPointer(), this->GetFrameSizeInBytes() ); 
  UpdateVtkImage();

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
  return PlusVideoFrame::GetNumberOfBytesPerPixel(GetITKScalarPixelType());
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
int PlusVideoFrame::GetVTKScalarPixelType()
{
  return PlusVideoFrame::GetVTKScalarPixelType(GetITKScalarPixelType());
}

//----------------------------------------------------------------------------
void PlusVideoFrame::SetITKImageBase( ImageBaseType * image )
{
  if( this->ItkImage.GetPointer() == image )
    {
    return;
    }

  this->ItkImage = image;
  
  UpdateVtkImage();
}

void PlusVideoFrame::UpdateVtkImage()
{
  CreatePipelineMacro( unsigned char );
  CreatePipelineMacro( char );
  CreatePipelineMacro( unsigned short );
  CreatePipelineMacro( short );
  CreatePipelineMacro( unsigned int );
  CreatePipelineMacro( int );
  CreatePipelineMacro( unsigned long );
  CreatePipelineMacro( long );
  CreatePipelineMacro( float );
  CreatePipelineMacro( double );

  this->Importer->Update();
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



//----------------------------------------------------------------------------
US_IMAGE_ORIENTATION PlusVideoFrame::GetUsImageOrientationFromString( const char* usImgOrientation )
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
const char* PlusVideoFrame::GetStringFromUsImageOrientation(US_IMAGE_ORIENTATION orientation)
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
PlusStatus PlusVideoFrame::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, vtkImageData* outUsOrintedImage )
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
PlusStatus PlusVideoFrame::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image< unsigned char, 2 >::Pointer& outUsOrintedImage )
{
  itk::Image< unsigned char, 2 >::Pointer itkimage = itk::Image< unsigned char, 2 >::New(); 
  if ( PlusVideoFrame::ConvertVtkImageToItkImage(inUsImage, itkimage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get MF oriented image from vtk image data!"); 
    return PLUS_FAIL;
  }

  return PlusVideoFrame::GetMFOrientedImage(itkimage, inUsImageOrientation, outUsOrintedImage); 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned char, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned char, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<char, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<char, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned short, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned short, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<short, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<short, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned int, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned int, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<int, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<int, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<unsigned long, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<unsigned long, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<long, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<long, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<float, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<float, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], int numberOfBitsPerPixel, itk::Image<double, 2>::Pointer& outUsOrintedImage)
{
  return GetMFOrientedImageGeneric<itk::Image<double, 2>>(imageDataPtr, inUsImageOrientation, frameSizeInPx, numberOfBitsPerPixel, outUsOrintedImage);
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus PlusVideoFrame::GetMFOrientedImageGeneric( unsigned char* imageDataPtr,                               
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

  return PlusVideoFrame::GetMFOrientedImage(inUsImage, inUsImageOrientation, outUsOrintedImage); 
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus PlusVideoFrame::FlipImage(typename const OutputImageType::Pointer inUsImage, const itk::FixedArray<bool, 2> &flipAxes, typename OutputImageType::Pointer& outUsOrintedImage)
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
PlusStatus PlusVideoFrame::SaveImageToFile(unsigned char* imageDataPtr, const int frameSizeInPx[2], int numberOfBitsPerPixel, const char* fileName)
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

  return PlusVideoFrame::SaveImageToFile(inUsImage, fileName); 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::SaveImageToFile( PlusVideoFrame& frame, const char* fileName)
{
  return PlusVideoFrame::SaveImageToFile(frame.GetDisplayableImage(), fileName); 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::SaveImageToFile( vtkImageData* frame, const char* fileName)
{
  PlusVideoFrame videoFrame; 
  if ( videoFrame.DeepCopyFrom(frame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save image to file - vtk image copy to plus video frame failed!"); 
    return PLUS_FAIL; 
  }

  return PlusVideoFrame::SaveImageToFile(videoFrame, fileName); 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::SaveImageToFile(const itk::Image< unsigned char, 2 >::Pointer image, const char* fileName)
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
PlusStatus PlusVideoFrame::ReadImageFromFile( PlusVideoFrame& frame, const char* fileName)
{
  typedef itk::ImageFileReader< itk::Image< unsigned char, 2 > > ImgReaderType;
  ImgReaderType::Pointer reader = ImgReaderType::New(); 

  reader->SetFileName(fileName); 

  try    
  {
    reader->Update();
  }	
  catch (itk::ExceptionObject & e)
  {
    LOG_ERROR(e.GetDescription());
    return PLUS_FAIL; 
  }

  frame.SetITKImageBase(reader->GetOutput()); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<unsigned char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned char, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<unsigned char, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<char, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<char, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<unsigned short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned short, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<unsigned short, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<short, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<short, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<unsigned int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned int, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<unsigned int, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<int, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<int, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<unsigned long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<unsigned long, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<unsigned long, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<long, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<long, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<float, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<float, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<float, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}
PlusStatus PlusVideoFrame::GetMFOrientedImage( const itk::Image<double, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, itk::Image<double, 2>::Pointer& outUsOrintedImage )
{
  return PlusVideoFrame::GetMFOrientedImageGeneric<itk::Image<double, 2>>(inUsImage, inUsImageOrientation, outUsOrintedImage );
}

//----------------------------------------------------------------------------
template<class OutputImageType>
static PlusStatus PlusVideoFrame::GetMFOrientedImageGeneric( typename const OutputImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, typename OutputImageType::Pointer& outUsOrintedImage )
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
PlusStatus PlusVideoFrame::ConvertVtkImageToItkImage(vtkImageData* inFrame, itk::Image< unsigned char, 2 >::Pointer& outFrame)
{
  LOG_TRACE("PlusVideoFrame::ConvertVtkImageToItkImage"); 

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
PlusStatus PlusVideoFrame::GetMFOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, PlusVideoFrame &outBufferItem)
{
  PlusStatus status=PLUS_FAIL;
  int bitsPerPixel=PlusVideoFrame::GetNumberOfBytesPerPixel(pixelType)*8;
  switch (pixelType)
  {
  case itk::ImageIOBase::UCHAR:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned char>());
    break;
  case itk::ImageIOBase::CHAR:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<char>());
    break;
  case itk::ImageIOBase::USHORT:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned short>());
    break;
  case itk::ImageIOBase::SHORT:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<short>());
    break;
  case itk::ImageIOBase::UINT:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned int>());
    break;
  case itk::ImageIOBase::INT:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<int>());
    break;
  case itk::ImageIOBase::ULONG:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<unsigned long>());
    break;
  case itk::ImageIOBase::LONG:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<long>());
    break;
  case itk::ImageIOBase::FLOAT:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<float>());
    break;
  case itk::ImageIOBase::DOUBLE:
    status=PlusVideoFrame::GetMFOrientedImage(imageDataPtr, inUsImageOrientation, frameSizeInPx, bitsPerPixel, outBufferItem.GetImage<double>());
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
int PlusVideoFrame::GetNumberOfBytesPerPixel(PlusCommon::ITKScalarPixelType pixelType)
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
int PlusVideoFrame::GetVTKScalarPixelType(PlusCommon::ITKScalarPixelType pixelType)
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
PlusCommon::ITKScalarPixelType PlusVideoFrame::GetITKScalarPixelType(int vtkScalarPixelType)
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


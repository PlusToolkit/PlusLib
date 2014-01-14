/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"

#include "vtkImageData.h"
#include "itkNormalizeImageFilter.h"
#include "vtkImageExport.h"
#include "itkVTKImageExport.h"

#include "itkImportImageContainer.h"

#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "itkImageBase.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"

#ifdef PLUS_USE_OpenIGTLink
#include "igtlImageMessage.h"
#endif 

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
  this->ImageType = US_IMG_BRIGHTNESS;
  this->ImageOrientation = US_IMG_ORIENT_MF;
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

  this->ImageType = videoItem.ImageType;
  this->ImageOrientation = videoItem.ImageOrientation;

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
PlusStatus PlusVideoFrame::FillBlank()
{
  if ( !this->IsImageValid() )
  {
    LOG_ERROR("Unable to fill image to blank, image data is NULL."); 
    return PLUS_FAIL; 
  }

  memset( this->GetBufferPointer(), 0, this->GetFrameSizeInBytes()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// Helper method, duplicates the image base if the specified pixel type matches the actual pixel type 
template <class TPixel >
PlusStatus CreateAndAllocateImageGeneric(PlusVideoFrame::ImageBasePointer &resultImage, int imageSize[2])
{
  typedef itk::Image< TPixel, 2> ImageType;

  typename ImageType::Pointer image = ImageType::New();
  
  PlusVideoFrame::ImageBaseType::SizeType size;
  size[0] = imageSize[0];
  size[1] = imageSize[1];
  PlusVideoFrame::ImageBaseType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  
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
    if ((unsigned int)imageSize[0] == region.GetSize()[0] &&
        (unsigned int)imageSize[1] == region.GetSize()[1] &&
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
US_IMAGE_ORIENTATION PlusVideoFrame::GetImageOrientation() const
{
  return this->ImageOrientation;
}

//----------------------------------------------------------------------------
void PlusVideoFrame::SetImageOrientation(US_IMAGE_ORIENTATION imgOrientation)
{
  this->ImageOrientation=imgOrientation;
}

//----------------------------------------------------------------------------
US_IMAGE_TYPE PlusVideoFrame::GetImageType() const
{
  return this->ImageType;
}

//----------------------------------------------------------------------------
void PlusVideoFrame::SetImageType(US_IMAGE_TYPE imgType)
{
  this->ImageType=imgType;
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
int PlusVideoFrame::GetVTKScalarPixelType() const
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
US_IMAGE_ORIENTATION PlusVideoFrame::GetUsImageOrientationFromString( const char* imgOrientationStr )
{
  US_IMAGE_ORIENTATION imgOrientation = US_IMG_ORIENT_XX; 
  if ( imgOrientationStr == NULL )
  {
    return imgOrientation; 
  }
  else if ( STRCASECMP(imgOrientationStr, "UF" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_UF; 
  }
  else if ( STRCASECMP(imgOrientationStr, "UN" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_UN; 
  }
  else if ( STRCASECMP(imgOrientationStr, "MF" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_MF; 
  }
  else if ( STRCASECMP(imgOrientationStr, "MN" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_MN; 
  }
  else if ( STRCASECMP(imgOrientationStr, "FU" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_FU; 
  }
  else if ( STRCASECMP(imgOrientationStr, "NU" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_NU; 
  }
  else if ( STRCASECMP(imgOrientationStr, "FM" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_FM; 
  }
  else if ( STRCASECMP(imgOrientationStr, "NM" ) == 0 )
  {
    imgOrientation = US_IMG_ORIENT_NM; 
  }

  return imgOrientation; 
}

//----------------------------------------------------------------------------
const char* PlusVideoFrame::GetStringFromUsImageOrientation(US_IMAGE_ORIENTATION imgOrientation)
{
  switch (imgOrientation)
  {
  case US_IMG_ORIENT_MF: return "MF";
  case US_IMG_ORIENT_MN: return "MN";
  case US_IMG_ORIENT_UF: return "UF";
  case US_IMG_ORIENT_UN: return "UN";
  case US_IMG_ORIENT_FM: return "FM";
  case US_IMG_ORIENT_NM: return "NM";
  case US_IMG_ORIENT_FU: return "FU";
  case US_IMG_ORIENT_NU: return "NU";
  default:
    return "XX";
  }
}

//----------------------------------------------------------------------------
US_IMAGE_TYPE PlusVideoFrame::GetUsImageTypeFromString( const char* imgTypeStr )
{
  US_IMAGE_TYPE imgType = US_IMG_TYPE_XX; 
  if ( imgTypeStr == NULL )
  {
    return imgType; 
  }
  else if ( STRCASECMP(imgTypeStr, "BRIGHTNESS" ) == 0 )
  {
    imgType = US_IMG_BRIGHTNESS; 
  }
  else if ( STRCASECMP(imgTypeStr, "RF_REAL" ) == 0 )
  {
    imgType = US_IMG_RF_REAL; 
  }
  else if ( STRCASECMP(imgTypeStr, "RF_IQ_LINE" ) == 0 )
  {
    imgType = US_IMG_RF_IQ_LINE; 
  }
  else if ( STRCASECMP(imgTypeStr, "RF_I_LINE_Q_LINE" ) == 0 )
  {
    imgType = US_IMG_RF_I_LINE_Q_LINE; 
  }
  return imgType; 
}

//----------------------------------------------------------------------------
const char* PlusVideoFrame::GetStringFromUsImageType(US_IMAGE_TYPE imgType)
{
  switch (imgType)
  {
  case US_IMG_BRIGHTNESS: return "BRIGHTNESS";
  case US_IMG_RF_REAL: return "RF_REAL";
  case US_IMG_RF_IQ_LINE: return "RF_IQ_LINE";
  case US_IMG_RF_I_LINE_Q_LINE: return "RF_I_LINE_Q_LINE";
  default:
    return "XX";
  }
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetFlipAxes(US_IMAGE_ORIENTATION usImageOrientation1, US_IMAGE_TYPE usImageType1, US_IMAGE_ORIENTATION usImageOrientation2, FlipInfoType& flipInfo)
{  
  flipInfo.doubleRow=false;
  flipInfo.doubleColumn=false;
  if (usImageType1==US_IMG_RF_I_LINE_Q_LINE)
  {
    if ( usImageOrientation1==US_IMG_ORIENT_FM
      || usImageOrientation1==US_IMG_ORIENT_FU
      || usImageOrientation1==US_IMG_ORIENT_NM
      || usImageOrientation1==US_IMG_ORIENT_NU )
    {
      // keep line pairs together
      flipInfo.doubleRow=true;
    }
    else
    {
      LOG_ERROR("RF scanlines are expected to be in image rows");
      return PLUS_FAIL;
    }
  }
  else if (usImageType1==US_IMG_RF_IQ_LINE)
  {
    if ( usImageOrientation1==US_IMG_ORIENT_FM
      || usImageOrientation1==US_IMG_ORIENT_FU
      || usImageOrientation1==US_IMG_ORIENT_NM
      || usImageOrientation1==US_IMG_ORIENT_NU )
    {
      // keep IQ value pairs together
      flipInfo.doubleColumn=true;
    }
    else
    {
      LOG_ERROR("RF scanlines are expected to be in image rows");
      return PLUS_FAIL;
    }
  }

  flipInfo.hFlip=false;
  flipInfo.vFlip=false;
  if ( usImageOrientation1 == US_IMG_ORIENT_XX ) 
  {
    LOG_ERROR("Failed to determine the necessary image flip - unknown input image orientation 1"); 
    return PLUS_FAIL; 
  }

  if ( usImageOrientation2 == US_IMG_ORIENT_XX ) 
  {
    LOG_ERROR("Failed to determine the necessary image flip - unknown input image orientation 2"); 
    return PLUS_SUCCESS; 
  }
  if (usImageOrientation1==usImageOrientation2)
  {
    // no flip
    return PLUS_SUCCESS;
  }
  if ((usImageOrientation1==US_IMG_ORIENT_UF && usImageOrientation2==US_IMG_ORIENT_MF)||
    (usImageOrientation1==US_IMG_ORIENT_MF && usImageOrientation2==US_IMG_ORIENT_UF)||
    (usImageOrientation1==US_IMG_ORIENT_UN && usImageOrientation2==US_IMG_ORIENT_MN)||
    (usImageOrientation1==US_IMG_ORIENT_MN && usImageOrientation2==US_IMG_ORIENT_UN)||
    (usImageOrientation1==US_IMG_ORIENT_FU && usImageOrientation2==US_IMG_ORIENT_NU)||
    (usImageOrientation1==US_IMG_ORIENT_NU && usImageOrientation2==US_IMG_ORIENT_FU)||
    (usImageOrientation1==US_IMG_ORIENT_FM && usImageOrientation2==US_IMG_ORIENT_NM)||
    (usImageOrientation1==US_IMG_ORIENT_NM && usImageOrientation2==US_IMG_ORIENT_FM))
  {
    // flip x
    flipInfo.hFlip=true;
    flipInfo.vFlip=false;
    return PLUS_SUCCESS;
  }
  if ((usImageOrientation1==US_IMG_ORIENT_UF && usImageOrientation2==US_IMG_ORIENT_UN)||
    (usImageOrientation1==US_IMG_ORIENT_MF && usImageOrientation2==US_IMG_ORIENT_MN)||
    (usImageOrientation1==US_IMG_ORIENT_UN && usImageOrientation2==US_IMG_ORIENT_UF)||
    (usImageOrientation1==US_IMG_ORIENT_MN && usImageOrientation2==US_IMG_ORIENT_MF)||
    (usImageOrientation1==US_IMG_ORIENT_FU && usImageOrientation2==US_IMG_ORIENT_FM)||
    (usImageOrientation1==US_IMG_ORIENT_NU && usImageOrientation2==US_IMG_ORIENT_NM)||
    (usImageOrientation1==US_IMG_ORIENT_FM && usImageOrientation2==US_IMG_ORIENT_FU)||
    (usImageOrientation1==US_IMG_ORIENT_NM && usImageOrientation2==US_IMG_ORIENT_NU))
  {
    // flip y
    flipInfo.hFlip=false;
    flipInfo.vFlip=true;
    return PLUS_SUCCESS;
  }
  if ((usImageOrientation1==US_IMG_ORIENT_UF && usImageOrientation2==US_IMG_ORIENT_MN)||
    (usImageOrientation1==US_IMG_ORIENT_MF && usImageOrientation2==US_IMG_ORIENT_UN)||
    (usImageOrientation1==US_IMG_ORIENT_UN && usImageOrientation2==US_IMG_ORIENT_MF)||
    (usImageOrientation1==US_IMG_ORIENT_MN && usImageOrientation2==US_IMG_ORIENT_UF)||
    (usImageOrientation1==US_IMG_ORIENT_FU && usImageOrientation2==US_IMG_ORIENT_NM)||
    (usImageOrientation1==US_IMG_ORIENT_NU && usImageOrientation2==US_IMG_ORIENT_FM)||
    (usImageOrientation1==US_IMG_ORIENT_FM && usImageOrientation2==US_IMG_ORIENT_NU)||
    (usImageOrientation1==US_IMG_ORIENT_NM && usImageOrientation2==US_IMG_ORIENT_FU))
  {
    // flip xy
    flipInfo.hFlip=true;
    flipInfo.vFlip=true;
    return PLUS_SUCCESS;
  }
  LOG_ERROR("Image orientation conversion between orientations "<<GetStringFromUsImageOrientation(usImageOrientation1)
    <<" and "<<GetStringFromUsImageOrientation(usImageOrientation2)
    <<" is not supported (image transpose is not allowed, only reordering of rows and/or columns");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
template<class PixelType>
PlusStatus FlipImageGeneric(void* inBuff, int width, int height, const PlusVideoFrame::FlipInfoType& flipInfo, void* outBuff)
{
  if (flipInfo.doubleRow)
  {
    if (height%2 != 0)
    {
      LOG_ERROR("Cannot flip image with pairs of rows kept together, as number of rows is odd ("<<height<<")");
      return PLUS_FAIL;
    }
    width*=2;
    height/=2;
  }
  if (flipInfo.doubleColumn)
  {
    if (width%2 != 0)
    {
      LOG_ERROR("Cannot flip image with pairs of columns kept together, as number of columns is odd ("<<width<<")");
      return PLUS_FAIL;
    }
  }

  if (!flipInfo.hFlip && flipInfo.vFlip)
  {
    // flip Y    
    PixelType* inputPixel=(PixelType*)inBuff;
    // Set the target position pointer to the first pixel of the last row
    PixelType* outputPixel=((PixelType*)outBuff)+width*(height-1);
    // Copy the image row-by-row, reversing the row order
    for (int y=height; y>0; y--)
    {
      memcpy(outputPixel, inputPixel, width*sizeof(PixelType));
      inputPixel+=width;
      outputPixel-=width;
    }
  }
  else if (flipInfo.hFlip && !flipInfo.vFlip)
  {
    // flip X    
    if (flipInfo.doubleColumn)
    {
      PixelType* inputPixel=(PixelType*)inBuff;
      // Set the target position pointer to the last pixel of the first row
      PixelType* outputPixel=((PixelType*)outBuff)+width-2;
      // Copy the image row-by-row, reversing the pixel order in each row
      for (int y=height; y>0; y--)
      {
        for (int x=width/2; x>0; x--)
        {
          *(outputPixel-1)=*inputPixel;
          *outputPixel=*(inputPixel+1);
          inputPixel+=2;
          outputPixel-=2;
        }
        outputPixel+=2*width;
      }
    }
    else
    {
      PixelType* inputPixel=(PixelType*)inBuff;
      // Set the target position pointer to the last pixel of the first row
      PixelType* outputPixel=((PixelType*)outBuff)+width-1;
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
  }
  else if (flipInfo.hFlip && flipInfo.vFlip)
  {
    // flip X and Y
    if (flipInfo.doubleColumn)
    {
      PixelType* inputPixel=(PixelType*)inBuff;
      // Set the target position pointer to the last pixel
      PixelType* outputPixel=((PixelType*)outBuff)+height*width-1;
      // Copy the image pixelpair-by-pixelpair, reversing the pixel order
      for (int p=width*height/2; p>0; p--)
      {
        *(outputPixel-1)=*inputPixel;
        *outputPixel=*(inputPixel+1);
        inputPixel+=2;
        outputPixel-=2;
      }
    }
    else
    {
      PixelType* inputPixel=(PixelType*)inBuff;
      // Set the target position pointer to the last pixel
      PixelType* outputPixel=((PixelType*)outBuff)+height*width-1;
      // Copy the image pixel-by-pixel, reversing the pixel order
      for (int p=width*height; p>0; p--)
      {
        *outputPixel=*inputPixel;
        inputPixel++;
        outputPixel--;
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION outUsImageOrientation, vtkImageData* outUsOrientedImage )
{
  if ( inUsImage == NULL )
  {
    LOG_ERROR("Failed to convert image data to the requested orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrientedImage == NULL )
  {
    LOG_ERROR("Failed to convert image data to the requested orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  FlipInfoType flipInfo;
  if (GetFlipAxes(inUsImageOrientation, inUsImageType, outUsImageOrientation, flipInfo)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from "<<GetStringFromUsImageOrientation(inUsImageOrientation)<<" to "<<GetStringFromUsImageOrientation(outUsImageOrientation));
    return PLUS_FAIL;
  }
  if ( !flipInfo.hFlip && !flipInfo.vFlip )
  {
    // no flip
    outUsOrientedImage->ShallowCopy( inUsImage ); 
    return PLUS_SUCCESS; 
  }

  // Allocate the output image
  outUsOrientedImage->SetExtent(inUsImage->GetExtent());
  outUsOrientedImage->SetScalarType(inUsImage->GetScalarType());
  outUsOrientedImage->SetNumberOfScalarComponents(1);
  outUsOrientedImage->AllocateScalars(); 

  int numberOfBytesPerPixel=PlusVideoFrame::GetNumberOfBytesPerPixel(PlusVideoFrame::GetITKScalarPixelType(inUsImage->GetScalarType()));

  int extent[6]={0,0,0,0,0,0}; 
  inUsImage->GetExtent(extent); 
  double width = extent[1] - extent[0] + 1; 
  double height = extent[3] - extent[2] + 1; 

  PlusStatus status=PLUS_FAIL;
  switch (numberOfBytesPerPixel)
  {
  case 1:
    status=FlipImageGeneric<vtkTypeUInt8>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
    break;
  case 2:
    status=FlipImageGeneric<vtkTypeUInt16>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
    break;
  default:
    LOG_ERROR("Unsupported bit depth: "<<numberOfBytesPerPixel<<" bytes per pixel");
  }
  return status;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image< unsigned char, 2 >::Pointer outUsOrientedImage )
{
  itk::Image< unsigned char, 2 >::Pointer itkimage = itk::Image< unsigned char, 2 >::New(); 
  if ( PlusVideoFrame::ConvertVtkImageToItkImage(inUsImage, itkimage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get MF oriented image from vtk image data!"); 
    return PLUS_FAIL;
  }

  return PlusVideoFrame::GetOrientedImage(itkimage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage); 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned char, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<unsigned char, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<char, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<char, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned short, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<unsigned short, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<short, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<short, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned int, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<unsigned int, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<int, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<int, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned long, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<unsigned long, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<long, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<long, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<float, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<float, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr,US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], int numberOfBitsPerPixel, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<double, 2>::Pointer outUsOrientedImage)
{
  return GetOrientedImageGeneric<itk::Image<double, 2> >(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, numberOfBitsPerPixel, outUsImageOrientation, outUsOrientedImage);
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus PlusVideoFrame::GetOrientedImageGeneric( unsigned char* imageDataPtr,                               
                                                      US_IMAGE_ORIENTATION  inUsImageOrientation, 
                                                      US_IMAGE_TYPE inUsImageType, 
                                                      const int    frameSizeInPx[2],
                                                      int    numberOfBitsPerPixel, 
                                                      US_IMAGE_ORIENTATION  outUsImageOrientation, 
                                                      typename OutputImageType::Pointer outUsOrientedImage
                                                      )
{

  if ( imageDataPtr == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrientedImage.IsNull() )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  if ( numberOfBitsPerPixel != sizeof(typename OutputImageType::PixelType)*8 )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - pixel size mismatch (input: " << numberOfBitsPerPixel << " bits, output: " << sizeof(typename OutputImageType::PixelType)*8 << " bits)!"); 
    return PLUS_FAIL; 
  }

  typename OutputImageType::Pointer inUsImage = OutputImageType::New(); 
  typename OutputImageType::SizeType size;
  size[0] = frameSizeInPx[0];
  size[1] = frameSizeInPx[1];
  typename OutputImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  typename OutputImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  inUsImage->SetRegions(region);
  typename OutputImageType::PixelContainer::Pointer pixelContainer = OutputImageType::PixelContainer::New(); 
  pixelContainer->SetImportPointer(reinterpret_cast<typename OutputImageType::PixelType*>(imageDataPtr), frameSizeInPx[0]*frameSizeInPx[1], false); 
  inUsImage->SetPixelContainer(pixelContainer); 

  return PlusVideoFrame::GetOrientedImage(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage); 
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus PlusVideoFrame::FlipImage(const typename OutputImageType::Pointer inUsImage, const PlusVideoFrame::FlipInfoType& flipInfo, typename OutputImageType::Pointer outUsOrientedImage)
{
  outUsOrientedImage->SetOrigin(inUsImage->GetOrigin());
  outUsOrientedImage->SetSpacing(inUsImage->GetSpacing());
  outUsOrientedImage->SetDirection(inUsImage->GetDirection());
  outUsOrientedImage->SetLargestPossibleRegion(inUsImage->GetLargestPossibleRegion());
  outUsOrientedImage->SetRequestedRegion(inUsImage->GetRequestedRegion());
  outUsOrientedImage->SetBufferedRegion(inUsImage->GetBufferedRegion());

  try 
  {
    outUsOrientedImage->Allocate(); 
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  typename OutputImageType::SizeType imageSize=inUsImage->GetLargestPossibleRegion().GetSize();
  int width=imageSize[0];
  int height=imageSize[1];

  return FlipImageGeneric<typename OutputImageType::PixelType>(inUsImage->GetBufferPointer(), width, height, flipInfo, outUsOrientedImage->GetBufferPointer());
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
  itk::Image< unsigned char, 2 >::SizeType size;
  size[0]  = frameSizeInPx[0];
  size[1]  = frameSizeInPx[1];
  itk::Image< unsigned char, 2 >::IndexType start;
  start[0] = 0;
  start[1] = 0;
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
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<unsigned char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned char, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<unsigned char, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<char, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<char, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<char, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<unsigned short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned short, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<unsigned short, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<short, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<short, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<short, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<unsigned int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned int, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<unsigned int, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<int, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<int, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<int, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<unsigned long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<unsigned long, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<unsigned long, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<long, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<long, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<long, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<float, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<float, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<float, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}
PlusStatus PlusVideoFrame::GetOrientedImage( const itk::Image<double, 2>::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, itk::Image<double, 2>::Pointer outUsOrientedImage )
{
  return PlusVideoFrame::GetOrientedImageGeneric<itk::Image<double, 2> >(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage );
}

//----------------------------------------------------------------------------
template<class OutputImageType>
PlusStatus PlusVideoFrame::GetOrientedImageGeneric( const typename OutputImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, US_IMAGE_TYPE inUsImageType, US_IMAGE_ORIENTATION  outUsImageOrientation, typename OutputImageType::Pointer outUsOrientedImage )
{
  if ( inUsImage.IsNull() )
  {
    LOG_ERROR("Failed to convert image data MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrientedImage.IsNull() )
  {
    LOG_ERROR("Failed to convert image data MF orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  outUsOrientedImage->SetOrigin(inUsImage->GetOrigin());
  outUsOrientedImage->SetSpacing(inUsImage->GetSpacing());
  outUsOrientedImage->SetDirection(inUsImage->GetDirection());
  outUsOrientedImage->SetLargestPossibleRegion(inUsImage->GetLargestPossibleRegion());
  outUsOrientedImage->SetRequestedRegion(inUsImage->GetRequestedRegion());
  outUsOrientedImage->SetBufferedRegion(inUsImage->GetBufferedRegion());

  try 
  {
    outUsOrientedImage->Allocate(); 
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  FlipInfoType flipInfo;
  if (GetFlipAxes(inUsImageOrientation, inUsImageType, outUsImageOrientation, flipInfo)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from "<<GetStringFromUsImageOrientation(inUsImageOrientation)<<" to "<<GetStringFromUsImageOrientation(outUsImageOrientation));
    return PLUS_FAIL;
  }

  if (!flipInfo.hFlip && !flipInfo.vFlip)
  {
    // We need to copy the raw data since we're using the image array as an OutputImageType::PixelContainer
    long bufferSize = inUsImage->GetLargestPossibleRegion().GetSize()[0]*inUsImage->GetLargestPossibleRegion().GetSize()[1]*sizeof(typename OutputImageType::PixelType); 
    memcpy(outUsOrientedImage->GetBufferPointer(), inUsImage->GetBufferPointer(), bufferSize); 
    return PLUS_SUCCESS; 
  }

  // Performance profiling showed that ITK's flip image filter (itk::FlipImageFilter ) is very slow,
  // therefore, an alternative implementation was tried, which does not use this filter.
  // Execution time of the alternative implementation in releaes mode does not seem to be
  // much faster, so for now keep using the flip image filter.
  return FlipImage<OutputImageType>(inUsImage, flipInfo, outUsOrientedImage);
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::ConvertVtkImageToItkImage(vtkImageData* inFrame, itk::Image< unsigned char, 2 >::Pointer outFrame)
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

  int extent[6]={0,0,0,0,0,0}; 
  inFrame->GetExtent(extent); 

  double width = extent[1] - extent[0] + 1; 
  double height = extent[3] - extent[2] + 1; 
  itk::Image< unsigned char, 2 >::SizeType size;
  size[0] = width;
  size[1] = height;
  itk::Image< unsigned char, 2 >::IndexType start;
  start[0]=0;
  start[1]=0;
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
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, US_IMAGE_ORIENTATION outUsImageOrientation, PlusVideoFrame &outBufferItem)
{
  PlusStatus status=PLUS_FAIL;
  int bitsPerPixel=PlusVideoFrame::GetNumberOfBytesPerPixel(pixelType)*8;
  switch (pixelType)
  {
  case itk::ImageIOBase::UCHAR:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<unsigned char>());
    break;
  case itk::ImageIOBase::CHAR:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<char>());
    break;
  case itk::ImageIOBase::USHORT:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<unsigned short>());
    break;
  case itk::ImageIOBase::SHORT:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<short>());
    break;
  case itk::ImageIOBase::UINT:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<unsigned int>());
    break;
  case itk::ImageIOBase::INT:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<int>());
    break;
  case itk::ImageIOBase::ULONG:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<unsigned long>());
    break;
  case itk::ImageIOBase::LONG:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<long>());
    break;
  case itk::ImageIOBase::FLOAT:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<float>());
    break;
  case itk::ImageIOBase::DOUBLE:
    status=PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, bitsPerPixel, outUsImageOrientation, outBufferItem.GetImage<double>());
    break;
  default:
    {
      LOG_ERROR("Unsupported pixel type: "<<pixelType); 
      return PLUS_FAIL; 
    }
  }
  outBufferItem.SetImageOrientation(outUsImageOrientation);
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
PlusCommon::ITKScalarPixelType PlusVideoFrame::GetITKScalarPixelType(PlusCommon::VTKScalarPixelType vtkScalarPixelType)
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

#ifdef PLUS_USE_OpenIGTLink
//----------------------------------------------------------------------------
// static 
PlusCommon::ITKScalarPixelType PlusVideoFrame::GetITKScalarPixelTypeFromIGTL(PlusCommon::IGTLScalarPixelType igtlPixelType)
{
  switch (igtlPixelType)
  {
  case igtl::ImageMessage::TYPE_INT8: return itk::ImageIOBase::CHAR;
  case igtl::ImageMessage::TYPE_UINT8: return itk::ImageIOBase::UCHAR;
  case igtl::ImageMessage::TYPE_INT16: return itk::ImageIOBase::SHORT;
  case igtl::ImageMessage::TYPE_UINT16: return itk::ImageIOBase::USHORT;
  case igtl::ImageMessage::TYPE_INT32: return itk::ImageIOBase::INT;
  case igtl::ImageMessage::TYPE_UINT32: return itk::ImageIOBase::UINT;
  case igtl::ImageMessage::TYPE_FLOAT32: return itk::ImageIOBase::FLOAT;
  case igtl::ImageMessage::TYPE_FLOAT64: return itk::ImageIOBase::DOUBLE;
  default:
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;      
  }
}
#endif

#ifdef PLUS_USE_OpenIGTLink
//----------------------------------------------------------------------------
// static 
PlusCommon::IGTLScalarPixelType PlusVideoFrame::GetIGTLScalarPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  switch (pixelType)
  {
  case itk::ImageIOBase::CHAR: return igtl::ImageMessage::TYPE_INT8;
  case itk::ImageIOBase::UCHAR: return igtl::ImageMessage::TYPE_UINT8;
  case itk::ImageIOBase::SHORT: return igtl::ImageMessage::TYPE_INT16;
  case itk::ImageIOBase::USHORT: return igtl::ImageMessage::TYPE_UINT16;
  case itk::ImageIOBase::INT: return igtl::ImageMessage::TYPE_INT32;
  case itk::ImageIOBase::UINT: return igtl::ImageMessage::TYPE_UINT32;
  case itk::ImageIOBase::FLOAT: return igtl::ImageMessage::TYPE_FLOAT32;
  case itk::ImageIOBase::DOUBLE: return igtl::ImageMessage::TYPE_FLOAT64;
  default:
    // There is no unkown IGT scalar pixel type, so display an error message 
    LOG_ERROR("Unknown conversion between ITK scalar pixel type (" << pixelType << ") and IGT pixel type - return igtl::ImageMessage::TYPE_INT8 by default!"); 
    return igtl::ImageMessage::TYPE_INT8;      
  }
}
#endif

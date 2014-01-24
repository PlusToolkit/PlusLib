/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "itkImageBase.h"
#include "vtkImageData.h"
#include "vtkImageReader.h"
#include "vtkObjectFactory.h"

#ifdef PLUS_USE_OpenIGTLink
#include "igtlImageMessage.h"
#endif

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame()
{
  this->ImageType = US_IMG_BRIGHTNESS;
  this->ImageOrientation = US_IMG_ORIENT_MF;
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

  this->ImageType = videoItem.ImageType;
  this->ImageOrientation = videoItem.ImageOrientation;

  // Copy the pixels. Don't use image duplicator, because that wouldn't reuse the existing buffer
  if ( videoItem.GetFrameSizeInBytes() > 0)
  {
    int frameSize[2] = {0,0};
    videoItem.GetFrameSize(frameSize);

    if ( this->AllocateFrame(frameSize, videoItem.GetVTKScalarPixelType()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to allocate memory for the new frame in the buffer!"); 
    }
    else
    {
      memcpy(this->GetScalarPointer(), videoItem.GetScalarPointer(), this->GetFrameSizeInBytes() ); 
    }
  }
  else
  {
    DELETE_IF_NOT_NULL(this->Image);
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

  memset( this->GetScalarPointer(), 0, this->GetFrameSizeInBytes()); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::AllocateFrame(int imageSize[2], PlusCommon::VTKScalarPixelType pixType)
{  
  if ( this->IsImageValid() )
  {
    int frameSize[2] = {0,0};
    this->GetFrameSize(frameSize);
    if (imageSize[0] == frameSize[0] &&
      imageSize[1] == frameSize[1] &&
      GetVTKScalarPixelType() == pixType)
    {
      // already allocated, no change
      return PLUS_SUCCESS;
    }        
  }

  DELETE_IF_NOT_NULL(this->Image);

  this->Image = vtkSmartPointer<vtkImageData>::New();
  this->Image->SetExtent(0, imageSize[0], 0, imageSize[1], 0, 1);
  this->Image->SetScalarType(pixType);
  this->Image->AllocateScalars();

  return PLUS_SUCCESS; 
}
//----------------------------------------------------------------------------
unsigned long PlusVideoFrame::GetFrameSizeInBytes() const
{
  if (!this->IsImageValid())
  {
    return 0;
  }
  int frameSize[2] = {0,0};
  this->GetFrameSize(frameSize);

  int bytesPerPixel = GetNumberOfBytesPerPixel(); 
  if (bytesPerPixel != 1 && bytesPerPixel != 2 && bytesPerPixel != 4)
  {
    LOG_ERROR("Unsupported pixel size: " << bytesPerPixel << " bytes/pixel");
  }
  unsigned long frameSizeInBytes = frameSize[0] * frameSize[1] * bytesPerPixel;
  return frameSizeInBytes; 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::DeepCopyFrom(vtkImageData* frame)
{
  if ( frame == NULL )
  {
    LOG_ERROR("Failed to deep copy from vtk image data - input frame is NULL!"); 
    return PLUS_FAIL; 
  }

  int* frameExtent = frame->GetExtent(); 
  int frameSize[2] = {( frameExtent[1] - frameExtent[0] + 1 ), ( frameExtent[3] - frameExtent[2] + 1 ) }; 

  if ( this->AllocateFrame(frameSize, frame->GetScalarType()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to allocate memory for plus video frame!"); 
    return PLUS_FAIL; 
  }

  memcpy(this->Image->GetScalarPointer(), frame->GetScalarPointer(), this->GetFrameSizeInBytes() );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType PlusVideoFrame::GetITKScalarPixelType() const
{
  return PlusVideoFrame::GetITKScalarPixelType(GetVTKScalarPixelType());
}

//----------------------------------------------------------------------------
int PlusVideoFrame::GetNumberOfBytesPerPixel() const
{
  return PlusVideoFrame::GetNumberOfBytesPerPixel(GetVTKScalarPixelType());
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
void* PlusVideoFrame::GetScalarPointer() const
{
  if (!this->IsImageValid())
  {
    LOG_ERROR("Cannot get buffer pointer, the buffer hasn't been created yet");
    return NULL;
  }

  return this->Image->GetScalarPointer();
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
  if (!this->IsImageValid())
  {
    //LOG_ERROR("Cannot get frame size, the buffer hasn't been created yet");
    frameSize[0] = frameSize[1] = -1;
    return PLUS_FAIL;
  }

  int extents[6] = {0,0,0,0,0,0};
  this->Image->GetExtent(extents);
  frameSize[0] = extents[1] - extents[0]; 
  frameSize[1] = extents[3] - extents[2];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusCommon::VTKScalarPixelType PlusVideoFrame::GetVTKScalarPixelType() const
{
  return PlusVideoFrame::GetVTKScalarPixelType(GetITKScalarPixelType());
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
  if (GetFlipAxes(inUsImageOrientation, inUsImageType, outUsImageOrientation, flipInfo) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from " << GetStringFromUsImageOrientation(inUsImageOrientation) << " to " << GetStringFromUsImageOrientation(outUsImageOrientation));
    return PLUS_FAIL;
  }
  if ( !flipInfo.hFlip && !flipInfo.vFlip )
  {
    // no flip
    outUsOrientedImage->ShallowCopy( inUsImage ); 
    return PLUS_SUCCESS; 
  }

  int outWidth = outUsOrientedImage->GetExtent()[1] - outUsOrientedImage->GetExtent()[0];
  int inWidth = inUsImage->GetExtent()[1] - inUsImage->GetExtent()[0];
  int outHeight = outUsOrientedImage->GetExtent()[3] - outUsOrientedImage->GetExtent()[2];
  int inHeight = inUsImage->GetExtent()[3] - inUsImage->GetExtent()[2];

  if( outHeight != inHeight || outWidth != inWidth || outUsOrientedImage->GetScalarType() != inUsImage->GetScalarType() || outUsOrientedImage->GetNumberOfScalarComponents() != 1 )
  {
    // Allocate the output image
    outUsOrientedImage->SetExtent(inUsImage->GetExtent());
    outUsOrientedImage->SetScalarType(inUsImage->GetScalarType());
    outUsOrientedImage->SetNumberOfScalarComponents(1);
    outUsOrientedImage->AllocateScalars(); 
  }

  int numberOfBytesPerPixel = PlusVideoFrame::GetNumberOfBytesPerPixel(inUsImage->GetScalarType());

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
  case 4:
    status=FlipImageGeneric<vtkTypeUInt32>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
    break;
  default:
    LOG_ERROR("Unsupported bit depth: "<<numberOfBytesPerPixel<<" bytes per pixel");
  }
  return status;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetOrientedImage(  unsigned char* imageDataPtr,                               
                                            US_IMAGE_ORIENTATION  inUsImageOrientation, 
                                            US_IMAGE_TYPE inUsImageType, 
                                            const int    frameSizeInPx[2],
                                            US_IMAGE_ORIENTATION  outUsImageOrientation, 
                                            vtkImageData* outUsOrientedImage
                                            )
{

  if ( imageDataPtr == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - input image is null!"); 
    return PLUS_FAIL; 
  }

  if ( outUsOrientedImage == NULL )
  {
    LOG_ERROR("Failed to convert image data to MF orientation - output image is null!"); 
    return PLUS_FAIL; 
  }

  int extents[6] = {0, frameSizeInPx[0], 0, frameSizeInPx[1], 0, 1};
  // Allocate the output image
  outUsOrientedImage->SetExtent(extents);
  outUsOrientedImage->SetScalarType(VTK_UNSIGNED_CHAR);
  outUsOrientedImage->SetNumberOfScalarComponents(1);
  outUsOrientedImage->AllocateScalars();

  vtkSmartPointer<vtkImageData> inUsImage = vtkSmartPointer<vtkImageData>::New();
  inUsImage->SetExtent(outUsOrientedImage->GetExtent());
  inUsImage->SetScalarType(outUsOrientedImage->GetScalarType());
  inUsImage->SetNumberOfScalarComponents(1);
  inUsImage->AllocateScalars();
  memcpy(inUsImage->GetScalarPointer(), imageDataPtr, frameSizeInPx[0]*frameSizeInPx[1]*sizeof(unsigned char));

  FlipInfoType flipInfo;
  if (GetFlipAxes(inUsImageOrientation, inUsImageType, outUsImageOrientation, flipInfo) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from " << GetStringFromUsImageOrientation(inUsImageOrientation) << " to " << GetStringFromUsImageOrientation(outUsImageOrientation));
    return PLUS_FAIL;
  }
  if ( !flipInfo.hFlip && !flipInfo.vFlip )
  {
    // no flip
    outUsOrientedImage->DeepCopy(inUsImage);
    return PLUS_SUCCESS; 
  }

  return PlusVideoFrame::GetOrientedImage(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage); 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, const int frameSizeInPx[2], US_IMAGE_ORIENTATION outUsImageOrientation, PlusVideoFrame &outBufferItem)
{
  return PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, frameSizeInPx, outUsImageOrientation, outBufferItem.GetImage());
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::FlipImage(vtkImageData* inUsImage, const PlusVideoFrame::FlipInfoType& flipInfo, vtkImageData* outUsOrientedImage)
{
  if( outUsOrientedImage == NULL )
  {
    LOG_ERROR("Null output image sent to flip image. Nothing to write into.");
    return PLUS_FAIL;
  }
  outUsOrientedImage->SetOrigin(inUsImage->GetOrigin());
  outUsOrientedImage->SetSpacing(inUsImage->GetSpacing());
  outUsOrientedImage->SetExtent(inUsImage->GetExtent());
  outUsOrientedImage->SetScalarType(inUsImage->GetScalarType());
  outUsOrientedImage->SetNumberOfScalarComponents(1);
  outUsOrientedImage->AllocateScalars();

  int extents[6] = {0,0,0,0,0,0};
  outUsOrientedImage->GetExtent(extents);
  int width = extents[1] - extents[0];
  int height = extents[3] - extents[2];
  switch(outUsOrientedImage->GetScalarType())
  {
  case VTK_UNSIGNED_CHAR: return FlipImageGeneric<vtkTypeUInt8>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_CHAR: return FlipImageGeneric<vtkTypeInt8>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_UNSIGNED_SHORT: return FlipImageGeneric<vtkTypeUInt16>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_SHORT: return FlipImageGeneric<vtkTypeInt16>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_UNSIGNED_INT: return FlipImageGeneric<vtkTypeUInt32>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_INT: return FlipImageGeneric<vtkTypeInt32>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_UNSIGNED_LONG: return FlipImageGeneric<unsigned long>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_LONG: return FlipImageGeneric<long>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_FLOAT: return FlipImageGeneric<vtkTypeFloat32>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_DOUBLE: return FlipImageGeneric<vtkTypeFloat64>(inUsImage->GetScalarPointer(), width, height, flipInfo, outUsOrientedImage->GetScalarPointer());
  default:
    LOG_ERROR("Unknown pixel type. Cannot re-orient image.");
    return PLUS_FAIL;
  }
}

/*
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
*/

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::ReadImageFromFile( PlusVideoFrame& frame, const char* fileName)
{
  vtkSmartPointer<vtkImageReader> reader = vtkSmartPointer<vtkImageReader>::New();

  reader->SetFileName(fileName);
  reader->Update();

  return frame.DeepCopyFrom(reader->GetOutput());
}


//----------------------------------------------------------------------------
int PlusVideoFrame::GetNumberOfBytesPerPixel(PlusCommon::VTKScalarPixelType pixelType)
{
  switch (pixelType)
  {
  case VTK_UNSIGNED_CHAR: return sizeof(vtkTypeUInt8);
  case VTK_CHAR: return sizeof(vtkTypeInt8);
  case VTK_UNSIGNED_SHORT: return sizeof(vtkTypeUInt16);
  case VTK_SHORT: return sizeof(vtkTypeInt16);
  case VTK_UNSIGNED_INT: return sizeof(vtkTypeUInt32);
  case VTK_INT: return sizeof(vtkTypeInt32);
  case VTK_UNSIGNED_LONG: return sizeof(unsigned long);
  case VTK_LONG: return sizeof(long);
  case VTK_FLOAT: return sizeof(vtkTypeFloat32);
  case VTK_DOUBLE: return sizeof(vtkTypeFloat64);
  default:
    LOG_ERROR("GetNumberOfBytesPerPixel: unknown pixel type " << pixelType);
    return VTK_VOID;
  }
}

//----------------------------------------------------------------------------
PlusCommon::VTKScalarPixelType PlusVideoFrame::GetVTKScalarPixelType(PlusCommon::ITKScalarPixelType pixelType)
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
PlusCommon::VTKScalarPixelType PlusVideoFrame::GetVTKScalarPixelTypeFromIGTL(PlusCommon::IGTLScalarPixelType igtlPixelType)
{
  switch (igtlPixelType)
  {
  case igtl::ImageMessage::TYPE_INT8: return VTK_CHAR;
  case igtl::ImageMessage::TYPE_UINT8: return VTK_UNSIGNED_CHAR;
  case igtl::ImageMessage::TYPE_INT16: return VTK_SHORT;
  case igtl::ImageMessage::TYPE_UINT16: return VTK_UNSIGNED_SHORT;
  case igtl::ImageMessage::TYPE_INT32: return VTK_INT;
  case igtl::ImageMessage::TYPE_UINT32: return VTK_UNSIGNED_INT;
  case igtl::ImageMessage::TYPE_FLOAT32: return VTK_FLOAT;
  case igtl::ImageMessage::TYPE_FLOAT64: return VTK_DOUBLE;
  default:
    return VTK_VOID;  
  }
}
#endif

#ifdef PLUS_USE_OpenIGTLink
//----------------------------------------------------------------------------
// static 
PlusCommon::IGTLScalarPixelType PlusVideoFrame::GetIGTLScalarPixelType(PlusCommon::VTKScalarPixelType pixelType)
{
  switch (pixelType)
  {
  case VTK_CHAR: return igtl::ImageMessage::TYPE_INT8;
  case VTK_UNSIGNED_CHAR: return igtl::ImageMessage::TYPE_UINT8;
  case VTK_SHORT: return igtl::ImageMessage::TYPE_INT16;
  case VTK_UNSIGNED_SHORT: return igtl::ImageMessage::TYPE_UINT16;
  case VTK_INT: return igtl::ImageMessage::TYPE_INT32;
  case VTK_UNSIGNED_INT: return igtl::ImageMessage::TYPE_UINT32;
  case VTK_FLOAT: return igtl::ImageMessage::TYPE_FLOAT32;
  case VTK_DOUBLE: return igtl::ImageMessage::TYPE_FLOAT64;
  default:
    // There is no unknown IGT scalar pixel type, so display an error message 
    LOG_ERROR("Unknown conversion between VTK scalar pixel type (" << pixelType << ") and IGT pixel type - return igtl::ImageMessage::TYPE_INT8 by default!"); 
    return igtl::ImageMessage::TYPE_INT8;      
  }
}

#endif

//----------------------------------------------------------------------------
vtkImageData* PlusVideoFrame::GetImage() const
{
  return this->Image;
}

//----------------------------------------------------------------------------
#define VTK_TO_STRING(pixType) case pixType: return "##pixType"
std::string PlusVideoFrame::GetStringFromVTKPixelType( PlusCommon::VTKScalarPixelType vtkScalarPixelType )
{
  switch(vtkScalarPixelType)
  {
    VTK_TO_STRING(VTK_CHAR);
    VTK_TO_STRING(VTK_UNSIGNED_CHAR);
    VTK_TO_STRING(VTK_SHORT);
    VTK_TO_STRING(VTK_UNSIGNED_SHORT);
    VTK_TO_STRING(VTK_INT);
    VTK_TO_STRING(VTK_UNSIGNED_INT);
    VTK_TO_STRING(VTK_FLOAT);
    VTK_TO_STRING(VTK_DOUBLE);
  default:
    return "Unknown";
  }
}
#undef VTK_TO_STRING
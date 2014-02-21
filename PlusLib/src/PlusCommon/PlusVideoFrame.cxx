/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "itkImageBase.h"
#include "vtkBMPReader.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPNMReader.h"
#include "vtkTIFFReader.h"

#ifdef PLUS_USE_OpenIGTLink
#include "igtlImageMessage.h"
#endif

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame()
: Image(NULL)
, ImageType(US_IMG_BRIGHTNESS)
, ImageOrientation(US_IMG_ORIENT_MF)
{
}

//----------------------------------------------------------------------------
PlusVideoFrame::PlusVideoFrame(const PlusVideoFrame &videoItem)
: Image(NULL)
, ImageType(US_IMG_BRIGHTNESS)
, ImageOrientation(US_IMG_ORIENT_MF)
{
  *this = videoItem;
}

//----------------------------------------------------------------------------
PlusVideoFrame::~PlusVideoFrame()
{
  DELETE_IF_NOT_NULL(this->Image);
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
      this->Image->Modified();
    }
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
PlusStatus PlusVideoFrame::AllocateFrame(vtkImageData* image, const int imageSize[2], PlusCommon::VTKScalarPixelType pixType)
{  
  if ( image == NULL )
  {
    LOG_ERROR("PlusVideoFrame::AllocateFrame failed: image is NULL");
    return PLUS_FAIL;
  }

  int inputImageSize[2]={0,0};
  PlusVideoFrame::GetImageSize(image, inputImageSize);
  if (imageSize[0] == inputImageSize[0] &&
    imageSize[1] == inputImageSize[1] &&
    image->GetScalarType() == pixType &&
    image->GetNumberOfScalarComponents() == 1)
  {
    // already allocated, no change
    return PLUS_SUCCESS;
  }

  image->SetExtent(0, imageSize[0]-1, 0, imageSize[1]-1, 0, 0);
  image->SetScalarType(pixType);
  image->SetNumberOfScalarComponents(1);
  image->AllocateScalars();

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::AllocateFrame(const int imageSize[2], PlusCommon::VTKScalarPixelType pixType)
{
  if (this->Image==NULL)
  {
    this->Image=vtkImageData::New();
  }
  return PlusVideoFrame::AllocateFrame(this->GetImage(), imageSize, pixType);
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

  if( frameSize[0] <= 0 || frameSize[1] <= 0 )
  {
    return 0;
  }

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

  int frameSize[2]={0,0};
  PlusVideoFrame::GetImageSize(frame, frameSize);
  if ( this->AllocateFrame(frameSize, frame->GetScalarType()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to allocate memory for plus video frame!"); 
    return PLUS_FAIL; 
  }

  memcpy(this->Image->GetScalarPointer(), frame->GetScalarPointer(), this->GetFrameSizeInBytes() );
  this->Image->Modified();

  return PLUS_SUCCESS;
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
PlusStatus PlusVideoFrame::GetFrameSize(int frameSize[2]) const
{
  if ( !this->IsImageValid() )
  {
    frameSize[0] = frameSize[1] = 0;
    return PLUS_FAIL;
  }

  PlusVideoFrame::GetImageSize(this->Image, frameSize);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusCommon::VTKScalarPixelType PlusVideoFrame::GetVTKScalarPixelType() const
{
  if( this->Image == NULL || !this->IsImageValid() )
  {
    return VTK_VOID;
  }
  return this->Image->GetScalarType();
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
  
  int inImageSize[2]={0,0};
  PlusVideoFrame::GetImageSize(inUsImage, inImageSize);

  int outImageSize[2]={0,0};
  PlusVideoFrame::GetImageSize(outUsOrientedImage, outImageSize);

  if( inImageSize[0] != outImageSize[0] || inImageSize[1] != outImageSize[1] || outUsOrientedImage->GetScalarType() != inUsImage->GetScalarType() || outUsOrientedImage->GetNumberOfScalarComponents() != 1 )
  {
    // Allocate the output image
    outUsOrientedImage->SetExtent(inUsImage->GetExtent());
    outUsOrientedImage->SetScalarType(inUsImage->GetScalarType());
    outUsOrientedImage->SetNumberOfScalarComponents(1);
    outUsOrientedImage->AllocateScalars(); 
  }

  int numberOfBytesPerPixel = PlusVideoFrame::GetNumberOfBytesPerPixel(inUsImage->GetScalarType());

  PlusStatus status=PLUS_FAIL;
  switch (numberOfBytesPerPixel)
  {
  case 1:
    status=FlipImageGeneric<vtkTypeUInt8>(inUsImage->GetScalarPointer(), inImageSize[0], inImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
    break;
  case 2:
    status=FlipImageGeneric<vtkTypeUInt16>(inUsImage->GetScalarPointer(), inImageSize[0], inImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
    break;
  case 4:
    status=FlipImageGeneric<vtkTypeUInt32>(inUsImage->GetScalarPointer(), inImageSize[0], inImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
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
                                            PlusCommon::VTKScalarPixelType pixType, 
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

  // Allocate the output image (the buffer is reused if the image properties are matching)
  PlusVideoFrame::AllocateFrame(outUsOrientedImage, frameSizeInPx, pixType);

  // vtkImageImport displays a warning when importing 1x1 size images, so handle that case separately
  if (frameSizeInPx[0]==1 && frameSizeInPx[1]==1)
  {
    memcpy(outUsOrientedImage->GetScalarPointer(), imageDataPtr, PlusVideoFrame::GetNumberOfBytesPerPixel(pixType));
    outUsOrientedImage->Modified();
    return PLUS_SUCCESS;
  }

  // Create a vtkImageData object that points to the existing input image buffer
  // (simply creating a new vtkImageData would require reallocating and copying memory from the pixel buffer)
  vtkSmartPointer<vtkImageImport> inUsImageImporter = vtkSmartPointer<vtkImageImport>::New();
  inUsImageImporter->SetWholeExtent(0,frameSizeInPx[0]-1,0,frameSizeInPx[1]-1,0,0);
  inUsImageImporter->SetDataExtentToWholeExtent();
  inUsImageImporter->SetDataScalarType(outUsOrientedImage->GetScalarType());
  inUsImageImporter->SetImportVoidPointer(imageDataPtr);
  inUsImageImporter->Update();
  vtkImageData* inUsImage=inUsImageImporter->GetOutput(); // the inUsImage object is owned by inUsImageImporter and will be deleted when inUsImageImporter is deleted

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
    outUsOrientedImage->Modified();
    return PLUS_SUCCESS; 
  }

  PlusStatus result = PlusVideoFrame::GetOrientedImage(inUsImage, inUsImageOrientation, inUsImageType, outUsImageOrientation, outUsOrientedImage); 
  outUsOrientedImage->Modified();
  return result;
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetOrientedImage( unsigned char* imageDataPtr, US_IMAGE_ORIENTATION  inUsImageOrientation, US_IMAGE_TYPE inUsImageType, PlusCommon::VTKScalarPixelType pixType, const int frameSizeInPx[2], US_IMAGE_ORIENTATION outUsImageOrientation, PlusVideoFrame &outBufferItem)
{
  return PlusVideoFrame::GetOrientedImage(imageDataPtr, inUsImageOrientation, inUsImageType, pixType, frameSizeInPx, outUsImageOrientation, outBufferItem.GetImage());
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::FlipImage(vtkImageData* inUsImage, const PlusVideoFrame::FlipInfoType& flipInfo, vtkImageData* outUsOrientedImage)
{
  if( outUsOrientedImage == NULL )
  {
    LOG_ERROR("Null output image sent to flip image. Nothing to write into.");
    return PLUS_FAIL;
  }

  int frameSize[2]={0,0};
  PlusVideoFrame::GetImageSize(inUsImage, frameSize);
  PlusVideoFrame::AllocateFrame(outUsOrientedImage, frameSize, inUsImage->GetScalarType()); // PlusVideoFrame::AllocateFrame only reallocates memory if necessary

  int outImageSize[2]={0,0};
  PlusVideoFrame::GetImageSize(outUsOrientedImage, outImageSize);
  switch(outUsOrientedImage->GetScalarType())
  {
  case VTK_UNSIGNED_CHAR: return FlipImageGeneric<vtkTypeUInt8>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_CHAR: return FlipImageGeneric<vtkTypeInt8>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_UNSIGNED_SHORT: return FlipImageGeneric<vtkTypeUInt16>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_SHORT: return FlipImageGeneric<vtkTypeInt16>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_UNSIGNED_INT: return FlipImageGeneric<vtkTypeUInt32>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_INT: return FlipImageGeneric<vtkTypeInt32>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_UNSIGNED_LONG: return FlipImageGeneric<unsigned long>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_LONG: return FlipImageGeneric<long>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_FLOAT: return FlipImageGeneric<vtkTypeFloat32>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  case VTK_DOUBLE: return FlipImageGeneric<vtkTypeFloat64>(inUsImage->GetScalarPointer(), outImageSize[0], outImageSize[1], flipInfo, outUsOrientedImage->GetScalarPointer());
  default:
    LOG_ERROR("Unknown pixel type. Cannot re-orient image.");
    return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::ReadImageFromFile( PlusVideoFrame& frame, const char* fileName)
{
  // TODO: we need to always use only VTK or only ITK image readers/writers consistently
  vtkImageReader2* reader(NULL);

  std::string extension = vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  std::vector< std::string > readerExtensions;

  if( reader == NULL )
  {
    reader = vtkTIFFReader::New();
    PlusCommon::SplitStringIntoTokens(reader->GetFileExtensions(), ' ', readerExtensions);

    if( std::find(readerExtensions.begin(), readerExtensions.end(), extension) == readerExtensions.end() )
    {
      DELETE_IF_NOT_NULL(reader);
    }
  }

  if( reader == NULL )
  {
    reader = vtkBMPReader::New();
    PlusCommon::SplitStringIntoTokens(reader->GetFileExtensions(), ' ', readerExtensions);

    if( std::find(readerExtensions.begin(), readerExtensions.end(), extension) == readerExtensions.end() )
    {
      DELETE_IF_NOT_NULL(reader);
    }
  }

  if( reader == NULL )
  {
    reader = vtkPNMReader::New();
    PlusCommon::SplitStringIntoTokens(reader->GetFileExtensions(), ' ', readerExtensions);

    if( std::find(readerExtensions.begin(), readerExtensions.end(), extension) == readerExtensions.end() )
    {
      DELETE_IF_NOT_NULL(reader);
    }
  }

  reader->SetFileName(fileName);
  reader->Update();

  PlusStatus result = frame.DeepCopyFrom(reader->GetOutput());
  reader->Delete();
  return result;
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
PlusCommon::IGTLScalarPixelType PlusVideoFrame::GetIGTLScalarPixelTypeFromVTK(PlusCommon::VTKScalarPixelType pixelType)
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
void PlusVideoFrame::SetImageData( vtkImageData* imageData )
{
  this->Image = imageData;
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

//----------------------------------------------------------------------------
PlusStatus PlusVideoFrame::GetImageSize(vtkImageData* image, int* imageSize)
{
  if (imageSize==NULL)
  {
    LOG_ERROR("GetImageSize failed: imageSize is NULL");
    return PLUS_FAIL;
  }
  if (image==NULL)
  {
    LOG_ERROR("GetImageSize failed: imageData is NULL");
    imageSize[0]=0;
    imageSize[1]=0;
    return PLUS_FAIL;
  }
  int imageExtents[6] = {0,0,0,0,0,0};
  image->GetExtent(imageExtents);
  imageSize[0] = imageExtents[1]-imageExtents[0]+1;
  imageSize[1] = imageExtents[3]-imageExtents[2]+1;
  return PLUS_SUCCESS;
}

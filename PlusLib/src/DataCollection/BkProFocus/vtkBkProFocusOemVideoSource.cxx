/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Define OFFLINE_TESTING to read image input from file instead of reading from the actual hardware device.
// This is useful only for testing and debugging without having access to an actual BK scanner.
//#define OFFLINE_TESTING
static const char OFFLINE_TESTING_FILENAME[]="c:\\Users\\lasso\\Downloads\\bktest.png";

#include "PlusConfigure.h"
#include "vtkBkProFocusOemVideoSource.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtk_png.h"
#include "vtksys/SystemTools.hxx"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "PixelCodec.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ostream>
#include <string>
#include "stdint.h"

#include "ParamConnectionSettings.h"
#include "ParamSyncConnection.h"
#include "TCPClient.h"
#include "WSAIF.h"
#include "OemParams.h"

#include "UseCaseParser.h"
#include "UseCaseStructs.h"


vtkCxxRevisionMacro(vtkBkProFocusOemVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkBkProFocusOemVideoSource);

class vtkBkProFocusOemVideoSource::vtkInternal
{
public:
  vtkBkProFocusOemVideoSource *External;

  vtkPlusChannel* Channel;

  ParamConnectionSettings BKparamSettings;
  WSAIF wsaif;
  TcpClient* OemClient;
  TcpClient* ToolboxClient;
  ParamSyncConnection *ParamSync;

  // Buffer to hold the query reply, it's a member variable to avoid memory allocation at each frame receiving
  std::vector<char> OemClientReadBuffer;

  // Image buffer to hold the decoded image frames, it's a member variable to avoid memory allocation at each frame receiving
  vtkImageData* DecodedImageFrame; 
  // Data buffer to hold temporary data during decoding, it's a member variable to avoid memory allocation at each frame receiving
  std::vector<unsigned char> DecodingBuffer;
  // Data buffer to hold temporary data during decoding (pointers to image lines), it's a member variable to avoid memory allocation at each frame receiving
  std::vector<png_bytep> DecodingLineBuffer;


  vtkBkProFocusOemVideoSource::vtkInternal::vtkInternal(vtkBkProFocusOemVideoSource* external) 
    : External(external)
    , Channel(NULL)
    , OemClient(NULL)
    , ToolboxClient(NULL)
    , ParamSync(NULL)
  {
    this->DecodedImageFrame=vtkImageData::New();

  }

  virtual vtkBkProFocusOemVideoSource::vtkInternal::~vtkInternal() 
  {    
    this->Channel = NULL;
    delete this->OemClient;
    this->OemClient=NULL;
    delete this->ToolboxClient;
    this->ToolboxClient=NULL;
    delete this->ParamSync;
    this->ParamSync=NULL;
    this->DecodedImageFrame->Delete();
    this->DecodedImageFrame=NULL;
    this->External = NULL;
  }  

};


//----------------------------------------------------------------------------
vtkBkProFocusOemVideoSource::vtkBkProFocusOemVideoSource()
{
  this->Internal = new vtkInternal(this);

  this->IniFileName=NULL;

  this->RequireImageOrientationInConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkBkProFocusOemVideoSource::~vtkBkProFocusOemVideoSource()
{ 
  if( !this->Connected )
  {
    this->Disconnect();
  }

  delete this->Internal;
  this->Internal=NULL;

  SetIniFileName(NULL);
}

//----------------------------------------------------------------------------
void vtkBkProFocusOemVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkBkProFocusOemVideoSource::InternalConnect" );

  if (this->Internal->Channel==NULL)
  {
    if (this->OutputChannels.empty())
    {
      LOG_ERROR("Cannot connect: no output channel is specified for device "<<this->GetDeviceId());
      return PLUS_FAIL;
    }
    this->Internal->Channel = this->OutputChannels[0];
  }

  std::string iniFilePath;
  GetFullIniFilePath(iniFilePath);
  if (!this->Internal->BKparamSettings.LoadSettingsFromIniFile(iniFilePath.c_str()))
  {
    LOG_ERROR("Could not load BK parameter settings from file: "<<iniFilePath.c_str());
    return PLUS_FAIL;
  }

  LOG_DEBUG("BK scanner address: " << this->Internal->BKparamSettings.GetScannerAddress());
  LOG_DEBUG("BK scanner OEM port: " << this->Internal->BKparamSettings.GetOemPort());
  LOG_DEBUG("BK scanner toolbox port: " << this->Internal->BKparamSettings.GetToolboxPort());

  // Clear buffer on connect because the new frames that we will acquire might have a different size 
  this->Internal->Channel->Clear();  

  delete this->Internal->OemClient;
  this->Internal->OemClient=NULL;
  delete this->Internal->ToolboxClient;
  this->Internal->ToolboxClient=NULL;
  delete this->Internal->ParamSync;
  this->Internal->ParamSync=NULL;
  this->Internal->OemClient=new TcpClient(&(this->Internal->wsaif), 8*1024*1024, this->Internal->BKparamSettings.GetOemPort(), this->Internal->BKparamSettings.GetScannerAddress());
  this->Internal->ToolboxClient=new TcpClient(&(this->Internal->wsaif), 8*1024*1024, this->Internal->BKparamSettings.GetToolboxPort(), this->Internal->BKparamSettings.GetScannerAddress());
  this->Internal->ParamSync=new ParamSyncConnection(this->Internal->OemClient, this->Internal->ToolboxClient);

#ifndef OFFLINE_TESTING
  LOG_DEBUG("Connecting to BK scanner");
  bool connected = this->Internal->ParamSync->ConnectOEMInterface();
  if (!connected)
  {
    LOG_ERROR("Could not connect to BKProFocusOem:"
      << " scanner address = " << this->Internal->BKparamSettings.GetScannerAddress()
      << ", OEM port = " << this->Internal->BKparamSettings.GetOemPort()
      << ", toolbox port = " << this->Internal->BKparamSettings.GetToolboxPort());
    return PLUS_FAIL;
  }
  LOG_DEBUG("Connected to BK scanner");
#endif

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalDisconnect()
{

  LOG_DEBUG("Disconnect from BKProFocusOem");

  this->StopRecording();

  delete this->Internal->ParamSync;
  this->Internal->ParamSync=NULL;

  delete this->Internal->OemClient;
  this->Internal->OemClient=NULL;
  delete this->Internal->ToolboxClient;
  this->Internal->ToolboxClient=NULL;

  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalStartRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalUpdate()
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  unsigned char* uncompressedPixelBuffer=0;
  unsigned int uncompressedPixelBufferSize=0;
#ifndef OFFLINE_TESTING
	try
	{
    std::string query = "query:capture_image \"PNG\";";
    LOG_TRACE("Query from vtkBkProFocusOemVideoSource: " << query);	
    size_t queryWrittenSize=this->Internal->OemClient->Write(query.c_str(), query.size());	
    if (queryWrittenSize!=query.size()+2) // OemClient->Write returns query.size()+2 on a successfully sent event (see #722)
    {
      LOG_ERROR("Failed to send query through BK OEM interface ("<<query<<")"<<queryWrittenSize << " vs " << query.size()<<"+2");
      return PLUS_FAIL;
    }

    // Set a buffer size that is likely to be able to hold a complete image
    const int maxReplySize=8*1024*1024;
    this->Internal->OemClientReadBuffer.resize(maxReplySize);
    LOG_TRACE("Before client read");
    size_t numBytes = this->Internal->OemClient->Read(&(this->Internal->OemClientReadBuffer[0]),maxReplySize);
    if (numBytes==0)
    {
      LOG_ERROR("Failed to read response from BK OEM interface");
      return PLUS_FAIL;
    }
    LOG_TRACE("Number of bytes read: " << numBytes);

    // First detect the #
    int n=0;
    for ( n = 0; this->Internal->OemClientReadBuffer[n]!='#' && n < numBytes; n++ ) ;
    n++;

    int numChars = (int)this->Internal->OemClientReadBuffer[n] - (int)('0');
    n++;
    LOG_TRACE("Number of bytes in the image: " << numChars);
    if (numChars==0)
    {
      LOG_ERROR("Failed to read image from BK OEM interface");
      return PLUS_FAIL;
    }
    
    for (int k = 0; k < numChars; k++, n++) 
    {
      uncompressedPixelBufferSize = uncompressedPixelBufferSize * 10 + ((int)this->Internal->OemClientReadBuffer[n] - '0');
    }
    LOG_TRACE("uncompressedPixelBufferSize = " << uncompressedPixelBufferSize);

    uncompressedPixelBuffer=(unsigned char*) &(this->Internal->OemClientReadBuffer[n]);
	}
	catch(TcpClientWaitException e)
	{
    LOG_ERROR("Communication error on the BK OEM interface (TcpClientWaitException: " << e.Message <<")")
    return PLUS_FAIL;
	}
#endif

  /*
  FILE * f;
  f = fopen("c:\\andrey\\bktest.bmp", "wb");
  if(f!=NULL){
  fwrite(&buf[n],1,jpgSize,f);
  fclose(f);
  }
  */

  if (DecodePngImage(uncompressedPixelBuffer, uncompressedPixelBufferSize, this->Internal->DecodedImageFrame)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to decode received PNG image on channel " << this->Internal->Channel->GetChannelId());
    return PLUS_FAIL;
  }

  this->FrameNumber++; 

  vtkPlusDataSource* aSource(NULL);
  if ( this->Internal->Channel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the BKProFocusOem device on channel " << this->Internal->Channel->GetChannelId());
    return PLUS_FAIL;
  }
  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( aSource->GetBuffer()->GetNumberOfItems() == 0 )
  {
    LOG_DEBUG("Set up BK ProFocus image buffer");
    int* frameExtent=this->Internal->DecodedImageFrame->GetExtent();
    int frameSizeInPix[2]={frameExtent[1]-frameExtent[0]+1, frameExtent[3]-frameExtent[2]+1};
    aSource->GetBuffer()->SetPixelType(this->Internal->DecodedImageFrame->GetScalarType());
    aSource->GetBuffer()->SetImageType(US_IMG_BRIGHTNESS);
    aSource->GetBuffer()->SetFrameSize( frameSizeInPix[0], frameSizeInPix[1] );
    aSource->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);

    LOG_INFO("Frame size: " << frameSizeInPix[0] << "x" << frameSizeInPix[1]
      << ", pixel type: " << vtkImageScalarTypeNameMacro(this->Internal->DecodedImageFrame->GetScalarType())
      << ", device image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetPortImageOrientation())
      << ", buffer image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetBuffer()->GetImageOrientation()));

  } 
  if( aSource->GetBuffer()->AddItem(this->Internal->DecodedImageFrame, aSource->GetPortImageOrientation(), US_IMG_BRIGHTNESS, this->FrameNumber) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error adding item to video source " << aSource->GetSourceId() << " on channel " << this->Internal->Channel->GetChannelId() );
    return PLUS_FAIL;
  }
  this->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{

  LOG_TRACE("vtkBkProFocusOemVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Epiphan video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* iniFileName = imageAcquisitionConfig->GetAttribute("IniFileName"); 
  if ( iniFileName != NULL) 
  {
    this->SetIniFileName(iniFileName); 
  }

  return PLUS_SUCCESS;

}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetAttribute("IniFileName", this->IniFileName);

  return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::GetFullIniFilePath(std::string &fullPath)
{
  if (this->IniFileName==NULL)
  {
    LOG_ERROR("Ini file name has not been set");
    return PLUS_FAIL;
  }
  if (vtksys::SystemTools::FileIsFullPath(this->IniFileName))
  {
    fullPath=this->IniFileName;
  }
  else
  {
    fullPath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->IniFileName);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkBkProFocusOemVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkBkProFocusOemVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  this->Internal->Channel = this->OutputChannels[0];

  return PLUS_SUCCESS;
}

void ReadDataFromByteArray(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
  if(png_ptr->io_ptr == NULL)
  {
    LOG_ERROR("ReadDataFromInputStream failed, no input pointer is set");
    png_error(png_ptr, "ReadDataFromInputStream failed, no input pointer is set");
    return;
  }

  unsigned char* bufferPointer = (unsigned char*)png_ptr->io_ptr;
  memcpy(outBytes, bufferPointer, byteCountToRead);
  bufferPointer+=byteCountToRead;

  png_ptr->io_ptr=bufferPointer;   
}

void PngErrorCallback(png_structp png_ptr, png_const_charp message)
{
  LOG_ERROR("PNG error: "<<(message?message:"no details available"));
}

void PngWarningCallback(png_structp png_ptr, png_const_charp message)
{
  LOG_WARNING("PNG warning: "<<(message?message:"no details available"));
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::DecodePngImage(unsigned char* pngBuffer, unsigned int pngBufferSize, vtkImageData* decodedImage)
{  

#ifdef OFFLINE_TESTING
  FILE *fp = fopen(OFFLINE_TESTING_FILENAME, "rb");
  if (!fp)
  {
    LOG_ERROR("Failed to read png");
    return PLUS_FAIL;
  }
  fseek (fp , 0 , SEEK_END);
  size_t fileSizeInBytes = ftell (fp);
  rewind (fp); 
  std::vector<unsigned char> fileReadBuffer;
  fileReadBuffer.resize(fileSizeInBytes);
  pngBuffer=&(fileReadBuffer[0]);
  fread(pngBuffer, 1, fileSizeInBytes, fp);
  fclose(fp);
#endif

  unsigned int headerSize=8;
  unsigned char* header=pngBuffer; // a 8-byte header
  int is_png = !png_sig_cmp(header, 0, headerSize);
  if (!is_png)
  {
    LOG_ERROR("Invalid PNG header");
    return PLUS_FAIL;
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
  if (!png_ptr)
  {
    LOG_ERROR("Failed to decode PNG buffer");
    return PLUS_FAIL;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr,(png_infopp)NULL, (png_infopp)NULL);
    LOG_ERROR("Failed to decode PNG buffer");
    return PLUS_FAIL;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    LOG_ERROR("Failed to decode PNG buffer");
    return PLUS_FAIL;
  }

  png_set_error_fn(png_ptr, NULL, PngErrorCallback, PngWarningCallback);

  // Set error handling
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
    LOG_ERROR("Failed to decode PNG buffer");
    return PLUS_FAIL;
  }  

  png_set_read_fn(png_ptr, pngBuffer+8, ReadDataFromByteArray);

  //png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type;
  int compression_type, filter_method;
  // get size and bit-depth of the PNG-image
  png_get_IHDR(png_ptr, info_ptr,
    &width, &height,
    &bit_depth, &color_type, &interlace_type,
    &compression_type, &filter_method);

  // set-up the transformations
  // convert palettes to RGB
  if (color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_set_palette_to_rgb(png_ptr);
  }

  // minimum of a byte per pixel
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
  {
#if PNG_LIBPNG_VER >= 10400
    png_set_expand_gray_1_2_4_to_8(png_ptr);
#else
    png_set_gray_1_2_4_to_8(png_ptr);
#endif
  }

  // add alpha if any alpha found
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
  {
    png_set_tRNS_to_alpha(png_ptr);
  }

  if (bit_depth > 8)
  {
#ifndef VTK_WORDS_BIGENDIAN
    png_set_swap(png_ptr);
#endif
  }

  // have libpng handle interlacing
  //int number_of_passes = png_set_interlace_handling(png_ptr);

  // update the info now that we have defined the filters
  png_read_update_info(png_ptr, info_ptr);

  int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  this->Internal->DecodingBuffer.resize(rowbytes*height);
  unsigned char *tempImage = &(this->Internal->DecodingBuffer[0]);
  
  this->Internal->DecodingLineBuffer.resize(height);
  png_bytep *row_pointers = &(this->Internal->DecodingLineBuffer[0]);
  for (unsigned int ui = 0; ui < height; ++ui)
  {
    row_pointers[ui] = tempImage + rowbytes*ui;
  }
  png_read_image(png_ptr, row_pointers);

  // copy the data into the outPtr
  if (width*3!=rowbytes)
  {
    LOG_WARNING("There is padding at the end of PNG lines, image may be skewed");
  }
  
  decodedImage->SetExtent(0,width-1,0,height-1,0,0);

#if (VTK_MAJOR_VERSION < 6)
  decodedImage->SetScalarTypeToUnsignedChar();
  decodedImage->SetNumberOfScalarComponents(1);
  decodedImage->AllocateScalars();
#else
  decodedImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
#endif

  PlusStatus status=PixelCodec::ConvertToGray(BI_RGB, width, height, &(this->Internal->DecodingBuffer[0]), (unsigned char*)decodedImage->GetScalarPointer());
  
  // close the file
  png_read_end(png_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  return status;
}

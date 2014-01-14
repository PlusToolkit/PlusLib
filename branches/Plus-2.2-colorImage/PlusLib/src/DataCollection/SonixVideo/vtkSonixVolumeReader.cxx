/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkSonixVolumeReader.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "PlusVideoFrame.h"
#include "vtkTrackedFrameList.h" 
#include "TrackedFrame.h" 

#include <iostream>
#include <sstream>

vtkCxxRevisionMacro(vtkSonixVolumeReader, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkSonixVolumeReader);

//----------------------------------------------------------------------------
vtkSonixVolumeReader::vtkSonixVolumeReader()
{
}


//----------------------------------------------------------------------------
vtkSonixVolumeReader::~vtkSonixVolumeReader()
{
}

//----------------------------------------------------------------------------
void vtkSonixVolumeReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkSonixVolumeReader::GenerateTrackedFrameFromSonixVolume(const char* volumeFileName, vtkTrackedFrameList* trackedFrameList, double acquisitionFrameRate/* = 10*/)
{
  if (volumeFileName == NULL)
  {
    LOG_ERROR("Failed to generate tracked frame from sonix volume - input file name is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( trackedFrameList == NULL )
  {
    LOG_ERROR("Failed to generate tracked frame from sonix volume - output tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  // read data file
  FILE * fp; 
  errno_t err = fopen_s (&fp,volumeFileName, "rb");

  if(err != 0)
  {
    LOG_ERROR("Error opening volume file: " << volumeFileName << " Error No.: " << err); 
    return PLUS_FAIL;
  }

  // Determine file size
  fseek (fp , 0 , SEEK_END);
  long fileSizeInBytes = ftell (fp);
  rewind (fp);

  // Ultrasonix header 
  uFileHeader hdr;

  // read header
  fread(&hdr, sizeof(hdr), 1, fp);   

  int dataType = hdr.type; 
  int sampleSizeInBytes = hdr.ss/8; 
  int numberOfFrames = hdr.frames; 
  int frameSizeInBytes = hdr.w * hdr.h * sampleSizeInBytes;
  int frameSize[2]={hdr.w, hdr.h};

  // Custom frame fields
  std::ostringstream strDataType; 
  strDataType << hdr.type; 

  std::ostringstream strTransmitFrequency; 
  strTransmitFrequency << hdr.txf; 

  std::ostringstream strSamplingFrequency; 
  strSamplingFrequency << hdr.sf; 

  std::ostringstream strProbeID; 
  strProbeID << hdr.probe; 

  std::ostringstream strDataRate; 
  strDataRate << hdr.dr; 

  std::ostringstream strLineDensity; 
  strLineDensity << hdr.ld; 

  int numberOfBytesToSkip = 0; 
  if ( (fileSizeInBytes - sizeof(hdr)) > frameSizeInBytes * numberOfFrames )
  {
    numberOfBytesToSkip = (fileSizeInBytes - sizeof(hdr)) / numberOfFrames  - frameSizeInBytes; 
    LOG_DEBUG("Each frame has " << numberOfBytesToSkip << " bytes header before the actual data"); 
  }
  else if ( (fileSizeInBytes - sizeof(hdr)) < frameSizeInBytes * numberOfFrames )
  {
    LOG_ERROR("Expected data size for reading (" << frameSizeInBytes * numberOfFrames 
      << " bytes) is larger than actual data size (" << fileSizeInBytes - sizeof(hdr) << " bytes)." );
    return PLUS_FAIL; 
  }
  
  // if vector data switch width and height, because the image
  // is not rasterized like a bitmap, but written rayline by rayline
  if( (dataType == udtBPre) || (dataType == udtRF) || (dataType == udtMPre) || (dataType == udtPWRF) ||  (dataType == udtColorRF) )
  {
    frameSize[0]=hdr.h; // number of data points recorded for one crystal (vectors)
    frameSize[1]=hdr.w; // number of transducer crystals (samples)
  }

  // Pointer to data from file 
  unsigned char* dataFromFile = new unsigned char[frameSizeInBytes];

  for (int i = 0; i < numberOfFrames; i++)
  {
    // Skip header data 
    fread(dataFromFile, numberOfBytesToSkip, 1, fp);

    // Read data from file 
    fread(dataFromFile, frameSizeInBytes, 1, fp);

    PlusCommon::ITKScalarPixelType pixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;    
    switch (dataType)
    {
    case udtBPost:
      pixelType=itk::ImageIOBase::UCHAR;
      break;
    case udtRF:
      pixelType=itk::ImageIOBase::SHORT;
      break;
    default:
      LOG_ERROR("Uknown pixel type for data type: " << dataType);
      continue; 
    }

    PlusVideoFrame videoFrame; 
    if ( videoFrame.AllocateFrame(frameSize, pixelType) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to allocate image data for frame #" << i); 
      continue; 
    }

    // Copy the frame data form file to vtkImageDataSet
    memcpy(videoFrame.GetBufferPointer(),dataFromFile,frameSizeInBytes);

    TrackedFrame trackedFrame; 
    trackedFrame.SetImageData(videoFrame); 
    trackedFrame.SetTimestamp( (1.0* (i + 1) ) / acquisitionFrameRate );  // Generate timestamp, but don't start from 0

    trackedFrame.SetCustomFrameField("SonixDataType", strDataType.str()); 
    trackedFrame.SetCustomFrameField("SonixTransmitFrequency", strTransmitFrequency.str()); 
    trackedFrame.SetCustomFrameField("SonixSamplingFrequency", strSamplingFrequency.str()); 
    trackedFrame.SetCustomFrameField("SonixDataRate", strDataRate.str()); 
    trackedFrame.SetCustomFrameField("SonixLineDensity", strLineDensity.str()); 
    trackedFrame.SetCustomFrameField("SonixProbeID", strProbeID.str()); 


    trackedFrameList->AddTrackedFrame(&trackedFrame); 
  }

  fclose(fp);
  delete [] dataFromFile;

  return PLUS_SUCCESS;
}



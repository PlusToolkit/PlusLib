/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusSonixVolumeReader.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "igsioVideoFrame.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "igsioTrackedFrame.h"

#include <iostream>
#include <sstream>

#include "ulterius_def.h"

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 1
  #include "utx_imaging_modes.h"
#endif


vtkStandardNewMacro(vtkPlusSonixVolumeReader);

//----------------------------------------------------------------------------
vtkPlusSonixVolumeReader::vtkPlusSonixVolumeReader()
{
}


//----------------------------------------------------------------------------
vtkPlusSonixVolumeReader::~vtkPlusSonixVolumeReader()
{
}

//----------------------------------------------------------------------------
void vtkPlusSonixVolumeReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusSonixVolumeReader::GenerateTrackedFrameFromSonixVolume(const char* volumeFileName, vtkIGSIOTrackedFrameList* trackedFrameList, double acquisitionFrameRate/* = 10*/)
{
  if (volumeFileName == NULL)
  {
    LOG_ERROR("Failed to generate tracked frame from sonix volume - input file name is NULL!");
    return PLUS_FAIL;
  }

  if (trackedFrameList == NULL)
  {
    LOG_ERROR("Failed to generate tracked frame from sonix volume - output tracked frame list is NULL!");
    return PLUS_FAIL;
  }

  // read data file
  FILE* fp;
  errno_t err = fopen_s(&fp, volumeFileName, "rb");

  if (err != 0)
  {
    LOG_ERROR("Error opening volume file: " << volumeFileName << " Error No.: " << err);
    return PLUS_FAIL;
  }

  // Determine file size
  fseek(fp, 0, SEEK_END);
  long fileSizeInBytes = ftell(fp);
  rewind(fp);

  // Ultrasonix header
  uFileHeader hdr;

  // read header
  fread(&hdr, sizeof(hdr), 1, fp);

  unsigned int dataType = static_cast<unsigned int>(hdr.type);
  unsigned int sampleSizeInBytes = static_cast<unsigned int>(hdr.ss / 8);
  unsigned int numberOfFrames = static_cast<unsigned int>(hdr.frames);
  unsigned int frameSizeInBytes = static_cast<unsigned int>(hdr.w * hdr.h * sampleSizeInBytes);
  FrameSizeType frameSize = { static_cast<unsigned int>(hdr.w), static_cast<unsigned int>(hdr.h), 1};

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

  unsigned int numberOfBytesToSkip = 0;
  if ((fileSizeInBytes - sizeof(hdr)) > frameSizeInBytes * numberOfFrames)
  {
    numberOfBytesToSkip = (fileSizeInBytes - sizeof(hdr)) / numberOfFrames  - frameSizeInBytes;
    LOG_DEBUG("Each frame has " << numberOfBytesToSkip << " bytes header before the actual data");
  }
  else if ((fileSizeInBytes - sizeof(hdr)) < frameSizeInBytes * numberOfFrames)
  {
    LOG_ERROR("Expected data size for reading (" << frameSizeInBytes * numberOfFrames
              << " bytes) is larger than actual data size (" << fileSizeInBytes - sizeof(hdr) << " bytes).");
    return PLUS_FAIL;
  }

  // if vector data switch width and height, because the image
  // is not rasterized like a bitmap, but written rayline by rayline
  if ((dataType == udtBPre) || (dataType == udtRF) || (dataType == udtMPre) || (dataType == udtPWRF) || (dataType == udtColorRF))
  {
    frameSize[0] = hdr.h; // number of data points recorded for one crystal (vectors)
    frameSize[1] = hdr.w; // number of transducer crystals (samples)
    frameSize[2] = 1; // only 1 slice
  }

  // Pointer to data from file
  unsigned char* dataFromFile = new unsigned char[frameSizeInBytes];

  for (unsigned int i = 0; i < numberOfFrames; i++)
  {
    // Skip header data
    fread(dataFromFile, numberOfBytesToSkip, 1, fp);

    // Read data from file
    fread(dataFromFile, frameSizeInBytes, 1, fp);

    igsioCommon::VTKScalarPixelType pixelType = VTK_VOID;
    switch (dataType)
    {
      case udtBPost:
        pixelType = VTK_UNSIGNED_CHAR;
        break;
      case udtRF:
        pixelType = VTK_SHORT;
        break;
      default:
        LOG_ERROR("Uknown pixel type for data type: " << dataType);
        continue;
    }

    // If in the future sonix generates color images, we can change this to support that
    unsigned int numberOfScalarComponents = 1;

    igsioVideoFrame videoFrame;
    if (videoFrame.AllocateFrame(frameSize, pixelType, numberOfScalarComponents) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to allocate image data for frame #" << i);
      continue;
    }

    // Copy the frame data form file to vtkImageDataSet
    memcpy(videoFrame.GetScalarPointer(), dataFromFile, frameSizeInBytes);

    igsioTrackedFrame trackedFrame;
    trackedFrame.SetImageData(videoFrame);
    trackedFrame.SetTimestamp((1.0 * (i + 1)) / acquisitionFrameRate);       // Generate timestamp, but don't start from 0

    trackedFrame.SetFrameField("SonixDataType", strDataType.str());
    trackedFrame.SetFrameField("SonixTransmitFrequency", strTransmitFrequency.str());
    trackedFrame.SetFrameField("SonixSamplingFrequency", strSamplingFrequency.str());
    trackedFrame.SetFrameField("SonixDataRate", strDataRate.str());
    trackedFrame.SetFrameField("SonixLineDensity", strLineDensity.str());
    trackedFrame.SetFrameField("SonixProbeID", strProbeID.str());


    trackedFrameList->AddTrackedFrame(&trackedFrame);
  }

  fclose(fp);
  delete [] dataFromFile;

  return PLUS_SUCCESS;
}



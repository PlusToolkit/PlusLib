/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// GRAB_FRAME API was partly contributed by by Xin Kang at SZI, Children's National Medical Center

// Local includes
#include "PlusConfigure.h"
#include "PixelCodec.h"
#include "vtkPlusBkProFocusOemVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtk_png.h>
#include <vtksys/SystemTools.hxx>
#include <vtkClientSocket.h>
#include <vtkMath.h>

// STL includes
#include <iostream>
#include <ostream>
#include <string>

// System includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int TIMESTAMP_SIZE = 4;

const char* vtkPlusBkProFocusOemVideoSource::KEY_DEPTH          = "Depth";
const char* vtkPlusBkProFocusOemVideoSource::KEY_GAIN           = "Gain";

vtkStandardNewMacro(vtkPlusBkProFocusOemVideoSource);

class vtkPlusBkProFocusOemVideoSource::vtkInternal
{
public:
  vtkPlusBkProFocusOemVideoSource* External;

  vtkPlusChannel* Channel;

  vtkSmartPointer<vtkClientSocket> VtkSocket;
  std::vector<char> OemMessage;

  // Image buffer to hold the decoded image frames, it's a member variable to avoid memory allocation at each frame receiving
  vtkImageData* DecodedImageFrame;
  // Data buffer to hold temporary data during decoding, it's a member variable to avoid memory allocation at each frame receiving
  std::vector<unsigned char> DecodingBuffer;
  // Data buffer to hold temporary data during decoding (pointers to image lines), it's a member variable to avoid memory allocation at each frame receiving
  std::vector<png_bytep> DecodingLineBuffer;


  vtkInternal(vtkPlusBkProFocusOemVideoSource* external)
    : External(external)
    , Channel(NULL)
  {
    this->DecodedImageFrame = vtkImageData::New();
  }

  virtual ~vtkInternal()
  {
    this->Channel = NULL;
    this->DecodedImageFrame->Delete();
    this->DecodedImageFrame = NULL;
    this->External = NULL;
  }

};


//----------------------------------------------------------------------------
vtkPlusBkProFocusOemVideoSource::vtkPlusBkProFocusOemVideoSource()
{
  this->Internal = new vtkInternal(this);

  this->ScannerAddress = "";
  this->OemPort = 0;

  this->UltrasoundWindowSize[0] = 0;
  this->UltrasoundWindowSize[1] = 0;
  this->ContinuousStreamingEnabled = false;
  this->ColorEnabled = false;
  this->OfflineTesting = false;
  this->OfflineTestingFilePath = NULL;
  this->StartLineX_m = 0;
  this->StartLineY_m = 0;
  this->StartLineAngle_rad = 0;
  this->StartDepth_m = 0;
  this->StopLineX_m = 0;
  this->StopLineY_m = 0;
  this->StopLineAngle_rad = 0;
  this->StopDepth_m = 0;
  this->pixelLeft_pix = 0;
  this->pixelTop_pix = 0;
  this->pixelRight_pix = 0;
  this->pixelBottom_pix = 0;
  this->tissueLeft_m = 0;
  this->tissueTop_m = 0;
  this->tissueRight_m = 0;
  this->tissueBottom_m = 0;
  this->gain_percent = 0;
  this->probeTypePortA = UNKNOWN;
  this->probeTypePortB = UNKNOWN;
  this->probeTypePortC = UNKNOWN;
  this->probeTypePortM = UNKNOWN;
  this->probePort = "";

  this->RequireImageOrientationInConfiguration = true;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 1; // image retrieval may slow down the exam software, so keep the frame rate low by default
}

//----------------------------------------------------------------------------
vtkPlusBkProFocusOemVideoSource::~vtkPlusBkProFocusOemVideoSource()
{
  if (!this->Connected)
  {
    this->Disconnect();
  }

  delete this->Internal;
  this->Internal = NULL;
  this->ScannerAddress = "";
  this->OfflineTestingFilePath = NULL;
}

//----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusBkProFocusOemVideoSource::InternalConnect");

  if (this->Internal->Channel == NULL)
  {
    if (this->OutputChannels.empty())
    {
      LOG_ERROR("Cannot connect: no output channel is specified for device " << this->GetDeviceId());
      return PLUS_FAIL;
    }
    this->Internal->Channel = this->OutputChannels[0];
  }

  // Clear buffer on connect because the new frames that we will acquire might have a different size
  this->Internal->Channel->Clear();
  this->Internal->VtkSocket = vtkSmartPointer<vtkClientSocket>::New();

  LOG_DEBUG("BK scanner address: " << this->ScannerAddress);
  LOG_DEBUG("BK scanner OEM port: " << this->OemPort);

  if (this->OfflineTesting)
  {
    LOG_INFO("Offline testing on");
    LOG_DEBUG("Offline testing file path: " << this->OfflineTestingFilePath);
  }
  else
  {
    LOG_DEBUG("Connecting to BK scanner");
    bool connected = (this->Internal->VtkSocket->ConnectToServer(this->ScannerAddress.c_str(), this->OemPort) == 0);
    if (!connected)
    {
      LOG_ERROR("Could not connect to BKProFocusOem:"
                << " scanner address = " << this->ScannerAddress
                << ", OEM port = " << this->OemPort);
      return PLUS_FAIL;
    }
    LOG_DEBUG("Connected to BK scanner");

    if (!(this->RequestParametersFromScanner()
          && this->ConfigEventsOn()
          && this->SubscribeToParameterChanges()
         ))
    {
      LOG_ERROR("Cound not init BK scanner");
      return PLUS_FAIL;
    }
    if (this->ContinuousStreamingEnabled)
    {
      if (!this->StartContinuousDataStreaming())
      {
        return PLUS_FAIL;
      }

      LOG_DEBUG("Connected to BK scanner");
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::StartContinuousDataStreaming()
{
  std::string query;
  if (this->ColorEnabled)
  {
    //Switch to power doppler
    /*if (this->CommandPowerDopplerOn() != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }*/
    query = "QUERY:GRAB_FRAME \"ON\",20,\"OVERLAY\";";
  }
  else
  {
    query = "QUERY:GRAB_FRAME \"ON\",20;";
  }

  LOG_DEBUG("Start data streaming. Query: " << query);
  if (!SendQuery(query))
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::StopContinuousDataStreaming()
{
  std::string query = "QUERY:GRAB_FRAME \"OFF\";";
  LOG_DEBUG("Stop data streaming. Query: " << query);
  if (!SendQuery(query))
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::InternalDisconnect()
{
  LOG_TRACE("Disconnect from BKProFocusOem");

  if (!this->OfflineTesting && this->ContinuousStreamingEnabled)
  {
    if (!this->StopContinuousDataStreaming())
    {
      return PLUS_FAIL;
    }
  }
  this->StopRecording();

  if (this->Internal->VtkSocket->GetConnected())
  { this->Internal->VtkSocket->CloseSocket(); }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::InternalStartRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::InternalUpdate()
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  unsigned char* uncompressedPixelBuffer = 0;
  unsigned int uncompressedPixelBufferSize = 0;
  int numBytesProcessed = 0;
  if (!this->OfflineTesting)
  {
    if (!this->ContinuousStreamingEnabled)
    {
      std::string query = "query:capture_image \"PNG\";";
      LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);
      if (!SendQuery(query))
      {
        return PLUS_FAIL;
      }
    }
    //Process all incoming messages until an image message is found
    if (!this->ProcessMessagesAndReadNextImage())
    {
      return PLUS_FAIL;
    }
    size_t numBytesReceived = this->Internal->OemMessage.size();

    // First detect the #
    for (numBytesProcessed = 0; this->Internal->OemMessage[numBytesProcessed] != '#' && numBytesProcessed < numBytesReceived; numBytesProcessed++)
      ;
    numBytesProcessed++;

    int numChars = (int)this->Internal->OemMessage[numBytesProcessed] - (int)('0');
    numBytesProcessed++;
    LOG_TRACE("Number of bytes in the image size: " << numChars); // 7 or 6
    if (numChars == 0)
    {
      LOG_ERROR("Failed to read image from BK OEM interface");
      return PLUS_FAIL;
    }

    for (int k = 0; k < numChars; k++, numBytesProcessed++)
    {
      uncompressedPixelBufferSize = uncompressedPixelBufferSize * 10 + ((int)this->Internal->OemMessage[numBytesProcessed] - '0');
    }
    LOG_TRACE("uncompressedPixelBufferSize = " << uncompressedPixelBufferSize);

    uncompressedPixelBuffer = (unsigned char*) & (this->Internal->OemMessage[numBytesProcessed]);

    if (this->ContinuousStreamingEnabled)
    {
      // Extract timestamp of the image
      char timeStamp[TIMESTAMP_SIZE];
      for (int k = 0; k < TIMESTAMP_SIZE; k++, numBytesProcessed++)
      {
        timeStamp[k] = this->Internal->OemMessage[numBytesProcessed];
      }
      // Seems this is NOT correct, but the format is NOT described in the manual
      unsigned int _timestamp = *(int*)timeStamp;
      //LOG_TRACE("Image timestamp = " << static_cast<std::ostringstream*>(&(std::ostringstream() << _timestamp))->str());
    }
  }

  if (!this->OfflineTesting && this->ContinuousStreamingEnabled)
  {
    this->Internal->DecodedImageFrame->SetExtent(0, this->UltrasoundWindowSize[0] - 1, 0, this->UltrasoundWindowSize[1] - 1, 0, 0);

    if (uncompressedPixelBufferSize > (this->UltrasoundWindowSize[0] * this->UltrasoundWindowSize[1] + TIMESTAMP_SIZE))
    {
      // we received color image
      this->Internal->DecodedImageFrame->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
      PlusStatus status = PixelCodec::ConvertToBmp24(PixelCodec::ComponentOrder_RGB, PixelCodec::PixelEncoding_BGR24, this->UltrasoundWindowSize[0], this->UltrasoundWindowSize[1],
                          (unsigned char*) & (this->Internal->OemMessage[numBytesProcessed]),
                          (unsigned char*)this->Internal->DecodedImageFrame->GetScalarPointer());
    }
    else
    {
      this->Internal->DecodedImageFrame->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
      std::memcpy(this->Internal->DecodedImageFrame->GetScalarPointer(),
                  (void*) & (this->Internal->OemMessage[numBytesProcessed]),
                  uncompressedPixelBufferSize);
      LOG_TRACE(uncompressedPixelBufferSize << " bytes copied, start at " << numBytesProcessed); // 29
    }
  }
  else
  {
    if (DecodePngImage(uncompressedPixelBuffer, uncompressedPixelBufferSize, this->Internal->DecodedImageFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to decode received PNG image on channel " << this->Internal->Channel->GetChannelId());
      return PLUS_FAIL;
    }
  }
  this->FrameNumber++;

  vtkPlusDataSource* aSource(NULL);
  if (this->Internal->Channel->GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the BKProFocusOem device on channel " << this->Internal->Channel->GetChannelId());
    return PLUS_FAIL;
  }
  // If the buffer is empty, set the pixel type and frame size to the first received properties
  if (aSource->GetNumberOfItems() == 0)
  {
    LOG_DEBUG("Set up BK ProFocus image buffer");
    int* frameDimensions = this->Internal->DecodedImageFrame->GetDimensions();
    if (frameDimensions[0] < 0 || frameDimensions[1] < 0 || frameDimensions[2] < 0)
    {
      LOG_ERROR("Invalid frame dimensions.");
      return PLUS_FAIL;
    }
    FrameSizeType frameSizeInPix = { static_cast<unsigned int>(frameDimensions[0]), static_cast<unsigned int>(frameDimensions[1]), static_cast<unsigned int>(frameDimensions[2]) };
    aSource->SetPixelType(this->Internal->DecodedImageFrame->GetScalarType());
    if (this->Internal->DecodedImageFrame->GetNumberOfScalarComponents() == 1)
    {
      aSource->SetImageType(US_IMG_BRIGHTNESS);
    }
    else
    {
      aSource->SetNumberOfScalarComponents(this->Internal->DecodedImageFrame->GetNumberOfScalarComponents());
      aSource->SetImageType(US_IMG_RGB_COLOR);
    }
    aSource->SetInputFrameSize(frameSizeInPix[0], frameSizeInPix[1], frameSizeInPix[2]);

    LOG_DEBUG("Frame size: " << frameSizeInPix[0] << "x" << frameSizeInPix[1]
              << ", pixel type: " << vtkImageScalarTypeNameMacro(this->Internal->DecodedImageFrame->GetScalarType())
              << ", buffer image orientation: " << igsioVideoFrame::GetStringFromUsImageOrientation(aSource->GetInputImageOrientation()));
    LOG_DEBUG("NumberOfScalarComponents: " << aSource->GetNumberOfScalarComponents());
  }

  double spacingZ_mm = 1.0;
  //TODO: Send spacing with image
  this->Internal->DecodedImageFrame->SetSpacing(GetSpacingX(), GetSpacingY(), spacingZ_mm);//Spacing is not being sent to IGTLink?

  this->AddParametersToFrameFields();
  if (aSource->AddItem(this->Internal->DecodedImageFrame, aSource->GetInputImageOrientation(), aSource->GetImageType(), this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &this->FrameFields) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error adding item to video source " << aSource->GetSourceId() << " on channel " << this->Internal->Channel->GetChannelId());
    return PLUS_FAIL;
  }
  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::ProcessMessagesAndReadNextImage()
{
  while (true)
  {
    LOG_TRACE("Before client read");
    if (!this->ReadNextMessage())
    {
      LOG_ERROR("Failed to read response from BK OEM interface");
      return PLUS_FAIL;
    }

    std::string fullMessage = this->ReadBufferIntoString();
    std::istringstream replyStream(fullMessage);

    std::string messageString;
    std::getline(replyStream, messageString, ' ');

    std::istringstream messageStream(messageString);
    std::string messageType;
    std::string messageName;
    std::string messageSubtype;//Typically A or B (view A or B on the scanner). Currently only view A is handled.
    std::getline(messageStream, messageType, ':');
    std::getline(messageStream, messageName, ':');
    std::getline(messageStream, messageSubtype, ':');

    LOG_DEBUG("Process message from BK: " << fullMessage);

    if (messageString.compare("DATA:CAPTURE_IMAGE") == 0)
    {
      return PLUS_SUCCESS;
    }
    else if (messageString.compare("DATA:GRAB_FRAME") == 0)
    {
      return PLUS_SUCCESS;
    }
    //Handle both replies to queries (DATA) and subscribed data (SDATA)
    else if ((messageType.compare("DATA") == 0) || (messageType.compare("SDATA") == 0))
    {
      if (messageName.compare("US_WIN_SIZE") == 0)
      {
        this->ParseImageSize(replyStream);
      }
      else if ((messageName.compare("B_GEOMETRY_SCANAREA") == 0) && (messageSubtype.compare("A") == 0))
      {
        this->ParseGeometryScanarea(replyStream);
      }
      else if ((messageName.compare("B_GEOMETRY_PIXEL") == 0) && (messageSubtype.compare("A") == 0))
      {
        this->ParseGeometryPixel(replyStream);
      }
      else if ((messageName.compare("B_GEOMETRY_TISSUE") == 0) && (messageSubtype.compare("A") == 0))
      {
        this->ParseGeometryTissue(replyStream);
      }
      else if ((messageName.compare("B_GEOMETRY_US_FRAME_GRAB") == 0) && (messageSubtype.compare("A") == 0))
      {
        this->ParseGeometryUsGrabFrame(replyStream);
      }
      else if ((messageName.compare("B_GAIN") == 0) && (messageSubtype.compare("A") == 0))
      {
        this->ParseGain(replyStream);
      }
      else if (messageName.compare("TRANSDUCER_LIST") == 0)
      {
        this->ParseTransducerList(replyStream);
      }
      else if ((messageName.compare("TRANSDUCER") == 0) && (messageSubtype.compare("A") == 0))
      {
        this->ParseTransducerData(replyStream);
      }
    }
    else if ((messageString.compare("EVENT:TRANSDUCER_CONNECT;") == 0)
             || (messageString.compare("EVENT:TRANSDUCER_DISCONNECT;") == 0)
             || (messageString.compare("EVENT:TRANSDUCER_SELECTED;") == 0))
    {
      this->QueryTransducerList();//Need to query, as this can't be subscribed to
    }
    else if (messageString.compare("EVENT:FREEZE;") == 0)
    {
      LOG_DEBUG("Freeze");
    }
    else if (messageString.compare("EVENT:UNFREEZE;") == 0)
    {
      LOG_DEBUG("Unfreeze");
    }
    else if (messageString.compare("ACK;") == 0)
    {
      LOG_DEBUG("Acknowledge message received");
    }
    else
    {
      LOG_WARNING("Received unknown message from BK: " << messageString);
    }
  }

  return PLUS_SUCCESS; //Should newer reach this
}

//-----------------------------------------------------------------------------
std::string vtkPlusBkProFocusOemVideoSource::ReadBufferIntoString()
{
  std::string retval(this->Internal->OemMessage.begin(), this->Internal->OemMessage.end());
  return retval;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::ReadNextMessage()
{
  std::vector<char> rawMessage;
  char character(0);
  unsigned totalBytes = 0;
  int receivedBytes = 1;
  while (character != EOT && receivedBytes >= 1)
  {
    receivedBytes = this->Internal->VtkSocket->Receive(&character, 1);
    rawMessage.push_back(character);
    totalBytes++;
    //Speedup by reading the large data blocks in one operation
    if (character == '#') //Read binary block
    {
      receivedBytes = this->Internal->VtkSocket->Receive(&character, 1);
      if (receivedBytes == 1)
      {
        int numChars = (int)character - (int)('0');
        rawMessage.push_back(character);
        totalBytes++;
        LOG_TRACE("Number of bytes in binary data block size: " << numChars); // Typically 7 or 6
        if (numChars <= 1 || numChars >= 9)
        {
          LOG_ERROR("Error in binary data block from BK OEM interface. Incorrect character after block start (#): " << character << "(char num: " << (int)character << ")" <<  " (should be 6 or 7) ");
          return PLUS_FAIL;
        }
        else
        {
          unsigned int uncompressedPixelBufferSize = 0;
          for (int k = 0; k < numChars; k++, totalBytes++)
          {
            receivedBytes = this->Internal->VtkSocket->Receive(&character, 1);
            if (receivedBytes != 1)
            {
              LOG_ERROR("Error in binary data block from BK OEM interface. Missing block size character.");
            }
            rawMessage.push_back(character);
            uncompressedPixelBufferSize = uncompressedPixelBufferSize * 10 + ((int)character - (int)'0');
          }
          LOG_DEBUG("uncompressedPixelBufferSize = " << uncompressedPixelBufferSize);

          int rawSize = rawMessage.size();
          rawMessage.resize(rawSize + uncompressedPixelBufferSize);
          receivedBytes = this->Internal->VtkSocket->Receive(&rawMessage[rawSize], uncompressedPixelBufferSize, true);

          int totalReceivedBytes = totalReceivedBytes;
          if (receivedBytes != uncompressedPixelBufferSize)
          {
            LOG_ERROR("Failed to read full binary data block from BK OEM interface receivedBytes: " << receivedBytes);
            return PLUS_FAIL;
          }
          totalBytes += uncompressedPixelBufferSize;

          // The BK5000 scanner used for testing did not send data blocks matching the specified size.
          // Currently all the binary data are added to the image.
          receivedBytes = this->addAdditionalBinaryDataToImageUntilEOTReached(character, rawMessage);

          totalBytes += receivedBytes;
        }
      }
      else
      {
        LOG_ERROR("Error in binary data block from BK OEM interface. No character after block start #");
        receivedBytes = 1;
      }
    }//if
  }//while
  this->Internal->OemMessage = removeSpecialCharacters(rawMessage);

  if (receivedBytes < 1)
  {
    LOG_ERROR("Error in binary data block from BK OEM interface. No data.");
    return PLUS_FAIL;
  }
  else
  {
    return PLUS_SUCCESS;
  }
}

//-----------------------------------------------------------------------------
int vtkPlusBkProFocusOemVideoSource::addAdditionalBinaryDataToImageUntilEOTReached(char& character, std::vector<char>& rawMessage)
{
  int receivedBytes = this->Internal->VtkSocket->Receive(&character, 1);
  if (receivedBytes != 0 && character != EOT)
  {
    LOG_WARNING("Unspecified charactes received. Adding these to image.");
    while (character != EOT)
    {
      rawMessage.push_back(character);
      receivedBytes = receivedBytes + this->Internal->VtkSocket->Receive(&character, 1);
    }
    LOG_DEBUG("Added additional characters to image: " << receivedBytes);
  }
  rawMessage.push_back(character); //Add EOT character
  return receivedBytes;
}

//-----------------------------------------------------------------------------
std::vector<char> vtkPlusBkProFocusOemVideoSource::removeSpecialCharacters(std::vector<char> inMessage)
{
  std::vector<char> retval;
  unsigned int inPos = 1;//Skip starting character SOH
  while (inPos < inMessage.size() - 1)//Skip ending character EOT
  {
    if ((inMessage[inPos]) != ESC)
    {
      retval.push_back(inMessage[inPos++]);
    }
    else
    {
      inPos++;
      retval.push_back(~inMessage[inPos++]);//Character after ESC is inverted
    }
  }
  return retval;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::RequestParametersFromScanner()
{
  if (this->QueryImageSize() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryGeometryScanarea() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryGeometryPixel() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryGeometryUsGrabFrame() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryGeometryTissue() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryGain() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryTransducerList() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->QueryTransducer() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
// QUERY:US_WIN_SIZE;
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryImageSize()
{
  LOG_DEBUG("Get ultrasound image size from BKProFocusOem");

  std::string query = "QUERY:US_WIN_SIZE;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);
  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseImageSize(std::istringstream& replyStream)
{
  std::string stringVal;
  std::getline(replyStream, stringVal, ',');
  this->UltrasoundWindowSize[0] = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ';');
  this->UltrasoundWindowSize[1] = atoi(stringVal.c_str());

  LOG_TRACE("Ultrasound image size = " << this->UltrasoundWindowSize[0] << " x " << this->UltrasoundWindowSize[1]);
}

//-----------------------------------------------------------------------------
// QUERY:B_GEOMETRY_SCANAREA;
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryGeometryScanarea()
{
  LOG_DEBUG("Get ultrasound geometry from BKProFocusOem");

  std::string query = "QUERY:B_GEOMETRY_SCANAREA:A;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseGeometryScanarea(std::istringstream& replyStream)
{
  std::string stringVal;
  std::getline(replyStream, stringVal, ',');
  StartLineX_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  StartLineY_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  StartLineAngle_rad = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  StartDepth_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  StopLineX_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  StopLineY_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  StopLineAngle_rad = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ';');
  StopDepth_m = atof(stringVal.c_str());

  LOG_DEBUG("Ultrasound geometry. StartLineX_m: " << StartLineX_m << " StartLineY_m: " << StartLineY_m << " StartLineAngle_rad: " << StartLineAngle_rad <<
            " StartDepth_m: " << StartDepth_m << " StopLineX_m: " << StopLineX_m << " StopLineY_m: " << StopLineY_m << " StopLineAngle_rad: " << StopLineAngle_rad << " StopDepth_m: " << StopDepth_m);
}

//-----------------------------------------------------------------------------
// QUERY:B_GEOMETRY_PIXEL;
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryGeometryPixel()
{
  std::string query = "QUERY:B_GEOMETRY_PIXEL:A;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseGeometryPixel(std::istringstream& replyStream)
{
  std::string stringVal;
  std::getline(replyStream, stringVal, ',');
  pixelLeft_pix = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  pixelTop_pix = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  pixelRight_pix = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ';');
  pixelBottom_pix = atoi(stringVal.c_str());

  LOG_DEBUG("Ultrasound geometry. pixelLeft_pix: " << pixelLeft_pix << " pixelTop_pix: " << pixelTop_pix << " pixelRight_pix: " << pixelRight_pix << " pixelBottom_pix: " << pixelBottom_pix);
}

//-----------------------------------------------------------------------------
// QUERY:B_GEOMETRY_US_FRAME_GRAB;
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryGeometryUsGrabFrame()
{
  std::string query = "QUERY:B_GEOMETRY_US_FRAME_GRAB:A;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseGeometryUsGrabFrame(std::istringstream& replyStream)
{
  std::string stringVal;
  std::getline(replyStream, stringVal, ',');
  grabFramePixelLeft_pix = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  grabFramePixelTop_pix = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  grabFramePixelRight_pix = atoi(stringVal.c_str());
  std::getline(replyStream, stringVal, ';');
  grabFramePixelBottom_pix = atoi(stringVal.c_str());

  LOG_DEBUG("Ultrasound grab frame geometry. grabFramePixelLeft_pix: " << grabFramePixelLeft_pix
            << " grabFramePixelTop_pix: " << grabFramePixelTop_pix
            << " grabFramePixelRight_pix: " << grabFramePixelRight_pix
            << " grabFramePixelBottom_pix: " << grabFramePixelBottom_pix);
}

//-----------------------------------------------------------------------------
// QUERY:B_GEOMETRY_TISSUE;
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryGeometryTissue()
{
  std::string query = "QUERY:B_GEOMETRY_TISSUE:A;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseGeometryTissue(std::istringstream& replyStream)
{
  std::string stringVal;
  std::getline(replyStream, stringVal, ',');
  tissueLeft_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  tissueTop_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ',');
  tissueRight_m = atof(stringVal.c_str());
  std::getline(replyStream, stringVal, ';');
  tissueBottom_m = atof(stringVal.c_str());
  LOG_DEBUG("Ultrasound geometry. tissueLeft_m: " << tissueLeft_m << " tissueTop_m: " << tissueTop_m << " tissueRight_m: " << tissueRight_m << " tissueBottom_m: " << tissueBottom_m);
}

//-----------------------------------------------------------------------------
// QUERY:B_GAIN;
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryGain()
{
  std::string query = "QUERY:B_GAIN:A;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseGain(std::istringstream& replyStream)
{
  std::string stringVal;
  std::getline(replyStream, stringVal, ';');
  gain_percent = atoi(stringVal.c_str());
  LOG_DEBUG("Ultrasound gain. gain_percent: " << gain_percent);
}

//-----------------------------------------------------------------------------
// QUERY:TRANSDUCER_LIST;
// Get list of transducers, connected to which port, and transducer type
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryTransducerList()
{
  std::string query = "QUERY:TRANSDUCER_LIST;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseTransducerList(std::istringstream& replyStream)
{
  std::string probeName;
  std::string probeType;
  //Port A
  std::getline(replyStream, probeName, ',');
  std::getline(replyStream, probeType, ',');
  this->SetProbeTypeForPort("A", RemoveQuotationMarks(probeType));
  //Port B
  std::getline(replyStream, probeName, ',');
  std::getline(replyStream, probeType, ',');
  this->SetProbeTypeForPort("B", RemoveQuotationMarks(probeType));
  //Port C
  std::getline(replyStream, probeName, ',');
  std::getline(replyStream, probeType, ',');
  this->SetProbeTypeForPort("C", RemoveQuotationMarks(probeType));
  //Port M
  std::getline(replyStream, probeName, ',');
  std::getline(replyStream, probeType, ';');
  this->SetProbeTypeForPort("M", RemoveQuotationMarks(probeType));
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::SetProbeTypeForPort(std::string port, std::string probeTypeString)
{
  IGTLIO_PROBE_TYPE probeTypeEnum = UNKNOWN;

  if (probeTypeString.compare("C") == 0)
  {
    probeTypeEnum = SECTOR;
  }
  else if (probeTypeString.compare("L") == 0)
  {
    probeTypeEnum = LINEAR;
  }
  else if (probeTypeString.compare("M") == 0)
  {
    probeTypeEnum = MECHANICAL;
  }

  if (port.compare("A") == 0)
  {
    probeTypePortA = probeTypeEnum;
  }
  else if (port.compare("B") == 0)
  {
    probeTypePortB = probeTypeEnum;
  }
  else if (port.compare("C") == 0)
  {
    probeTypePortC = probeTypeEnum;
  }
  else if (port.compare("M") == 0)
  {
    probeTypePortM = probeTypeEnum;
  }
}

//-----------------------------------------------------------------------------
std::string vtkPlusBkProFocusOemVideoSource::RemoveQuotationMarks(std::string inString)
{
  std::string retval;
  std::istringstream inStream(inString);
  std::getline(inStream, retval, '"');//Removes first "
  std::getline(inStream, retval, '"');//Read characters until next " is found
  return retval;
}

//-----------------------------------------------------------------------------
// QUERY:TRANSDUCER:A;
// Get transducer that is used to create view A
PlusStatus vtkPlusBkProFocusOemVideoSource::QueryTransducer()
{
  std::string query = "QUERY:TRANSDUCER:A;";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
void vtkPlusBkProFocusOemVideoSource::ParseTransducerData(std::istringstream& replyStream)
{
  std::string probePortString;
  std::string probeName;
  std::getline(replyStream, probePortString, ',');
  std::getline(replyStream, probeName, ';');
  probePort = this->RemoveQuotationMarks(probePortString);
}

//-----------------------------------------------------------------------------
// CONFIG:DATA:SUBSCRIBE;
PlusStatus vtkPlusBkProFocusOemVideoSource::SubscribeToParameterChanges()
{
  std::string query = "CONFIG:DATA:SUBSCRIBE ";
  query += "\"US_WIN_SIZE\"";
  query += ",\"B_GEOMETRY_SCANAREA\"";
  query += ",\"B_GEOMETRY_PIXEL\"";
  query += ",\"B_GEOMETRY_US_FRAME_GRAB\"";
  query += ",\"B_GEOMETRY_TISSUE\"";
  query += ",\"B_GAIN\"";
  query += ",\"TRANSDUCER\"";
  query += ";";
  LOG_TRACE("Query from vtkPlusBkProFocusOemVideoSource: " << query);

  return SendQuery(query);
}

//-----------------------------------------------------------------------------
// CONFIG:EVENTS;
PlusStatus vtkPlusBkProFocusOemVideoSource::ConfigEventsOn()
{
  std::string query = "CONFIG:EVENTS 1;";
  LOG_TRACE("Command from vtkPlusBkProFocusOemVideoSource: " << query);
  return SendQuery(query);
}

//-----------------------------------------------------------------------------
// COMMAND:P_MODE;
/*PlusStatus vtkPlusBkProFocusOemVideoSource::CommandPowerDopplerOn()
{
  std::string query = "COMMAND:P_MODE: \"ON\";";
  LOG_TRACE("Command from vtkPlusBkProFocusOemVideoSource: " << query);
  return SendQuery(query);
}*/

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::SendQuery(std::string query)
{
  std::string codedQuery = this->AddSpecialCharacters(query);

  if (!this->Internal->VtkSocket->Send(codedQuery.c_str(), codedQuery.size()))
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkPlusBkProFocusOemVideoSource::AddSpecialCharacters(std::string query)
{
  std::string retval;
  const char special[] = { SOH, EOT, ESC, 0 }; // 0 is not special, it is an indicator for end of string
  retval += SOH; //Add start character
  for (unsigned int i = 0; i < query.size(); i++)
  {
    char ch = query[i];
    if (NULL != strchr(special, ch))
    {
      retval += ESC; //Escape special character
      ch = ~ch; //Invert special character
    }
    retval += ch;
  }
  retval += EOT; //Add end character
  return retval;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ScannerAddress, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, OemPort, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ContinuousStreamingEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ColorEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(OfflineTesting, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(OfflineTestingFilePath, deviceConfig);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(ScannerAddress, deviceConfig);
  std::stringstream ss;
  ss << this->OemPort;
  deviceConfig->SetAttribute("OemPort", ss.str().c_str());
  XML_WRITE_BOOL_ATTRIBUTE(ContinuousStreamingEnabled, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(ColorEnabled, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(OfflineTesting, deviceConfig);
  XML_WRITE_CSTRING_ATTRIBUTE_IF_NOT_NULL(OfflineTestingFilePath, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusBkProFocusOemVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusBkProFocusOemVideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  this->Internal->Channel = this->OutputChannels[0];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void ReadDataFromByteArray(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
  if (png_ptr->io_ptr == NULL)
  {
    LOG_ERROR("ReadDataFromInputStream failed, no input pointer is set");
    png_error(png_ptr, "ReadDataFromInputStream failed, no input pointer is set");
    return;
  }

  unsigned char* bufferPointer = (unsigned char*)png_ptr->io_ptr;
  memcpy(outBytes, bufferPointer, byteCountToRead);
  bufferPointer += byteCountToRead;

  png_ptr->io_ptr = bufferPointer;
}

//----------------------------------------------------------------------------
void PngErrorCallback(png_structp png_ptr, png_const_charp message)
{
  LOG_ERROR("PNG error: " << (message ? message : "no details available"));
}

//----------------------------------------------------------------------------
void PngWarningCallback(png_structp png_ptr, png_const_charp message)
{
  LOG_WARNING("PNG warning: " << (message ? message : "no details available"));
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::DecodePngImage(unsigned char* pngBuffer, unsigned int pngBufferSize, vtkImageData* decodedImage)
{
  std::vector<unsigned char> fileReadBuffer;
  if (this->OfflineTesting)
  {
    FILE* fp = fopen(this->OfflineTestingFilePath, "rb");
    if (!fp)
    {
      LOG_ERROR("Failed to read png");
      return PLUS_FAIL;
    }
    fseek(fp, 0, SEEK_END);
    size_t fileSizeInBytes = ftell(fp);
    rewind(fp);
    fileReadBuffer.resize(fileSizeInBytes);
    pngBuffer = &(fileReadBuffer[0]);
    fread(pngBuffer, 1, fileSizeInBytes, fp);
    fclose(fp);
  }

  unsigned int headerSize = 8;
  unsigned char* header = pngBuffer; // a 8-byte header
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
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
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
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    LOG_ERROR("Failed to decode PNG buffer");
    return PLUS_FAIL;
  }

  png_set_read_fn(png_ptr, pngBuffer + 8, ReadDataFromByteArray);

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
  this->Internal->DecodingBuffer.resize(rowbytes * height);
  unsigned char* tempImage = &(this->Internal->DecodingBuffer[0]);

  this->Internal->DecodingLineBuffer.resize(height);
  png_bytep* row_pointers = &(this->Internal->DecodingLineBuffer[0]);
  for (unsigned int ui = 0; ui < height; ++ui)
  {
    row_pointers[ui] = tempImage + rowbytes * ui;
  }
  png_read_image(png_ptr, row_pointers);

  int numberOfScalarComponents = png_get_channels(png_ptr, info_ptr);

  if (width * numberOfScalarComponents != rowbytes)
  {
    LOG_WARNING("There is padding at the end of PNG lines, image may be skewed");
  }

  decodedImage->SetExtent(0, width - 1, 0, height - 1, 0, 0);

  if (!this->ColorEnabled)
  {
    numberOfScalarComponents = 1;
  }

  int bitDepth = png_get_bit_depth(png_ptr, info_ptr);
#if (VTK_MAJOR_VERSION < 6)
  if (bitDepth <= 8)
  { decodedImage->SetScalarTypeToUnsignedChar(); }
  else
  { decodedImage->SetScalarTypeToUnsignedShort(); }
  decodedImage->SetNumberOfScalarComponents(numberOfScalarComponents);
  decodedImage->AllocateScalars();
#else
  if (bitDepth <= 8)
  { decodedImage->AllocateScalars(VTK_UNSIGNED_CHAR, numberOfScalarComponents); }
  else
  { decodedImage->AllocateScalars(VTK_UNSIGNED_SHORT, numberOfScalarComponents); }
#endif


  if (!this->ColorEnabled)
  {
    PlusStatus status = PixelCodec::ConvertToGray(PixelCodec::PixelEncoding_RGBA32, width, height, &(this->Internal->DecodingBuffer[0]), (unsigned char*)decodedImage->GetScalarPointer());
  }
  else
  {
    unsigned char* source = &(this->Internal->DecodingBuffer[0]);
    unsigned char* destination = (unsigned char*)decodedImage->GetScalarPointer();
    for (unsigned int i = 0; i < width * height * numberOfScalarComponents; ++i)
    {
      destination[i] = source[i];
    }
  }

  // close the file
  png_read_end(png_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBkProFocusOemVideoSource::AddParametersToFrameFields()
{
  vtkPlusUsDevice::InternalUpdate();// Move to beginning of vtkPlusBkProFocusOemVideoSource::InternalUpdate()?

  this->FrameFields[IGTLIO_KEY_PROBE_TYPE]   = igsioCommon::ToString<int>(this->GetProbeType());
  std::string output;
  igsioCommon::JoinTokensIntoString<double>(this->CalculateOrigin(), output, ' ');
  this->FrameFields[IGTLIO_KEY_ORIGIN]       = output;
  igsioCommon::JoinTokensIntoString<double>(this->CalculateAngles(), output, ' ');
  this->FrameFields[IGTLIO_KEY_ANGLES]       = output;
  igsioCommon::JoinTokensIntoString<double>(this->CalculateBoundingBox(), output, ' ');
  this->FrameFields[IGTLIO_KEY_BOUNDING_BOX] = output;
  igsioCommon::JoinTokensIntoString<double>(this->CalculateDepths(), output, ' ');
  this->FrameFields[IGTLIO_KEY_DEPTHS]       = output;
  this->FrameFields[IGTLIO_KEY_LINEAR_WIDTH] = igsioCommon::ToString<double>(this->CalculateLinearWidth());

  this->FrameFields[IGTLIO_KEY_SPACING_X]    = igsioCommon::ToString<double>(this->GetSpacingX());
  this->FrameFields[IGTLIO_KEY_SPACING_Y]    = igsioCommon::ToString<double>(this->GetSpacingY());

  this->FrameFields[KEY_DEPTH]        = igsioCommon::ToString<double>(this->CalculateDepthMm());
  this->FrameFields[KEY_GAIN]         = igsioCommon::ToString<int>(this->CalculateGain());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<double> vtkPlusBkProFocusOemVideoSource::CalculateOrigin()
{
  std::vector<double> retval;
  int* dimensions = this->Internal->DecodedImageFrame->GetDimensions();
  double originX = dimensions[0] / 2.0; //For both continuous streaming and offline testing

  if (!this->ContinuousStreamingEnabled && !this->OfflineTesting)
  {
    // Even if the X-value of UltrasoundWindowSize and pixelLeft_pix don't refer to the actual ultrasound boundaries,
    // they seem to refer to an area where the ultrasound is placed in the middle,
    // so it looks like the calculation below works for finding originX.
    // (Studying a grabbed picture in detail, there seems to be an offset of 1 pixel,
    // but this is not verified for other settings and probes, so it is not added the the equation.)
    originX = pixelLeft_pix + this->UltrasoundWindowSize[0] / 2.0; //+1?
  }

  double originY = 0;
  if (this->IsSectorProbe())
  {
    double originHeightAboveBox_mm = (this->GetStartLineX()) / tan(this->CalculateWidthInRadians() / 2.0);
    originY = 0 - (originHeightAboveBox_mm / this->GetSpacingY());
  }

  double originZ = 0;

  retval.push_back(originX);
  retval.push_back(originY);
  retval.push_back(originZ);
  return retval;
}

//----------------------------------------------------------------------------
std::vector<double> vtkPlusBkProFocusOemVideoSource::CalculateAngles()
{
  std::vector<double> retval;
  if (this->OfflineTesting)
  {
    retval.push_back(0);
    retval.push_back(0);
  }
  else
  {
    retval.push_back(this->GetStartLineAngle() - vtkMath::Pi() / 2.0);
    retval.push_back(this->GetStopLineAngle() - vtkMath::Pi() / 2.0);
  }
  retval.push_back(0);
  retval.push_back(0);
  return retval;
}

//----------------------------------------------------------------------------
std::vector<double> vtkPlusBkProFocusOemVideoSource::CalculateBoundingBox()
{
  std::vector<double> retval;

  int* dimensions = this->Internal->DecodedImageFrame->GetDimensions();

  retval.push_back(0);
  retval.push_back(dimensions[0] - 1);
  retval.push_back(0);
  retval.push_back(dimensions[1] - 1);
  retval.push_back(0);
  retval.push_back(0);
  return retval;
}

//----------------------------------------------------------------------------
std::vector<double> vtkPlusBkProFocusOemVideoSource::CalculateDepths()
{
  std::vector<double> retval;
  if (this->OfflineTesting)
  {
    int* dimensions = this->Internal->DecodedImageFrame->GetDimensions();
    double* spacing = this->Internal->DecodedImageFrame->GetSpacing();
    retval.push_back(0);
    retval.push_back(dimensions[1]*spacing[1]);
    return retval;
  }

  double originDistanceToStartLine_mm = 0.0;
  if (!this->ContinuousStreamingEnabled)
  {
    originDistanceToStartLine_mm = pixelTop_pix * this->GetSpacingY();
  }

  if (this->IsSectorProbe())
  {
    originDistanceToStartLine_mm += (this->GetStartLineX()) / sin(this->CalculateWidthInRadians() / 2.0);
  }

  double depthStart = this->GetStartDepth() + originDistanceToStartLine_mm;
  double depthEnd = this->GetStopDepth() + originDistanceToStartLine_mm;

  retval.push_back(depthStart);
  retval.push_back(depthEnd);
  return retval;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::CalculateLinearWidth()
{
  if (this->OfflineTesting)
  {
    int* dimensions = this->Internal->DecodedImageFrame->GetDimensions();
    double* spacing = this->Internal->DecodedImageFrame->GetSpacing();
    return dimensions[0] * spacing[0];
  }

  double width = fabs(this->GetStartLineX() - this->GetStopLineX());
  return width;
}

//----------------------------------------------------------------------------
bool vtkPlusBkProFocusOemVideoSource::IsSectorProbe()
{
  bool sectorProbe = false;
  if (this->GetProbeType() == SECTOR)
  {
    sectorProbe = true;
  }
  return sectorProbe;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::CalculateWidthInRadians()
{
  double width_radians = fabs(this->GetStartLineAngle() - this->GetStopLineAngle());
  return width_radians;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::CalculateDepthMm()
{
  double depth_mm = (StopDepth_m - StartDepth_m) * 1000.0;
  return depth_mm;
}

//----------------------------------------------------------------------------
int vtkPlusBkProFocusOemVideoSource::CalculateGain()
{
  return gain_percent;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStartDepth()
{
  return StartDepth_m * 1000.0;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStopDepth()
{
  return StopDepth_m * 1000.0;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStartLineX()
{
  return StartLineX_m * 1000.0;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStartLineY()
{
  return StartLineY_m * 1000.0;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStopLineX()
{
  return StopLineX_m * 1000.0;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStopLineY()
{
  return StopLineY_m * 1000.0;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStartLineAngle()
{
  return StartLineAngle_rad;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetStopLineAngle()
{
  return StopLineAngle_rad;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetSpacingX()
{
  double spacingX_mm = 0.1;
  if (!this->OfflineTesting)
  {
    if (this->ContinuousStreamingEnabled)
    {
      spacingX_mm = 1000.0 * (tissueRight_m - tissueLeft_m) / (grabFramePixelRight_pix - grabFramePixelLeft_pix + 1);
    }
    else
    {
      //      spacingX_mm = 1000.0 * (tissueRight_m - tissueLeft_m) / this->UltrasoundWindowSize[0];
      //The values for pixelRight_pix, pixelLeft_pix and UltrasoundWindowSize[0] seem to be referring
      //to a larger area than the actual ultrasound and cannot be used here.
      //Assume equal spacing in x and y and return y spacing instead
      return this->GetSpacingY();
    }
  }
  return spacingX_mm;
}

//----------------------------------------------------------------------------
double vtkPlusBkProFocusOemVideoSource::GetSpacingY()
{
  double spacingY_mm = 0.1;
  if (!this->OfflineTesting)
  {
    if (this->ContinuousStreamingEnabled)
    {
      spacingY_mm = 1000.0 * (tissueTop_m - tissueBottom_m) / (grabFramePixelBottom_pix - grabFramePixelTop_pix + 1);
    }
    else
    {
      spacingY_mm = 1000.0 * (tissueTop_m - tissueBottom_m) / this->UltrasoundWindowSize[1];
    }
  }
  return spacingY_mm;
}

//----------------------------------------------------------------------------
IGTLIO_PROBE_TYPE vtkPlusBkProFocusOemVideoSource::GetProbeType()
{
  if (this->OfflineTesting)
  {
    return LINEAR;
  }

  if (probePort.compare("A"))
  {
    return probeTypePortA;
  }
  else if (probePort.compare("B"))
  {
    return probeTypePortB;
  }
  else if (probePort.compare("C"))
  {
    return probeTypePortC;
  }
  else if (probePort.compare("M"))
  {
    return probeTypePortM;
  }
  else
  {
    return UNKNOWN;
  }
}

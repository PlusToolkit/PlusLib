/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkOpenIGTLinkVideoSource.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkVideoBuffer.h"
#include "PlusVideoFrame.h"

#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlPlusClientInfoMessage.h"
#include "vtkPlusIgtlMessageCommon.h"

#include <vector>
#include <string>

vtkCxxRevisionMacro(vtkOpenIGTLinkVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkOpenIGTLinkVideoSource);

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::vtkOpenIGTLinkVideoSource()
{
  this->MessageType = NULL; 
  this->ServerAddress = NULL; 
  this->ServerPort = -1; 
  this->ClientSocket = igtl::ClientSocket::New(); 
  this->SpawnThreadForRecording = true;
  this->NumberOfRetryAttempts = 3; 
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::~vtkOpenIGTLinkVideoSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if ( this->ServerAddress )
  {
    os << indent << "Server address: " << this->ServerAddress << "\n";
  }
  os << indent << "Server port: " << this->ServerPort << "\n";
  if ( this->MessageType )
  {
    os << indent << "Message type: " << this->ServerAddress << "\n";
  }

}
//----------------------------------------------------------------------------
std::string vtkOpenIGTLinkVideoSource::GetSdkVersion()
{
  std::ostringstream version; 
  version << "OpenIGTLink v" << PLUS_OPENIGTLINK_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalConnect()
{
   LOG_TRACE( "vtkOpenIGTLinkVideoSource::InternalConnect" ); 

  if ( this->ClientSocket->GetConnected() )
  {
    return PLUS_SUCCESS; 
  }

  if ( this->ServerAddress == NULL )
  {
    LOG_ERROR("Unable to connect OpenIGTLink server - server address is undefined" ); 
    return PLUS_FAIL; 
  }

  if ( this->ServerPort < 0 )
  {
    LOG_ERROR("Unable to connect OpenIGTLink server - server port is invalid: " << this->ServerPort ); 
    return PLUS_FAIL; 
  }

  int r = this->ClientSocket->ConnectToServer( this->ServerAddress, this->ServerPort );

  if ( r != 0 )
  {
    LOG_ERROR( "Cannot connect to the server (" << this->ServerAddress << ":" << this->ServerPort << ")." );
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG( "Client successfully connected to server (" << this->ServerAddress << ":" << this->ServerPort << ")."  );
  }

  this->ClientSocket->SetTimeout(500); 

  // Clear buffer on connect 
  this->GetBuffer()->Clear(); 

  // If we specified message type, try to send it to the server
  if ( this->MessageType )
  {
    // Send clinet info request to the server
    PlusIgtlClientInfo clientInfo; 
    // Set message type
    clientInfo.IgtlMessageTypes.push_back(this->MessageType); 

    // Pack client info message 
    igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
    clientInfoMsg->SetClientInfo(clientInfo); 
    clientInfoMsg->Pack(); 

    // Send message to server 
    int retValue = 0, numOfTries = 0; 
    while ( retValue == 0 && numOfTries < this->NumberOfRetryAttempts )
    {
      retValue = this->ClientSocket->Send( clientInfoMsg->GetPackPointer(), clientInfoMsg->GetPackSize() ); 
      numOfTries++; 
    }

    if ( retValue == 0 )
    {
      LOG_ERROR("Failed to send PlusClientInfo message to server!"); 
      return PLUS_FAIL; 
    }
  }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalDisconnect()
{
  this->ClientSocket->CloseSocket(); 
  return this->StopRecording();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalStartRecording()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStartRecording" ); 
  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }

  return this->Connect(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalGrab()
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();

  int numOfBytesReceived = 0, numOfTries = 0; 
  while ( numOfBytesReceived == 0 && numOfTries < this->NumberOfRetryAttempts )
  {
    numOfBytesReceived = this->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
    numOfTries++; 
  }

  // No message received - server disconnected 
  if ( numOfBytesReceived == 0 ) 
  {
    LOG_ERROR("OpenIGTLink video source connection lost with server - try to reconnect!");
    this->Connected = 0; 
    this->ClientSocket->CloseSocket(); 
    return this->Connect();
  }

  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
  igtl::Matrix4x4 igtlMatrix;
  igtl::IdentityMatrix(igtlMatrix);
  std::string igtlTransformName; 
  
  headerMsg->Unpack();
  if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
  {
    // Message body handler for IMAGE
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    imgMsg->SetMessageHeader(headerMsg);
    imgMsg->AllocatePack();

    this->ClientSocket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());

    //  If 1 is specified it performs CRC check and unpack the data only if CRC passes
    int c = imgMsg->Unpack(1);
    if (! (c & igtl::MessageHeader::UNPACK_BODY) ) 
    {
      LOG_ERROR("Couldn't receive image message from server!"); 
      return PLUS_FAIL; 
    }

    // if CRC check is OK. Read image data.
    imgMsg->GetTimeStamp(igtlTimestamp); 

    // Set scalar pixel type
    PlusCommon::ITKScalarPixelType pixelType = PlusVideoFrame::GetITKScalarPixelTypeFromIGTL(imgMsg->GetScalarType()); 

    int imgSize[3]={0}; // image dimension
    imgMsg->GetDimensions(imgSize);

    // Set unfiltered and filtered timestamp by converting UTC to system timestamp
    double unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(igtlTimestamp->GetTimeStamp());
    double filteredTimestamp = unfilteredTimestamp;  

    // The timestamps are already defined, so we don't need to filter them, 
    // for simplicity, we increase frame number always by 1.
    this->FrameNumber++;

    // If the buffer is empty, set the pixel type and frame size to the first received properties 
    if ( this->GetBuffer()->GetNumberOfItems() == 0 )
    {
      this->GetBuffer()->SetPixelType(pixelType);  
      this->GetBuffer()->SetFrameSize( imgSize[0], imgSize[1] );
    }

    PlusStatus status = this->Buffer->AddItem( imgMsg->GetScalarPointer() , this->GetUsImageOrientation(), imgSize, pixelType, 0, this->FrameNumber, unfilteredTimestamp, filteredTimestamp); 
    this->Modified();
    return status;

  }
  else if (strcmp(headerMsg->GetDeviceType(), "TRACKEDFRAME") == 0)
  {
    TrackedFrame trackedFrame; 
    if ( vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage( headerMsg, this->ClientSocket, trackedFrame ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't get tracked frame from OpenIGTLink server!"); 
      return PLUS_FAIL; 
    }

    // Set unfiltered and filtered timestamp by converting UTC to system timestamp
    double unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(trackedFrame.GetTimestamp());  
    double filteredTimestamp = unfilteredTimestamp;  

    // The timestamps are already defined, so we don't need to filter them, 
    // for simplicity, we increase frame number always by 1.
    this->FrameNumber++;
   
    // If the buffer is empty, set the pixel type and frame size to the first received properties 
    if ( this->GetBuffer()->GetNumberOfItems() == 0 )
    {
      this->GetBuffer()->SetPixelType(trackedFrame.GetImageData()->GetITKScalarPixelType() );  
      this->GetBuffer()->SetFrameSize( trackedFrame.GetFrameSize() );
    }
    TrackedFrame::FieldMapType customFields=trackedFrame.GetCustomFields();
    PlusStatus status = this->Buffer->AddItem( trackedFrame.GetImageData(), this->GetUsImageOrientation(), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &customFields); 
    this->Modified();
    return status;
  }
  else
  {
    // if the data type is unknown, skip reading. 
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS; 
  }

  return PLUS_SUCCESS; 
}


//-----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkOpenIGTLinkVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Sonix video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* messageType = imageAcquisitionConfig->GetAttribute("MessageType"); 
  if ( messageType != NULL )
  {
    this->SetMessageType(messageType); 
  }

  const char* serverAddress = imageAcquisitionConfig->GetAttribute("ServerAddress"); 
  if ( serverAddress != NULL )
  {
    this->SetServerAddress(serverAddress); 
  }
  else
  {
    LOG_ERROR("Unable to find ServerAddress attribute!"); 
    return PLUS_FAIL; 
  }

  int serverPort = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("ServerPort", serverPort ) )
  {
    this->SetServerPort(serverPort); 
  }
  else
  {
    LOG_ERROR("Unable to find ServerPort attribute!"); 
    return PLUS_FAIL; 
  } 

  return PLUS_SUCCESS;
}


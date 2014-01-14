/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusVideoFrame.h"
#include "TrackedFrame.h"
#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlPlusClientInfoMessage.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkOpenIGTLinkVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusBuffer.h"
#include "vtksys/SystemTools.hxx"

#include <vector>
#include <string>

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkCxxRevisionMacro(vtkOpenIGTLinkVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkOpenIGTLinkVideoSource);

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::vtkOpenIGTLinkVideoSource()
{
  this->MessageType = NULL; 
  this->ServerAddress = NULL; 
  this->ServerPort = -1; 
  this->IgtlMessageCrcCheckEnabled = 0; 
  this->ClientSocket = igtl::ClientSocket::New(); 
  this->NumberOfRetryAttempts = 10; 
  this->DelayBetweenRetryAttemptsSec = 0.100; // there is already a delay with a CLIENT_SOCKET_TIMEOUT_MSEC timeout, so we just add a little extra idle delay

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

  int errorCode = 0; // 0 means success
  RETRY_UNTIL_TRUE( 
    (errorCode = this->ClientSocket->ConnectToServer( this->ServerAddress, this->ServerPort ))==0,
    this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

  if ( errorCode != 0 )
  {
    LOG_ERROR( "Cannot connect to the server (" << this->ServerAddress << ":" << this->ServerPort << ")." );
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG( "Client successfully connected to server (" << this->ServerAddress << ":" << this->ServerPort << ")."  );
  }

  this->ClientSocket->SetTimeout(CLIENT_SOCKET_TIMEOUT_MSEC); 

  // Clear buffer on connect 
  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];
  outputChannel->Clear();

  // If we specified message type, try to send it to the server
  if ( this->MessageType )
  {
    // Send client info request to the server
    PlusIgtlClientInfo clientInfo; 
    // Set message type
    clientInfo.IgtlMessageTypes.push_back(this->MessageType); 

    // Pack client info message 
    igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
    clientInfoMsg->SetClientInfo(clientInfo); 
    clientInfoMsg->Pack(); 

    // Send message to server 
    int retValue=0;
    RETRY_UNTIL_TRUE( 
      (retValue = this->ClientSocket->Send( clientInfoMsg->GetPackPointer(), clientInfoMsg->GetPackSize()))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

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
PlusStatus vtkOpenIGTLinkVideoSource::InternalUpdate()
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();

  int numOfBytesReceived = 0;
  RETRY_UNTIL_TRUE( 
    (numOfBytesReceived = this->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize()))!=0,
    this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

  if ( numOfBytesReceived == 0 ) 
  {
    // No message received - server disconnected 
    LOG_ERROR("OpenIGTLink video source connection lost with server - try to reconnect!");
    this->Connected = 0; 
    this->ClientSocket->CloseSocket(); 
    return this->Connect();
  }
  
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);

  TrackedFrame trackedFrame;

  if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
  {      
    if (vtkPlusIgtlMessageCommon::UnpackImageMessage( headerMsg, this->ClientSocket, trackedFrame, this->ImageMessageEmbeddedTransformName, this->IgtlMessageCrcCheckEnabled)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't get image from OpenIGTLink server!"); 
      return PLUS_FAIL;
    }
  }
  else if (strcmp(headerMsg->GetDeviceType(), "TRACKEDFRAME") == 0)
  {
    if ( vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage( headerMsg, this->ClientSocket, trackedFrame, this->IgtlMessageCrcCheckEnabled ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't get tracked frame from OpenIGTLink server!"); 
      return PLUS_FAIL; 
    }
  }
  else
  {
    // if the data type is unknown, skip reading. 
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS; 
  }

  // Set unfiltered and filtered timestamp by converting UTC to system timestamp
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(trackedFrame.GetTimestamp());  
  double filteredTimestamp = unfilteredTimestamp;  

  // The timestamps are already defined, so we don't need to filter them, 
  // for simplicity, we increase frame number always by 1.
  this->FrameNumber++;

  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the OpenIGTLinkVideo device.");
    return PLUS_FAIL;
  }

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( aSource->GetBuffer()->GetNumberOfItems() == 0 )
  {
    PlusVideoFrame* videoFrame=trackedFrame.GetImageData();
    if (videoFrame==NULL)
    {
      LOG_ERROR("Invalid video frame received, cannot use it to initialize the video buffer");
      return PLUS_FAIL;
    }
    aSource->GetBuffer()->SetPixelType( videoFrame->GetITKScalarPixelType() );  
    aSource->GetBuffer()->SetFrameSize( trackedFrame.GetFrameSize() );
  }
  TrackedFrame::FieldMapType customFields=trackedFrame.GetCustomFields();
  PlusStatus status = aSource->GetBuffer()->AddItem( trackedFrame.GetImageData(), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &customFields); 
  this->Modified();

  return status;
}


//-----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkOpenIGTLinkVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure OpenIGTLink video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
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

  const char* igtlMessageCrcCheckEnabled = imageAcquisitionConfig->GetAttribute("IgtlMessageCrcCheckEnabled"); 
  if ( igtlMessageCrcCheckEnabled != NULL )
  {
    if ( STRCASECMP(igtlMessageCrcCheckEnabled, "true") == 0 )
    {
      this->SetIgtlMessageCrcCheckEnabled(1);
    }
    else
    {
      this->SetIgtlMessageCrcCheckEnabled(0);
    }
  }

  const char* imageMessageEmbeddedTransformName = imageAcquisitionConfig->GetAttribute("ImageMessageEmbeddedTransformName"); 
  if ( imageMessageEmbeddedTransformName != NULL )
  {
    this->ImageMessageEmbeddedTransformName.SetTransformName(imageMessageEmbeddedTransformName);
  }  

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkOpenIGTLinkVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkOpenIGTLinkVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
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

#include <vector>
#include <string>

vtkCxxRevisionMacro(vtkOpenIGTLinkVideoSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkOpenIGTLinkVideoSource);

//----------------------------------------------------------------------------

vtkOpenIGTLinkVideoSource* vtkOpenIGTLinkVideoSource::Instance = 0;
vtkOpenIGTLinkVideoSourceCleanup vtkOpenIGTLinkVideoSource::Cleanup;

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSourceCleanup::vtkOpenIGTLinkVideoSourceCleanup()
{
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSourceCleanup::~vtkOpenIGTLinkVideoSourceCleanup()
{
  // Destroy any remaining output window.
  vtkOpenIGTLinkVideoSource::SetInstance(NULL);
}
//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::vtkOpenIGTLinkVideoSource()
{
  this->ServerAddress = NULL; 
  this->ServerPort = -1; 
  this->ClientSocket = igtl::ClientSocket::New(); 
  this->SpawnThreadForRecording=true;
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
// Up the reference count so it behaves like New
vtkOpenIGTLinkVideoSource* vtkOpenIGTLinkVideoSource::New()
{
  vtkOpenIGTLinkVideoSource* ret = vtkOpenIGTLinkVideoSource::GetInstance();
  ret->Register(NULL);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkOpenIGTLinkVideoSource* vtkOpenIGTLinkVideoSource::GetInstance()
{
  if(!vtkOpenIGTLinkVideoSource::Instance)
  {
    // Try the factory first
    vtkOpenIGTLinkVideoSource::Instance = (vtkOpenIGTLinkVideoSource*)vtkObjectFactory::CreateInstance("vtkOpenIGTLinkVideoSource");    
    if(!vtkOpenIGTLinkVideoSource::Instance)
    {
      vtkOpenIGTLinkVideoSource::Instance = new vtkOpenIGTLinkVideoSource();     
    }
    if(!vtkOpenIGTLinkVideoSource::Instance)
    {
      int error = 0;
    }
  }
  // return the instance
  return vtkOpenIGTLinkVideoSource::Instance;
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkVideoSource::SetInstance(vtkOpenIGTLinkVideoSource* instance)
{
  if (vtkOpenIGTLinkVideoSource::Instance==instance)
  {
    return;
  }
  // preferably this will be NULL
  if (vtkOpenIGTLinkVideoSource::Instance)
  {
    vtkOpenIGTLinkVideoSource::Instance->Delete();;
  }
  vtkOpenIGTLinkVideoSource::Instance = instance;
  if (!instance)
  {
    return;
  }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}

//----------------------------------------------------------------------------
std::string vtkOpenIGTLinkVideoSource::GetSdkVersion()
{
  std::ostringstream version; 
  version << "OpenIGTLink v" << PLUS_OPENIGTLINK_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
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
    LOG_ERROR("Unable to connect OpenIGTLink server - server address is NULL" ); 
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
    LOG_TRACE( "Client successfully connected to server (" << this->ServerAddress << ":" << this->ServerPort << ")."  );
  }

  // Clear buffer on connect 
  this->GetBuffer()->Clear(); 

  // Wait before we send thre clinet info request 
  vtkAccurateTimer::Delay(1.0); 

  // Send clinet info request to the server
  PlusIgtlClientInfo clientInfo; 
  // We need IMAGE message type
  clientInfo.IgtlMessageTypes.push_back("IMAGE"); 
  
  // Pack client info message 
  igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
  clientInfoMsg->SetClientInfo(clientInfo); 
  clientInfoMsg->Pack(); 

  // Send message to server 
  int retValue = 0, numOfTries = 0; 
  while ( retValue == 0 && numOfTries < 3 )
  {
    retValue = this->ClientSocket->Send( clientInfoMsg->GetPackPointer(), clientInfoMsg->GetPackSize() ); 
    numOfTries++; 
  }

  if ( retValue == 0 )
  {
    LOG_ERROR("Failed to send PlusClientInfo message to server!"); 
    return PLUS_FAIL; 
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

  int retValue = 0, numOfTries = 0; 
  while ( retValue == 0 && numOfTries < 3 )
  {
    retValue = this->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
    numOfTries++; 
  }

  // No message received - server disconnected 
  if ( retValue == 0 ) 
  {
    LOG_ERROR("OpenIGTLink video source connection lost with server - image acquisition stopped!");
    this->Disconnect(); 
    return PLUS_FAIL; 
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
    if (c & igtl::MessageHeader::UNPACK_BODY) 
    {
      // if CRC check is OK. Read image data.
      imgMsg->GetTimeStamp(igtlTimestamp); 
      
      // Set scalar pixel type
      PlusCommon::ITKScalarPixelType pixelType = PlusVideoFrame::GetITKScalarPixelTypeFromIGTL(imgMsg->GetScalarType()); 

      int imgSize[3]={0}; // image dimension
      imgMsg->GetDimensions(imgSize);

      // Set unfiltered and filtered timestamp
      double unfilteredTimestamp = igtlTimestamp->GetTimeStamp();  
      double filteredTimestamp = igtlTimestamp->GetTimeStamp();  

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
    else
    {
      LOG_ERROR("Couldn't receive image message from server!"); 
      return PLUS_FAIL; 
    }
  }
 
  // if the data type is unknown, skip reading. 
  this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
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


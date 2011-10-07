
#include "OpenIGTLinkReceiveServer.h"


#include "PlusConfigure.h"

#include "vtkMultiThreader.h"

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"



/**
 * This function runs on a separate thread to collect OpenIGTLink messages.
 */
static
void*
vtkReceiverThread( vtkMultiThreader::ThreadInfo *data )
{
  OpenIGTLinkReceiveServer* self = (OpenIGTLinkReceiveServer*)( data->UserData );
  
  igtl::Socket::Pointer socket;
  
  
    // Loop until receiver stopped.
  
  while ( true )
  {
    if ( self->GetActive() == false ) return NULL;
    
    socket = self->GetServerSocket()->WaitForConnection( 400 );
    
    if ( socket.IsNotNull() ) // if client connected
    {
      
        // Create a message buffer to receive header
      
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      for (int i = 0; i < 100; i ++)
      {
        headerMsg->InitPack();

          // Receive generic header from the socket
        
        int r = socket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
        if ( r == 0 )
        {
          socket->CloseSocket();
        }
        if ( r != headerMsg->GetPackSize() )
        {
          continue;
        }

          // Deserialize the header
        
        headerMsg->Unpack();
        
        
        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
        {
          self->ReceiveTransform( socket, headerMsg );
        }
        else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
        {
          self->ReceiveImage( socket, headerMsg );
        }
        else
        {
          std::cerr << "Receiving unknown type: " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip( headerMsg->GetBodySizeToRead(), 0 );
        }
        
        
        self->NumberOfReceivedMessages ++;
      }
    }
  }
}



OpenIGTLinkReceiveServer
::OpenIGTLinkReceiveServer( int port )
{
  this->IsActive = false;
  this->ThreadId = -1;
  this->NumberOfReceivedMessages = 0;
  
  this->Threader = vtkMultiThreader::New();
  
  this->ServerSocket = igtl::ServerSocket::New();
  int r = this->ServerSocket->CreateServer( port );

  if ( r < 0 )
  {
    LOG_ERROR( "Could not create OpenIGTLink server" );
    return;
  }
}



OpenIGTLinkReceiveServer
::~OpenIGTLinkReceiveServer()
{
  this->Stop();
  
  this->Threader->Delete();
}



PlusStatus
OpenIGTLinkReceiveServer
::Start()
{
  if ( this->IsActive ) return PLUS_FAIL;
  
  this->IsActive = true;
  this->ThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&vtkReceiverThread, this );
  
  return PLUS_SUCCESS;
}



PlusStatus
OpenIGTLinkReceiveServer
::Stop()
{
  if ( ! this->IsActive ) return PLUS_SUCCESS;
  
  this->ThreadId = -1;
  this->IsActive = false;

    // Let's give a chance to the thread to stop before we kill the tracker connection.
  
  vtkAccurateTimer::Delay( 0.5 );

  return PLUS_SUCCESS;
}



bool
OpenIGTLinkReceiveServer
::GetActive()
{
  return this->IsActive;
}



int
OpenIGTLinkReceiveServer
::GetNumberOfReceivedMessages()
{
  return this->NumberOfReceivedMessages;
}



igtl::ServerSocket::Pointer
OpenIGTLinkReceiveServer
::GetServerSocket()
{
  return this->ServerSocket;
}



int
OpenIGTLinkReceiveServer
::ReceiveTransform( igtl::Socket * socket, igtl::MessageHeader * header )
{
  std::cerr << std::endl << "Receiving TRANSFORM data type." << std::endl;
  
  
    // Create a message buffer to receive transform data
  
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  
    // Receive transform data from the socket
  
  socket->Receive( transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize() );
  
  
    // Deserialize the transform data
    // If you want to skip CRC check, call Unpack() without argument.
  
  int c = transMsg->Unpack( 1 );
  
  if ( c & igtl::MessageHeader::UNPACK_BODY ) // if CRC check is OK
  {
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    igtl::PrintMatrix(matrix);
    return 1;
  }

  return 0;
}



int
OpenIGTLinkReceiveServer
::ReceiveImage( igtl::Socket * socket, igtl::MessageHeader * header )
{
  std::cerr << std::endl << "Receiving IMAGE data type." << std::endl;
  
  
    // Create a message buffer to receive transform data
  
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();
  
  
    // Receive transform data from the socket
  
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
  
    // Deserialize the transform data
    // If you want to skip CRC check, call Unpack() without argument.
  
  int c = imgMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    // Retrive the image data
    int   size[3];          // image dimension
    float spacing[3];       // spacing (mm/pixel)
    int   svsize[3];        // sub-volume size
    int   svoffset[3];      // sub-volume offset
    int   scalarType;       // scalar type

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);

    std::cerr << "Device Name           : " << imgMsg->GetDeviceName() << std::endl;
    std::cerr << "Scalar Type           : " << scalarType << std::endl;
    std::cerr << "Dimensions            : ("
              << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
    std::cerr << "Spacing               : ("
              << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << ")" << std::endl;
    std::cerr << "Sub-Volume dimensions : ("
              << svsize[0] << ", " << svsize[1] << ", " << svsize[2] << ")" << std::endl;
    std::cerr << "Sub-Volume offset     : ("
              << svoffset[0] << ", " << svoffset[1] << ", " << svoffset[2] << ")" << std::endl;
    return 1;
  }

  return 0;

}

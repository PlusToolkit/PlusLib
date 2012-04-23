/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPlusIgtlMessageCommon.h"

#include "vtkObjectFactory.h"
#include "vtkTransformRepository.h" 
#include "vtkTrackedFrameList.h" 
#include "TrackedFrame.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h" 
#include "PlusVideoFrame.h" 

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusIgtlMessageCommon, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPlusIgtlMessageCommon);

//----------------------------------------------------------------------------
vtkPlusIgtlMessageCommon::vtkPlusIgtlMessageCommon()
{	
}

//----------------------------------------------------------------------------
vtkPlusIgtlMessageCommon::~vtkPlusIgtlMessageCommon()
{
}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageCommon::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtl::Matrix4x4& igtlMatrix, vtkTransformRepository* transformRepository, PlusTransformName& transformName)
{
  igtl::IdentityMatrix(igtlMatrix); 

  if ( transformRepository == NULL || !transformName.IsValid() )
  {
    return PLUS_SUCCESS; 
  }

  bool valid = false; 
  vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( transformRepository->GetTransform(transformName,vtkMatrix, &valid) != PLUS_SUCCESS ) 
  {
    LOG_ERROR("Failed to get transform from transform repository (" << transformName.From() << " to " << transformName.To() << ")");
    return PLUS_FAIL;
  }

  if ( !valid )
  {
    LOG_WARNING("Skipped transformation matrix - Invalid transform in the transform repository (" << transformName.From() << " to " << transformName.To() << ")" ); 
    return PLUS_FAIL;
  }

  // Copy vtk matrix to igt matrix 
  for ( int r = 0; r < 4; ++r )
  {
    for (int c = 0; c < 4; ++c )
    {
      igtlMatrix[r][c] = vtkMatrix->GetElement(r,c); 
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::PackTrackedFrameMessage(igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage, TrackedFrame& trackedFrame )
{
  if ( trackedFrameMessage.IsNull() )
  {
    LOG_ERROR("Failed to pack tracked frame message - input tracked frame message is NULL"); ;
    return PLUS_FAIL; 
  }

  PlusStatus status = trackedFrameMessage->SetTrackedFrame(trackedFrame); 
  trackedFrameMessage->Pack(); 

  return status; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage( igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, TrackedFrame& trackedFrame)
{
  if ( headerMsg.IsNull() )
  {
    LOG_ERROR("Unable to unpack tracked frame message - header message is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( socket == NULL ) 
  {
    LOG_ERROR("Unable to unpack tracked frame message - socket is NULL!"); 
    return PLUS_FAIL; 
  }

  igtl::PlusTrackedFrameMessage::Pointer trackedFrameMsg = igtl::PlusTrackedFrameMessage::New();
  trackedFrameMsg->SetMessageHeader(headerMsg);
  trackedFrameMsg->AllocatePack();

  socket->Receive(trackedFrameMsg->GetPackBodyPointer(), trackedFrameMsg->GetPackBodySize());

  //  If 1 is specified it performs CRC check and unpack the data only if CRC passes
  int c = trackedFrameMsg->Unpack(1);
  if ( !(c & igtl::MessageHeader::UNPACK_BODY) )
  {
    LOG_ERROR("Couldn't receive tracked frame message from server!"); 
    return PLUS_FAIL; 
  }

  // if CRC check is OK. get tracked frame data.
  trackedFrame = trackedFrameMsg->GetTrackedFrame(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::PackUsMessage(igtl::PlusUsMessage::Pointer usMessage, TrackedFrame& trackedFrame )
{
  if ( usMessage.IsNull() )
  {
    LOG_ERROR("Failed to pack US message - input US message is NULL"); ;
    return PLUS_FAIL; 
  }

  PlusStatus status = usMessage->SetTrackedFrame(trackedFrame); 
  usMessage->Pack(); 

  return status; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::UnpackUsMessage( igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, TrackedFrame& trackedFrame)
{
  if ( headerMsg.IsNull() )
  {
    LOG_ERROR("Unable to unpack US message - header message is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( socket == NULL ) 
  {
    LOG_ERROR("Unable to unpack US message - socket is NULL!"); 
    return PLUS_FAIL; 
  }

  igtl::PlusUsMessage::Pointer usMsg = igtl::PlusUsMessage::New();
  usMsg->SetMessageHeader(headerMsg);
  usMsg->AllocatePack();

  socket->Receive(usMsg->GetPackBodyPointer(), usMsg->GetPackBodySize());

  // If 1 is specified it performs CRC check and unpack the data only if CRC passes
  int c = usMsg->Unpack(0);
  if ( !(c & igtl::MessageHeader::UNPACK_BODY) )
  {
    LOG_ERROR("Couldn't receive US message from server!"); 
    return PLUS_FAIL; 
  }

  // if CRC check is OK. get tracked frame data.
  trackedFrame = usMsg->GetTrackedFrame(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::PackImageMessage(igtl::ImageMessage::Pointer imageMessage, TrackedFrame& trackedFrame, igtl::Matrix4x4& igtlMatrix )
{
  if ( imageMessage.IsNull() )
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL"); ;
    return PLUS_FAIL; 
  }

   if ( !trackedFrame.GetImageData()->IsImageValid() )
  {
    LOG_WARNING("Unable to send image message - image data is NOT valid!"); 
    return PLUS_FAIL; 
  }

  double timestamp = trackedFrame.GetTimestamp();
  vtkImageData* frameImage = trackedFrame.GetImageData()->GetVtkImage();

  igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
  igtlFrameTime->SetTime( timestamp );
  
  int imageSizePixels[3]={0}, subSizePixels[3]={0}, subOffset[3]={0};
  double imageSpacingMm[3]={0};
  int scalarType = PlusVideoFrame::GetIGTLScalarPixelType( trackedFrame.GetImageData()->GetITKScalarPixelType() ); 

  frameImage->GetDimensions( imageSizePixels );
  frameImage->GetSpacing( imageSpacingMm );
  frameImage->GetDimensions( subSizePixels );

  float spacingFloat[3]={0};
  for ( int i = 0; i < 3; ++ i ) spacingFloat[ i ] = (float)imageSpacingMm[ i ];

  imageMessage->SetDimensions( imageSizePixels );
  imageMessage->SetSpacing( spacingFloat );
  imageMessage->SetScalarType( scalarType );
  imageMessage->SetSubVolume( subSizePixels, subOffset );
  imageMessage->AllocateScalars();

  unsigned char* igtlImagePointer = (unsigned char*)( imageMessage->GetScalarPointer() );
  unsigned char* vtkImagePointer = (unsigned char*)( frameImage->GetScalarPointer() );

  memcpy(igtlImagePointer, vtkImagePointer, imageMessage->GetImageSize());

  imageMessage->SetMatrix( igtlMatrix );
  imageMessage->SetTimeStamp( igtlFrameTime );

  imageMessage->Pack();

  return PLUS_SUCCESS; 

}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::UnpackImageMessage( igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, TrackedFrame& trackedFrame)
{
  if ( headerMsg.IsNull() )
  {
    LOG_ERROR("Unable to unpack image message - header message is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( socket == NULL ) 
  {
    LOG_ERROR("Unable to unpack image message - socket is NULL!"); 
    return PLUS_FAIL; 
  }

  // Message body handler for IMAGE
  igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(headerMsg);
  imgMsg->AllocatePack();

  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());

  //  If 1 is specified it performs CRC check and unpack the data only if CRC passes
  int c = imgMsg->Unpack(1);
  if (! (c & igtl::MessageHeader::UNPACK_BODY) ) 
  {
    LOG_ERROR("Couldn't receive image message from server!"); 
    return PLUS_FAIL; 
  }

  // if CRC check is OK. Read data.
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
  imgMsg->GetTimeStamp(igtlTimestamp); 

  int imgSize[3]={0}; // image dimension
  imgMsg->GetDimensions(imgSize);

  // Set scalar pixel type
  PlusCommon::ITKScalarPixelType pixelType = PlusVideoFrame::GetITKScalarPixelTypeFromIGTL(imgMsg->GetScalarType()); 
  PlusVideoFrame frame; 
  if ( frame.AllocateFrame(imgSize, pixelType) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to allocate image data for tracked frame!"); 
    return PLUS_FAIL;
  }

  // Copy image to buffer 
  memcpy(frame.GetBufferPointer(), imgMsg->GetScalarPointer(), frame.GetFrameSizeInBytes() ); 

  trackedFrame.SetImageData(frame); 
  trackedFrame.SetTimestamp(igtlTimestamp->GetTimeStamp());

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::PackTransformMessage(igtl::TransformMessage::Pointer transformMessage, PlusTransformName& transformName, 
                                                           igtl::Matrix4x4& igtlMatrix, double timestamp )
{
  if ( transformMessage.IsNull() )
  {
    LOG_ERROR("Failed to pack transform message - input transform message is NULL"); ;
    return PLUS_FAIL; 
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime( timestamp );

  std::string strTransformName;
  transformName.GetTransformName( strTransformName );

  transformMessage->SetMatrix( igtlMatrix );
  transformMessage->SetTimeStamp( igtlTime );
  transformMessage->SetDeviceName( strTransformName.c_str() );
  transformMessage->Pack();
  
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::UnpackTransformMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, 
                                                           vtkMatrix4x4* transformMatrix, std::string& transformName, double& timestamp )
{
  if ( headerMsg.IsNull() )
  {
    LOG_ERROR("Unable to unpack transform message - header message is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( socket == NULL )
  {
    LOG_ERROR("Unable to unpack transform message - socket is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( transformMatrix == NULL )
  {
    LOG_ERROR("Unable to unpack transform message - matrix is NULL!"); 
    return PLUS_FAIL; 
  }

  igtl::TransformMessage::Pointer transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(headerMsg);
  transMsg->AllocatePack();

  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());

  //  If 1 is specified it performs CRC check and unpack the data only if CRC passes
  int c = transMsg->Unpack(1);
  if ( !(c & igtl::MessageHeader::UNPACK_BODY) )
  {
    LOG_ERROR("Couldn't receive transform message from server!"); 
    return PLUS_FAIL; 
  }
  // if CRC check is OK. Read transform data.
  igtl::Matrix4x4 igtlMatrix;
  igtl::IdentityMatrix(igtlMatrix);
  transMsg->GetMatrix(igtlMatrix);

  // convert igtl matrix to vtk matrix 
  for ( int r = 0; r < 4; r++ )
  {
    for ( int c = 0; c < 4; c++ )
    {
      transformMatrix->SetElement(r,c, igtlMatrix[r][c]); 
    }
  }

  // Get timestamp 
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
  transMsg->GetTimeStamp(igtlTimestamp); 
  timestamp = igtlTimestamp->GetTimeStamp();  

  // Get transform name 
  transformName = transMsg->GetDeviceName(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::PackPositionMessage(igtl::PositionMessage::Pointer positionMessage, PlusTransformName& transformName, 
                                                           igtl::Matrix4x4& igtlMatrix, double timestamp )
{
  if ( positionMessage.IsNull() )
  {
    LOG_ERROR("Failed to pack position message - input position message is NULL"); ;
    return PLUS_FAIL; 
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime( timestamp );

  std::string strTransformName;
  transformName.GetTransformName( strTransformName );

  positionMessage->SetPosition( igtlMatrix[0][3], igtlMatrix[1][3], igtlMatrix[2][3] );
  positionMessage->SetTimeStamp( igtlTime );
  positionMessage->SetDeviceName( strTransformName.c_str() );
  positionMessage->Pack();
  
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// static 
PlusStatus vtkPlusIgtlMessageCommon::UnpackPositionMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, 
                                                            float position[3], std::string& positionName, double& timestamp )
{
  if ( headerMsg.IsNull() )
  {
    LOG_ERROR("Unable to unpack position message - header message is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( socket == NULL ) 
  {
    LOG_ERROR("Unable to unpack position message - socket is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( position == NULL )
  {
    LOG_ERROR("Unable to unpack position message - position is NULL!"); 
    return PLUS_FAIL; 
  }

  igtl::PositionMessage::Pointer posMsg = igtl::PositionMessage::New();
  posMsg->SetMessageHeader(headerMsg);
  posMsg->AllocatePack();

  socket->Receive(posMsg->GetPackBodyPointer(), posMsg->GetPackBodySize());

  //  If 1 is specified it performs CRC check and unpack the data only if CRC passes
  int c = posMsg->Unpack(1);
  if ( !(c & igtl::MessageHeader::UNPACK_BODY) )
  {
    LOG_ERROR("Couldn't receive position message from server!"); 
    return PLUS_FAIL; 
  }

  // if CRC check is OK. Read position data.
  posMsg->GetPosition(position);

  // Get timestamp 
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
  posMsg->GetTimeStamp(igtlTimestamp); 
  timestamp = igtlTimestamp->GetTimeStamp();  

  // Get transform name 
  positionName = posMsg->GetDeviceName(); 

  return PLUS_SUCCESS; 
}


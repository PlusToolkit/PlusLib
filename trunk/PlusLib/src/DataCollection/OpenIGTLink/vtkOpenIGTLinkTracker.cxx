/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkOpenIGTLinkTracker.h"

#include "igtlPositionMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlTransformMessage.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIgtlMessageCommon.h"

vtkStandardNewMacro(vtkOpenIGTLinkTracker);

//----------------------------------------------------------------------------
vtkOpenIGTLinkTracker::vtkOpenIGTLinkTracker()
: TrackerInternalCoordinateSystemName(NULL)
, UseLastTransformsOnReceiveTimeout(false)
{
  SetTrackerInternalCoordinateSystemName("Reference");
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkTracker::~vtkOpenIGTLinkTracker() 
{
  SetTrackerInternalCoordinateSystemName(NULL);
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Disconnect" ); 

  // If we need TDATA, request server to stop streaming.
  if ( std::string( this->MessageType ).compare( "TDATA" ) == 0 )
  {
    igtl::StopTrackingDataMessage::Pointer stpMsg = igtl::StopTrackingDataMessage::New();
    stpMsg->SetDeviceName("");
    stpMsg->Pack();

    int retValue = 0;
    RETRY_UNTIL_TRUE( 
      (retValue = this->ClientSocket->Send( stpMsg->GetPackPointer(), stpMsg->GetPackSize() ))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

    if ( retValue == 0 )
    {
      LOG_ERROR("Failed to send STP_TDATA message to server!"); 
      return PLUS_FAIL; 
    } 
  }

  return Superclass::InternalDisconnect();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalUpdate()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalUpdate" );

  if ( ! this->Recording )
  {
    LOG_ERROR("called Update() when not tracking" );
    return PLUS_FAIL;
  }

  igtl::MessageHeader::Pointer headerMsg;
  ReceiveMessageHeaderWithErrorHandling(headerMsg);
  
  if (headerMsg.IsNull())
  {
    // Has not received data
    if (this->UseLastTransformsOnReceiveTimeout)
    {
      // The server only sends update if a transform is modified, it's not an error
      LOG_TRACE("No OpenIGTLink message has been received in device "<<this->GetDeviceId());
      // Store the last known transform values (useful when the server only notifies about transform changes
      double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
      StoreMostRecentTransformValues(unfilteredTimestamp);
      return PLUS_SUCCESS;
    }
    else
    {
      OnReceiveTimeout();
      return PLUS_FAIL;
    }
  }

  // We've received valid header data
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);
  if (strcmp( headerMsg->GetDeviceType(), "TDATA" ) == 0 )
  {
    // TDATA message
    return ProcessTDataMessage(headerMsg);
  }
  
  // TRANSFORM or POSITION message
  return ProcessTransformMessage(headerMsg);
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::SendRequestedMessageTypes()
{
  // If we need TDATA, request server to start streaming.
  if ( std::string( this->MessageType ).compare( "TDATA" ) == 0 )
  {
    igtl::StartTrackingDataMessage::Pointer sttMsg = igtl::StartTrackingDataMessage::New();
    sttMsg->SetDeviceName("");
    sttMsg->SetResolution(50);
    sttMsg->SetCoordinateName(this->TrackerInternalCoordinateSystemName);
    sttMsg->Pack();

    int retValue = 0;
    RETRY_UNTIL_TRUE( 
      (retValue = this->ClientSocket->Send( sttMsg->GetPackPointer(), sttMsg->GetPackSize() ))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

    if ( retValue == 0 )
    {
      LOG_ERROR("Failed to send STT_TDATA message to server!"); 
      return PLUS_FAIL; 
    }
  }

  return this->Superclass::SendRequestedMessageTypes();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::StoreMostRecentTransformValues(double unfilteredTimestamp)
{
  PlusStatus status=PLUS_SUCCESS;
  // Set status for tools with non-detected markers
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkSmartPointer<vtkMatrix4x4> toolMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    // retrieve latest transform value from the buffer
    if (it->second->GetNumberOfItems()>0)
    {
      BufferItemUidType latestItemUid = it->second->GetLatestItemUidInBuffer();
      StreamBufferItem item; 
      if ( it->second->GetStreamBufferItem(latestItemUid, &item) == ITEM_OK )
      {
        if (item.GetUnfilteredTimestamp(this->GetLocalTimeOffsetSec()) >= unfilteredTimestamp)
        {
          // this item already has an updated value for this timestamp, no need to store it again
          continue;
        }
        if (item.GetMatrix(toolMatrix)!=PLUS_SUCCESS)
        {
          LOG_WARNING("Failed to get matrix from buffer item with UID: " << latestItemUid );
          status=PLUS_FAIL;
        }
      }
      else
      {
        LOG_WARNING("Failed to get buffer item with UID: " << latestItemUid );
        status=PLUS_FAIL;
      }
    }
    if ( this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetSourceId(), toolMatrix, TOOL_OK, unfilteredTimestamp, unfilteredTimestamp) != PLUS_SUCCESS )
    {
      LOG_INFO("ToolTimeStampedUpdate failed for tool: " << it->second->GetSourceId() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
      status=PLUS_FAIL;
    }
  }
  
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ProcessTDataMessage(igtl::MessageHeader::Pointer headerMsg)
{
  igtl::TrackingDataMessage::Pointer tdataMsg = igtl::TrackingDataMessage::New();
  tdataMsg->SetMessageHeader( headerMsg );
  tdataMsg->AllocatePack();

  this->ClientSocket->Receive( tdataMsg->GetPackBodyPointer(), tdataMsg->GetPackBodySize() );
  int c = tdataMsg->Unpack( this->IgtlMessageCrcCheckEnabled );
  if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
  {
    LOG_ERROR( "Couldn't receive TDATA message from server!" );
    return PLUS_FAIL;
  }

  // for now just use system time, all coordinates will be sequential.
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  double filteredTimestamp = unfilteredTimestamp; // No need to filter already filtered timestamped items received over OpenIGTLink 
  // We store the list of identified tools (tools we get information about from the tracker).
  // The tools that are missing from the tracker message are assumed to be out of view. 
  std::set<std::string> identifiedToolSourceIds;
  for ( int i = 0; i < tdataMsg->GetNumberOfTrackingDataElements(); ++ i )
  {
    igtl::TrackingDataElement::Pointer tdataElem = igtl::TrackingDataElement::New();
    tdataMsg->GetTrackingDataElement( i, tdataElem );

    igtl::Matrix4x4 igtlMatrix;
    tdataElem->GetMatrix(igtlMatrix);
    vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    // convert igtl matrix to vtk matrix 
    for ( int r = 0; r < 4; r++ )
    {
      for ( int c = 0; c < 4; c++ )
      {
        toolMatrix->SetElement(r,c, igtlMatrix[r][c]); 
      }
    }

    // Get timestamp 
    igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
    tdataMsg->GetTimeStamp(igtlTimestamp); 
    double unfilteredTimestampUtc = igtlTimestamp->GetTimeStamp();  

    // Get igtl transform name 
    std::string igtlTransformName = tdataElem->GetName();

    // Set internal transform name
    PlusTransformName transformName(igtlTransformName.c_str(), this->TrackerInternalCoordinateSystemName);
    if ( this->ToolTimeStampedUpdateWithoutFiltering(transformName.GetTransformName().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) == PLUS_SUCCESS )
    {
      identifiedToolSourceIds.insert(transformName.GetTransformName());
    }
    else
    {
      LOG_INFO("ToolTimeStampedUpdate failed for tool: " << transformName.From() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
      // DO NOT return here: we want to update the other tools.
    }
  }
  // Set status for non-detected tools
  vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  toolMatrix->Identity();
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {    
    if (identifiedToolSourceIds.find(it->second->GetSourceId())!=identifiedToolSourceIds.end())
    {
      // this tool has been found and update has been already called with the correct transform
      LOG_TRACE("Tool "<<it->second->GetSourceId()<<": found");
      continue;
    }
    LOG_TRACE("Tool "<<it->second->GetSourceId()<<": not found");
    this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetSourceId(), toolMatrix, TOOL_OUT_OF_VIEW, unfilteredTimestamp, filteredTimestamp);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ProcessTransformMessage(igtl::MessageHeader::Pointer headerMsg)
{
  double unfilteredTimestampUtc = 0; 
  vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  std::string igtlTransformName; 
  
  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
  {
    if ( vtkPlusIgtlMessageCommon::UnpackTransformMessage(headerMsg, this->ClientSocket.GetPointer(), toolMatrix, igtlTransformName, unfilteredTimestampUtc, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't receive transform message from server!"); 
      return PLUS_FAIL;
    }
  }
  else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
  {
    float position[3] = {0}; 
    if ( vtkPlusIgtlMessageCommon::UnpackPositionMessage(headerMsg, this->ClientSocket.GetPointer(), position, igtlTransformName, unfilteredTimestampUtc, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't receive position message from server!"); 
      return PLUS_FAIL;
    }
    toolMatrix->Identity(); 
    toolMatrix->SetElement(0,3, position[0]); 
    toolMatrix->SetElement(1,3, position[1]); 
    toolMatrix->SetElement(2,3, position[2]); 
  }
  else
  {
    // if the data type is unknown, skip reading. 
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS; 
  }

  // Set transform name
  PlusTransformName transformName;
  if ( transformName.SetTransformName( igtlTransformName.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tracker tool - unrecognized transform name: " << igtlTransformName ); 
    return PLUS_FAIL; 
  }
    
  double unfilteredTimestamp = 0;
  if (this->UseReceivedTimestamps)
  {
    // Use the timestamp in the OpenIGTLink message
    // The received timestamp is in UTC and timestampts in the buffer are in system time, so conversion is needed
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc); 
  }
  else
  {
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  }  

  // No need to filter already filtered timestamped items received over OpenIGTLink 
  // If the original timestamps are not used it's still safer not to use filtering, as filtering assumes uniform framerate, which is not guaranteed
  double filteredTimestamp = unfilteredTimestamp;

  // Store the transform that we've just received
  if ( this->ToolTimeStampedUpdateWithoutFiltering(transformName.GetTransformName().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_INFO("ToolTimeStampedUpdate failed for tool: " << transformName.GetTransformName() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
    return PLUS_FAIL;
  }
  if ( this->UseLastTransformsOnReceiveTimeout )
  {
    // Store all the other transforms with the last known value
    StoreMostRecentTransformValues(filteredTimestamp);
  }

  return PLUS_SUCCESS;
  }

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(TrackerInternalCoordinateSystemName, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseLastTransformsOnReceiveTimeout, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("TrackerInternalCoordinateSystemName", this->TrackerInternalCoordinateSystemName);
  deviceConfig->SetAttribute("UseLastTransformsOnReceiveTimeout", this->UseLastTransformsOnReceiveTimeout?"true":"false");
  return PLUS_SUCCESS;
}

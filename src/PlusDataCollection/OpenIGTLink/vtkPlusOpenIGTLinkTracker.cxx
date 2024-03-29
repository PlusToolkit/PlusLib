/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkTracker.h"

#include "igtlPositionMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlTransformMessage.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIgtlMessageCommon.h"

#include <set>

vtkStandardNewMacro(vtkPlusOpenIGTLinkTracker);

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkTracker::vtkPlusOpenIGTLinkTracker()
  : UseLastTransformsOnReceiveTimeout(false)
{
  SetToolReferenceFrameName("Reference");
}

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkTracker::~vtkPlusOpenIGTLinkTracker()
{
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "UseLastTransformsOnReceiveTimeout: " << this->UseLastTransformsOnReceiveTimeout;

  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusOpenIGTLinkTracker::Disconnect");
  if (this->IsTDataMessageType())
  {
    // If we need TDATA, request server to stop streaming.
    auto stpMsg = igtl::StopTrackingDataMessage::New();
    stpMsg->SetDeviceName("");
    stpMsg->Pack();

    int retValue = 0;
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
      RETRY_UNTIL_TRUE(
        (retValue = this->ClientSocket->Send(stpMsg->GetBufferPointer(), stpMsg->GetBufferSize())) != 0,
        this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
    }

    if (retValue == 0)
    {
      LOG_ERROR("Failed to send STP_TDATA message to server!");
      return PLUS_FAIL;
    }
  }

  return Superclass::InternalDisconnect();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::InternalUpdate()
{
  LOG_TRACE("vtkPlusOpenIGTLinkTracker::InternalUpdate");

  if (!this->Recording)
  {
    LOG_ERROR("called Update() when not tracking");
    return PLUS_FAIL;
  }

  if (this->IsTDataMessageType())
  {
    return this->InternalUpdateTData();
  }
  else
  {
    return this->InternalUpdateGeneral();
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::InternalUpdateTData()
{
  LOG_TRACE("vtkPlusOpenIGTLinkTracker::InternalUpdateTData");

  igtl::MessageBase::Pointer bodyMsg;
  igtl::MessageHeader::Pointer headerMsg;

  while (true)
  {
    ReceiveMessageHeaderWithErrorHandling(headerMsg);

    if (headerMsg.IsNull())
    {
      // Has not received data
      if (this->UseLastTransformsOnReceiveTimeout)
      {
        // The server only sends update if a transform is modified, it's not an error
        LOG_TRACE("No OpenIGTLink message has been received in device " << this->GetDeviceId());
        // Store the last known transform values (useful when the server only notifies about transform changes
        double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
        StoreMostRecentTransformValues(unfilteredTimestamp);
        return PLUS_SUCCESS;
      }
      else
      {
        OnReceiveTimeout();
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
        if (!this->ClientSocket->GetConnected())
        {
          // Could not restore the connection, set transform status to INVALID
          double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
          StoreInvalidTransforms(unfilteredTimestamp);
        }
        return PLUS_FAIL;
      }
    }

    // We've received valid header data
    headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);

    bodyMsg = this->MessageFactory->CreateReceiveMessage(headerMsg);
    if (typeid(*bodyMsg) == typeid(igtl::TrackingDataMessage))
    {
      // received a TDATA message
      break;
    }

    // data type is unknown, ignore it
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
  }

  // TDATA message
  igtl::TrackingDataMessage::Pointer tdataMsg = dynamic_cast<igtl::TrackingDataMessage*>(bodyMsg.GetPointer());
  tdataMsg->SetMessageHeader(headerMsg);
  tdataMsg->AllocateBuffer();

  {
    bool timeout(false);
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    this->ClientSocket->Receive(tdataMsg->GetBufferBodyPointer(), tdataMsg->GetBufferBodySize(), timeout);
  }
  int c = tdataMsg->Unpack(this->IgtlMessageCrcCheckEnabled);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive TDATA message from server!");
    return PLUS_FAIL;
  }

  // for now just use system time, all coordinates will be sequential.
  double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  double filteredTimestamp = unfilteredTimestamp; // No need to filter already filtered timestamped items received over OpenIGTLink
  // We store the list of identified tools (tools we get information about from the tracker).
  // The tools that are missing from the tracker message are assumed to be out of view.
  std::set<std::string> identifiedToolSourceIds;
  for (int i = 0; i < tdataMsg->GetNumberOfTrackingDataElements(); ++ i)
  {
    auto tdataElem = igtl::TrackingDataElement::New();
    tdataMsg->GetTrackingDataElement(i, tdataElem);

    igtl::Matrix4x4 igtlMatrix;
    tdataElem->GetMatrix(igtlMatrix);
    vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    // convert igtl matrix to vtk matrix
    for (int r = 0; r < 4; r++)
    {
      for (int c = 0; c < 4; c++)
      {
        toolMatrix->SetElement(r, c, igtlMatrix[r][c]);
      }
    }

    // Get timestamp
    auto igtlTimestamp = igtl::TimeStamp::New();
    tdataMsg->GetTimeStamp(igtlTimestamp);

    // Get igtl transform name
    std::string igtlTransformName = tdataElem->GetName();

    // Set internal transform name
    igsioTransformName transformName;
    if (igtlTransformName.find("To") != std::string::npos)
    {
      // Plus style transform name sent
      transformName = igtlTransformName;
    }
    else
    {
      // Brainlab style transform name sent
      transformName = igsioTransformName(igtlTransformName.c_str(), this->ToolReferenceFrameName);
    }

    if (this->ToolTimeStampedUpdateWithoutFiltering(transformName.GetTransformName().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) == PLUS_SUCCESS)
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
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if (identifiedToolSourceIds.find(it->second->GetId()) != identifiedToolSourceIds.end())
    {
      // this tool has been found and update has been already called with the correct transform
      LOG_TRACE("Tool " << it->second->GetId() << ": found");
      continue;
    }
    LOG_TRACE("Tool " << it->second->GetId() << ": not found");
    this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetId(), toolMatrix, TOOL_OUT_OF_VIEW, unfilteredTimestamp, filteredTimestamp);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::InternalUpdateGeneral()
{
  LOG_TRACE("vtkPlusOpenIGTLinkTracker::InternalUpdateGeneral");

  double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  double maxAllocatedProcessingTime = 2.0;
  // set maxAllocatedProcessingTime to 2 acquisition periods to allow reading all transforms even when there aare slight delays
  if (this->GetAcquisitionRate() > 2.0 / maxAllocatedProcessingTime)
  {
    maxAllocatedProcessingTime = 2.0 / this->GetAcquisitionRate();
  }

  igtl::MessageHeader::Pointer headerMsg;

  bool moreMessagesPossible = true;
  while (moreMessagesPossible)
  {
    if (ProcessTransformMessageGeneral(moreMessagesPossible) != PLUS_SUCCESS)
    {
      // an error occurred, stop processing the messages in this update iteration
      break;
    }
    if (vtkIGSIOAccurateTimer::GetSystemTime() - unfilteredTimestamp > maxAllocatedProcessingTime)
    {
      // no more time for processing messages in this iteration
      break;
    }
  }

  if (this->UseLastTransformsOnReceiveTimeout)
  {
    // Store all the other transforms with the last known value
    // that has not been updated in this update iteration
    StoreMostRecentTransformValues(unfilteredTimestamp);
  }
  else
  {
    // Set all those transforms to invalid that contains stale transform values
    StoreInvalidTransforms(unfilteredTimestamp);
  }

  if (!this->ClientSocket->GetConnected())
  {
    // Could not restore the connection, set transform status to INVALID
    double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
    StoreInvalidTransforms(unfilteredTimestamp);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::ProcessTransformMessageGeneral(bool& moreMessagesPossible)
{
  igtl::MessageHeader::Pointer headerMsg;
  ReceiveMessageHeaderWithErrorHandling(headerMsg);
  if (headerMsg.IsNull())
  {
    // did not receive transform message, so there are no more available
    moreMessagesPossible = false;
    return PLUS_SUCCESS;
  }

  moreMessagesPossible = true;

  // We've received valid header data
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);

  // Accept TRANSFORM or POSITION message
  double unfilteredTimestampUtc = 0;
  vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  std::string igtlTransformName;
  ToolStatus toolStatus(TOOL_UNKNOWN);

  igtl::MessageBase::Pointer bodyMsg = this->MessageFactory->CreateReceiveMessage(headerMsg);
  if (typeid(*bodyMsg) == typeid(igtl::TransformMessage))
  {
    if (vtkPlusIgtlMessageCommon::UnpackTransformMessage(bodyMsg, this->ClientSocket.GetPointer(), toolMatrix, toolStatus, igtlTransformName, unfilteredTimestampUtc, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't receive transform message from server!");
      return PLUS_FAIL;
    }
  }
  else if (typeid(*bodyMsg) == typeid(igtl::PositionMessage))
  {
    if (vtkPlusIgtlMessageCommon::UnpackPositionMessage(bodyMsg, this->ClientSocket.GetPointer(), toolMatrix, igtlTransformName, toolStatus, unfilteredTimestampUtc, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't receive position message from server!");
      return PLUS_FAIL;
    }
  }
  else
  {
    // if the data type is unknown, skip reading.
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS;
  }

  // Set transform name
  igsioTransformName transformName;
  if (transformName.SetTransformName(igtlTransformName.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to update tracker tool - unrecognized transform name: " << igtlTransformName);
    return PLUS_FAIL;
  }

  double unfilteredTimestamp = 0;
  if (this->UseReceivedTimestamps)
  {
    // Use the timestamp in the OpenIGTLink message
    // The received timestamp is in UTC and timestamps in the buffer are in system time, so conversion is needed
    unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc);
  }
  else
  {
    unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }

  // No need to filter already filtered timestamped items received over OpenIGTLink
  // If the original timestamps are not used it's still safer not to use filtering, as filtering assumes uniform frame rate, which is not guaranteed
  double filteredTimestamp = unfilteredTimestamp;

  // Store the transform that we've just received
  // TODO: we should not write it into the buffer until we have all the tools ready (if we are not using the original timestamps)
  if (this->ToolTimeStampedUpdateWithoutFiltering(transformName.GetTransformName().c_str(), toolMatrix, toolStatus, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS)
  {
    LOG_INFO("ToolTimeStampedUpdate failed for tool: " << transformName.GetTransformName() << " with timestamp: " << std::fixed << unfilteredTimestamp);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::SendRequestedMessageTypes()
{
  if (this->Superclass::SendRequestedMessageTypes() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  // NOTE: Command above sends clientinfo message, which implicitly stops TDATA sending
  // Command below must be sent after the clientinfo message is sent

  // If we need TDATA, request server to start streaming.
  if (this->IsTDataMessageType())
  {
    auto sttMsg = igtl::StartTrackingDataMessage::New();
    sttMsg->SetDeviceName("");
    sttMsg->SetResolution(50);
    sttMsg->SetCoordinateName(this->ToolReferenceFrameName.c_str());
    sttMsg->Pack();

    int retValue = 0;
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
      RETRY_UNTIL_TRUE(
        (retValue = this->ClientSocket->Send(sttMsg->GetBufferPointer(), sttMsg->GetBufferSize())) != 0,
        this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
    }

    if (retValue == 0)
    {
      LOG_ERROR("Failed to send STT_TDATA message to server!");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::StoreMostRecentTransformValues(double unfilteredTimestamp)
{
  PlusStatus status = PLUS_SUCCESS;
  // Set transform values for tools with non-detected markers
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    // retrieve latest transform value from the buffer
    if (it->second->GetNumberOfItems() > 0)
    {
      BufferItemUidType latestItemUid = it->second->GetLatestItemUidInBuffer();
      StreamBufferItem item;
      if (it->second->GetStreamBufferItem(latestItemUid, &item) == ITEM_OK)
      {
        if (item.GetUnfilteredTimestamp(this->GetLocalTimeOffsetSec()) >= unfilteredTimestamp)
        {
          // this item already has an updated value for this timestamp, no need to store it again
          continue;
        }
        if (item.GetMatrix(toolMatrix) != PLUS_SUCCESS)
        {
          LOG_WARNING("Failed to get matrix from buffer item with UID: " << latestItemUid);
          status = PLUS_FAIL;
        }
      }
      else
      {
        LOG_WARNING("Failed to get buffer item with UID: " << latestItemUid);
        status = PLUS_FAIL;
      }
    }
    if (this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetId(), toolMatrix, TOOL_OK, unfilteredTimestamp, unfilteredTimestamp) != PLUS_SUCCESS)
    {
      LOG_INFO("ToolTimeStampedUpdate failed for tool: " << it->second->GetId() << " with timestamp: " << std::fixed << unfilteredTimestamp);
      status = PLUS_FAIL;
    }
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::StoreInvalidTransforms(double unfilteredTimestamp)
{
  PlusStatus status = PLUS_SUCCESS;

  // If the item is delayed with maximum itemArrivalDelayTolerance then we don't set its status to invalid yet
  // itemArrivalDelayTolerance is one frame period (but maximum 3 seconds).
  double itemArrivalDelayToleranceSec = 3.0;
  if (this->GetAcquisitionRate() > 1.0 / itemArrivalDelayToleranceSec)
  {
    itemArrivalDelayToleranceSec = 1.0 / this->GetAcquisitionRate();
  }

  // Set invalid status for tools with non-detected markers
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    // retrieve latest transform value from the buffer
    if (it->second->GetNumberOfItems() > 0)
    {
      BufferItemUidType latestItemUid = it->second->GetLatestItemUidInBuffer();
      StreamBufferItem item;
      if (it->second->GetStreamBufferItem(latestItemUid, &item) == ITEM_OK)
      {
        if (item.GetUnfilteredTimestamp(this->GetLocalTimeOffsetSec()) + itemArrivalDelayToleranceSec >= unfilteredTimestamp)
        {
          // this item already has an updated value for this timestamp, no need to store it again
          continue;
        }
        if (item.GetMatrix(toolMatrix) != PLUS_SUCCESS)
        {
          LOG_WARNING("Failed to get matrix from buffer item with UID: " << latestItemUid);
          status = PLUS_FAIL;
        }
      }
      else
      {
        LOG_WARNING("Failed to get buffer item with UID: " << latestItemUid);
        status = PLUS_FAIL;
      }
    }

    LOG_TRACE("No update was received for tool: " << it->second->GetId() << ", assume that it does not provide valid transform anymore");
    if (this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetId(), toolMatrix, TOOL_INVALID, unfilteredTimestamp, unfilteredTimestamp) != PLUS_SUCCESS)
    {
      LOG_INFO("ToolTimeStampedUpdate failed for tool: " << it->second->GetId() << " with timestamp: " << std::fixed << unfilteredTimestamp);
      status = PLUS_FAIL;
    }
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseLastTransformsOnReceiveTimeout, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("UseLastTransformsOnReceiveTimeout", this->UseLastTransformsOnReceiveTimeout ? "true" : "false");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusOpenIGTLinkTracker::IsTDataMessageType()
{
  if (this->MessageType.empty())
  {
    return false;
  }
  return (igsioCommon::IsEqualInsensitive(this->MessageType, "TDATA"));
}

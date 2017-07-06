/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOptiTrack.h"

#include "PlusMath.h"

#include "vtkMath.h"
#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

#include <math.h>
#include <sstream>

// OptiTrack NatNet SDK
#include "NatNetTypes.h"
#include "NatNetClient.h"

NatNetClient* natNetClient;
void ReceiveDataCallback(sFrameOfMocapData* data, void* pUserData);

vtkStandardNewMacro(vtkPlusOptiTrack);

//-----------------------------------------------------------------------
vtkPlusOptiTrack::vtkPlusOptiTrack()
{ 
  this->RequirePortNameInDeviceSetConfiguration = false;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=false; 

}

//-------------------------------------------------------------------------
vtkPlusOptiTrack::~vtkPlusOptiTrack() 
{

}

//-------------------------------------------------------------------------
void vtkPlusOptiTrack::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalConnect()
{
  LOG_TRACE( "vtkPlusOptiTrack::Connect" ); 
  // Connect tracker
  // Load Calibration file
  // Set camera parameters read from the PLUS XML configuration file
  // Add tools from Configuration Files
  // Start tracker
  natNetClient = new NatNetClient(ConnectionType_Multicast);
  natNetClient->SetVerbosityLevel(Verbosity_None);
  natNetClient->SetVerbosityLevel(Verbosity_Warning);
  natNetClient->SetDataCallback(ReceiveDataCallback, this);

  //TODO: config ip
  int retCode = natNetClient->Initialize((char*)IPClient.c_str(), (char*)IPServer.c_str());

  // Should fail if motive is not open, but it doesn't
  if (retCode != ErrorCode_OK)
  {
    LOG_ERROR("vtkPlusOptiTrack::InternalConnect: Could not connect to camera");
    return PLUS_FAIL;
  }

  void* response;
  int nBytes;
  if (natNetClient->SendMessageAndWait("UnitsToMillimeters", &response, &nBytes) == ErrorCode_OK)
  {
    this->UnitsToMm = (*(float*)response);
  }
  else
  {
    // Fail if motive is not running
    LOG_ERROR("vtkPlusOptiTrack::InternalConnect: Could not connect to camera");
    return PLUS_FAIL;
  }

  this->MatchTrackedTools();

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
void vtkPlusOptiTrack::MatchTrackedTools()
{
  LOG_TRACE("vtkPlusOptiTrack::MatchTrackedTools");

  this->TrackedToolMap.clear();

  sDataDescriptions* dataDescriptions;
  natNetClient->GetDataDescriptions(&dataDescriptions);
  for (int i = 0; i < dataDescriptions->nDataDescriptions; ++i)
  {
    sDataDescription currentDiscription = dataDescriptions->arrDataDescriptions[i];
    if (currentDiscription.type == Descriptor_RigidBody)
    {
      // Map the numerical ID of the tracked tool from motive to the name of the tool
      this->TrackedToolMap[currentDiscription.Data.RigidBodyDescription->ID] = currentDiscription.Data.RigidBodyDescription->szName;
    }
  }

  if (this->TrackedTools.size() != this->TrackedToolMap.size())
  {
    LOG_WARNING("vtkPlusOptiTrack::MatchTrackedTools: Could not find data for all tracked tools, make sure that all tools have correct names and unique ids")
  }

}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalDisconnect()
{
  LOG_TRACE( "vtkPlusOptiTrack::Disconnect" ); 

  if (natNetClient)
  {
    natNetClient->Uninitialize();
    delete natNetClient;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalUpdate()
{
  if (Superclass::InternalUpdate() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalCallback(sFrameOfMocapData* data)
{
  LOG_TRACE( "vtkPlusOptiTrack::InternalCallback" );

  this->MatchTrackedTools();

  sDataDescriptions* dataDescriptions;
  natNetClient->GetDataDescriptions(&dataDescriptions);

  int numberOfRigidBodies = data->nRigidBodies;
  sRigidBodyData* rigidBodies = data->RigidBodies;

  if (data->nOtherMarkers)
  {
    LOG_WARNING("vtkPlusOptiTrack::InternalCallback: Untracked markers detected. Check for interference or make sure that the entire tool is in view")
  }

  for (int rigidBodyId = 0; rigidBodyId < numberOfRigidBodies; ++rigidBodyId)
  {
    const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();
    sRigidBodyData currentRigidBody = rigidBodies[rigidBodyId];

    // Convert translation to Mm
    double translation[3] = { currentRigidBody.x * this->UnitsToMm, currentRigidBody.y * this->UnitsToMm, currentRigidBody.z * this->UnitsToMm };

    // Convert rotation from quaternion to 3x3 matrix
    double quaternion[4] = { currentRigidBody.qw, currentRigidBody.qx, currentRigidBody.qy, currentRigidBody.qz };
    double rotation[3][3] = { 0,0,0, 0,0,0, 0,0,0 };
    vtkMath::QuaternionToMatrix3x3(quaternion, rotation);

    // Construct the transformation matrix from the rotation and translation components
    vtkSmartPointer<vtkMatrix4x4> rigidBodyToTrackerTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        rigidBodyToTrackerTransformMatrix->SetElement(i, j, rotation[i][j]);
      }
      rigidBodyToTrackerTransformMatrix->SetElement(i, 3, translation[i]);
    }

    // Make sure the tool was specified in the Config file
    std::string toolName = this->TrackedToolMap[currentRigidBody.ID];
    if (this->TrackedTools.count(toolName))
    {
      PlusTransformName toolTransformName = PlusTransformName(toolName, this->GetToolReferenceFrameName());
      ToolTimeStampedUpdate(toolTransformName.GetTransformName(), rigidBodyToTrackerTransformMatrix, TOOL_OK, FrameNumber, unfilteredTimestamp);
    }

  }

  FrameNumber++;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(IPClient, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(IPServer, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(toolDataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }
    if (toolDataElement->GetAttribute("Type") != NULL && STRCASECMP(toolDataElement->GetAttribute("Type"), "Tool") != 0)
    {
      // if this is not a Tool element, skip it
      continue;
    }

    const char* toolId = toolDataElement->GetAttribute("Id");
    if (toolId == NULL)
    {
      // tool doesn't have ID needed to generate transform
      LOG_ERROR("Failed to initialize OptiTrack tool: DataSource Id is missing");
      continue;
    }

    this->TrackedTools.insert(toolId);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void ReceiveDataCallback(sFrameOfMocapData* data, void* pUserData)
{
  vtkPlusOptiTrack* internalCallback = (vtkPlusOptiTrack*)pUserData;
  internalCallback->InternalCallback(data);
}
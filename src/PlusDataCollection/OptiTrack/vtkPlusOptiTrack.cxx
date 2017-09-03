/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusOptiTrack.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkXMLDataElement.h>

// Motive API includes
#include <NPTrackingTools.h>

vtkStandardNewMacro(vtkPlusOptiTrack);

#define M_TO_MM 1000

//----------------------------------------------------------------------------
class vtkPlusOptiTrack::vtkInternal
{
public:
  vtkPlusOptiTrack* External;

  vtkInternal(vtkPlusOptiTrack* external)
    : External(external)
  {
  }

  virtual ~vtkInternal()
  {
  }

  std::string ProjectFile;
  std::string CalibrationFile;
  std::vector<std::string> AdditionalRigidBodyFiles;
  std::map<std::string, PlusTransformName> RBNameToTransformName;

  void PrintMotiveErrorMessage(NPRESULT result);
};

//-----------------------------------------------------------------------
void vtkPlusOptiTrack::vtkInternal::PrintMotiveErrorMessage(NPRESULT result)
{
  LOG_ERROR("Motive error message: " << TT_GetResultString(result));
}

//-----------------------------------------------------------------------
vtkPlusOptiTrack::vtkPlusOptiTrack()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{ 
  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
}

//-------------------------------------------------------------------------
vtkPlusOptiTrack::~vtkPlusOptiTrack() 
{
  delete Internal;
  Internal = nullptr;
}

//-------------------------------------------------------------------------
void vtkPlusOptiTrack::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(ProjectFile, this->Internal->ProjectFile, deviceConfig);

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

    std::string toolId(toolDataElement->GetAttribute("Id"));
    if (toolId.empty())
    {
      // tool doesn't have ID needed to generate transform
      LOG_ERROR("Failed to initialize OptiTrack tool: DataSource Id is missing. This should be the name of the Motive Rigid Body that tracks the tool.");
      continue;
    }

    PlusTransformName toolTransformName(toolId, this->GetToolReferenceFrameName());
    std::pair<std::string, PlusTransformName> newTool(toolId, toolTransformName);
    bool wasInserted = this->Internal->RBNameToTransformName.insert(newTool).second;
    if (!wasInserted)
    {
      // duplicate toolId
      LOG_ERROR("Duplicate tool found in Plus config file with Id \"" << toolId << "\". All tool Id's are required to be unique.");
      return PLUS_FAIL;
    }

    if (toolDataElement->GetAttribute("RigidBodyFile") != NULL)
    {
      // this tool has an associated rigid body definition
      const char* rigidBodyFile = toolDataElement->GetAttribute("RigidBodyFile");
      this->Internal->AdditionalRigidBodyFiles.push_back(rigidBodyFile);
    }
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
PlusStatus vtkPlusOptiTrack::Probe()
{
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalConnect()
{
  // initialize the API
  if (TT_Initialize() != NPRESULT_SUCCESS)
  {
    LOG_ERROR("Failed to start Motive.");
    return PLUS_FAIL;
  }

  // pick up recently-arrived cameras
  TT_Update();

  // open project file
  std::string projectFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->Internal->ProjectFile);
  NPRESULT ttpLoad = TT_LoadProject(projectFilePath.c_str());
  if (ttpLoad != NPRESULT_SUCCESS)
  {
    LOG_ERROR("Failed to load Motive project file. Motive error: " << TT_GetResultString(ttpLoad));
    return PLUS_FAIL;
  }

  // add any additional rigid body files to project
  std::string rbFilePath;
  for (auto it = this->Internal->AdditionalRigidBodyFiles.begin(); it != this->Internal->AdditionalRigidBodyFiles.end(); it++)
  {
    rbFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(*it);
    NPRESULT addRBResult = TT_AddRigidBodies(rbFilePath.c_str());
    if (addRBResult != NPRESULT_SUCCESS)
    {
      LOG_ERROR("Failed to load rigid body file located at: " << rbFilePath);
      this->Internal->PrintMotiveErrorMessage(addRBResult);
      return PLUS_FAIL;
    }
  }

  // verify all rigid bodies in Motive have unique names
  std::set<std::string> rigidBodies;
  bool isUnique = true;
  for (int i = 0; i < TT_RigidBodyCount(); i++)
  {
    std::string rbName(TT_RigidBodyName(i));
    if (!rigidBodies.insert(rbName).second)
    {
      // non-unique tool name found in Motive project with added rigid bodies
      isUnique = false;
      LOG_ERROR("Duplicate Motive rigid bodies with name: " << TT_RigidBodyName(i));
    }
  }
  if (!isUnique)
  {
    LOG_ERROR("Please resolve non-unique rigid body names. Please note, if a rigid body is defined in the project file DO NOT include the markers TRA file in the tool DataSoure element.");
    return PLUS_FAIL;
  }
  
  // verify every rigid body has an associated tool
  bool hasAssociatedTool = true;
  for (int i = 0; i < TT_RigidBodyCount(); i++)
  {
    std::string rb(TT_RigidBodyName(i));
    if (this->Internal->RBNameToTransformName.find(rb) == this->Internal->RBNameToTransformName.end())
    {
      // tool name found in Motive project with no associated tool in PLUS config
      hasAssociatedTool = false;
      LOG_ERROR("Missing data source element in PLUS config for rigid body \"" << TT_RigidBodyName(i) << "\"");
    }
  }
  if (!hasAssociatedTool)
  {
    LOG_ERROR("A tool exists in the Motive project or additional rigid body TRA files whose name is not listed as a tool Id in the PLUS config file. Please do the following:");
    LOG_ERROR("1) Ensure that the Motive project rigid body names of all tools you intend to track match the name of exactly one DataSource element in the PLUS config file.");
    LOG_ERROR("2) Ensure that all tools added via TRA files have tool names that match their corresponding tool Id in the PLUS config file.");
    LOG_ERROR("3) Remove any rigid bodies from the project file that you do not intend to track.");
    return PLUS_FAIL;
  }

  LOG_INFO("\n---------------------------------MOTIVE SETTINGS--------------------------------")
  // list connected cameras
  LOG_INFO("Connected cameras:")
  for (int i = 0; i < TT_CameraCount(); i++)
  {
    LOG_INFO(i << ": " << TT_CameraName(i));
  }
  // list project file
  LOG_INFO("\nUsing Motive project file located at:\n" << projectFilePath);
  // list rigid bodies
  LOG_INFO("\nTracked rigid bodies:");
  std::map<std::string, PlusTransformName>::iterator it;
  for (it = this->Internal->RBNameToTransformName.begin(); it != this->Internal->RBNameToTransformName.end(); it++)
  {
    LOG_INFO(it->first);
  }
  LOG_INFO("--------------------------------------------------------------------------------\n");

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalDisconnect()
{
  TT_Shutdown();

  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalStartRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalUpdate()
{
  if (TT_Update() == NPRESULT_SUCCESS)
  {
    
    this->FrameNumber++;
    const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();

    for (int i = 0; i < TT_RigidBodyCount(); i++)
    {
      std::string rbName = TT_RigidBodyName(i);

      if (TT_IsRigidBodyTracked(i))
      {
        // rigid body is tracked
        // variables to store rigid body data
        float   yaw, pitch, roll;
        float   x, y, z;
        float   qx, qy, qz, qw;
        vtkSmartPointer<vtkMatrix4x4> rigidBodyToTracker = vtkSmartPointer<vtkMatrix4x4>::New();
        rigidBodyToTracker->Identity();

        // poll location of rigid body and build transform
        TT_RigidBodyLocation(i, &x, &y, &z, &qx, &qy, &qz, &qw, &yaw, &pitch, &roll);
        double translation[3] = { x, y, z };
        double quaternion[4] = { qw, qx, qy, qz };
        double rotation[3][3] = { 0,0,0, 0,0,0, 0,0,0 };
        vtkMath::QuaternionToMatrix3x3(quaternion, rotation);
        for (int i = 0; i < 3; ++i)
        {
          for (int j = 0; j < 3; ++j)
          {
            rigidBodyToTracker->SetElement(i, j, rotation[i][j]);
          }
          rigidBodyToTracker->SetElement(i, 3, M_TO_MM*translation[i]);
        }

        PlusTransformName toolTransform = this->Internal->RBNameToTransformName.find(rbName)->second;
        ToolTimeStampedUpdate(toolTransform.GetTransformName(), rigidBodyToTracker, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
      }
      else
      {
        // rigid body out of frame
        vtkSmartPointer<vtkMatrix4x4> blankTransform = vtkSmartPointer<vtkMatrix4x4>::New();
        PlusTransformName toolTransform = this->Internal->RBNameToTransformName.find(rbName)->second;
        ToolTimeStampedUpdate(toolTransform.GetTransformName(), blankTransform, TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
      }
    }
    
  }

  return PLUS_SUCCESS;
}
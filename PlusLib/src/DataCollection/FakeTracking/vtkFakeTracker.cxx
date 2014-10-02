/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "TrackedFrame.h"

#include "vtkFakeTracker.h"
#include "vtkMatrix4x4.h"
#include "vtkMinimalStandardRandomSequence.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkFakeTracker);

//----------------------------------------------------------------------------
vtkFakeTracker::vtkFakeTracker()
: Frame(0)
, InternalTransform(vtkTransform::New())
, Mode(FakeTrackerMode_Undefined)
, TransformRepository(NULL)
, RandomSeed(0)
, Counter(-1)
, PhantomLandmarks(NULL)
, TrackedStylusTipFrames(NULL)
{
  vtkSmartPointer<vtkPoints> phantomLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetPhantomLandmarks(phantomLandmarks);

  vtkSmartPointer<vtkTrackedFrameList>trackedStylusTipFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  this->SetTrackedStylusTipFrames(trackedStylusTipFrames);

  this->RequirePortNameInDeviceSetConfiguration = true;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkFakeTracker::~vtkFakeTracker()
{
  this->InternalTransform->Delete();

  // Remove reference from the transform repository
  this->SetTransformRepository(NULL);

  this->SetPhantomLandmarks(NULL);
  this->SetTrackedStylusTipFrames(NULL);
}

//----------------------------------------------------------------------------
void vtkFakeTracker::SetMode(FakeTrackerMode mode)
{
  LOG_TRACE("vtkFakeTracker::SetMode(" << mode << ")"); 

  this->Mode = mode;
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalConnect()
{
  LOG_TRACE("vtkFakeTracker::InternalConnect"); 

  vtkPlusDataSource* tool = NULL; 
  switch (this->Mode)
  {
  case (FakeTrackerMode_Default):

    {
      //*************************************************************
      // Check Reference
      PlusTransformName sourceId("Reference", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Reference in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stationary");
    }
    {
      //*************************************************************
      // Check Stylus
      PlusTransformName sourceId("Stylus", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Rotate");
    }
    {
      //*************************************************************
      // Check Stlyus-2
      PlusTransformName sourceId("Stylus-2", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus-2 in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Rotate");
    }
    {
      //*************************************************************
      // Check Stlyus-3
      PlusTransformName sourceId("Stylus-3", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus-3 in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Spin");
    }
    break;

  case ( FakeTrackerMode_SmoothMove ): 
    {
      //*************************************************************
      // Check Probe
      PlusTransformName sourceId("Probe", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Probe in FakeTracker SmoothMove mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
        return PLUS_FAIL; 
      }
    }
    {
      //*************************************************************
      // Check Reference
      PlusTransformName sourceId("Reference", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Reference in FakeTracker SmoothMove mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
        return PLUS_FAIL; 
      }
    }
    {
      //*************************************************************
      // Check MissingTool
      PlusTransformName sourceId("MissingTool", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: MissingTool in FakeTracker SmoothMove mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      break; 
    }
  case (FakeTrackerMode_PivotCalibration):
    {
      //*************************************************************
      // Check Reference
      PlusTransformName sourceId("Reference", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Reference in FakeTracker PivotCalibration mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stationary");
    }
    {
      //*************************************************************
      // Check Stylus
      PlusTransformName sourceId("Stylus", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus in FakeTracker PivotCalibration mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stylus");
    }
    break;

  case (FakeTrackerMode_LandmarkDetection):
    {
      //*************************************************************
      // Check Reference
      PlusTransformName sourceId("Reference", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Reference in FakeTracker PivotCalibration mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stationary");
    }
    {
      //*************************************************************
      // Check Stylus
      PlusTransformName sourceId("Stylus", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus in FakeTracker PivotCalibration mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stylus");
    }
    break;
  case (FakeTrackerMode_RecordedPoseFrameList):
    {
      //*************************************************************
      // Check Reference
      PlusTransformName sourceId("Reference", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Reference in FakeTracker RecordedPoseFrameList mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stationary");
    }
    {
      //*************************************************************
      // Check Stylus
      PlusTransformName sourceId("Stylus", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus in FakeTracker RecordedPoseFrameList mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stylus");
    }
    break;
  case (FakeTrackerMode_RecordPhantomLandmarks):
    {
      //*************************************************************
      // Check Reference
      PlusTransformName sourceId("Reference", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Reference in FakeTracker RecordPhantomLandmarks mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }
      tool->SetCustomProperty("PartNumber", "Stationary");
    }
    {
      //*************************************************************
      // Check Stylus
      PlusTransformName sourceId("Stylus", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus in FakeTracker RecordPhantomLandmarks mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }

      tool->SetCustomProperty("PartNumber", "Stylus");
    }
    this->Counter = -1;

    break;
  case (FakeTrackerMode_ToolState):
    {
      //*************************************************************
      // Check Test
      PlusTransformName sourceId("Test", this->GetToolReferenceFrameName());
      if ( this->GetTool(sourceId.GetTransformName(), tool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Test in FakeTracker ToolState mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        return PLUS_FAIL; 
      }

      tool->SetCustomProperty("PartNumber", "Stationary");
    }
    this->Counter = 0;
    break;
  default:
    break;
  }


  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalDisconnect()
{
  LOG_TRACE("vtkFakeTracker::InternalDisconnect"); 

  return this->StopRecording(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::Probe()
{
  LOG_TRACE("vtkFakeTracker::Probe"); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalStartRecording()
{
  LOG_TRACE("vtkFakeTracker::InternalStartRecording"); 

  this->RandomSeed = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalStopRecording()
{
  LOG_TRACE("vtkFakeTracker::InternalStopRecording"); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalUpdate()
{
  //LOG_TRACE("vtkFakeTracker::InternalUpdate"); 

  if (!this->IsRecording())
  {
    LOG_TRACE("vtkFakeTracker::InternalUpdate is called while not tracking any more"); 
    return PLUS_SUCCESS;
  }

  if (this->Frame++ > 355559)
  {
    this->Frame = 0;
  }

  switch (this->Mode)
  {
  case (FakeTrackerMode_Default): // Spins the tools around different axis to fake movement
    {
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
      for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
      {
        ToolStatus toolStatus = TOOL_OK;

        int rotation = this->Frame/1000;

        if ( STRCASECMP(it->second->GetPortName(), "0") == 0 )
        {
          // This tool is stationary
          this->InternalTransform->Identity();
          this->InternalTransform->Translate(0, 150, 200);
        }
        else if ( STRCASECMP(it->second->GetPortName(), "1") == 0 )
        {
          // This tool rotates about a path on the Y axis
          this->InternalTransform->Identity();
          this->InternalTransform->RotateY(rotation);
          this->InternalTransform->Translate(0, 300, 0);
        }
        else if ( STRCASECMP(it->second->GetPortName(), "2") == 0 )
        {
          // This tool rotates about a path on the X axis
          this->InternalTransform->Identity();
          this->InternalTransform->RotateX(rotation);
          this->InternalTransform->Translate(0, 300, 200);
        }
        else if ( STRCASECMP(it->second->GetPortName(), "3") == 0 )
        {
          // This tool spins on the X axis
          this->InternalTransform->Identity();
          this->InternalTransform->Translate(100, 300, 0);
          this->InternalTransform->RotateX(rotation);
        }

        this->ToolTimeStampedUpdate(it->second->GetSourceId(),this->InternalTransform->GetMatrix(),toolStatus,this->Frame, unfilteredTimestamp);   
      }
    }
    break;

  case (FakeTrackerMode_SmoothMove): 
    {
      ToolStatus toolStatus = TOOL_OK;
      if ( this->Frame % 10 == 0 )
      {
        toolStatus = TOOL_MISSING; 
      }

      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
      double tx = (this->Frame % 100 ); // 0 - 99
      double ty = (this->Frame % 100 ) + 100; // 100 - 199 
      double tz = (this->Frame % 100 ) + 200; // 200 - 299  

      double ry = (this->Frame % 100 ) / 2.0; // 0 - 50 

      this->InternalTransform->Identity();
      this->InternalTransform->Translate(tx, ty, tz);
      this->InternalTransform->RotateY(ry); 
      // Probe transform
      {
        PlusTransformName name("Probe", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(),this->InternalTransform->GetMatrix(),toolStatus,this->Frame, unfilteredTimestamp);   
      }

      this->InternalTransform->Identity();
      this->InternalTransform->Translate(0, 0, 50); 
      // Reference transform 
      {
        PlusTransformName name("Reference", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(),this->InternalTransform->GetMatrix(),toolStatus,this->Frame, unfilteredTimestamp);   
      }
      {
        PlusTransformName name("MissingTool", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(),this->InternalTransform->GetMatrix(),TOOL_MISSING,this->Frame, unfilteredTimestamp);   
      }

    }
    break;

  case (FakeTrackerMode_PivotCalibration): // Moves around a stylus with the tip fixed to a position
    {
      vtkMinimalStandardRandomSequence* random = vtkMinimalStandardRandomSequence::New();
      random->SetSeed(RandomSeed++); // To get completely random numbers, timestamp should use instead of constant seed

      // Set flags
      ToolStatus toolStatus = TOOL_OK;

      // Once in every 50 request, the tracker provides 'out of view' flag - FOR TEST PURPOSES
      /*
      random->Next();
      double outOfView = random->GetValue();
      if (outOfView < 0.02) {
      toolStatus = TOOL_OUT_OF_VIEW;
      }
      */

      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for reference (tool 0)
      vtkSmartPointer<vtkTransform> referenceToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      referenceToTrackerTransform->Identity();
      referenceToTrackerTransform->Translate(300, 400, 700);
      referenceToTrackerTransform->RotateZ(90);

      {
        PlusTransformName name("Reference", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), referenceToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);
      }
      // create random positions along a sphere (with built-in error)
      double exactRadius = 210.0;
      double deltaTheta = 60.0;
      double deltaPhi = 60.0;
      double variance = 1.0;

      random->Next();
      double theta = random->GetRangeValue(-deltaTheta, deltaTheta);

      random->Next();
      double phi = random->GetRangeValue(-deltaPhi, deltaPhi);

      random->Next();
      double radius = random->GetRangeValue(exactRadius-variance, exactRadius+variance);

      vtkSmartPointer<vtkTransform> stylusToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
      stylusToReferenceTransform->Identity();
      stylusToReferenceTransform->Translate(205.0, 305.0, 55.0); // Some distance from the reference
      stylusToReferenceTransform->RotateY(phi);
      stylusToReferenceTransform->RotateZ(theta);
      stylusToReferenceTransform->Translate(-radius, 0.0, 0.0);

      vtkSmartPointer<vtkTransform> stylusToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      stylusToTrackerTransform->Identity();
      stylusToTrackerTransform->Concatenate(referenceToTrackerTransform);
      stylusToTrackerTransform->Concatenate(stylusToReferenceTransform);

      random->Delete();
      {
        PlusTransformName name("Stylus", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);       
      }
      break;
    }
  case (FakeTrackerMode_LandmarkDetection): // Linear translation and small rotation then moves around a stylus with the tip fixed to a position
    {
      vtkMinimalStandardRandomSequence* random = vtkMinimalStandardRandomSequence::New();
      random->SetSeed(RandomSeed++); // To get completely random numbers, timestamp should use instead of constant seed

      // Set flags
      ToolStatus toolStatus = TOOL_OK;
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for reference (tool 0)
      vtkSmartPointer<vtkTransform> referenceToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      referenceToTrackerTransform->Identity();
      referenceToTrackerTransform->Translate(300, 400, 700);
      referenceToTrackerTransform->RotateZ(90);
      {
        PlusTransformName name("Reference", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), referenceToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);
      }

      double exactRadius = 210.0;
      double variance = 1.0;
      double radius = random->GetRangeValue(exactRadius-variance, exactRadius+variance);

      const double LINEAR_CYCLE=80.0;
      const double CIRCULAR_CYCLE=3*360.0;
      const double OFFSET_Y=100.0;
      const double OFFSET_Z=200.0;

      int currentState = 0;
      currentState=this->Frame % (2*(int)LINEAR_CYCLE+(int)CIRCULAR_CYCLE);

      if(currentState<LINEAR_CYCLE)
      {
        double tx = (currentState); // [0, LINEAR_CYCLE)
        double ty = (currentState) + OFFSET_Y; // [OFFSET, LINEAR_CYCLE+OFFSET) Offset from the reference
        double tz = (currentState) + OFFSET_Z; //
        double ry = (LINEAR_CYCLE/2.0)-(currentState) / 2.0; // [LINEAR_CYCLE/2, 0)

        vtkSmartPointer<vtkTransform> stylusToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
        stylusToReferenceTransform->Identity();
        stylusToReferenceTransform->Translate(tx-radius, ty, tz);
        stylusToReferenceTransform->RotateY(ry);
        //stylusToReferenceTransform->RotateZ(theta);
        //stylusToReferenceTransform->Translate(-radius, 0.0, 0.0);

        vtkSmartPointer<vtkTransform> stylusToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
        stylusToTrackerTransform->Identity();
        stylusToTrackerTransform->Concatenate(referenceToTrackerTransform);
        stylusToTrackerTransform->Concatenate(stylusToReferenceTransform);

        {
          PlusTransformName name("Stylus", this->GetToolReferenceFrameName());
          this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp); 
        }
      }
      else if(currentState >= LINEAR_CYCLE && currentState < (LINEAR_CYCLE+CIRCULAR_CYCLE))
      {
        // create a progresive changing position along a sphere (with built-in error)
        //double deltaTheta = 60.0; double deltaPhi = 60.0;
        random->Next();
        double theta = currentState- LINEAR_CYCLE + random->GetRangeValue(-variance, variance);// random->GetRangeValue(-deltaTheta, deltaTheta);

        random->Next();
        double phi = currentState- LINEAR_CYCLE +random->GetRangeValue(-variance, variance);//random->GetRangeValue(-deltaPhi, deltaPhi);

        random->Next();
        vtkSmartPointer<vtkTransform> stylusToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
        stylusToReferenceTransform->Identity();
        stylusToReferenceTransform->Translate(LINEAR_CYCLE, LINEAR_CYCLE+OFFSET_Y, LINEAR_CYCLE+OFFSET_Z); // Offset from the reference
        stylusToReferenceTransform->RotateY(phi);
        stylusToReferenceTransform->RotateZ(theta);
        stylusToReferenceTransform->Translate(-radius, 0.0, 0.0);

        vtkSmartPointer<vtkTransform> stylusToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
        stylusToTrackerTransform->Identity();
        stylusToTrackerTransform->Concatenate(referenceToTrackerTransform);
        stylusToTrackerTransform->Concatenate(stylusToReferenceTransform);

        random->Delete();
        {
          PlusTransformName name("Stylus", this->GetToolReferenceFrameName());
          this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);
        }
      }
      else
      {
        double tx = 2*LINEAR_CYCLE+CIRCULAR_CYCLE-currentState;//100-(currentState-460); // 0 - 99
        double ty = OFFSET_Y+2*LINEAR_CYCLE+CIRCULAR_CYCLE-currentState;//200-(currentState-460); // 100 - 199 
        double tz = OFFSET_Z+2*LINEAR_CYCLE+CIRCULAR_CYCLE-currentState;//300-(currentState-460); // 200 - 299  
        double ry = (currentState-(LINEAR_CYCLE+CIRCULAR_CYCLE)) / 2.0; // [0, LINEAR_CYCLE/2)

        vtkSmartPointer<vtkTransform> stylusToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
        stylusToReferenceTransform->Identity();
        stylusToReferenceTransform->Translate(tx-radius, ty, tz); // 
        stylusToReferenceTransform->RotateY(ry);
        //stylusToReferenceTransform->RotateZ(theta);
        //stylusToReferenceTransform->Translate(-radius, 0.0, 0.0);

        vtkSmartPointer<vtkTransform> stylusToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
        stylusToTrackerTransform->Identity();
        stylusToTrackerTransform->Concatenate(referenceToTrackerTransform);
        stylusToTrackerTransform->Concatenate(stylusToReferenceTransform);

        {
          PlusTransformName name("Stylus", this->GetToolReferenceFrameName());
          this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);       
        }
      }
      break;
    }
  case (  FakeTrackerMode_RecordedPoseFrameList): // Touches recorded poses
    {
      ToolStatus toolStatus = TOOL_OK;
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      int currentState = 0;
      currentState=this->Frame % this->TrackedStylusTipFrames->GetNumberOfTrackedFrames();
      // Translate to actual landmark point
      if (currentState >= 0 &&currentState <this->TrackedStylusTipFrames->GetNumberOfTrackedFrames())
      {
        PlusTransformName name( "Reference",this->GetToolReferenceFrameName());
        // create stationary position for reference (tool 0)
        vtkSmartPointer<vtkMatrix4x4> referenceToTrackerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        this->TrackedStylusTipFrames->GetTrackedFrame(currentState)->GetCustomFrameTransform(name,referenceToTrackerMatrix);
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), referenceToTrackerMatrix, toolStatus, this->Frame, unfilteredTimestamp);

        PlusTransformName nameStylus("Stylus", this->GetToolReferenceFrameName());
        vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        this->TrackedStylusTipFrames->GetTrackedFrame(currentState)->GetCustomFrameTransform(nameStylus,stylusToReferenceMatrix);
        this->ToolTimeStampedUpdate(nameStylus.GetTransformName().c_str(), stylusToReferenceMatrix, toolStatus, this->Frame, unfilteredTimestamp);   
      }
    }
    break;
  case (FakeTrackerMode_RecordPhantomLandmarks): // Touches some positions with 1 sec difference
    {
      ToolStatus toolStatus = TOOL_OK;
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for phantom reference (tool 0)
      vtkSmartPointer<vtkTransform> referenceToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      referenceToTrackerTransform->Identity();

      referenceToTrackerTransform->Translate(300, 400, 700);
      referenceToTrackerTransform->RotateZ(90);

      {
        PlusTransformName name("Reference", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), referenceToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);
      }
      // touch landmark points
      vtkSmartPointer<vtkTransform> landmarkToPhantomTransform = vtkSmartPointer<vtkTransform>::New();
      landmarkToPhantomTransform->Identity();

      // Translate to actual landmark point
      if (this->Counter >= 0 && this->Counter < this->PhantomLandmarks->GetNumberOfPoints())
      {
        double currentLandmarkPosition[3];
        this->PhantomLandmarks->GetPoint(this->Counter, currentLandmarkPosition);
        landmarkToPhantomTransform->Translate(currentLandmarkPosition[0], currentLandmarkPosition[1], currentLandmarkPosition[2]);
      }

      // Get stylus calibration inverse transform
      vtkSmartPointer<vtkTransform> stylusToStylusTipTransform = vtkSmartPointer<vtkTransform>::New();
      stylusToStylusTipTransform->Identity();
      if (this->TransformRepository)
      {
        vtkSmartPointer<vtkMatrix4x4> stylusToStylusTipTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        PlusTransformName stylusToStylusTipTransformName("Stylus", "StylusTip");
        bool valid(false); 
        if ( this->TransformRepository->GetTransform(stylusToStylusTipTransformName, stylusToStylusTipTransformMatrix, &valid) == PLUS_SUCCESS )
        {
          stylusToStylusTipTransform->Concatenate(stylusToStylusTipTransformMatrix);
        }
      }

      // Rotate to make motion visible even if the camera is reset every time
      if (this->Counter < 7)
      {
        landmarkToPhantomTransform->RotateY(this->Counter * 5.0);
      }
      else
      {
        landmarkToPhantomTransform->RotateY(180.0);
      }
      landmarkToPhantomTransform->RotateZ(this->Counter * 5.0);
      vtkSmartPointer<vtkTransform> phantomToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
      phantomToReferenceTransform->Identity();
      phantomToReferenceTransform->Translate(-75, -50, -150);

      vtkSmartPointer<vtkTransform> stylusToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      stylusToTrackerTransform->Concatenate(referenceToTrackerTransform);
      stylusToTrackerTransform->Concatenate(phantomToReferenceTransform);
      stylusToTrackerTransform->Concatenate(landmarkToPhantomTransform);
      stylusToTrackerTransform->Concatenate(stylusToStylusTipTransform); // Un-calibrate it

      {
        PlusTransformName name("Stylus", this->GetToolReferenceFrameName());
        this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);   
      }
    }
    break;
  case (FakeTrackerMode_ToolState): // Changes the state of the tool from time to time
    {
      // Set flags
      ToolStatus toolStatus = TOOL_OK;

      const int state = (this->Counter / 100) % 3;
      switch (state)
      {
      case 1:
        toolStatus = TOOL_OUT_OF_VIEW;
        break;
      case 2:
        toolStatus = TOOL_MISSING;
        break;
      default:
        break;
      }
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for phantom reference (tool 0)
      vtkSmartPointer<vtkTransform> identityTransform = vtkSmartPointer<vtkTransform>::New();
      identityTransform->Identity();

      this->ToolTimeStampedUpdate("Test", identityTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);

      this->Counter++;
    }
    break;
  default:
    break;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkFakeTracker::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  if ( !this->Recording )
  {
    // Read mode

    const char* mode = deviceConfig->GetAttribute("Mode"); 
    if ( mode != NULL ) 
    {
      if (STRCASECMP(mode, "Default") == 0)
      {
        this->SetMode(FakeTrackerMode_Default); 
      }
      else if (STRCASECMP(mode, "SmoothMove") == 0)
      {
        this->SetMode(FakeTrackerMode_SmoothMove); 
      }
      else if (STRCASECMP(mode, "PivotCalibration") == 0)
      {
        this->SetMode(FakeTrackerMode_PivotCalibration); 
      }
      else if (STRCASECMP(mode, "LandmarkDetection") == 0)
      {
        this->SetMode(FakeTrackerMode_LandmarkDetection); 
      }
      else if (STRCASECMP(mode, "RecordedPoseFrameList") == 0)
      {
        this->SetMode(FakeTrackerMode_RecordedPoseFrameList); 
      }
      else if (STRCASECMP(mode, "RecordPhantomLandmarks") == 0)
      {
        this->SetMode(FakeTrackerMode_RecordPhantomLandmarks); 
      }
      else if (STRCASECMP(mode, "ToolState") == 0)
      {
        this->SetMode(FakeTrackerMode_ToolState); 
      }
      else
      {
        this->SetMode(FakeTrackerMode_Undefined);
      }
    }

    if(this->Mode==FakeTrackerMode_RecordedPoseFrameList)
    {
      const char* trackedPoseListFileName = deviceConfig->GetAttribute("TrackedPoseListFileName"); 

      // Read stylus tracker data

      if( trackedPoseListFileName != NULL )
      {
        //this->TrackedStylusTipFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
        this->TrackedStylusTipFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK);
        LOG_INFO("Read stylus tracker data from " << this->TrackedStylusTipFrames);
        if ( this->TrackedStylusTipFrames->ReadFromSequenceMetafile(trackedPoseListFileName) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to read stylus data from sequence metafile: " << trackedPoseListFileName << ". Exiting...");
          exit(EXIT_FAILURE);
        }
        this->TrackedStylusTipFrames->Register(NULL);
      }
      else
      {
        LOG_ERROR("Empty file name to read sequence metafile: " );
      }
    }
    // Read landmarks for RecordPhantomLandmarks mode
    bool phantomLandmarksFound = true;
    vtkXMLDataElement* landmarks = NULL;
    vtkXMLDataElement* phantomDefinition = rootConfigElement->FindNestedElementWithName("PhantomDefinition");
    if (phantomDefinition == NULL)
    {
      phantomLandmarksFound = false;
    }
    else
    {
      vtkXMLDataElement* geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
      if (geometry == NULL)
      {
        phantomLandmarksFound = false;
      }
      else
      {
        landmarks = geometry->FindNestedElementWithName("Landmarks"); 
        if (landmarks == NULL)
        {
          phantomLandmarksFound = false;
        }
      }
    }

    if (!phantomLandmarksFound)
    {
      if (this->Mode == FakeTrackerMode_RecordPhantomLandmarks)
      {
        LOG_ERROR("Phantom landmarks cannot be found in the configuration XML tree - FakeTracker in RecordPhantomLandmarks mode cannot be started!");
        return PLUS_FAIL;
      }
      else
      {
        LOG_DEBUG("Phantom landmarks cannot be found in the configuration XML tree - FakeTracker in RecordPhantomLandmarks mode cannot be started.");
      }
    }
    else
    {
      this->PhantomLandmarks->Reset();
      int numberOfLandmarks = landmarks->GetNumberOfNestedElements();
      for (int i=0; i<numberOfLandmarks; ++i)
      {
        vtkXMLDataElement* landmark = landmarks->GetNestedElement(i);

        if ((landmark == NULL) || (STRCASECMP("Landmark", landmark->GetName())))
        {
          LOG_WARNING("Invalid landmark definition found!");
          continue;
        }

        double landmarkPosition[3];
        if (! landmark->GetVectorAttribute("Position", 3, landmarkPosition))
        {
          LOG_WARNING("Invalid landmark position!");
          continue;
        }

        this->PhantomLandmarks->InsertPoint(i, landmarkPosition);
      }
    }
  }

  return PLUS_SUCCESS;
}

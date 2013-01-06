/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkFakeTracker.h"
#include "vtkMatrix4x4.h"
#include "vtkMinimalStandardRandomSequence.h"
#include "vtkObjectFactory.h"
#include "vtkPlusStreamTool.h"
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
{
  vtkSmartPointer<vtkPoints> phantomLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetPhantomLandmarks(phantomLandmarks);

  this->RequireDeviceImageOrientationInDeviceSetConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = false;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireToolAveragedItemsForFilteringInDeviceSetConfiguration = true;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;
}

//----------------------------------------------------------------------------
vtkFakeTracker::~vtkFakeTracker()
{
  this->InternalTransform->Delete();

  // Remove reference from the transform repository
  this->SetTransformRepository(NULL);

  this->SetPhantomLandmarks(NULL);
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

  vtkPlusStreamTool* tool = NULL; 
  switch (this->Mode)
  {
  case (FakeTrackerMode_Default):

    //*************************************************************
    // Check Reference
    if ( this->GetTool("Reference", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Reference in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
      return PLUS_FAIL; 
    }
    tool->SetToolRevision("1.3");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Stationary");
    tool->SetToolSerialNumber("A34643");
    

    //*************************************************************
    // Check Stylus
    if ( this->GetTool("Stylus", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Stylus in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
      return PLUS_FAIL; 
    }
    tool->SetToolRevision("1.1");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Rotate");
    tool->SetToolSerialNumber("B3464C");
    

    //*************************************************************
    // Check Stlyus-2
    if ( this->GetTool("Stylus-2", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Stylus-2 in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
      return PLUS_FAIL; 
    }
    tool->SetToolRevision("1.1");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Rotate");
    tool->SetToolSerialNumber("Q45P5");
    

    //*************************************************************
    // Check Stlyus-3
    if ( this->GetTool("Stylus-3", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Stylus-3 in FakeTracker Default mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
      return PLUS_FAIL; 
    }
    tool->SetToolRevision("2.0");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Spin");
    tool->SetToolSerialNumber("Q34653");
    
    break;

  case ( FakeTrackerMode_SmoothMove ): 

    //*************************************************************
    // Check Probe
    if ( this->GetTool("Probe", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Probe in FakeTracker SmoothMove mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
      return PLUS_FAIL; 
    }

    //*************************************************************
    // Check Reference
    if ( this->GetTool("Reference", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Reference in FakeTracker SmoothMove mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()); 
      return PLUS_FAIL; 
    }

    //*************************************************************
    // Check MissingTool
    if ( this->GetTool("MissingTool", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: MissingTool in FakeTracker SmoothMove mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }
    break; 

  case (FakeTrackerMode_PivotCalibration):

    //*************************************************************
    // Check Reference
    if ( this->GetTool("Reference", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Reference in FakeTracker PivotCalibration mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }

    tool->SetToolRevision("1.3");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Stationary");
    tool->SetToolSerialNumber("A11111");
    

    //*************************************************************
    // Check Stylus
    if ( this->GetTool("Stylus", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Stylus in FakeTracker PivotCalibration mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }

    tool->SetToolRevision("1.1");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Stylus");
    tool->SetToolSerialNumber("B22222");
    
    break;

  case (FakeTrackerMode_RecordPhantomLandmarks):
    //*************************************************************
    // Check Reference
    if ( this->GetTool("Reference", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Reference in FakeTracker RecordPhantomLandmarks mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }
    
    tool->SetToolRevision("1.3");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Stationary");
    tool->SetToolSerialNumber("A11111");

    //*************************************************************
    // Check Stylus
    if ( this->GetTool("Stylus", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Stylus in FakeTracker RecordPhantomLandmarks mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }

    tool->SetToolRevision("1.1");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Stylus");
    tool->SetToolSerialNumber("B22222");
    

    this->Counter = -1;

    break;
  case (FakeTrackerMode_ToolState):

    //*************************************************************
    // Check Test
    if ( this->GetTool("Test", tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool: Test in FakeTracker ToolState mode, please add to config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }

    tool->SetToolRevision("1.3");
    tool->SetToolManufacturer("ACME Inc.");
    tool->SetToolPartNumber("Stationary");
    tool->SetToolSerialNumber("A11111");
    

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
      for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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

        this->ToolTimeStampedUpdate(it->second->GetToolName(),this->InternalTransform->GetMatrix(),toolStatus,this->Frame, unfilteredTimestamp);   
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
      this->ToolTimeStampedUpdate("Probe",this->InternalTransform->GetMatrix(),toolStatus,this->Frame, unfilteredTimestamp);   

      this->InternalTransform->Identity();
      this->InternalTransform->Translate(0, 0, 50); 
      // Reference transform 
      this->ToolTimeStampedUpdate("Reference",this->InternalTransform->GetMatrix(),toolStatus,this->Frame, unfilteredTimestamp);   
      this->ToolTimeStampedUpdate("MissingTool",this->InternalTransform->GetMatrix(),TOOL_MISSING,this->Frame, unfilteredTimestamp);   

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

      this->ToolTimeStampedUpdate("Reference", referenceToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);
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
      this->ToolTimeStampedUpdate("Stylus", stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);       }
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

      this->ToolTimeStampedUpdate("Reference", referenceToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);
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

      this->ToolTimeStampedUpdate("Stylus", stylusToTrackerTransform->GetMatrix(), toolStatus, this->Frame, unfilteredTimestamp);   
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
PlusStatus vtkFakeTracker::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkFakeTracker::ReadConfiguration");

  if ( config == NULL ) 
  {
    LOG_WARNING("Unable to find FakeTracker XML data element");
    return PLUS_FAIL; 
  }

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  if ( !this->Recording )
  {
    // Read mode
    vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(config);
    if (trackerConfig == NULL) 
    {
      LOG_ERROR("Cannot find Tracker element in XML tree!");
      return PLUS_FAIL;
    }

    const char* mode = trackerConfig->GetAttribute("Mode"); 
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

    // Read landmarks for RecordPhantomLandmarks mode
    bool phantomLandmarksFound = true;
    vtkXMLDataElement* landmarks = NULL;
    vtkXMLDataElement* phantomDefinition = config->FindNestedElementWithName("PhantomDefinition");
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

  return Superclass::ReadConfiguration(config); 
}

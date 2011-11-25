/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkFakeTracker.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTrackerTool.h"
#include "vtkMinimalStandardRandomSequence.h"

//----------------------------------------------------------------------------
vtkFakeTracker* vtkFakeTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFakeTracker");
  if(ret)
  {
    return (vtkFakeTracker*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkFakeTracker;
}

//----------------------------------------------------------------------------
vtkFakeTracker::vtkFakeTracker() 
{
  this->Frame = 0;
  this->InternalTransform = vtkTransform::New();
  this->Mode = FakeTrackerMode_Undefined;
  this->Counter = -1;
}

//----------------------------------------------------------------------------
vtkFakeTracker::~vtkFakeTracker()
{
  this->InternalTransform->Delete();
}

//----------------------------------------------------------------------------
void vtkFakeTracker::SetMode(FakeTrackerMode mode)
{
  LOG_TRACE("vtkFakeTracker::SetMode(" << mode << ")"); 

  this->Mode = mode;
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::Connect()
{
  LOG_TRACE("vtkFakeTracker::Connect"); 

  vtkTrackerTool* tool = NULL; 
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
    // Check Stlus
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
    // Check Stlus-2
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
    // Check Stlus-3
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
PlusStatus vtkFakeTracker::Disconnect()
{
  LOG_TRACE("vtkFakeTracker::Disconnect"); 

  return this->StopTracking(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::Probe()
{
  LOG_TRACE("vtkFakeTracker::Probe"); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalStartTracking()
{
  LOG_TRACE("vtkFakeTracker::InternalStartTracking"); 

  this->RandomSeed = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalStopTracking()
{
  LOG_TRACE("vtkFakeTracker::InternalStopTracking"); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::InternalUpdate()
{
  //LOG_TRACE("vtkFakeTracker::InternalUpdate"); 

  if (!this->IsTracking())
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
      for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
      {
        TrackerStatus trackerStatus = TR_OK;

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

        this->ToolTimeStampedUpdate(it->second->GetToolName(),this->InternalTransform->GetMatrix(),trackerStatus,this->Frame, unfilteredTimestamp);   
      }
    }
    break;

  case (FakeTrackerMode_SmoothMove): 
    {
      TrackerStatus trackerStatus = TR_OK;
      if ( this->Frame % 10 == 0 )
      {
        trackerStatus = TR_MISSING; 
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
      this->ToolTimeStampedUpdate("Probe",this->InternalTransform->GetMatrix(),trackerStatus,this->Frame, unfilteredTimestamp);   

      this->InternalTransform->Identity();
      this->InternalTransform->Translate(0, 0, 50); 
      // Reference transform 
      this->ToolTimeStampedUpdate("Reference",this->InternalTransform->GetMatrix(),trackerStatus,this->Frame, unfilteredTimestamp);   
      this->ToolTimeStampedUpdate("MissingTool",this->InternalTransform->GetMatrix(),TR_MISSING,this->Frame, unfilteredTimestamp);   

    }
    break;

  case (FakeTrackerMode_PivotCalibration): // Moves around a stylus with the tip fixed to a position
    {
      vtkMinimalStandardRandomSequence* random = vtkMinimalStandardRandomSequence::New();
      random->SetSeed(RandomSeed++); // To get completely random numbers, timestamp should use instead of constant seed

      // Set flags
      TrackerStatus trackerStatus = TR_OK;

      // Once in every 50 request, the tracker provides 'out of view' flag - FOR TEST PURPOSES
      /*
      random->Next();
      double outOfView = random->GetValue();
      if (outOfView < 0.02) {
      trackerStatus = TR_OUT_OF_VIEW;
      }
      */

      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for reference (tool 0)
      vtkSmartPointer<vtkTransform> trackerToReferenceToolTransform = vtkSmartPointer<vtkTransform>::New();
      trackerToReferenceToolTransform->Identity();
      trackerToReferenceToolTransform->Translate(300, 400, 700);
      trackerToReferenceToolTransform->RotateZ(90);

      this->ToolTimeStampedUpdate("Reference", trackerToReferenceToolTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);   

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

      vtkSmartPointer<vtkTransform> trackerToStylusTipTransform = vtkSmartPointer<vtkTransform>::New();
      trackerToStylusTipTransform->Identity();
      trackerToStylusTipTransform->Translate(705.0, 505.0, 2605.0); // Some distance from the tracker (5 because of the error, the actual number is sometimes below, sometimes over the hundred and it is hard to read)
      trackerToStylusTipTransform->RotateY(phi);
      trackerToStylusTipTransform->RotateZ(theta);
      trackerToStylusTipTransform->Translate(-radius, 0.0, 0.0);

      vtkSmartPointer<vtkTransform> referenceToolToStylusTipTransform = vtkSmartPointer<vtkTransform>::New();
      referenceToolToStylusTipTransform->Identity();
      trackerToReferenceToolTransform->Inverse();
      referenceToolToStylusTipTransform->Concatenate(trackerToReferenceToolTransform);
      referenceToolToStylusTipTransform->Concatenate(trackerToStylusTipTransform);

      random->Delete();
      this->ToolTimeStampedUpdate("Stylus", referenceToolToStylusTipTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);   
    }
    break;

  case (FakeTrackerMode_RecordPhantomLandmarks): // Touches some positions with 1 sec difference
    {
      TrackerStatus trackerStatus = TR_OK;
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for phantom reference (tool 0)
      vtkSmartPointer<vtkTransform> phantomReferenceToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      phantomReferenceToTrackerTransform->Identity();

      phantomReferenceToTrackerTransform->Translate(300, 400, 700);
      phantomReferenceToTrackerTransform->RotateZ(90);

      this->ToolTimeStampedUpdate("Reference", phantomReferenceToTrackerTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);

      // touch landmark points
      vtkSmartPointer<vtkTransform> phantomToLandmarkTransform = vtkSmartPointer<vtkTransform>::New();
      phantomToLandmarkTransform->Identity();

      // Translate to actual landmark point
      switch (this->Counter) { // TODO Read from xml?
  case 0:
    phantomToLandmarkTransform->Translate(95.0, 5.0, 15.0);
    break;
  case 1:
    phantomToLandmarkTransform->Translate(95.0, 40.0, 15.0);
    break;
  case 2:
    phantomToLandmarkTransform->Translate(95.0, 40.0, 0.0);
    break;
  case 3:
    phantomToLandmarkTransform->Translate(95.0, 0.0, 0.0);
    break;
  case 4:
    phantomToLandmarkTransform->Translate(-25.0, 40.0, 15.0);
    break;
  case 5:
    phantomToLandmarkTransform->Translate(-25.0, 0.0, 10.0);
    break;
  case 6:
    phantomToLandmarkTransform->Translate(-25.0, 0.0, 0.0);
    break;
  case 7:
    phantomToLandmarkTransform->Translate(-25.0, 40.0, 0.0);
    break;
      }

      // Rotate to make motion visible even if the camera is reset every time
      if (this->Counter < 7) {
        phantomToLandmarkTransform->RotateY(this->Counter * 5.0);
      } else {
        phantomToLandmarkTransform->RotateY(180.0);
      }
      phantomToLandmarkTransform->RotateZ(this->Counter * 5.0);

      vtkSmartPointer<vtkTransform> phantomReferenceToPhantomTransform = vtkSmartPointer<vtkTransform>::New();
      phantomReferenceToPhantomTransform->Identity();
      phantomReferenceToPhantomTransform->Translate(-75, -50, -150);

      // Apply inverse calibration so that when applying calibration during acquisition, the position is right
      vtkSmartPointer<vtkTransform> stylustipToStylusTransform = vtkSmartPointer<vtkTransform>::New();
      stylustipToStylusTransform->Identity();
      vtkTrackerTool * stylus = NULL; 
      if ( this->GetTool("Stylus", stylus) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tool: Stylus"); 
        return PLUS_FAIL; 
      }

      stylustipToStylusTransform->SetMatrix(stylus->GetCalibrationMatrix());
      stylustipToStylusTransform->Inverse();
      stylustipToStylusTransform->Modified();

      vtkSmartPointer<vtkTransform> phantomReferenceToLandmarkTransform = vtkSmartPointer<vtkTransform>::New();
      phantomReferenceToLandmarkTransform->Identity();

      phantomReferenceToLandmarkTransform->Concatenate(phantomReferenceToPhantomTransform);
      phantomReferenceToLandmarkTransform->Concatenate(phantomToLandmarkTransform);
      phantomReferenceToLandmarkTransform->Concatenate(stylustipToStylusTransform);

      this->ToolTimeStampedUpdate("Stylus", phantomReferenceToLandmarkTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);   
    }
    break;

  case (FakeTrackerMode_ToolState): // Changes the state of the tool from time to time
    {
      // Set flags
      TrackerStatus trackerStatus = TR_OK;

      const int state = (this->Counter / 100) % 3;
      switch (state)
      {
      case 1:
        trackerStatus = TR_OUT_OF_VIEW;
        break;
      case 2:
        trackerStatus = TR_MISSING;
        break;
      default:
        break;
      }
      const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

      // create stationary position for phantom reference (tool 0)
      vtkSmartPointer<vtkTransform> identityTransform = vtkSmartPointer<vtkTransform>::New();
      identityTransform->Identity();

      this->ToolTimeStampedUpdate("Test", identityTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);

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
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  if ( !this->Tracking )
  {
    const char* mode = trackerConfig->GetAttribute("Mode"); 
    if ( mode != NULL ) 
    {
      if (STRCASECMP(mode, "Default") == 0) {
        this->SetMode(FakeTrackerMode_Default); 
      } else if (STRCASECMP(mode, "SmoothMove") == 0) {
        this->SetMode(FakeTrackerMode_SmoothMove); 
      } else if (STRCASECMP(mode, "PivotCalibration") == 0) {
        this->SetMode(FakeTrackerMode_PivotCalibration); 
      } else if (STRCASECMP(mode, "RecordPhantomLandmarks") == 0) {
        this->SetMode(FakeTrackerMode_RecordPhantomLandmarks); 
      } else if (STRCASECMP(mode, "ToolState") == 0) {
        this->SetMode(FakeTrackerMode_ToolState); 
      } else {
        this->SetMode(FakeTrackerMode_Undefined);
      }
    }
  }

  return Superclass::ReadConfiguration(config); 
}

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

  switch (this->Mode)
  {
  case (FakeTrackerMode_Default):
    this->SetNumberOfTools(4);

    this->Tools[0]->SetToolType(TRACKER_TOOL_REFERENCE);
    this->Tools[0]->SetToolRevision("1.3");
    this->Tools[0]->SetToolManufacturer("ACME Inc.");
    this->Tools[0]->SetToolPartNumber("Stationary");
    this->Tools[0]->SetToolSerialNumber("A34643");

    this->Tools[1]->SetToolType(TRACKER_TOOL_STYLUS);
    this->Tools[1]->SetToolRevision("1.1");
    this->Tools[1]->SetToolManufacturer("ACME Inc.");
    this->Tools[1]->SetToolPartNumber("Rotate");
    this->Tools[1]->SetToolSerialNumber("B3464C");

    this->Tools[2]->SetToolType(TRACKER_TOOL_STYLUS);
    this->Tools[2]->SetToolRevision("1.1");
    this->Tools[2]->SetToolManufacturer("ACME Inc.");
    this->Tools[2]->SetToolPartNumber("Rotate");
    this->Tools[2]->SetToolSerialNumber("Q45P5");

    this->Tools[3]->SetToolType(TRACKER_TOOL_STYLUS);
    this->Tools[3]->SetToolRevision("2.0");
    this->Tools[3]->SetToolManufacturer("ACME Inc.");
    this->Tools[3]->SetToolPartNumber("Spin");
    this->Tools[3]->SetToolSerialNumber("Q34653");

    break;

  case ( FakeTrackerMode_SmoothMove ): 
    this->SetNumberOfTools(3); 
    this->Tools[0]->SetToolName("Probe"); 
    this->Tools[0]->SetToolType( TRACKER_TOOL_PROBE ); 
    this->Tools[0]->EnabledOn(); 
    this->Tools[1]->SetToolName("Reference"); 
    this->Tools[1]->SetToolType( TRACKER_TOOL_GENERAL ); 
    this->Tools[1]->EnabledOn(); 
    this->Tools[2]->SetToolName("DisabledTool"); 
    this->Tools[2]->SetToolType( TRACKER_TOOL_GENERAL ); 
    this->Tools[2]->EnabledOff();
    break; 

  case (FakeTrackerMode_PivotCalibration):
    this->SetNumberOfTools(2);

    this->Tools[0]->SetToolType(TRACKER_TOOL_REFERENCE);
    this->Tools[0]->SetToolRevision("1.3");
    this->Tools[0]->SetToolManufacturer("ACME Inc.");
    this->Tools[0]->SetToolPartNumber("Stationary");
    this->Tools[0]->SetToolSerialNumber("A11111");
    this->Tools[0]->SetToolName("Reference");

    this->Tools[1]->SetToolType(TRACKER_TOOL_STYLUS);
    this->Tools[1]->SetToolRevision("1.1");
    this->Tools[1]->SetToolManufacturer("ACME Inc.");
    this->Tools[1]->SetToolPartNumber("Stylus");
    this->Tools[1]->SetToolSerialNumber("B22222");
    this->Tools[1]->SetToolName("Stylus");

    break;
  case (FakeTrackerMode_RecordPhantomLandmarks):
    this->SetNumberOfTools(2);

    this->Tools[0]->SetToolType(TRACKER_TOOL_REFERENCE);
    this->Tools[0]->SetToolRevision("1.3");
    this->Tools[0]->SetToolManufacturer("ACME Inc.");
    this->Tools[0]->SetToolPartNumber("Stationary");
    this->Tools[0]->SetToolSerialNumber("A11111");
    this->Tools[0]->SetToolName("Reference");

    this->Tools[1]->SetToolType(TRACKER_TOOL_STYLUS);
    this->Tools[1]->SetToolRevision("1.1");
    this->Tools[1]->SetToolManufacturer("ACME Inc.");
    this->Tools[1]->SetToolPartNumber("Stylus");
    this->Tools[1]->SetToolSerialNumber("B22222");
    this->Tools[1]->SetToolName("Stylus");

    this->Counter = -1;

    break;
  case (FakeTrackerMode_ToolState):
    this->SetNumberOfTools(1);

    this->Tools[0]->SetToolType(TRACKER_TOOL_PROBE);
    this->Tools[0]->SetToolRevision("1.3");
    this->Tools[0]->SetToolManufacturer("ACME Inc.");
    this->Tools[0]->SetToolPartNumber("Stationary");
    this->Tools[0]->SetToolSerialNumber("A11111");
    this->Tools[0]->SetToolName("Test");

    this->Counter = 0;

    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkFakeTracker::Connect()
{
  LOG_TRACE("vtkFakeTracker::Connect"); 

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

  for (int i=0; i<this->GetNumberOfTools(); ++i) 
  {
    this->Tools[i]->EnabledOn();
  }

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
      for (int tool = 0; tool < 4; tool++) 
      {
        TrackerStatus trackerStatus = TR_OK;

        int rotation = this->Frame/1000;

        switch (tool)
        {
        case 0:
          // This tool is stationary
          this->InternalTransform->Identity();
          this->InternalTransform->Translate(0, 150, 200);
          break;
        case 1:
          // This tool rotates about a path on the Y axis
          this->InternalTransform->Identity();
          this->InternalTransform->RotateY(rotation);
          this->InternalTransform->Translate(0, 300, 0);
          break;
        case 2:
          // This tool rotates about a path on the X axis
          this->InternalTransform->Identity();
          this->InternalTransform->RotateX(rotation);
          this->InternalTransform->Translate(0, 300, 200);
          break;
        case 3:
          // This tool spins on the X axis
          this->InternalTransform->Identity();
          this->InternalTransform->Translate(100, 300, 0);
          this->InternalTransform->RotateX(rotation);
          break;
        }

        this->ToolTimeStampedUpdate(tool,this->InternalTransform->GetMatrix(),trackerStatus,this->Frame, unfilteredTimestamp);   
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
      this->ToolTimeStampedUpdate(0,this->InternalTransform->GetMatrix(),trackerStatus,this->Frame, unfilteredTimestamp);   

      this->InternalTransform->Identity();
      this->InternalTransform->Translate(0, 0, 50); 
      // Reference transform 
      this->ToolTimeStampedUpdate(1,this->InternalTransform->GetMatrix(),trackerStatus,this->Frame, unfilteredTimestamp);   
      this->ToolTimeStampedUpdate(2,this->InternalTransform->GetMatrix(),TR_MISSING,this->Frame, unfilteredTimestamp);   

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

      this->ToolTimeStampedUpdate(0, trackerToReferenceToolTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);   

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
      this->ToolTimeStampedUpdate(1, referenceToolToStylusTipTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);   
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

      this->ToolTimeStampedUpdate(0, phantomReferenceToTrackerTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);

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
      stylustipToStylusTransform->SetMatrix(this->GetTool(this->GetToolPortByName("Stylus"))->GetCalibrationMatrix());
      stylustipToStylusTransform->Inverse();
      stylustipToStylusTransform->Modified();

      vtkSmartPointer<vtkTransform> phantomReferenceToLandmarkTransform = vtkSmartPointer<vtkTransform>::New();
      phantomReferenceToLandmarkTransform->Identity();

      phantomReferenceToLandmarkTransform->Concatenate(phantomReferenceToPhantomTransform);
      phantomReferenceToLandmarkTransform->Concatenate(phantomToLandmarkTransform);
      phantomReferenceToLandmarkTransform->Concatenate(stylustipToStylusTransform);

      this->ToolTimeStampedUpdate(1, phantomReferenceToLandmarkTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);   
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

      this->ToolTimeStampedUpdate(0, identityTransform->GetMatrix(), trackerStatus, this->Frame, unfilteredTimestamp);

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

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
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

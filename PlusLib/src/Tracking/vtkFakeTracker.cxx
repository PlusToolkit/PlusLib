/* ============================================================================

File: vtkFakeTracker.cxx
Author: Kyle Charbonneau <kcharbon@imaging.robarts.ca>
Language: C++
Description: 
This class represents a fake tracking system with tools that have
predetermined behaviour. This allows someonew who doesn't have access to
a tracking system to test code that relies on having one active.

============================================================================ */
#include "PlusConfigure.h"
#include "vtkFakeTracker.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTrackerTool.h"
#include "vtkMinimalStandardRandomSequence.h"
#include "vtkFrameToTimeConverter.h"

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
	this->SerialPort = 0;
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
	this->Mode = mode;

	switch (this->Mode)
	{
	case (FakeTrackerMode_Default):
		this->SetNumberOfTools(4);

		this->Tools[0]->SetToolType("Marker");
		this->Tools[0]->SetToolRevision("1.3");
		this->Tools[0]->SetToolManufacturer("ACME Inc.");
		this->Tools[0]->SetToolPartNumber("Stationary");
		this->Tools[0]->SetToolSerialNumber("A34643");

		this->Tools[1]->SetToolType("Pointer");
		this->Tools[1]->SetToolRevision("1.1");
		this->Tools[1]->SetToolManufacturer("ACME Inc.");
		this->Tools[1]->SetToolPartNumber("Rotate");
		this->Tools[1]->SetToolSerialNumber("B3464C");

		this->Tools[2]->SetToolType("Pointer");
		this->Tools[2]->SetToolRevision("1.1");
		this->Tools[2]->SetToolManufacturer("ACME Inc.");
		this->Tools[2]->SetToolPartNumber("Rotate");
		this->Tools[2]->SetToolSerialNumber("Q45P5");

		this->Tools[3]->SetToolType("Pointer");
		this->Tools[3]->SetToolRevision("2.0");
		this->Tools[3]->SetToolManufacturer("ACME Inc.");
		this->Tools[3]->SetToolPartNumber("Spin");
		this->Tools[3]->SetToolSerialNumber("Q34653");

		break;

	case (FakeTrackerMode_PivotCalibration):
		this->SetNumberOfTools(2);

		this->Tools[0]->SetToolType("Marker");
		this->Tools[0]->SetToolRevision("1.3");
		this->Tools[0]->SetToolManufacturer("ACME Inc.");
		this->Tools[0]->SetToolPartNumber("Stationary");
		this->Tools[0]->SetToolSerialNumber("A11111");
		this->Tools[0]->SetToolName("Reference");

		this->Tools[1]->SetToolType("Pointer");
		this->Tools[1]->SetToolRevision("1.1");
		this->Tools[1]->SetToolManufacturer("ACME Inc.");
		this->Tools[1]->SetToolPartNumber("Stylus");
		this->Tools[1]->SetToolSerialNumber("B22222");
		this->Tools[1]->SetToolName("Stylus");

		this->SetReferenceToolName("Reference");
		this->SetDefaultToolName("Stylus");

		break;
	case (FakeTrackerMode_RecordPhantomLandmarks):
		this->SetNumberOfTools(2);

		this->Tools[0]->SetToolType("Marker");
		this->Tools[0]->SetToolRevision("1.3");
		this->Tools[0]->SetToolManufacturer("ACME Inc.");
		this->Tools[0]->SetToolPartNumber("Stationary");
		this->Tools[0]->SetToolSerialNumber("A11111");
		this->Tools[0]->SetToolName("Reference");

		this->Tools[1]->SetToolType("Pointer");
		this->Tools[1]->SetToolRevision("1.1");
		this->Tools[1]->SetToolManufacturer("ACME Inc.");
		this->Tools[1]->SetToolPartNumber("Stylus");
		this->Tools[1]->SetToolSerialNumber("B22222");
		this->Tools[1]->SetToolName("Stylus");

		this->SetReferenceToolName("Reference");
		this->SetDefaultToolName("Stylus");

		this->Counter = -1;

		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------
int vtkFakeTracker::Connect()
{
	return 1; 
}

//----------------------------------------------------------------------------
void vtkFakeTracker::Disconnect()
{
	this->StopTracking(); 
}

//----------------------------------------------------------------------------
int vtkFakeTracker::Probe()
{
	return 1;
}

//----------------------------------------------------------------------------
int vtkFakeTracker::InternalStartTracking()
{
	// for accurate timing
	this->Timer->Initialize();
	this->RandomSeed = 0;

  	for (int i=0; i<this->GetNumberOfTools(); ++i) 
	{
		this->Tools[i]->EnabledOn();
	}

	return 1; 
}

int vtkFakeTracker::InternalStopTracking()
{
	return 1;
}

//----------------------------------------------------------------------------
void vtkFakeTracker::InternalUpdate()
{
	if (this->Frame++ > 355559)
	{
		this->Frame = 0;
	}

	switch (this->Mode)
	{
	case (FakeTrackerMode_Default): // Spins the tools around different axis to fake movement
		{
		// Create timestamp 
		double unfilteredtimestamp(0), filteredtimestamp(0); 
		this->Timer->GetTimeStampForFrame(this->Frame, unfilteredtimestamp, filteredtimestamp);

		for (int tool = 0; tool < 4; tool++) 
		{
			int flags = 0;

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

			this->ToolUpdate(tool,this->InternalTransform->GetMatrix(),flags,this->Frame, unfilteredtimestamp, filteredtimestamp);   
		}
		}
		break;


	case (FakeTrackerMode_PivotCalibration): // Moves around a stylus with the tip fixed to a position
		{
			vtkMinimalStandardRandomSequence* random = vtkMinimalStandardRandomSequence::New();
			random->SetSeed(RandomSeed++); // To get completely random numbers, timestamp should use instead of constant seed

			// Create timestamp 
			double unfilteredtimestamp(0), filteredtimestamp(0); 
			this->Timer->GetTimeStampForFrame(this->Frame, unfilteredtimestamp, filteredtimestamp);

			// Set flags
			int flags = 0;

			// Once in every 50 request, the tracker provides 'out of view' flag - FOR TEST PURPOSES
			/*
			random->Next();
			double outOfView = random->GetValue();
			if (outOfView < 0.02) {
				flags = TR_OUT_OF_VIEW;
			}
			*/

			// create stationary position for reference (tool 0)
			vtkSmartPointer<vtkTransform> trackerToReferenceToolTransform = vtkSmartPointer<vtkTransform>::New();
			trackerToReferenceToolTransform->Identity();
			trackerToReferenceToolTransform->Translate(300, 400, 700);
			trackerToReferenceToolTransform->RotateZ(90);

			this->ToolUpdate(0, trackerToReferenceToolTransform->GetMatrix(), flags, this->Frame, unfilteredtimestamp, filteredtimestamp);   

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
			this->ToolUpdate(1, referenceToolToStylusTipTransform->GetMatrix(), flags, this->Frame, unfilteredtimestamp, filteredtimestamp);   
		}
		break;

	case (FakeTrackerMode_RecordPhantomLandmarks): // Touches some positions with 1 sec difference
		{
			// Create timestamp 
			double unfilteredtimestamp(0), filteredtimestamp(0); 
			this->Timer->GetTimeStampForFrame(this->Frame, unfilteredtimestamp, filteredtimestamp);

			// Set flags - one in every 50 request, the tracker provides 'out of view' flag
			int flags = 0;

			// create stationary position for phantom reference (tool 0)
			vtkSmartPointer<vtkTransform> phantomReferenceToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
			phantomReferenceToTrackerTransform->Identity();
			
			phantomReferenceToTrackerTransform->Translate(300, 400, 700);
			phantomReferenceToTrackerTransform->RotateZ(90);
			
			this->ToolUpdate(0, phantomReferenceToTrackerTransform->GetMatrix(), flags, this->Frame, unfilteredtimestamp, filteredtimestamp);   

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
			stylustipToStylusTransform->SetMatrix(this->GetTool(1)->GetCalibrationMatrix());
			stylustipToStylusTransform->Inverse();
			stylustipToStylusTransform->Modified();

			vtkSmartPointer<vtkTransform> phantomReferenceToLandmarkTransform = vtkSmartPointer<vtkTransform>::New();
			phantomReferenceToLandmarkTransform->Identity();
			phantomReferenceToLandmarkTransform->Concatenate(phantomReferenceToPhantomTransform);
			phantomReferenceToLandmarkTransform->Concatenate(phantomToLandmarkTransform);
			phantomReferenceToLandmarkTransform->Concatenate(stylustipToStylusTransform);

			this->ToolUpdate(1, phantomReferenceToLandmarkTransform->GetMatrix(), flags, this->Frame, unfilteredtimestamp, filteredtimestamp);   
		}
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------
void vtkFakeTracker::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkFakeTracker::ReadConfiguration"); 
	if ( config == NULL ) 
	{
		LOG_WARNING("Unable to find FakeTracker XML data element");
		return; 
	}

	if ( !this->Tracking )
	{
		const char* mode = config->GetAttribute("Mode"); 
		if ( mode != NULL ) 
		{
			if (STRCASECMP(mode, "Default") == 0) {
				this->SetMode(FakeTrackerMode_Default); 
			} else if (STRCASECMP(mode, "PivotCalibration") == 0) {
				this->SetMode(FakeTrackerMode_PivotCalibration); 
			} else if (STRCASECMP(mode, "RecordPhantomLandmarks") == 0) {
				this->SetMode(FakeTrackerMode_RecordPhantomLandmarks); 
			} else {
				this->SetMode(FakeTrackerMode_Undefined);
			}
		}
	}

	Superclass::ReadConfiguration(config); 
}

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOptimetConoProbeMeasurer.h"

#include "PlusMath.h"

#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

#include <math.h>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#include <Smart32Interface.h>

vtkStandardNewMacro(vtkPlusOptimetConoProbeMeasurer);

//-----------------------------------------------------------------------
vtkPlusOptimetConoProbeMeasurer::vtkPlusOptimetConoProbeMeasurer()
{ 
  this->MeasurementTool = NULL;
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->AcquisitionRate = 100;

  // ConoProbe specific
  this->ConoProbe = NULL;
  this->LensOriginAlignment[0] = 1.0;
  this->LensOriginAlignment[1] = 1.0;
  this->LensOriginAlignment[2] = 1.0;
  this->LensOriginAlignment[3] = 1.0;
  this->LensOriginAlignment[4] = 1.0;
  this->LensOriginAlignment[5] = 1.0;
  this->DelayBetweenMeasurements = 1;
  this->Frequency= 100;
  this->CoarseLaserPower = 13;
  this->FineLaserPower = 0;
  this->ProbeDialogOpen = false;
  this->LaserPower = 0;

  // Thread
  this->Thread = vtkMultiThreader::New();
  this->ThreadID = -1;

  this->StartThreadForInternalUpdates=true; 
}

//-------------------------------------------------------------------------
vtkPlusOptimetConoProbeMeasurer::~vtkPlusOptimetConoProbeMeasurer() 
{
  this->Stop();
  if (this->Thread)
  {
    this->Thread->Delete();
  }

  this->MeasurementTool = NULL;
  if (this->ConoProbe)
  {
    ISmart::Destroy(this->ConoProbe);
    this->ConoProbe = NULL;
  }
}

//-------------------------------------------------------------------------
void vtkPlusOptimetConoProbeMeasurer::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::InternalConnect()
{
  LOG_TRACE( "vtkPlusOptimetConoProbeMeasurer::Connect" ); 

  this->ConoProbe = ISmart::Create();
  try
  {
    // Initialize the probe with IP 1.2.3.4 and LAN IP 1.2.3.9
    this->ConoProbe->Init(0x01020304, 0x01020309, 10000);	
  }
  catch (const SmartException& e)
  {
    LOG_ERROR(e.ErrorString());
    return PLUS_FAIL;
  }

  try
  {
	// Set acquisition parameters
	  this->ConoProbe->SetAcquisitionParams(AcquisitionMode::TimeAcquisitionMode, this->Frequency, this->CalculateCompositeLaserPower(this->CoarseLaserPower, this->FineLaserPower), this->DelayBetweenMeasurements);
	  this->SetLaserPower(this->CalculateCompositeLaserPower(this->CoarseLaserPower, this->FineLaserPower));
  }
  catch (const SmartException& e)
  {
    LOG_ERROR(e.ErrorString());
    return PLUS_FAIL;
  }

  this->MeasurementTool = NULL;
 GetToolByPortName("Measurement", this->MeasurementTool);

 LOG_DEBUG("Successfully connected to ConoProbe device");
 return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::InternalDisconnect()
{
  LOG_TRACE( "vtkPlusOptimetConoProbeMeasurer::InternalDisconnect" ); 
  this->MeasurementTool = NULL;
  if (this->ConoProbe)
  {
    ISmart::Destroy(this->ConoProbe);
    this->ConoProbe = NULL;
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::InternalUpdate()
{     
  LOG_TRACE( "vtkPlusOptimetConoProbeMeasurer::InternalUpdate" ); 
  
  if (this->MeasurementTool != NULL)
  {
	vtkSmartPointer<vtkTransform> measurementToMeasurerTransform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkTransform> parametersToMeasurerTransform = vtkSmartPointer<vtkTransform>::New();

	if (!this->ProbeDialogOpen)
	{
		Measurement measurement;
		try
		{
			// Get ConoProbe measurement and write to MeasurementToMeasurerTransform
			measurement = this->ConoProbe->GetSingleMeasurement();
		}
		catch (const SmartException& e)
		{
			LOG_ERROR(e.ErrorString());
			return PLUS_FAIL;
		}
		catch (const SmartExceptionBadResponse& e)
		{
			LOG_WARNING(e.MessageType());
		}

		// Get distance (mm), Snr (%), Total, and set correct lens origin parameters
		double d = measurement.Distance;
		double snr = measurement.Snr / 10;
		double total = measurement.Total;
		double dx = this->LensOriginAlignment[0];
		double dy = this->LensOriginAlignment[1];
		double dz = this->LensOriginAlignment[2];
		double lx = this->LensOriginAlignment[3];
		double ly = this->LensOriginAlignment[4];
		double lz = this->LensOriginAlignment[5];

		// Create transforms
		measurementToMeasurerTransform->Translate(dx * d + lx, dy * d + ly, dz * d + lz);
		double params[16]{ d, snr, total, 0.0,
						   this->Frequency, this->LaserPower, 0.0, 0.0,
				           0.0, 0.0, 0.0, 0.0,
					       0.0, 0.0, 0.0, 0.0, };
		parametersToMeasurerTransform->SetMatrix(params);
	}
	else
	{
		measurementToMeasurerTransform->Identity();
		parametersToMeasurerTransform->Identity();
	}

	// Create frame number and time stamp
	unsigned long frameNumber = this->MeasurementTool->GetFrameNumber() + 1 ;
	const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

	// Send transforms
	igsioTransformName name("Measurement", this->GetToolReferenceFrameName());
	this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), measurementToMeasurerTransform->GetMatrix(), ToolStatus::TOOL_OK, frameNumber, unfilteredTimestamp);
	igsioTransformName parameters("Parameters", this->GetToolReferenceFrameName());
	this->ToolTimeStampedUpdate(parameters.GetTransformName().c_str(), parametersToMeasurerTransform->GetMatrix(), ToolStatus::TOOL_OK, frameNumber, unfilteredTimestamp);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  double lensOriginAlignment[6];
  if (deviceConfig->GetVectorAttribute("LensOriginAlignment", 6, lensOriginAlignment))
  {
	{
		this->LensOriginAlignment[0] = lensOriginAlignment[0];
		this->LensOriginAlignment[1] = lensOriginAlignment[1];
		this->LensOriginAlignment[2] = lensOriginAlignment[2];
		this->LensOriginAlignment[3] = lensOriginAlignment[3];
		this->LensOriginAlignment[4] = lensOriginAlignment[4];
		this->LensOriginAlignment[5] = lensOriginAlignment[5];
	}
  }

  int delayBetweenMeasurements = 0; 
  if ( deviceConfig->GetScalarAttribute("DelayBetweenMeasurements", delayBetweenMeasurements ) ) 
  {
    {
      this->DelayBetweenMeasurements = static_cast<unsigned short>(delayBetweenMeasurements);
    }
  }

  int frequency = 0; 
  if ( deviceConfig->GetScalarAttribute("Frequency", frequency ) ) 
  {
    {
      this->Frequency = static_cast<unsigned short>(frequency);
    }
  }

  int coarseLaserPower = 0; 
  if ( deviceConfig->GetScalarAttribute("CoarseLaserPower", coarseLaserPower ) ) 
  {
    {
      this->CoarseLaserPower = static_cast<unsigned short>(coarseLaserPower);
    }
  }

  int fineLaserPower = 0; 
  if ( deviceConfig->GetScalarAttribute("FineLaserPower", fineLaserPower ) ) 
  {
    {
      this->FineLaserPower = static_cast<unsigned short>(fineLaserPower);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("DelayBetweenMeasurements", static_cast<unsigned int>(this->DelayBetweenMeasurements));
  deviceConfig->SetIntAttribute("Frequency", static_cast<unsigned int>(this->Frequency));
  deviceConfig->SetIntAttribute("CoarseLaserPower", static_cast<unsigned int>(this->CoarseLaserPower));
  deviceConfig->SetIntAttribute("FineLaserPower", static_cast<unsigned int>(this->FineLaserPower));

  return PLUS_SUCCESS;
}

//---------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::Start()
{
	if (this->ThreadID == 0)
	{
		return PLUS_FAIL;
	}
	this->ProbeDialogOpen = true;
	this->ThreadID = 0;
	this->Thread->SpawnThread((vtkThreadFunctionType)&vtkPlusOptimetConoProbeMeasurer::ProbeDialogThread, this);

	return PLUS_SUCCESS;
}

//---------------------------------------------------------------------------
PlusStatus vtkPlusOptimetConoProbeMeasurer::Stop()
{
	if (this->ThreadID == 0)
	{
		this->ThreadID = -1;
		this->ProbeDialogOpen = false;
		this->Thread->TerminateThread(0);
		
		return PLUS_SUCCESS;
	}
	else
	{
		return PLUS_FAIL;
	}
}

//----------------------------------------------------------------------------
void* vtkPlusOptimetConoProbeMeasurer::ProbeDialogThread(void* ptr)
{
	vtkMultiThreader::ThreadInfo* vinfo = static_cast<vtkMultiThreader::ThreadInfo*>(ptr);
	vtkPlusOptimetConoProbeMeasurer* logic = reinterpret_cast<vtkPlusOptimetConoProbeMeasurer*>(vinfo->UserData);

	ProbeDialogParams p;
	ProbeDialogResult r;

	ZeroMemory(&p, sizeof (ProbeDialogParams));
	ZeroMemory(&r, sizeof (ProbeDialogResult));

	p.DlgProc = NULL;
	p.Units = MMUnits;
	p.Power = logic->CoarseLaserPower;
	p.FinePower = logic->FineLaserPower;
	p.Frequency = logic->Frequency;
	p.DlgProc = NULL;

	bool okPressed = ISmart::ShowProbeDialog(&logic->ConoProbe, 1, &p, &r);

	logic->ConoProbe->SetAcquisitionParams(AcquisitionMode::TimeAcquisitionMode, logic->Frequency, logic->CalculateCompositeLaserPower(r.Power, r.FinePower), logic->DelayBetweenMeasurements);
	logic->SetLaserPower(logic->CalculateCompositeLaserPower(r.Power, r.FinePower));
	logic->SetFrequency(r.Frequency);

	logic->Stop();

	return NULL;
}

//----------------------------------------------------------------------------
unsigned short vtkPlusOptimetConoProbeMeasurer::CalculateCompositeLaserPower(UINT16 coarseLaserPower, UINT16 fineLaserPower)
{
	unsigned short compositeLaserPower = coarseLaserPower;
	compositeLaserPower <<= 6;
	compositeLaserPower |= fineLaserPower;
  return compositeLaserPower;
}
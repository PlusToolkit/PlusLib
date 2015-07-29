/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Date: 
July 2015
Authors include:
* Mikael Brudfors (Laboratorio de Imagen Médica, Unidad de Medicina y Cirugía Experimental, Hospital General Universitario)
* Laura Sanz (Laboratorio de Imagen Médica, Unidad de Medicina y Cirugía Experimental, Hospital General Universitario)
=========================================================================*/  

#include "PlusConfigure.h"
#include "vtkOptimetConoProbeTracker.h"

#include "PlusMath.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
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

vtkStandardNewMacro(vtkOptimetConoProbeTracker);

//-----------------------------------------------------------------------
vtkOptimetConoProbeTracker::vtkOptimetConoProbeTracker()
{ 
  this->DistanceTool = NULL;

  this->RequirePortNameInDeviceSetConfiguration = true;

  // ConoProbe specific
  this->ConoProbe = NULL;
  this->AcquisitionRate = 100;
  this->DelayBetweenMeasurements = 1;
  this->Frequency= 1000;
  this->CoarseLaserPower = 13;
  this->FineLaserPower = 0;

  this->StartThreadForInternalUpdates=true; 
}

//-------------------------------------------------------------------------
vtkOptimetConoProbeTracker::~vtkOptimetConoProbeTracker() 
{
  this->DistanceTool = NULL;
  if (this->ConoProbe)
  {
    ISmart::Destroy(this->ConoProbe);
    this->ConoProbe = NULL;
  }
}

//-------------------------------------------------------------------------
void vtkOptimetConoProbeTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeTracker::InternalConnect()
{
  LOG_TRACE( "vtkOptimetConoProbeTracker::Connect" ); 

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
    this->ConoProbe->SetAcquisitionParams(AcquisitionMode::TimeAcquisitionMode, this->Frequency, this->GetCompositeLaserPower(), this->DelayBetweenMeasurements);
  }
  catch (const SmartException& e)
  {
    LOG_ERROR(e.ErrorString());
    return PLUS_FAIL;
  }

	this->DistanceTool = NULL;
	GetToolByPortName("Distance", this->DistanceTool);

	LOG_DEBUG("Successfully connected to ConoProbe device");
	return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkOptimetConoProbeTracker::InternalDisconnect" ); 
  this->DistanceTool = NULL;
  if (this->ConoProbe)
  {
    ISmart::Destroy(this->ConoProbe);
    this->ConoProbe = NULL;
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeTracker::InternalUpdate()
{     
  LOG_TRACE( "vtkOptimetConoProbeTracker::InternalUpdate" ); 
  
  if (this->DistanceTool != NULL)
  {
    Measurement measurement;
    try
    {
      // Get ConoProbe measurement and write to DistanceToTrackerTransform
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

	  vtkSmartPointer<vtkTransform> DistanceToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
    DistanceToTrackerTransform->Identity();
    
    // Get distance (mm), Signal-to-noise ratio (%) and the Total
    DistanceToTrackerTransform->Translate(measurement.Distance, measurement.Snr / 10, measurement.Total); 
		
	  unsigned long frameNumber = this->DistanceTool->GetFrameNumber() + 1 ;
	  PlusTransformName name("Distance", this->GetToolReferenceFrameName());
    const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
	  this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), DistanceToTrackerTransform->GetMatrix(), ToolStatus::TOOL_OK, frameNumber, unfilteredTimestamp);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

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
PlusStatus vtkOptimetConoProbeTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("DelayBetweenMeasurements", static_cast<unsigned int>(this->DelayBetweenMeasurements));
  deviceConfig->SetIntAttribute("Frequency", static_cast<unsigned int>(this->Frequency));
  deviceConfig->SetIntAttribute("CoarseLaserPower", static_cast<unsigned int>(this->CoarseLaserPower));
  deviceConfig->SetIntAttribute("FineLaserPower", static_cast<unsigned int>(this->FineLaserPower));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeTracker::SetFrequency(int frequency)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
unsigned short vtkOptimetConoProbeTracker::GetCompositeLaserPower()
{
  unsigned short compositeLaserPower = this->CoarseLaserPower;
	compositeLaserPower <<= 6;
  compositeLaserPower |= this->FineLaserPower;
  return compositeLaserPower;
}
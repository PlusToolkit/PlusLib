/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkOptimetConoProbeMeasurer.h"

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

vtkStandardNewMacro(vtkOptimetConoProbeMeasurer);

//-----------------------------------------------------------------------
vtkOptimetConoProbeMeasurer::vtkOptimetConoProbeMeasurer()
{ 
  this->MeasurementTool = NULL;
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->AcquisitionRate = 100;

  // ConoProbe specific
  this->ConoProbe = NULL;
  this->LensOriginPosition[0] = 1.0;
  this->LensOriginPosition[1] = 1.0;
  this->LensOriginPosition[2] = 1.0;
  this->LensOriginPosition[3] = 1.0;
  this->LensOriginPosition[4] = 1.0;
  this->LensOriginPosition[5] = 1.0;
  this->DelayBetweenMeasurements = 1;
  this->Frequency= 1000;
  this->CoarseLaserPower = 13;
  this->FineLaserPower = 0;

  this->StartThreadForInternalUpdates=true; 
}

//-------------------------------------------------------------------------
vtkOptimetConoProbeMeasurer::~vtkOptimetConoProbeMeasurer() 
{
  this->MeasurementTool = NULL;
  if (this->ConoProbe)
  {
    ISmart::Destroy(this->ConoProbe);
    this->ConoProbe = NULL;
  }
}

//-------------------------------------------------------------------------
void vtkOptimetConoProbeMeasurer::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeMeasurer::InternalConnect()
{
  LOG_TRACE( "vtkOptimetConoProbeMeasurer::Connect" ); 

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

	this->MeasurementTool = NULL;
	GetToolByPortName("Measurement", this->MeasurementTool);

	LOG_DEBUG("Successfully connected to ConoProbe device");
	return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeMeasurer::InternalDisconnect()
{
  LOG_TRACE( "vtkOptimetConoProbeMeasurer::InternalDisconnect" ); 
  this->MeasurementTool = NULL;
  if (this->ConoProbe)
  {
    ISmart::Destroy(this->ConoProbe);
    this->ConoProbe = NULL;
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeMeasurer::InternalUpdate()
{     
  LOG_TRACE( "vtkOptimetConoProbeMeasurer::InternalUpdate" ); 
  
  if (this->MeasurementTool != NULL)
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

	double d = measurement.Distance;
	double dx = this->LensOriginPosition[0];
	double dy = this->LensOriginPosition[1];
	double dz = this->LensOriginPosition[2];
	double lx = this->LensOriginPosition[3];
	double ly = this->LensOriginPosition[4];
	double lz = this->LensOriginPosition[5];

	vtkSmartPointer<vtkTransform> MeasurementToMeasurerTransform = vtkSmartPointer<vtkTransform>::New();
    MeasurementToMeasurerTransform->Identity();
    
    // Get distance (mm), Signal-to-noise ratio (%) and the Total
	MeasurementToMeasurerTransform->Translate(dx * d + lx, dy * d + ly, dz * d + lz);  // (measurement.Distance, measurement.Snr / 10, measurement.Total)
		
	unsigned long frameNumber = this->MeasurementTool->GetFrameNumber() + 1 ;
	PlusTransformName name("Measurement", this->GetToolReferenceFrameName());
    const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
	this->ToolTimeStampedUpdate(name.GetTransformName().c_str(), MeasurementToMeasurerTransform->GetMatrix(), ToolStatus::TOOL_OK, frameNumber, unfilteredTimestamp);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeMeasurer::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  double lensOriginPosition[6];
  if (deviceConfig->GetVectorAttribute("LensOriginPosition", 6, lensOriginPosition))
  {
	{
		this->LensOriginPosition[0] = lensOriginPosition[0];
		this->LensOriginPosition[1] = lensOriginPosition[1];
		this->LensOriginPosition[2] = lensOriginPosition[2];
		this->LensOriginPosition[3] = lensOriginPosition[3];
		this->LensOriginPosition[4] = lensOriginPosition[4];
		this->LensOriginPosition[5] = lensOriginPosition[5];
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
PlusStatus vtkOptimetConoProbeMeasurer::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("DelayBetweenMeasurements", static_cast<unsigned int>(this->DelayBetweenMeasurements));
  deviceConfig->SetIntAttribute("Frequency", static_cast<unsigned int>(this->Frequency));
  deviceConfig->SetIntAttribute("CoarseLaserPower", static_cast<unsigned int>(this->CoarseLaserPower));
  deviceConfig->SetIntAttribute("FineLaserPower", static_cast<unsigned int>(this->FineLaserPower));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOptimetConoProbeMeasurer::SetFrequency(int frequency)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
unsigned short vtkOptimetConoProbeMeasurer::GetCompositeLaserPower()
{
  unsigned short compositeLaserPower = this->CoarseLaserPower;
	compositeLaserPower <<= 6;
  compositeLaserPower |= this->FineLaserPower;
  return compositeLaserPower;
}
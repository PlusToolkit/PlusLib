/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsImagingParameters.h"


//----------------------------------------------------------------------------

vtkUsImagingParameters::vtkUsImagingParameters(vtkPlusDevice* aImagingDevice)
: vtkObject()
, FrequencyMhz(-1)
, DepthMm(-1)
, SectorPercent(-1)
, GainPercent(-1)
, DynRangeDb(-1)
, ZoomFactor(-1)
, SoundVelocity(1540)
{
   //this->ImagingDevice = imagingDevice;
   this->ImagingDevice = aImagingDevice;
}

//----------------------------------------------------------------------------

vtkUsImagingParameters::~vtkUsImagingParameters()
{
  LOG_TRACE("vtkUsImagingParameters::~vtkUsImagingParameters()");
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetFrequencyMhz(double aFrequencyMhz)
{
   this->FrequencyMhz = aFrequencyMhz;
   return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetFrequencyMhz(double& aFrequencyMhz)
{
	aFrequencyMhz = this->FrequencyMhz;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDepthMm(double aDepthMm)
{
  this->DepthMm = aDepthMm;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDepthMm(double& aDepthMm)
{
  aDepthMm = this->DepthMm;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetGainPercent(double aGainPercent)
{
    this->GainPercent = aGainPercent;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetGainPercent(double& aGainPercent)
{
	aGainPercent = this->GainPercent;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDynRangeDb(double aDynRangeDb)
{
    this->DynRangeDb = aDynRangeDb;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDynRangeDb(double& aDynRangeDb)
{
    aDynRangeDb = this->DynRangeDb ;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetZoomFactor(double aZoomFactor)
{
    this->ZoomFactor = aZoomFactor;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetZoomFactor(double& aZoomFactor)
{
	aZoomFactor = this->ZoomFactor;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSectorPercent(double aSectorPercent)
{
    this->SectorPercent = aSectorPercent;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSectorPercent(double& aSectorPercent)
{
    aSectorPercent = this->SectorPercent;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSoundVelocity(double aSoundVelocity)
{
    this->SoundVelocity = aSoundVelocity;
    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSoundVelocity(double& aSoundVelocity)
{
    aSoundVelocity = this->SoundVelocity;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::PrintListOfImagingParameters()
{

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDisplayedFrameRate(double &aFrameRate)
{

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::ReadConfiguration(vtkXMLDataElement* config)
{

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::WriteConfiguration(vtkXMLDataElement* config)
{

  return PLUS_SUCCESS;
}


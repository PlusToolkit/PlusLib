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
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetFrequencyMhz(double& aFrequencyMhz)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDepthMm(double aDepthMm)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDepthMm(double& aDepthMm)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetGainPercent(double aGainFactor)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetGainPercent(double& aGainFactor)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDynRangedB(double aDynRangeFactor)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDynRangedB(double& aDynRangeFactor)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetZoomFactor(double aZoomFactor)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetZoomFactor(double& aZoomFactor)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSectorPercent(double aSectorPercent)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSectorPercent(double& aSectorPercent)
{
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

